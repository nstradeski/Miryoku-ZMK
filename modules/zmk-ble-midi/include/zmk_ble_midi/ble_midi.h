/*
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Queue one MIDI message (status byte first; a SysEx message is F0 .. F7) to
 * be sent to the active BLE host. Messages are dropped, not queued, while no
 * host is subscribed to the MIDI characteristic. */
int zmk_ble_midi_send(const uint8_t *msg, size_t len);

/* True when the active profile's host has subscribed to MIDI notifications. */
bool zmk_ble_midi_is_connected(void);

/* Advertise the MIDI service UUID for CONFIG_ZMK_BLE_MIDI_PAIR_ADV_SECONDS so
 * a MIDI app's Bluetooth panel can discover the (already connected) keyboard. */
int zmk_ble_midi_pair(void);

/* Current MIDI channel, 0-15. Shared by all behaviors and the trackpad. */
uint8_t zmk_ble_midi_channel(void);
void zmk_ble_midi_set_channel(uint8_t channel);

/* Note-on velocity used by every note key, 1-127. Setting it updates the
 * status display. */
uint8_t zmk_ble_midi_velocity(void);
void zmk_ble_midi_set_velocity(int velocity);

static inline int zmk_ble_midi_send_cc(uint8_t cc, uint8_t value) {
    const uint8_t msg[] = {0xB0 | zmk_ble_midi_channel(), cc & 0x7F, value & 0x7F};
    return zmk_ble_midi_send(msg, sizeof(msg));
}
