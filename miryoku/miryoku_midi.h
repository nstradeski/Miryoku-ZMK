// MIDI layer (custom) -- Bluetooth MIDI controller for iPad / Mac.
//
// Used by keymaps that define MIRYOKU_MIDI and build the zmk-ble-midi module
// (modules/zmk-ble-midi). Reached from NAV: hold Space (NAV), double-tap the
// T position. The left outer thumb (Esc position) goes back to BASE.
//
// Unlike every other layer this one covers all 42 keys, including the outer
// pinky columns, so it bypasses the Corne mapping macro (which pins those
// columns to the Witch / Mouseless / WINDOW keys) and lists positions
// directly, row by row: 12 top, 12 home, 12 bottom, 6 thumbs.
//
// Notes follow GarageBand's Musical Typing: the home row is the white keys
// from C on the A position, and a sharp sits directly ABOVE its natural (key
// above = one semitone up; the gaps above E and B are empty). Octave keys
// shift everything; a held note always releases correctly.
//
//        outer  Q/A   W/S   E/D   R/F   T/G  |  Y/H   U/J   I/K   O/L   P/'  outer
//   top   --    C#    D#    --    F#    G#   |  A#    --    C#'   D#'   --   F#'
//   home  B,    C     D     E     F     G    |  A     B     C'    D'    E'   F'
//   bot  Panic Oct-  Oct+  Vel-  Vel+  Mod   | Tg20  Tg21  Tg22  Tg23  Tg24  Pair
//   thumbs            Base  Sustain CC25     | Rec   Play  Stop
//
//   C = middle C (MIDI 60; GarageBand calls it C3).
//   Mod     = mod wheel (CC 1) at full while held
//   Sustain = sustain pedal (CC 64) while held
//   CC25    = CC 25 at 127 while held, for MIDI Learn (e.g. momentary mute)
//   Tg20-24 = CC 20-24 toggle 127 / 0 per press, for MIDI Learn (mutes, FX)
//   Rec/Play/Stop = MIDI Machine Control, plus MIDI Start/Stop on Play/Stop
//   Pair    = advertise the MIDI service so the iPad's MIDI app can find the
//             keyboard (see docs/MIDI.md)
//
// While this layer is active the trackpad is an XY pad instead of a mouse:
// left/right drives CC 16, up/down drives CC 17 (see config/toucan.keymap).

#pragma once

#include <dt-bindings/zmk/midi.h>

#define U_MN(note, octave) &midi_note MIDI_N(note, octave)

#define MIRYOKU_LAYERMAPPING_MIDI(...) __VA_ARGS__
#define MIRYOKU_LAYER_MIDI \
U_NU            U_MN(MN_CS, 4)  U_MN(MN_DS, 4)  U_NU            U_MN(MN_FS, 4)  U_MN(MN_GS, 4)      U_MN(MN_AS, 4)  U_NU            U_MN(MN_CS, 5)  U_MN(MN_DS, 5)  U_NU            U_MN(MN_FS, 5) \
U_MN(MN_B, 3)   U_MN(MN_C, 4)   U_MN(MN_D, 4)   U_MN(MN_E, 4)   U_MN(MN_F, 4)   U_MN(MN_G, 4)       U_MN(MN_A, 4)   U_MN(MN_B, 4)   U_MN(MN_C, 5)   U_MN(MN_D, 5)   U_MN(MN_E, 5)   U_MN(MN_F, 5)  \
&midi_ctl MIDI_PANIC  &midi_ctl MIDI_OCT_DN  &midi_ctl MIDI_OCT_UP  &midi_ctl MIDI_VEL_DN  &midi_ctl MIDI_VEL_UP  &midi_cc 1 \
    &midi_cc_tog 20  &midi_cc_tog 21  &midi_cc_tog 22  &midi_cc_tog 23  &midi_cc_tog 24  &midi_ctl MIDI_PAIR \
                                &to U_BASE      &midi_cc 64     &midi_cc 25         &midi_ctl MIDI_REC  &midi_ctl MIDI_PLAY  &midi_ctl MIDI_STOP
