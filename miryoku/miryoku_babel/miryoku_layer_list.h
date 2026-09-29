// Copyright 2022 Manna Harbour
// https://github.com/manna-harbour/miryoku
// generated -*- buffer-read-only: t -*-

#pragma once

#if !defined (MIRYOKU_LAYER_LIST)

// MIDI layer (custom): only on keyboards whose keymap defines MIRYOKU_MIDI
// and builds the zmk-ble-midi module (see miryoku/miryoku_midi.h).
#if defined (MIRYOKU_MIDI)
#define U_MIRYOKU_LAYER_MIDI MIRYOKU_X(MIDI, "MIDI")
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
#define U_MIDI   11

#endif
