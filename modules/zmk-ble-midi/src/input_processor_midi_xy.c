/*
 * Trackpad as a MIDI controller. Put it in an input-listener layer override:
 *
 *   one finger       X and Y drive two CCs (param1 / param2), 0-127, like
 *                    a pair of knobs: an XY pad for effects
 *   two fingers up/down
 *                    the velocity slider: sets the note velocity used by
 *                    every note key (shown on the status screen)
 *   pinch            a third CC (pinch-cc), 0-127
 *
 * The trackpad driver already tells these apart: one finger arrives as
 * REL_X/REL_Y, a two-finger scroll as REL_WHEEL/REL_HWHEEL and a pinch as
 * REL_MISC. So the three can never disturb each other.
 *
 * Every event it sees is neutralized, so the cursor, scrolling, zoom and
 * clicks are off while that layer is active.
 *
 * Returning ZMK_INPUT_PROC_STOP is not enough on its own: in ZMK v0.3 a layer
 * override without process-next discards the processor's STOP and hands the
 * event to the listener anyway. So the events are also emptied in place.
 *
 *   &midi_xy <cc for X> <cc for Y>
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_input_processor_midi_xy

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/input/input.h>
#include <zephyr/logging/log.h>
#include <drivers/input_processor.h>

#include <zmk_ble_midi/ble_midi.h>

LOG_MODULE_DECLARE(zmk_ble_midi, CONFIG_ZMK_LOG_LEVEL);

struct midi_xy_config {
    int32_t divisor;
    int32_t velocity_divisor;
    bool velocity_invert;
    int32_t pinch_cc;
    int32_t pinch_divisor;
};

struct midi_xy_axis {
    int32_t remainder;
    uint8_t value;
    uint8_t sent;
    uint8_t cc;
};

struct midi_xy_data {
    struct midi_xy_axis x, y, pinch;
    int32_t velocity_remainder;
    uint8_t swallowed_buttons;
    struct k_work_delayable send_work;
};

/* Sends from a work item rather than per event, so a fast swipe becomes at
 * most one CC per axis every CONFIG_ZMK_BLE_MIDI_XY_INTERVAL_MS. */
static void midi_xy_send(struct k_work *work) {
    struct k_work_delayable *dwork = k_work_delayable_from_work(work);
    struct midi_xy_data *data = CONTAINER_OF(dwork, struct midi_xy_data, send_work);

    struct midi_xy_axis *axes[] = {&data->x, &data->y, &data->pinch};
    for (int i = 0; i < ARRAY_SIZE(axes); i++) {
        struct midi_xy_axis *a = axes[i];
        if (a->value != a->sent && a->cc <= 127) {
            a->sent = a->value;
            zmk_ble_midi_send_cc(a->cc, a->value);
        }
    }
}

/* Turns movement into whole steps, keeping the leftover for next time so slow
 * movement still adds up. */
static int32_t take_steps(int32_t *remainder, int32_t delta, int32_t divisor) {
    int32_t total = *remainder + delta;
    int32_t steps = total / divisor;
    *remainder = total - steps * divisor;
    return steps;
}

static void move_axis(struct midi_xy_axis *a, int32_t delta, int32_t divisor) {
    a->value = CLAMP((int32_t)a->value + take_steps(&a->remainder, delta, divisor), 0, 127);
}

static int midi_xy_handle_event(const struct device *dev, struct input_event *event,
                                uint32_t param1, uint32_t param2,
                                struct zmk_input_processor_state *state) {
    const struct midi_xy_config *cfg = dev->config;
    struct midi_xy_data *data = dev->data;

    if (event->type == INPUT_EV_REL &&
        (event->code == INPUT_REL_X || event->code == INPUT_REL_Y)) {
        data->x.cc = param1 & 0x7F;
        data->y.cc = param2 & 0x7F;
        if (event->code == INPUT_REL_X) {
            move_axis(&data->x, event->value, cfg->divisor);
        } else {
            /* Screen Y grows downwards; a knob should go up when you swipe up. */
            move_axis(&data->y, -event->value, cfg->divisor);
        }
        k_work_schedule(&data->send_work, K_MSEC(CONFIG_ZMK_BLE_MIDI_XY_INTERVAL_MS));
    } else if (event->type == INPUT_EV_REL && event->code == INPUT_REL_WHEEL) {
        /* Positive wheel = fingers moving up (the Toucan sets invert-scroll-y
         * for natural scrolling). Up = louder. */
        int32_t delta = cfg->velocity_invert ? -event->value : event->value;
        int32_t steps = take_steps(&data->velocity_remainder, delta, cfg->velocity_divisor);
        if (steps) {
            zmk_ble_midi_set_velocity(zmk_ble_midi_velocity() + steps);
        }
    } else if (event->type == INPUT_EV_REL && event->code == INPUT_REL_MISC) {
        data->pinch.cc = cfg->pinch_cc < 0 ? 0xFF : cfg->pinch_cc & 0x7F;
        move_axis(&data->pinch, event->value, cfg->pinch_divisor);
        k_work_schedule(&data->send_work, K_MSEC(CONFIG_ZMK_BLE_MIDI_XY_INTERVAL_MS));
    }

    if (event->type == INPUT_EV_REL) {
        event->value = 0;
    } else if (event->type == INPUT_EV_KEY && event->code >= INPUT_BTN_0 &&
               event->code <= INPUT_BTN_4) {
        /* Swallow taps/clicks. A release is only swallowed if its press was,
         * so a button pressed before entering the layer cannot get stuck. */
        uint8_t bit = BIT(event->code - INPUT_BTN_0);
        bool swallow = event->value > 0 || (data->swallowed_buttons & bit);
        if (swallow) {
            WRITE_BIT(data->swallowed_buttons, event->code - INPUT_BTN_0, event->value > 0);
            event->code = INPUT_BTN_TOUCH; /* a code the listener ignores */
        }
    }

    return ZMK_INPUT_PROC_STOP;
}

static int midi_xy_init(const struct device *dev) {
    struct midi_xy_data *data = dev->data;
    data->x.value = data->x.sent = 64;
    data->y.value = data->y.sent = 64;
    data->pinch.value = data->pinch.sent = 64;
    data->pinch.cc = 0xFF; /* set on first pinch */
    k_work_init_delayable(&data->send_work, midi_xy_send);
    return 0;
}

static const struct zmk_input_processor_driver_api midi_xy_driver_api = {
    .handle_event = midi_xy_handle_event,
};

#define MIDI_XY_INST(n)                                                                            \
    static struct midi_xy_data midi_xy_data_##n;                                                   \
    static const struct midi_xy_config midi_xy_config_##n = {                                      \
        .divisor = MAX(DT_INST_PROP(n, divisor), 1),                                               \
        .velocity_divisor = MAX(DT_INST_PROP(n, velocity_divisor), 1),                             \
        .velocity_invert = DT_INST_PROP(n, velocity_invert),                                       \
        .pinch_cc = DT_INST_PROP(n, pinch_cc),                                                     \
        .pinch_divisor = MAX(DT_INST_PROP(n, pinch_divisor), 1),                                   \
    };                                                                                             \
    DEVICE_DT_INST_DEFINE(n, midi_xy_init, NULL, &midi_xy_data_##n, &midi_xy_config_##n,           \
                          POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &midi_xy_driver_api);

DT_INST_FOREACH_STATUS_OKAY(MIDI_XY_INST)
