// Copyright 2022 Manna Harbour
// https://github.com/manna-harbour/miryoku
// generated -*- buffer-read-only: t -*-

#pragma once

#if !defined (MIRYOKU_LAYER_LIST)

// MIDI layers (custom): only on keyboards whose keymap defines MIRYOKU_MIDI
// and builds the zmk-ble-midi module (see miryoku/miryoku_midi.h). Display
// names are kept to five characters so they fit the Toucan's status screen.
#if defined (MIRYOKU_MIDI)
#define U_MIRYOKU_LAYER_MIDI \
MIRYOKU_X(MIDI_PIANO, "Piano") \
MIRYOKU_X(MIDI_GRID,  "Grid") \
MIRYOKU_X(MIDI_DRUMS, "Drums") \
MIRYOKU_X(MIDI_CHORD, "Chord") \
MIRYOKU_X(MIDI_CTRL,  "Ctrl") \
MIRYOKU_X(MIDI_DJ,    "DJ") \
MIRYOKU_X(MIDI_MODE,  "Mode")
#else
#define U_MIRYOKU_LAYER_MIDI
#endif

#define MIRYOKU_LAYER_LIST \
MIRYOKU_X(BASE,   "Base") \
MIRYOKU_X(EXTRA,  "Extra") \
MIRYOKU_X(TAP,    "Tap") \
MIRYOKU_X(BUTTON, "Button") \
MIRYOKU_X(NAV,    "Nav") \
MIRYOKU_X(MOUSE,  "Mouse") \
MIRYOKU_X(MEDIA,  "Media") \
MIRYOKU_X(NUM,    "Num") \
MIRYOKU_X(SYM,    "Sym") \
MIRYOKU_X(FUN,    "Fun") \
MIRYOKU_X(WINDOW, "Window") \
U_MIRYOKU_LAYER_MIDI

#define U_BASE   0
#define U_EXTRA  1
#define U_TAP    2
#define U_BUTTON 3
#define U_NAV    4
#define U_MOUSE  5
#define U_MEDIA  6
#define U_NUM    7
#define U_SYM    8
#define U_FUN    9
#define U_WINDOW 10
#define U_MIDI_PIANO 11
#define U_MIDI_GRID  12
#define U_MIDI_DRUMS 13
#define U_MIDI_CHORD 14
#define U_MIDI_CTRL  15
#define U_MIDI_DJ    16
#define U_MIDI_MODE  17

#endif
