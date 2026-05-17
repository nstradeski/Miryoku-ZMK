// Copyright 2022 Manna Harbour
// https://github.com/manna-harbour/miryoku

#pragma once

#include "miryoku_babel/miryoku_layer_selection.h"
#include "miryoku_babel/miryoku_layer_list.h"

#define U_MACRO_VA_ARGS(macro, ...) macro(__VA_ARGS__)
#define U_STRINGIFY(x) #x
#define U_MACRO(name,...) \
/ { \
  macros { \
    name: name { \
      compatible = "zmk,behavior-macro"; \
      #binding-cells = <0>; \
      __VA_ARGS__ \
    }; \
  }; \
};

#define U_NP &none // key is not present
#define U_NA &none // present but not available for use
#define U_NU &none // available but not used

#ifndef U_TAPPING_TERM
#define U_TAPPING_TERM 200
#endif

// Home-row-mod tuning, overridable from custom_config.h.
#ifndef U_HRM_FLAVOR
#define U_HRM_FLAVOR "balanced"
#endif
#ifndef U_HRM_QUICK_TAP_MS
#define U_HRM_QUICK_TAP_MS 175
#endif
#ifndef U_HRM_PRIOR_IDLE_MS
#define U_HRM_PRIOR_IDLE_MS 150
#endif

// Positional ("timeless"/bilateral) home-row mods. A left-hand HRM only
// engages if the NEXT key is on the right hand or a thumb, and vice versa,
// so same-hand rolls can never misfire. Position indices are the keymap
// binding offsets from miryoku/mapping/42/corne.h (identical for Corne and
// Corne-ish Zen). Left keys -> trigger on RIGHT+THUMB positions; right keys
// -> trigger on LEFT+THUMB positions.
#ifndef U_HRM_LEFT_TRIGGER_POSITIONS
#define U_HRM_LEFT_TRIGGER_POSITIONS 6 7 8 9 10 18 19 20 21 22 30 31 32 33 34 36 37 38 39 40 41
#endif
#ifndef U_HRM_RIGHT_TRIGGER_POSITIONS
#define U_HRM_RIGHT_TRIGGER_POSITIONS 1 2 3 4 5 13 14 15 16 17 25 26 27 28 29 36 37 38 39 40 41
#endif

#include "miryoku_clipboard.h"

#include "miryoku_double_tap_guard.h"

#include "miryoku_shift_functions.h"

#include "miryoku_mousekeys.h"

#if defined (MIRYOKU_KLUDGE_TAPDELAY)
  #include "miryoku_kludge_tapdelay.h"
#else
  #include "miryoku_behaviors.h"
#endif
