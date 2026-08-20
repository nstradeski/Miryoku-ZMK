// Copyright 2022 Manna Harbour
// https://github.com/manna-harbour/miryoku

#pragma once

// Cursor speed knobs (custom):
//   MOVE_VAL  = top cursor velocity (max px/s-ish). Raise for a faster ceiling.
//   MOVE_TIME = time-to-max-speed (ms): how long a held direction ramps from 0
//               to MOVE_VAL. LOWER = snappier; short taps actually move. This is
//               the main "feels slow" knob -- stock 1500ms means taps crawl.
//   EXPONENT  = accel curve (0 constant, 1 linear, 2 quadratic). 1 = proportional.
// Retune: too twitchy -> lower MOVE_VAL or raise MOVE_TIME; still too slow ->
// raise MOVE_VAL or lower MOVE_TIME further.
#define ZMK_POINTING_DEFAULT_MOVE_VAL 2000
#define ZMK_POINTING_DEFAULT_SCRL_VAL 100

#define U_MOUSE_MOVE_EXPONENT 1
#define U_MOUSE_MOVE_TIME 600
#define U_MOUSE_MOVE_DELAY 0
#define U_MOUSE_SCROLL_EXPONENT 1
#define U_MOUSE_SCROLL_TIME 5000
#define U_MOUSE_SCROLL_DELAY 0

#define U_BTN1 &mkp MB1
#define U_BTN2 &mkp MB2
#define U_BTN3 &mkp MB3

#define U_MS_D &mmv MOVE_DOWN
#define U_MS_L &mmv MOVE_LEFT
#define U_MS_R &mmv MOVE_RIGHT
#define U_MS_U &mmv MOVE_UP
#define U_WH_D &msc SCRL_DOWN
#define U_WH_L &msc SCRL_LEFT
#define U_WH_R &msc SCRL_RIGHT
#define U_WH_U &msc SCRL_UP
