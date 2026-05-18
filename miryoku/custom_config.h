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
//
// TIMELESS / POSITIONAL (bilateral) home-row mods are also enabled. Left-hand
// HRMs (u_mt_l) only engage if the next key is right-hand or thumb; right-hand
// HRMs (u_mt_r) only engage if next key is left-hand or thumb. Same-hand rolls
// can never misfire, so timing barely matters -> "timeless". Patched into the
// active BASE (Colemak-DH) and EXTRA (QWERTY) blocks via U_MT_L / U_MT_R.
// Position lists default in miryoku.h (derived from mapping/42/corne.h, valid
// for both Corne and Corne-ish Zen). Override here only if you change layout.
//
// TRADEOFF: same-hand modified keypresses (e.g. a Ctrl HRM + a same-hand key)
// no longer register the mod. Use the opposite-hand mirrored mod for those.
// ---------------------------------------------------------------------------

#define U_HRM_FLAVOR "balanced"
#define U_TAPPING_TERM 200
#define U_HRM_QUICK_TAP_MS 175
#define U_HRM_PRIOR_IDLE_MS 150

// ---------------------------------------------------------------------------
// macOS "British" (Apple) layout compatibility
//
// The host MacBook uses the Apple "British" input source on its built-in
// keyboard, and we don't want to switch input source when swapping keyboards.
// ZMK sends US-ANSI key codes; macOS-British reinterprets six of them, so the
// affected SYM/NUM keys were remapped (in miryoku_babel/
// miryoku_layer_alternatives.h, active non-flip SYM + NUM blocks) so the
// printed Miryoku guide stays correct WHILE macOS is set to British:
//
//   @  AT    -> LS(SQT)             (British: Shift+' = @)
//   #  HASH  -> LA(N3)              (British: Option+3 = #)
//   ~  TILDE -> LS(NON_US_BACKSLASH)   (Shift + ISO key left of Z)
//   `  GRAVE -> NON_US_BACKSLASH       (ISO key left of Z)
//   \  BSLH  -> NON_US_HASH         (ISO key left of Return)
//   |  PIPE  -> LS(NON_US_HASH)     (Shift + ISO key left of Return)
//
// @ and # are certain. The four ISO-key ones follow the standard Apple-British
// mapping; if any of ~ ` \ | is wrong after flashing, it's a one-line edit of
// that key in the SYM/NUM block (swap NON_US_BACKSLASH <-> NON_US_HASH for the
// backtick/backslash pair). All other symbols are identical between US-ANSI
// and Apple-British and were left untouched.
// ---------------------------------------------------------------------------
