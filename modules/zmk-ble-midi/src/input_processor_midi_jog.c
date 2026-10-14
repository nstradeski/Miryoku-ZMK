/*
 * Trackpad as a DJ jog wheel / platter for scratching. Put it in an
 * input-listener layer override:
 *
 *   finger down   "platter touch" note on for the scratch deck: in DJ apps
 *                 this grabs the record (vinyl / scratch mode)
 *   slide         relative jog CC for the scratch deck: left/right moves
 *                 the record backward/forward
 *   finger up     touch note off: the record plays on
 *
 * The scratch deck (1 or 2) is chosen with &midi_ctl MIDI_SCRATCH_D1/D2.
 *
 * Unlike the XY pad, movement is not rate-limited: every report is queued at
 * once (summed only while the previous one is still waiting to be sent), so
 * scratches are as tight as Bluetooth allows. Like the XY pad, every event is
 * neutralized so the cursor, scrolling and clicks are off on this layer.
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_input_processor_midi_jog

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/input/input.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/atomic.h>
#include <drivers/input_processor.h>

#include <zmk_ble_midi/ble_midi.h>

LOG_MODULE_DECLARE(zmk_ble_midi, CONFIG_ZMK_LOG_LEVEL);

struct midi_jog_config {
    uint8_t channel; /* 0-15 */
    uint8_t jog_cc[2];
    uint8_t touch_note[2];
    int32_t divisor;
    bool invert;
    bool twos_complement;
};

struct midi_jog_data {
    const struct midi_jog_config *cfg;
    atomic_t pending; /* jog steps not yet sent */
    int32_t remainder;
    uint8_t touched_deck; /* 0 = not touching, else the deck the touch went to */
    uint8_t swallowed_buttons;
    struct k_work send_work;
};

/* Relative encodings DJ apps understand for jog wheels:
 *   offset 64 (default): 64 + delta, so 65 = one step forward, 63 = back
 *   two's complement:    1..63 forward, 127..65 back (127 = -1) */
static uint8_t encode(const struct midi_jog_config *cfg, int32_t delta) {
    if (cfg->twos_complement) {
        return delta >= 0 ? delta : 128 + delta;
    }
    return 64 + delta;
}

static void midi_jog_send(struct k_work *work) {
    struct midi_jog_data *data = CONTAINER_OF(work, struct midi_jog_data, send_work);
    const struct midi_jog_config *cfg = data->cfg;
    int32_t steps = atomic_set(&data->pending, 0);
    uint8_t cc = cfg->jog_cc[zmk_ble_midi_scratch_deck() - 1];

    /* One message carries at most +-63 steps. */
    while (steps != 0) {
        int32_t chunk = CLAMP(steps, -63, 63);
        zmk_ble_midi_send_cc_on(cfg->channel, cc, encode(cfg, chunk));
        steps -= chunk;
    }
}

static void send_touch(const struct midi_jog_config *cfg, uint8_t deck, bool on) {
    const uint8_t msg[] = {(on ? 0x90 : 0x80) | cfg->channel, cfg->touch_note[deck - 1],
                           on ? 127 : 0};
    zmk_ble_midi_send(msg, sizeof(msg));
}

static int midi_jog_handle_event(const struct device *dev, struct input_event *event,
                                 uint32_t param1, uint32_t param2,
                                 struct zmk_input_processor_state *state) {
    const struct midi_jog_config *cfg = dev->config;
    struct midi_jog_data *data = dev->data;

    if (event->type == INPUT_EV_REL && event->code == INPUT_REL_X) {
        int32_t delta = cfg->invert ? -event->value : event->value;
        int32_t total = data->remainder + delta;
        int32_t steps = total / cfg->divisor;
        data->remainder = total - steps * cfg->divisor;
        if (steps) {
            atomic_add(&data->pending, steps);
            k_work_submit(&data->send_work);
        }
    } else if (event->type == INPUT_EV_KEY && event->code == INPUT_BTN_TOUCH) {
        if (event->value && !data->touched_deck) {
            data->touched_deck = zmk_ble_midi_scratch_deck();
            data->remainder = 0;
            send_touch(cfg, data->touched_deck, true);
        } else if (!event->value && data->touched_deck) {
            /* Flush movement still waiting to be sent first, so nothing
             * moves the record after it was let go. Then release the deck
             * that was grabbed, even if the scratch deck changed mid-touch. */
            midi_jog_send(&data->send_work);
            send_touch(cfg, data->touched_deck, false);
            data->touched_deck = 0;
        }
    }

    if (event->type == INPUT_EV_REL) {
        event->value = 0;
    } else if (event->type == INPUT_EV_KEY && event->code >= INPUT_BTN_0 &&
               event->code <= INPUT_BTN_4) {
        /* Swallow taps/clicks; a release only if its press was swallowed. */
        uint8_t bit = BIT(event->code - INPUT_BTN_0);
        if (event->value > 0 || (data->swallowed_buttons & bit)) {
            WRITE_BIT(data->swallowed_buttons, event->code - INPUT_BTN_0, event->value > 0);
            event->code = INPUT_BTN_TOUCH; /* a code the listener ignores */
        }
    }

    return ZMK_INPUT_PROC_STOP;
}

static int midi_jog_init(const struct device *dev) {
    struct midi_jog_data *data = dev->data;
    data->cfg = dev->config;
    k_work_init(&data->send_work, midi_jog_send);
    return 0;
}

static const struct zmk_input_processor_driver_api midi_jog_driver_api = {
    .handle_event = midi_jog_handle_event,
};

#define MIDI_JOG_INST(n)                                                                           \
    static struct midi_jog_data midi_jog_data_##n;                                                 \
    static const struct midi_jog_config midi_jog_config_##n = {                                    \
        .channel = (DT_INST_PROP(n, channel) - 1) & 0x0F,                                          \
        .jog_cc = DT_INST_PROP(n, jog_cc),                                                         \
        .touch_note = DT_INST_PROP(n, touch_note),                                                 \
        .divisor = MAX(DT_INST_PROP(n, divisor), 1),                                               \
        .invert = DT_INST_PROP(n, invert),                                                         \
        .twos_complement = DT_INST_ENUM_IDX(n, encoding) == 1,                                     \
    };                                                                                             \
    DEVICE_DT_INST_DEFINE(n, midi_jog_init, NULL, &midi_jog_data_##n, &midi_jog_config_##n,        \
                          POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &midi_jog_driver_api);

DT_INST_FOREACH_STATUS_OKAY(MIDI_JOG_INST)
