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
// Base alpha layout: QWERTY (was Colemak-DH)
//
// Inverts Miryoku's BASE and EXTRA. Stock Miryoku defaults to Colemak-DH on
// BASE with QWERTY on the toggled EXTRA layer; these flip that so QWERTY is the
// layer you type on and Colemak-DH lives on EXTRA. The TAP layer (the
// momentary no-home-row-mods version, for gaming etc.) is set to QWERTY too so
// it matches the new base.
//
// HRMs are preserved: the QWERTY BASE block already carries our timeless
// positional home-row mods (U_MT_L / U_MT_R) and bottom-row RALT, so typing on
// QWERTY keeps the same mod behavior. TAP_QWERTY is plain &kp (no mods) by
// design.
#define MIRYOKU_ALPHAS_QWERTY      // BASE  = QWERTY
#define MIRYOKU_EXTRA_COLEMAKDH    // EXTRA = Colemak-DH
#define MIRYOKU_TAP_QWERTY         // TAP   = QWERTY (no HRMs)
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Browser controls on the MEDIA layer (macOS)
//
// The MEDIA top row was RGB underglow control, inert on these boards (no
// underglow LEDs). Repurpose those 5 keys -- plus the external-power toggle
// directly below (also removed so it can't be pressed by accident) -- as
// browser controls. Right hand, reached by holding the left-thumb Esc/MEDIA
// key; the media transport (prev/vol/next) on the rest of the middle row stays:
//
//   top:  Back   Forward  Prev-tab  Next-tab  Refresh
//   mid:  AddrBar | Prev  Vol-  Vol+  Next
//
#define U_BRO_BACK    &kp LG(LBKT)     // Back          Cmd+[
#define U_BRO_FWD     &kp LG(RBKT)     // Forward       Cmd+]
#define U_BRO_TABP    &kp LC(LS(TAB))  // Previous tab  Ctrl+Shift+Tab
#define U_BRO_TABN    &kp LC(TAB)      // Next tab      Ctrl+Tab
#define U_BRO_RELOAD  &kp LG(R)        // Refresh       Cmd+R
#define U_BRO_URL     &kp LG(L)        // Address bar   Cmd+L
// ---------------------------------------------------------------------------

// Clipboard cluster (undo/cut/copy/paste/redo on NAV/MOUSE/BUTTON layers).
// Miryoku's default uses K_UNDO/LS(INS)/LC(INS)/LS(DEL)/K_AGAIN, which macOS
// ignores. MAC maps them to Cmd combos (Cmd+Z/X/C/V, Shift+Cmd+Z). Host is
// macOS, so use the Mac variant.
#define MIRYOKU_CLIPBOARD_MAC

// ---------------------------------------------------------------------------
// macOS "British" layout compatibility
//
// The host's active macOS input source ("British") was verified via Keyboard
// Viewer screenshots. Despite the name it behaves like US-ANSI for the symbols
// Miryoku uses (Shift+2=@, Shift+grave=~, grave=`, the \ key=\ /|, the ' key
// ='/") -- the ONLY difference is Shift+3 = GBP, not #; on this layout # is
// Option+3.
//
// Therefore stock Miryoku is correct for every symbol EXCEPT #. The SYM/NUM
// blocks are stock except one key:
//
//   #  HASH -> LA(N3)   (Option+3 = # on this layout; stock Shift+3 = GBP)
//
// Everything else (@ ~ ` \ | { } & * ( ) : $ % ^ + ! _ [ ] ; = - and digits)
// is upstream-stock and correct as-is. If the active input source is ever
// changed away from this "British", revert this one line to stock &kp HASH.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Outer-column app hotkeys (custom)
//
// The Corne / Corne-ish Zen are 42-key, but Miryoku only uses the inner 36
// keys; the outer pinky column on each half (6 keys: 3 rows x 2 halves) is
// blanked to &none on every layer by mapping/42/corne.h. We fill those slots
// as follows: the top and home rows are global launch hotkeys for two macOS
// keyboard-nav apps (each owns a row, with a DISTINCT key per half so every
// slot is an independent trigger), and the BOTTOM row is a bare HYPER modifier
// on both halves — hold it to chord window-management shortcuts (Amethyst etc.)
// with the other hand. The edits live in mapping/42/corne.h and apply to ALL
// layers (both keyboards), so they're reachable regardless of the held layer.
//
//   Layout (outer slots):
//     top    row:  [Witch L    ] ...base...  [Witch R    ]   (e.g. prev / next)
//     home   row:  [Mouseless L ] ...base...  [Mouseless R]
//     bottom row:  [HYPER (hold)] ...base...  [HYPER (hold)]  (window mgmt mod)
//
// The app keys (top/home) are sent as HYPER chords (Ctrl+Alt+Gui+Shift + an
// F-key). Rationale: a bare F13-F24 that an app fails to CONSUME passes through
// to the front app, and terminals interpret high F-keys as CSI escape sequences
// (the "leak" seen with Mouseless, whose activation only observes the key
// instead of swallowing it). A Hyper-modified key can't render as a stray
// terminal escape and is captured reliably. Bind each app's global shortcut(s)
// in its own settings to the matching Hyper+F-key below (F16-F19, all
// macOS-visible and unbound by default).
//
// The bottom-row U_HYPER_MOD keys hold Ctrl+Alt+Gui+Shift while pressed (they
// emit nothing on their own), so configure Amethyst / your WM to use the Hyper
// combo as its modifier and chord Hyper+<key> from either pinky.
//
// To retune: change an F-key, or swap U_HYPER(...) for a plain &kp Fnn to go
// back to bare F-keys. Each app's left/right keys can do whatever two actions
// you assign them in-app (Witch is naturally prev/next).
// ---------------------------------------------------------------------------

#define U_HYPER(key)  LC(LA(LG(LS(key))))   // Ctrl+Alt+Gui+Shift (modified keycode)
#define U_HYPER_MOD   &kp LC(LA(LG(LSHIFT)))  // hold = Hyper modifier (no keycode)

#define U_WITCH_L      &kp U_HYPER(F16)      // top-left    outer  (Witch:     e.g. prev)
#define U_WITCH_R      &kp U_HYPER(F17)      // top-right   outer  (Witch:     e.g. next)
#define U_MOUSELESS_L  &kp U_HYPER(F18)      // home-left   outer  (Mouseless: left half)
#define U_MOUSELESS_R  &kp U_HYPER(F19)      // home-right  outer  (Mouseless: right half)
// bottom row -> U_HYPER_MOD on both halves (defined above; window-mgmt modifier)
// ---------------------------------------------------------------------------
