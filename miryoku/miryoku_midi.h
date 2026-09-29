// MIDI layers (custom) -- Bluetooth MIDI controller for iPad / Mac.
//
// Used by keymaps that define MIRYOKU_MIDI and build the zmk-ble-midi module
// (modules/zmk-ble-midi). Full guide: docs/MIDI.md.
//
//   Enter / leave:  press both bottom outer pinky keys together (a combo in
//                   config/toucan.keymap). Entering always lands on Piano.
//   Switch mode:    hold the right outer thumb (MODE) and tap a top-row key
//                   (Q Piano, W Grid, E Drums, R Chord, T Ctrl, Y DJ).
//
// The MIDI layers cover all 42 keys, including the outer pinky columns, so
// they bypass the Corne mapping macro (which pins those columns to the Witch
// / Mouseless / WINDOW keys) and list positions directly, row by row:
// 12 top, 12 home, 12 bottom, 6 thumbs.
//
// Every playing mode shares the same thumbs:
//   Oct-  Sustain  Oct+  |  Mod  Hold  MODE      (Drums: drum sounds instead)
//   Mod  = mod wheel (CC 1) while held; Hold = CC 85 while held (MIDI Learn)
//
// The trackpad is an XY pad on every MIDI layer (CC 16 / CC 17).

#pragma once

#include <dt-bindings/zmk/midi.h>

#define U_MN(note, octave) &midi_note MIDI_N(note, octave)
#define U_MIDI_MODE_MO &mo U_MIDI_MODE

#define U_MIDI_PLAY_THUMBS \
&midi_ctl MIDI_OCT_DN  &midi_cc 64  &midi_ctl MIDI_OCT_UP      &midi_cc 1  &midi_cc 85  U_MIDI_MODE_MO

// ---------------------------------------------------------------------------
// PIANO -- GarageBand Musical Typing, plus a lower octave of white keys.
//
// The home row is the white keys with middle C on the A key; each sharp sits
// directly ABOVE its natural (key above = one semitone up; above E and B is
// empty). The bottom row repeats the home row an octave lower (key below =
// octave down), mostly for left-hand bass. 32 keys, B2..F#5.
//
//        outer  Q/A   W/S   E/D   R/F   T/G  |  Y/H   U/J   I/K   O/L   P/'  outer
//   top   --    C#4   D#4   --    F#4   G#4  |  A#4   --    C#5   D#5   --    F#5
//   home  B3    C4    D4    E4    F4    G4   |  A4    B4    C5    D5    E5    F5
//   bot   B2    C3    D3    E3    F3    G3   |  A3    B3    C4    D4    E4    F4
//
// C4 is middle C (MIDI 60); GarageBand calls it C3.
#define MIRYOKU_LAYERMAPPING_MIDI_PIANO(...) __VA_ARGS__
#define MIRYOKU_LAYER_MIDI_PIANO \
U_NU            U_MN(MN_CS, 4)  U_MN(MN_DS, 4)  U_NU            U_MN(MN_FS, 4)  U_MN(MN_GS, 4)      U_MN(MN_AS, 4)  U_NU            U_MN(MN_CS, 5)  U_MN(MN_DS, 5)  U_NU            U_MN(MN_FS, 5) \
U_MN(MN_B, 3)   U_MN(MN_C, 4)   U_MN(MN_D, 4)   U_MN(MN_E, 4)   U_MN(MN_F, 4)   U_MN(MN_G, 4)       U_MN(MN_A, 4)   U_MN(MN_B, 4)   U_MN(MN_C, 5)   U_MN(MN_D, 5)   U_MN(MN_E, 5)   U_MN(MN_F, 5)  \
U_MN(MN_B, 2)   U_MN(MN_C, 3)   U_MN(MN_D, 3)   U_MN(MN_E, 3)   U_MN(MN_F, 3)   U_MN(MN_G, 3)       U_MN(MN_A, 3)   U_MN(MN_B, 3)   U_MN(MN_C, 4)   U_MN(MN_D, 4)   U_MN(MN_E, 4)   U_MN(MN_F, 4)  \
U_MIDI_PLAY_THUMBS

// ---------------------------------------------------------------------------
// GRID -- one octave per row, chromatic left to right. 36 notes, C3..B5,
// every semitone exactly once. Key above = octave up.
//
//        col:   1     2     3     4     5     6   |  7     8     9     10    11    12
//   top         C5    C#5   D5    D#5   E5    F5  |  F#5   G5    G#5   A5    A#5   B5
//   home        C4    C#4   D4    D#4   E4    F4  |  F#4   G4    G#4   A4    A#4   B4
//   bot         C3    C#3   D3    D#3   E3    F3  |  F#3   G3    G#3   A3    A#3   B3
#define U_MIDI_GRID_ROW(o) \
U_MN(MN_C, o)   U_MN(MN_CS, o)  U_MN(MN_D, o)   U_MN(MN_DS, o)  U_MN(MN_E, o)   U_MN(MN_F, o)       U_MN(MN_FS, o)  U_MN(MN_G, o)   U_MN(MN_GS, o)  U_MN(MN_A, o)   U_MN(MN_AS, o)  U_MN(MN_B, o)
#define MIRYOKU_LAYERMAPPING_MIDI_GRID(...) __VA_ARGS__
#define MIRYOKU_LAYER_MIDI_GRID \
U_MIDI_GRID_ROW(5) \
U_MIDI_GRID_ROW(4) \
U_MIDI_GRID_ROW(3) \
U_MIDI_PLAY_THUMBS

// ---------------------------------------------------------------------------
// DRUMS -- General MIDI drum kit. Fixed notes: octave and key don't move them.
// Kick and snare are on the thumbs as well as under the index fingers.
//
//        outer   Q/A    W/S    E/D    R/F    T/G   |  Y/H    U/J    I/K    O/L    P/'    outer
//   top  Crash2 Splash OpenHH Crash  HiTom  HMTom  |  LMTom  LoTom  Ride   Bell   China  Ride2
//   home Tamb   Stick  HiHat  Snare  Kick   Clap   |  Kick   Snare  FlrHi  FlrLo  Cowbl  Maraca
//   bot  Claves PedHH  Snare2 BongoH BongoL CongaO |  CongaM CongaL TimbH  TimbL  Tri    Cabasa
//   thumbs            HiHat  Kick   PedHH          |  Snare  Clap   MODE
#define U_MD(n) &midi_drum GM_##n
#define MIRYOKU_LAYERMAPPING_MIDI_DRUMS(...) __VA_ARGS__
#define MIRYOKU_LAYER_MIDI_DRUMS \
U_MD(CRASH2)    U_MD(SPLASH)    U_MD(HIHAT_OPEN)  U_MD(CRASH)     U_MD(TOM_HI)    U_MD(TOM_HIMID)       U_MD(TOM_LOMID)       U_MD(TOM_LO)      U_MD(RIDE)          U_MD(RIDE_BELL)     U_MD(CHINA)       U_MD(RIDE2)   \
U_MD(TAMBOURINE) U_MD(SIDESTICK) U_MD(HIHAT_CLOSED) U_MD(SNARE)   U_MD(KICK)      U_MD(CLAP)            U_MD(KICK)            U_MD(SNARE)       U_MD(FLOOR_TOM_HI)  U_MD(FLOOR_TOM_LO)  U_MD(COWBELL)     U_MD(MARACAS) \
U_MD(CLAVES)    U_MD(HIHAT_PEDAL) U_MD(SNARE2)    U_MD(BONGO_HI)  U_MD(BONGO_LO)  U_MD(CONGA_HI_OPEN)   U_MD(CONGA_HI_MUTE)   U_MD(CONGA_LO)    U_MD(TIMBALE_HI)    U_MD(TIMBALE_LO)    U_MD(TRIANGLE_OPEN) U_MD(CABASA) \
U_MD(HIHAT_CLOSED)  U_MD(KICK)  U_MD(HIHAT_PEDAL)     U_MD(SNARE)  U_MD(CLAP)  U_MIDI_MODE_MO

// ---------------------------------------------------------------------------
// CHORD -- one key, one chord, always in the current key and scale.
//
// Columns are scale degrees I..VII then I..III an octave up. Top row = 7th
// chords, home row = triads, bottom row = just the root an octave lower (a
// bass note for the left hand). In C major: C Dm Em F G Am Bdim C Dm Em.
// Change key and every chord follows, so the same shapes work in any key.
//
//        outer  Q/A   W/S   E/D   R/F   T/G  |  Y/H   U/J   I/K   O/L   P/'  outer
//   top   Key-  I7    ii7   iii7  IV7   V7   |  vi7   vii7  I7'   ii7'  iii7' Key+
//   home  Maj/m I     ii    iii   IV    V    |  vi    vii   I'    ii'   iii'  Inv
//   bot   Panic I     ii    iii   IV    V    |  vi    vii   I'    ii'   iii'  Reset   (bass)
//
//   Key-/Key+  move the key a semitone (display shows it, e.g. "G MAJ")
//   Maj/m      toggle major / natural minor
//   Inv        cycle root position -> 1st -> 2nd inversion
//   Reset      back to C major, root position, octave 0
#define U_MC(flags) &midi_chord (flags)
#define U_MIDI_CHORD_ROW(f) \
U_MC(MC_I | f)  U_MC(MC_II | f) U_MC(MC_III | f) U_MC(MC_IV | f) U_MC(MC_V | f)     U_MC(MC_VI | f) U_MC(MC_VII | f) U_MC(MC_I | MC_8VA | f) U_MC(MC_II | MC_8VA | f) U_MC(MC_III | MC_8VA | f)
#define MIRYOKU_LAYERMAPPING_MIDI_CHORD(...) __VA_ARGS__
#define MIRYOKU_LAYER_MIDI_CHORD \
&midi_ctl MIDI_KEY_DN   U_MIDI_CHORD_ROW(MC_7TH)   &midi_ctl MIDI_KEY_UP \
&midi_ctl MIDI_SCALE    U_MIDI_CHORD_ROW(0)        &midi_ctl MIDI_INV    \
&midi_ctl MIDI_PANIC    U_MIDI_CHORD_ROW(MC_BASS)  &midi_ctl MIDI_KEY_RST \
U_MIDI_PLAY_THUMBS

// ---------------------------------------------------------------------------
// CTRL -- a MIDI Learn control surface for AUM / Loopy Pro / Logic.
//
//        outer  Q/A   W/S   E/D   R/F   T/G  |  Y/H   U/J   I/K   O/L   P/'  outer
//   top   PC-   T20   T21   T22   T23   T24  |  T25   T26   T27   T28   T29   PC+
//   home  Ch-   M102  M103  M104  M105  M106 |  M107  M108  M109  M110  M111  Ch+
//   bot   Panic Rec   Play  Stop  Cont  Vel- |  Vel+  Oct-  Oct+  Key-  Key+  Pair
//   thumbs            Rec   Play  Stop       |  --    --    MODE
//
//   T = CC toggle (127 / 0 per press): mutes, FX on/off, loop tracks
//   M = CC 127 while held: clip / scene launch, momentary actions
//   PC = program change (previous / next patch), Ch = MIDI channel
#define U_MCT(cc) &midi_cc_tog cc
#define U_MCM(cc) &midi_cc cc
#define MIRYOKU_LAYERMAPPING_MIDI_CTRL(...) __VA_ARGS__
#define MIRYOKU_LAYER_MIDI_CTRL \
&midi_ctl MIDI_PC_DN  U_MCT(20)  U_MCT(21)  U_MCT(22)  U_MCT(23)  U_MCT(24)      U_MCT(25)  U_MCT(26)  U_MCT(27)  U_MCT(28)  U_MCT(29)  &midi_ctl MIDI_PC_UP \
&midi_ctl MIDI_CH_DN  U_MCM(102) U_MCM(103) U_MCM(104) U_MCM(105) U_MCM(106)     U_MCM(107) U_MCM(108) U_MCM(109) U_MCM(110) U_MCM(111) &midi_ctl MIDI_CH_UP \
&midi_ctl MIDI_PANIC  &midi_ctl MIDI_REC  &midi_ctl MIDI_PLAY  &midi_ctl MIDI_STOP  &midi_ctl MIDI_CONT  &midi_ctl MIDI_VEL_DN \
    &midi_ctl MIDI_VEL_UP  &midi_ctl MIDI_OCT_DN  &midi_ctl MIDI_OCT_UP  &midi_ctl MIDI_KEY_DN  &midi_ctl MIDI_KEY_UP  &midi_ctl MIDI_PAIR \
&midi_ctl MIDI_REC  &midi_ctl MIDI_PLAY  &midi_ctl MIDI_STOP     U_NU  U_NU  U_MIDI_MODE_MO

// ---------------------------------------------------------------------------
// DJ -- two decks for Algoriddim djay (or any DJ app with MIDI Learn).
//
// Left hand = deck 1, right hand = deck 2, as mirror images: every function
// is under the same finger on both hands (Play under the index finger, hot
// cue 1 under the pinky). Each key sends its own note on MIDI channel 16, so
// djay's MIDI Learn can map it and the other modes (channel 1) never trigger
// djay by accident. Hold-to-act keys (Cue, Nudge) send note off on release.
//
//   per hand, pinky -> index:
//   top     Load   HC1    HC2    HC3    HC4    FX1
//   home    PFL    Loop/2 Loop*2 Loop   Play   Cue
//   bottom  Sync   LoopIn LoopOut Nudge- Nudge+ FX2
//   thumbs  Browse up  Browse down  Spare1 | Spare2  Spare3  MODE
//
//   PFL = headphone cue. Loop = auto loop on/off. HC = hot cue.
//   Trackpad: X = crossfader (CC 1), Y = CC 2, two fingers = CC 3,
//   pinch = CC 4, all on channel 16 (see config/toucan.keymap).
//
// Note numbers (channel 16): deck 1 = 0 + function, deck 2 = 32 + function,
// thumbs 64-68. Nothing depends on them; djay learns whatever it receives.
#define U_DJ_LOAD 0
#define U_DJ_HC1 1
#define U_DJ_HC2 2
#define U_DJ_HC3 3
#define U_DJ_HC4 4
#define U_DJ_FX1 5
#define U_DJ_PFL 6
#define U_DJ_LOOP_HALF 7
#define U_DJ_LOOP_DOUBLE 8
#define U_DJ_LOOP 9
#define U_DJ_PLAY 10
#define U_DJ_CUE 11
#define U_DJ_SYNC 12
#define U_DJ_LOOP_IN 13
#define U_DJ_LOOP_OUT 14
#define U_DJ_NUDGE_DN 15
#define U_DJ_NUDGE_UP 16
#define U_DJ_FX2 17
#define U_DJ_CH 16
#define U_DJ(deck, fn) &midi_drum MIDI_ON_CH(U_DJ_CH, ((deck) - 1) * 32 + U_DJ_##fn)
#define U_DJ_THUMB(n) &midi_drum MIDI_ON_CH(U_DJ_CH, 64 + (n))
// One row per hand: a..f from pinky to index. Deck 1 reads left to right,
// deck 2 is the mirror image, so it is listed index-first.
#define U_DJ_ROW(a, b, c, d, e, f) \
U_DJ(1, a) U_DJ(1, b) U_DJ(1, c) U_DJ(1, d) U_DJ(1, e) U_DJ(1, f) \
U_DJ(2, f) U_DJ(2, e) U_DJ(2, d) U_DJ(2, c) U_DJ(2, b) U_DJ(2, a)
#define MIRYOKU_LAYERMAPPING_MIDI_DJ(...) __VA_ARGS__
#define MIRYOKU_LAYER_MIDI_DJ \
U_DJ_ROW(LOAD, HC1, HC2, HC3, HC4, FX1) \
U_DJ_ROW(PFL, LOOP_HALF, LOOP_DOUBLE, LOOP, PLAY, CUE) \
U_DJ_ROW(SYNC, LOOP_IN, LOOP_OUT, NUDGE_DN, NUDGE_UP, FX2) \
U_DJ_THUMB(0)  U_DJ_THUMB(1)  U_DJ_THUMB(2)      U_DJ_THUMB(3)  U_DJ_THUMB(4)  U_MIDI_MODE_MO

// ---------------------------------------------------------------------------
// MODE -- held with the right outer thumb from any MIDI layer.
//
//        outer  Q/A    W/S    E/D    R/F    T/G   |  Y/H    U/J   I/K   O/L   P/'  outer
//   top   --    Piano  Grid   Drums  Chord  Ctrl  |  DJ     --    --    --    --    --
//   home  --    Vel-   Vel+   Ch-    Ch+    OctR  |  KeyR   --    --    --    --    --
//   bot   Panic --     --     --     --     --    |  --     --    --    --    --    Pair
//   thumbs             Exit   --     --           |  --     --    (held)
#define MIRYOKU_LAYERMAPPING_MIDI_MODE(...) __VA_ARGS__
#define MIRYOKU_LAYER_MIDI_MODE \
U_NU  &to U_MIDI_PIANO  &to U_MIDI_GRID  &to U_MIDI_DRUMS  &to U_MIDI_CHORD  &to U_MIDI_CTRL      &to U_MIDI_DJ  U_NU  U_NU  U_NU  U_NU  U_NU \
U_NU  &midi_ctl MIDI_VEL_DN  &midi_ctl MIDI_VEL_UP  &midi_ctl MIDI_CH_DN  &midi_ctl MIDI_CH_UP  &midi_ctl MIDI_OCT_RST \
      &midi_ctl MIDI_KEY_RST  U_NU  U_NU  U_NU  U_NU  U_NU \
&midi_ctl MIDI_PANIC  U_NU  U_NU  U_NU  U_NU  U_NU      U_NU  U_NU  U_NU  U_NU  U_NU  &midi_ctl MIDI_PAIR \
&to U_BASE  U_NU  U_NU      U_NU  U_NU  U_NU
