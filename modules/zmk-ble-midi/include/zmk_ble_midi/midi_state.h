/*
 * MIDI performance state (octave, key, velocity...) and the event raised
 * whenever it changes, e.g. for a status display.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <zmk/event_manager.h>

enum zmk_midi_scale {
    ZMK_MIDI_SCALE_MAJOR,
    ZMK_MIDI_SCALE_MINOR,
};

struct zmk_midi_state_changed {
    int8_t shift;      /* left hand's shift in half octaves, -8..+8 (2 = one octave) */
    int8_t shift_r;    /* right hand's shift in half octaves */
    int8_t transpose;  /* key, in semitones from C: -11..+11 */
    uint8_t velocity;  /* 1..127 */
    uint8_t channel;   /* 0..15 (shown as 1..16) */
    uint8_t scale;     /* enum zmk_midi_scale */
    uint8_t inversion; /* 0 root, 1 first, 2 second */
    uint8_t scratch_deck; /* 1 or 2: the deck the jog and crossfader cut act on */
    bool connected;    /* a MIDI app is subscribed */
};

ZMK_EVENT_DECLARE(zmk_midi_state_changed);

/* Snapshot of the current state. */
struct zmk_midi_state_changed zmk_ble_midi_state(void);

/* Raise zmk_midi_state_changed with the current state (from a work item, so
 * it is safe to call from any thread). */
void zmk_ble_midi_state_notify(void);

/* True if the keymap marked this layer as a MIDI layer (the zmk,ble-midi-layers
 * node). A display uses this to decide when to show the MIDI status. */
bool zmk_ble_midi_layer_is_midi(uint8_t layer);

/* True if the keymap marked this layer as a scratch layer (scratch-layers on
 * the zmk,ble-midi-layers node): the display shows the scratch deck there. */
bool zmk_ble_midi_layer_is_scratch(uint8_t layer);

/* True if the keymap marked this layer as using half-octave shifts
 * (half-layers on the zmk,ble-midi-layers node); elsewhere the display shows
 * whole octaves only, matching what the keys play there. */
bool zmk_ble_midi_layer_uses_half(uint8_t layer);

/* "C", "C#", ... "B" for a pitch class 0-11. */
const char *zmk_ble_midi_key_name(int transpose);
