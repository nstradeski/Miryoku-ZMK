/*
 * Bluetooth LE MIDI (Apple / MMA "MIDI over Bluetooth Low Energy") service.
 *
 * The keyboard exposes the standard MIDI service next to ZMK's HID service on
 * the same connection, so the iPad keeps typing on it and additionally sees a
 * Bluetooth MIDI device once a MIDI app connects to it.
 *
 * SPDX-License-Identifier: MIT
 */

#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/logging/log.h>

#include <zmk/ble.h>
#include <zmk/event_manager.h>
#include <zmk/events/ble_active_profile_changed.h>
#include <zmk_ble_midi/ble_midi.h>
#include <zmk_ble_midi/midi_state.h>

LOG_MODULE_REGISTER(zmk_ble_midi, CONFIG_ZMK_LOG_LEVEL);

/* 03B80E5A-EDE8-4B33-A751-6CE34EC4C700 */
#define BT_UUID_MIDI_SERVICE_VAL                                                                   \
    BT_UUID_128_ENCODE(0x03B80E5A, 0xEDE8, 0x4B33, 0xA751, 0x6CE34EC4C700)
/* 7772E5DB-3868-4112-A1A9-F2669D106BF3 */
#define BT_UUID_MIDI_IO_VAL BT_UUID_128_ENCODE(0x7772E5DB, 0x3868, 0x4112, 0xA1A9, 0xF2669D106BF3)

#define MIDI_MSG_MAX 7 /* longest message we emit: an MMC SysEx, F0 7F 7F 06 cc F7 */

struct midi_msg {
    uint8_t len;
    uint8_t data[MIDI_MSG_MAX];
};

static uint8_t midi_channel;

uint8_t zmk_ble_midi_channel(void) { return midi_channel; }
void zmk_ble_midi_set_channel(uint8_t channel) { midi_channel = channel & 0x0F; }

static void pair_adv_stop_cb(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(pair_adv_stop_work, pair_adv_stop_cb);

static ssize_t read_midi_io(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf,
                            uint16_t len, uint16_t offset) {
    /* The spec requires reads to succeed with an empty payload. */
    return bt_gatt_attr_read(conn, attr, buf, len, offset, NULL, 0);
}

static ssize_t write_midi_io(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                             const void *buf, uint16_t len, uint16_t offset, uint8_t flags) {
    /* MIDI from the host (clock, feedback) is accepted and ignored. */
    return len;
}

static void midi_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value) {
    bool on = value == BT_GATT_CCC_NOTIFY;
    LOG_INF("MIDI notifications %s", on ? "enabled" : "disabled");
    zmk_ble_midi_state_notify();
    if (on) {
        /* A MIDI app found us; no need to keep advertising for it. */
        k_work_reschedule(&pair_adv_stop_work, K_NO_WAIT);
    }
}

BT_GATT_SERVICE_DEFINE(midi_svc, BT_GATT_PRIMARY_SERVICE(BT_UUID_DECLARE_128(BT_UUID_MIDI_SERVICE_VAL)),
                       BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_128(BT_UUID_MIDI_IO_VAL),
                                              BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE_WITHOUT_RESP |
                                                  BT_GATT_CHRC_NOTIFY,
                                              BT_GATT_PERM_READ_ENCRYPT |
                                                  BT_GATT_PERM_WRITE_ENCRYPT,
                                              read_midi_io, write_midi_io, NULL),
                       BT_GATT_CCC(midi_ccc_changed,
                                   BT_GATT_PERM_READ_ENCRYPT | BT_GATT_PERM_WRITE_ENCRYPT));

#define MIDI_IO_ATTR (&midi_svc.attrs[2])

/* Returns the active host's connection (referenced) if it is subscribed. */
static struct bt_conn *subscribed_conn(void) {
    struct bt_conn *conn = zmk_ble_active_profile_conn();
    if (conn == NULL) {
        return NULL;
    }
    if (!bt_gatt_is_subscribed(conn, MIDI_IO_ATTR, BT_GATT_CCC_NOTIFY)) {
        bt_conn_unref(conn);
        return NULL;
    }
    return conn;
}

bool zmk_ble_midi_is_connected(void) {
    struct bt_conn *conn = subscribed_conn();
    if (conn == NULL) {
        return false;
    }
    bt_conn_unref(conn);
    return true;
}

/* Sending: bt_gatt_notify can block waiting for buffers, so messages go
 * through a queue drained on a dedicated work queue, like ZMK's own HID
 * reports. */

K_MSGQ_DEFINE(midi_msgq, sizeof(struct midi_msg), CONFIG_ZMK_BLE_MIDI_QUEUE_SIZE, 1);
K_THREAD_STACK_DEFINE(midi_q_stack, CONFIG_ZMK_BLE_MIDI_THREAD_STACK_SIZE);
static struct k_work_q midi_work_q;

static void send_packet(struct bt_conn *conn, const struct midi_msg *msg) {
    /* BLE-MIDI packet: header (bit7 set, timestamp bits 12-7), then each
     * message preceded by a timestamp byte (bit7 set, timestamp bits 6-0).
     * A SysEx end (F7) needs its own timestamp byte in front of it. */
    uint16_t ts = k_uptime_get_32() & 0x1FFF;
    uint8_t ts_lo = 0x80 | (ts & 0x7F);
    uint8_t pkt[2 + MIDI_MSG_MAX + 1];
    size_t n = 0;

    pkt[n++] = 0x80 | ((ts >> 7) & 0x3F);
    pkt[n++] = ts_lo;
    for (size_t i = 0; i < msg->len; i++) {
        if (i > 0 && msg->data[i] == 0xF7) {
            pkt[n++] = ts_lo;
        }
        pkt[n++] = msg->data[i];
    }

    int err = bt_gatt_notify(conn, MIDI_IO_ATTR, pkt, n);
    if (err) {
        LOG_DBG("MIDI notify failed: %d", err);
    }
}

static void send_work_cb(struct k_work *work) {
    struct midi_msg msg;

    while (k_msgq_get(&midi_msgq, &msg, K_NO_WAIT) == 0) {
        struct bt_conn *conn = subscribed_conn();
        if (conn == NULL) {
            k_msgq_purge(&midi_msgq);
            return;
        }
        send_packet(conn, &msg);
        bt_conn_unref(conn);
    }
}

static K_WORK_DEFINE(send_work, send_work_cb);

int zmk_ble_midi_send(const uint8_t *data, size_t len) {
    struct midi_msg msg;

    if (len == 0 || len > MIDI_MSG_MAX) {
        return -EINVAL;
    }
    if (!zmk_ble_midi_is_connected()) {
        return -ENOTCONN;
    }

    msg.len = len;
    memcpy(msg.data, data, len);
    int err = k_msgq_put(&midi_msgq, &msg, K_NO_WAIT);
    if (err) {
        LOG_WRN("MIDI queue full, dropping message");
        return err;
    }
    k_work_submit_to_queue(&midi_work_q, &send_work);
    return 0;
}

/* Discovery.
 *
 * MIDI apps on iPadOS (GarageBand: Settings > Advanced > Bluetooth MIDI
 * Devices; AUM; Loopy Pro) only list devices that are ADVERTISING the MIDI
 * service. Once the iPad is connected for typing, ZMK stops advertising, and
 * its own advertising packet only lists HID and Battery anyway. MIDI_PAIR
 * therefore advertises the MIDI service for a short while from the same
 * identity; the iPad already holds a link to that address, so "connecting" in
 * the MIDI app reuses the existing encrypted connection instead of opening a
 * new one.
 *
 * It only starts while the active profile is connected, which is exactly when
 * ZMK itself is not advertising. If that changes mid-window (the host drops,
 * or another profile is selected) this advertiser gets out of the way and ZMK
 * is asked to redo its own advertising.
 */

/* Not in a ZMK header, but a global function in app/src/ble.c. */
extern int update_advertising(void);

static const struct bt_data midi_ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_MIDI_SERVICE_VAL),
};

#define MIDI_ADV_PARAM                                                                             \
    BT_LE_ADV_PARAM(BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_ONE_TIME | BT_LE_ADV_OPT_USE_NAME,  \
                    BT_GAP_ADV_FAST_INT_MIN_2, BT_GAP_ADV_FAST_INT_MAX_2, NULL)

static bool pair_adv_active;

static void pair_adv_stop(void) {
    if (!pair_adv_active) {
        return;
    }
    pair_adv_active = false;
    int err = bt_le_adv_stop();
    if (err) {
        LOG_WRN("Failed to stop MIDI advertising: %d", err);
    }
    LOG_INF("MIDI advertising stopped");
}

static void pair_adv_stop_cb(struct k_work *work) { pair_adv_stop(); }

int zmk_ble_midi_pair(void) {
    if (!zmk_ble_active_profile_is_connected()) {
        /* ZMK is advertising for this profile already; connect it first. */
        LOG_WRN("MIDI pair: active profile not connected, pair the keyboard first");
        return -ENOTCONN;
    }

    if (!pair_adv_active) {
        int err = bt_le_adv_start(MIDI_ADV_PARAM, midi_ad, ARRAY_SIZE(midi_ad), NULL, 0);
        if (err) {
            LOG_ERR("Failed to start MIDI advertising: %d", err);
            return err;
        }
        pair_adv_active = true;
        LOG_INF("Advertising MIDI service");
    }
    k_work_reschedule(&pair_adv_stop_work, K_SECONDS(CONFIG_ZMK_BLE_MIDI_PAIR_ADV_SECONDS));
    return 0;
}

static void midi_connected(struct bt_conn *conn, uint8_t err) {
    struct bt_conn_info info;
    bt_conn_get_info(conn, &info);
    if (info.role == BT_CONN_ROLE_PERIPHERAL && pair_adv_active) {
        /* A one-time connectable advertiser stops itself on connection. */
        pair_adv_active = false;
        k_work_cancel_delayable(&pair_adv_stop_work);
    }
}

static void midi_disconnected(struct bt_conn *conn, uint8_t reason) {
    struct bt_conn_info info;
    bt_conn_get_info(conn, &info);
    if (info.role != BT_CONN_ROLE_PERIPHERAL) {
        return;
    }
    /* ZMK restarts its own advertising from a work item after a disconnect.
     * Get out of its way synchronously, before that runs. */
    k_work_cancel_delayable(&pair_adv_stop_work);
    pair_adv_stop();
    zmk_ble_midi_state_notify();
}

/* Selecting another profile makes ZMK try to advertise straight away, which
 * fails while this advertiser holds the radio. Hand it back and retry. */
static int midi_profile_changed_listener(const zmk_event_t *eh) {
    /* A different host may or may not be subscribed to MIDI. */
    zmk_ble_midi_state_notify();
    if (pair_adv_active) {
        k_work_cancel_delayable(&pair_adv_stop_work);
        pair_adv_stop();
        update_advertising();
    }
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(ble_midi, midi_profile_changed_listener);
ZMK_SUBSCRIPTION(ble_midi, zmk_ble_active_profile_changed);

BT_CONN_CB_DEFINE(midi_conn_callbacks) = {
    .connected = midi_connected,
    .disconnected = midi_disconnected,
};

static int ble_midi_init(void) {
    static const struct k_work_queue_config queue_config = {.name = "BLE MIDI Send Work"};
    k_work_queue_start(&midi_work_q, midi_q_stack, K_THREAD_STACK_SIZEOF(midi_q_stack),
                       CONFIG_ZMK_BLE_MIDI_THREAD_PRIORITY, &queue_config);
    return 0;
}

SYS_INIT(ble_midi_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
