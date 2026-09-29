/*
 * The zmk_midi_state_changed event, and which keymap layers are MIDI layers.
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>

#include <zmk/event_manager.h>
#include <zmk_ble_midi/midi_state.h>

ZMK_EVENT_IMPL(zmk_midi_state_changed);

static void notify_work_cb(struct k_work *work) {
    raise_zmk_midi_state_changed(zmk_ble_midi_state());
}

static K_WORK_DEFINE(notify_work, notify_work_cb);

void zmk_ble_midi_state_notify(void) { k_work_submit(&notify_work); }

#define MIDI_LAYERS_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(zmk_ble_midi_layers)

bool zmk_ble_midi_layer_is_midi(uint8_t layer) {
#if DT_NODE_EXISTS(MIDI_LAYERS_NODE)
    static const uint8_t midi_layers[] = DT_PROP(MIDI_LAYERS_NODE, layers);
    for (int i = 0; i < ARRAY_SIZE(midi_layers); i++) {
        if (midi_layers[i] == layer) {
            return true;
        }
    }
#endif
    return false;
}

const char *zmk_ble_midi_key_name(int transpose) {
    static const char *const names[] = {"C",  "C#", "D",  "D#", "E",  "F",
                                        "F#", "G",  "G#", "A",  "A#", "B"};
    return names[((transpose % 12) + 12) % 12];
}
