# zmk-ble-midi

A ZMK module that adds a standard Bluetooth LE MIDI service to a keyboard,
plus key behaviors and a pointer-to-CC input processor. Written for ZMK v0.3.

- `src/ble_midi.c`: the BLE-MIDI GATT service, the send queue, and the
  `MIDI_PAIR` advertiser that lets iPadOS MIDI apps discover a keyboard that
  is already connected for typing.
- `src/behavior_midi.c`: `&midi_note`, `&midi_drum`, `&midi_chord`
  (diatonic chords in the current key and scale), `&midi_cc`, `&midi_cc_tog`
  and `&midi_ctl` (compatible `zmk,behavior-midi`, selected by `midi-type`).
- `src/midi_state.c`: the `zmk_midi_state_changed` event (octave, key,
  scale, velocity, channel, connection) for status displays, and the
  `zmk,ble-midi-layers` node that tells a display which layers are MIDI
  layers.
- `src/input_processor_midi_xy.c`: `&midi_xy <x cc> <y cc>`, which turns
  relative X/Y movement into two 0–127 CCs.

It builds only on the central half (`ZMK_BLE_MIDI` depends on
`!ZMK_SPLIT || ZMK_SPLIT_ROLE_CENTRAL`). Usage and the iPad setup steps are in
[`docs/MIDI.md`](../../docs/MIDI.md).
