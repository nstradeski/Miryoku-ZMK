// Copyright 2021 Manna Harbour
// https://github.com/manna-harbour/miryoku

// ---------------------------------------------------------------------------
// Home-row-mod tuning (custom)
//
// Stock Miryoku uses flavor "tap-preferred" + a 200ms tapping term, which
// forces you to hold a home-row key the FULL term before it acts as a
// modifier. That is the lag on capitals. Settings below fix it:
//
//   U_HRM_FLAVOR        "balanced": a mod resolves the instant you tap AND
//                       release another key while holding it -> capitals are
//                       immediate. ("hold-preferred" is even faster but
//                       misfires on same-hand rolls; "tap-preferred" is stock.)
//   U_TAPPING_TERM      Max ms a key can be held and still count as a tap.
//                       Lower = snappier mods, more accidental holds.
//   U_HRM_QUICK_TAP_MS  Tap, then within this window press+hold the same key
//                       -> auto-repeat the tap (e.g. hold to repeat a letter)
//                       instead of triggering the mod.
//   U_HRM_PRIOR_IDLE_MS If another key was pressed within this many ms, the
//                       home-row key is forced to TAP. This is the big one
//                       for fast typists: it kills accidental mods mid-word
//                       while leaving deliberate (paused) mod presses intact.
//
// Tuning guide: if you still get accidental mods while typing fast, RAISE
// U_HRM_PRIOR_IDLE_MS (try 175). If deliberate mods feel like they need too
// long a pause, LOWER it (try 125). Leave flavor at "balanced".
// ---------------------------------------------------------------------------

#define U_HRM_FLAVOR "balanced"
#define U_TAPPING_TERM 200
#define U_HRM_QUICK_TAP_MS 175
#define U_HRM_PRIOR_IDLE_MS 150
