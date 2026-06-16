# Miryoku ZMK — Custom Build (Timeless Home-Row Mods)

Personal Miryoku ZMK setup for two keyboards, tuned to fix home-row-mod lag
(especially on capitals) and made misfire-resistant with timeless / bilateral
positional home-row mods.

- **Typeractive Corne Wireless** — `nice!nano v2` controller + stock `corne` shield + nice!view displays
- **Corne-ish Zen v2** — onboard controller (GB R3)

Both are 6-column 42-key splits and share the same Miryoku 42-key mapping.

---

## 1. File locations

### Repos / working dirs

| Path | What |
|---|---|
| `/Users/nstradeski/Projects/miryoku` | Miryoku ZMK (this repo). Branch: `miryoku-hrm-tuning` |
| `/Users/nstradeski/Projects/zmk-build` | Local ZMK build workspace |
| `/Users/nstradeski/Projects/zmk-build/zmk` | ZMK source + Zephyr west workspace (topdir) |
| `/Users/nstradeski/Projects/zmk-build/.venv` | Python venv with `west` + Zephyr deps |
| `/Users/nstradeski/Projects/zmk-build/arm/arm-gnu-toolchain-14.2.rel1-darwin-x86_64-arm-none-eabi` | ARM GCC toolchain (user-local, no sudo) |
| `/Users/nstradeski/Projects/zmk-build/build-all.sh` | Build script for all four images |
| `/Users/nstradeski/Projects/zmk-build/firmware/` | Output `.uf2` files |

### Customized Miryoku source files

| File | Change |
|---|---|
| `miryoku/custom_config.h` | All tunable knobs + documentation (the file you edit to retune) |
| `miryoku/miryoku.h` | `#ifndef`-guarded defaults: tapping term, flavor, quick-tap, prior-idle, positional trigger position lists |
| `miryoku/miryoku_behaviors.h` | `U_MT_L` / `U_MT_R` macros (left/right positional) |
| `miryoku/miryoku_behaviors.dtsi` | `u_mt`, `u_lt` retuned; new `u_mt_l` / `u_mt_r` positional behaviors |
| `miryoku/miryoku_babel/miryoku_layer_alternatives.h` | `U_MT` → `U_MT_L`/`U_MT_R` positional HRMs in both the **QWERTY** and **Colemak-DH** base blocks (BASE is now QWERTY, EXTRA is Colemak-DH — see `custom_config.h`) |
| `miryoku/mapping/42/corne.h` | All 6 outer pinky keys (were `&none`): top = Witch (`U_WITCH_L`/`_R`), home = Mouseless (`U_MOUSELESS_L`/`_R`), bottom = bare Hyper modifier (`U_HYPER_MOD`, both halves) for window mgmt — on **all layers**, both keyboards |
| `config/corne.keymap` | Stock Miryoku Corne keymap (used for Typeractive Corne) |
| `config/corneish_zen.keymap` | Stock Miryoku Corne-ish Zen keymap |

### Tunable knobs (in `miryoku/custom_config.h`)

```c
#define U_HRM_FLAVOR "balanced"     // mod resolves on tap+release of next key
#define U_TAPPING_TERM 200          // ms; max hold still counted as a tap
#define U_HRM_QUICK_TAP_MS 175      // tap-then-hold repeats the tap
#define U_HRM_PRIOR_IDLE_MS 150     // recent keypress -> force tap (anti-misfire)
```

Position lists for positional HRMs default in `miryoku/miryoku.h`
(`U_HRM_LEFT_TRIGGER_POSITIONS` / `U_HRM_RIGHT_TRIGGER_POSITIONS`); override in
`custom_config.h` only if the physical layout changes.

---

## 2. Build instructions

### One-shot: rebuild everything

```sh
/Users/nstradeski/Projects/zmk-build/build-all.sh
```

Produces all four `.uf2` files in `/Users/nstradeski/Projects/zmk-build/firmware/`.
First build compiles all of Zephyr (~several min); ccache makes reruns fast.

### Tune-and-rebuild loop

1. Edit a value in `/Users/nstradeski/Projects/miryoku/miryoku/custom_config.h`
2. Run `build-all.sh`
3. Reflash (section 3)

### What the build does (per board)

`west build` is invoked with the local toolchain and explicit ZMK board roots:

```sh
export ZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb
export GNUARMEMB_TOOLCHAIN_PATH=/Users/nstradeski/Projects/zmk-build/arm/arm-gnu-toolchain-14.2.rel1-darwin-x86_64-arm-none-eabi
cd /Users/nstradeski/Projects/zmk-build/zmk
../.venv/bin/west build -p -s app -d build/<name> -b <BOARD> -- \
  -DZMK_CONFIG=/Users/nstradeski/Projects/miryoku/config \
  -DBOARD_ROOT=/Users/nstradeski/Projects/zmk-build/zmk/app/module \
  -DSHIELD_ROOT=/Users/nstradeski/Projects/zmk-build/zmk/app \
  -DDTS_ROOT=/Users/nstradeski/Projects/zmk-build/zmk/app/module \
  [-DSHIELD="<SHIELD>"]
```

| Image | BOARD | SHIELD |
|---|---|---|
| `corne_left` | `nice_nano_v2` | `corne_left nice_view_adapter nice_view` |
| `corne_right` | `nice_nano_v2` | `corne_right nice_view_adapter nice_view` |
| `corneish_zen_v2_left` | `corneish_zen_v2_left` | *(none — onboard)* |
| `corneish_zen_v2_right` | `corneish_zen_v2_right` | *(none — onboard)* |

> Because ZMK is pinned to **v0.3.0**, boards use the flat pre-HWMv2 names
> (`nice_nano_v2`, `corneish_zen_v2_left/right`) — the `board/soc/variant`
> form (`nice_nano/nrf52840/zmk`) only exists on newer ZMK main and is
> rejected by v0.3.0. These boards live in ZMK's default `app/boards`, so no
> board roots are required.

### Fresh environment setup (only if rebuilding the toolchain from scratch)

```sh
brew install cmake ninja gperf ccache qemu dtc libusb wget
python3 -m venv /Users/nstradeski/Projects/zmk-build/.venv
/Users/nstradeski/Projects/zmk-build/.venv/bin/pip install --upgrade pip west
# Pin to the v0.3.0 release, NOT main: ZMK main has a deep-sleep regression that
# drains the split central half (zmkfirmware/zmk#3207). v0.3.0 keeps HWMv2 board
# names so the build args below are unchanged. To move off the pin later, drop
# `--branch v0.3.0` (and re-check #3207 is fixed first).
cd /Users/nstradeski/Projects/zmk-build && git clone --branch v0.3.0 https://github.com/zmkfirmware/zmk.git
cd zmk && ../.venv/bin/west init -l app && ../.venv/bin/west update && ../.venv/bin/west zephyr-export
../.venv/bin/pip install -r zephyr/scripts/requirements.txt
# ARM toolchain (no sudo): download + extract the macOS x86_64 tarball
#   arm-gnu-toolchain-14.2.rel1-darwin-x86_64-arm-none-eabi.tar.xz
#   into /Users/nstradeski/Projects/zmk-build/arm/
```
> Do **not** use `brew install --cask gcc-arm-embedded` here — its `.pkg`
> installer needs an interactive sudo password and will fail. Use the tarball.

---

## 3. Flashing instructions (macOS)

Both keyboards use a UF2 bootloader over USB. **Each half is flashed
separately with its own image.**

> **macOS warning — drag-and-drop / `cp` is broken here.** On recent macOS,
> copying a `.uf2` to these nRF UF2 bootloader volumes via Finder, `cp`, or
> `cat` **silently fails** (Finder error **-36**, or `Device not configured`,
> or a fake instant "success" that doesn't actually write). You **must** use
> the raw `dd` methods below. (On Windows/Linux, plain drag-and-drop works
> fine — if you have access to one, that's the easy path.)
>
> The two keyboards need **opposite** `dd` methods (different bootloader ages).

### Which half must be flashed

The split's **left half is the central**: it holds the keymap and runs *all*
behaviour for keys on **both** hands. The right half is only a peripheral
(reports key positions). So for any keymap/behaviour change, **only the left
half must be reflashed**; flashing the right half is hygiene only (keeps both
on the same build) and never affects behaviour. The Corne-ish Zen is a
separate keyboard — flash it only if you use it.

### Finding the device node (both keyboards)

Double-tap the half's **reset** button to enter the bootloader, then:

```sh
diskutil info /Volumes/NICENANO    | grep "Device Node"   # Typeractive Corne
diskutil info /Volumes/CORNEISHZEN | grep "Device Node"   # Corne-ish Zen v2
```

Sanity check: the keyboard is always the small **~33 MB external, removable**
disk. Your Mac's drive is the ~500 GB internal `disk0`/`disk1` — never target
that. Below, replace `N` with the number from the command above (e.g. `disk4`
→ use `rdisk4` / `disk4`).

### Typeractive Corne Wireless (nice!nano v2) — raw device, while mounted

Images: `corne_left.uf2` / `corne_right.uf2`. Volume: `NICENANO`.

1. Double-tap reset on the half → `NICENANO` mounts.
2. Get the node (above), then write to the **raw** device (`rdiskN`):
   ```sh
   sudo dd if=firmware/corne_left.uf2 of=/dev/rdiskN bs=1m
   ```
3. A **real write takes ~10–15 s** and prints `8xxxxx bytes transferred`.
   ~1–2 s = it did **not** write — retry. The board then resets.

### Corne-ish Zen v2 — buffered device, unmount first

Images: `corneish_zen_v2_left.uf2` / `corneish_zen_v2_right.uf2`. Volume:
`CORNEISHZEN`. Its older bootloader needs the **opposite** of the Corne —
unmount first and write the **buffered** device (`diskN`, *not* `rdiskN`):

```sh
diskutil unmountDisk /dev/diskN && sudo dd if=firmware/corneish_zen_v2_left.uf2 of=/dev/diskN bs=1m
```

`dd` may report only ~2 s / full byte count (buffered — deceptive). Verify it
actually landed (see below) rather than trusting the time.

### Verifying a flash actually took

The bootloader exposes `CURRENT.UF2` (a readback of the chip). Re-enter the
bootloader after flashing and byte-compare it to the source image — this is
definitive, independent of the misleading `dd` timing/errors:

```sh
diskutil mountDisk /dev/diskN                       # Zen only (Corne stays mounted)
cp /Volumes/<VOL>/CURRENT.UF2 /tmp/readback.uf2
python3 - <<'EOF'
import struct
def blocks(fn):
    d=open(fn,'rb').read(); o={}
    for i in range(0,len(d),512):
        b=d[i:i+512]
        if len(b)<512: break
        m0,m1,fl,a,pl,bn,nb,fm=struct.unpack('<IIIIIIII',b[:32])
        if m0==0x0A324655 and m1==0x9E5D5157: o[a]=b[32:32+pl]
    return o
s=blocks('firmware/corne_left.uf2'); r=blocks('/tmp/readback.uf2')
c=set(s)&set(r); m=sum(1 for a in c if s[a]==r[a])
print("MATCH" if s and m==len(s)==len(c) else "DIFFERS — not flashed")
EOF
```

### After flashing

- Single-tap reset (or replug) so the half boots the firmware instead of
  staying in the bootloader. Halves re-pair to each other automatically.
- If a half behaves oddly after a major change, flash a ZMK `settings_reset`
  image once, then reflash the keymap image.

---

## 4. Summary of what was done

### Problem
Stock Miryoku felt laggy, especially capitals. Root cause: Miryoku's home-row
mods use `flavor = "tap-preferred"` with a 200 ms term, so a home-row Shift
only registers as Shift after the **full 200 ms hold** — a mandatory stall on
every capital.

### Changes

1. **Flavor fix** — switched home-row behaviors to `flavor = "balanced"`: the
   mod resolves the instant the next key is tapped and released, so capitals
   are immediate.

2. **Anti-misfire timing** — added `require-prior-idle-ms` (150) and
   `quick-tap-ms` (175): a key pressed right after typing is forced to a tap,
   killing accidental mods during fast rolls.

3. **Timeless / bilateral positional HRMs** — added two behaviors:
   - `u_mt_l` (left-hand mods): hold engages **only if the next key is on the
     right hand or a thumb**
   - `u_mt_r` (right-hand mods): hold engages **only if the next key is on the
     left hand or a thumb**

   Plus `hold-trigger-on-release` so multi-mod chords still work. Same-hand
   rolls can no longer misfire, so the result is robust across typing speeds
   ("timeless" — it doesn't depend on getting the term exactly right).

4. **Wiring** — Miryoku uses one generic `&u_mt` for both hands, so the active
   layers were patched to route each side to the correct behavior
   (`U_MT_L` / `U_MT_R`), in the **Colemak-DH base** layer and the
   **QWERTY extra** layer (plus their bottom-row `RALT` mods). Position indices
   were derived from `mapping/42/corne.h` and are correct for both keyboards.

5. **Tunability** — every timing value and the trigger-position lists are
   `#ifndef`-guarded and overridable from `miryoku/custom_config.h`, so
   retuning is a one-number edit + rebuild, and Miryoku upstream updates won't
   silently clobber the settings.

6. **QWERTY layout — kept stock.** An earlier change relocated `'`/`P` to make
   QWERTY's apostrophe match the Colemak-DH guide position, but the non-standard
   `P` placement hurt QWERTY muscle memory more than it helped, so it was
   reverted: `BASE_QWERTY`/`TAP_QWERTY` are back to upstream (`P` top-right, `'`
   on the home-right pinky). (As of the base swap below QWERTY is the BASE
   layer, so this applies to your everyday layer.)

7. **macOS "British" compatibility (single-key remap).** The host's active
   macOS input source ("British") was verified by Keyboard Viewer: it behaves
   like US-ANSI for every Miryoku symbol *except* `#` (its Shift+3 is `£`; `#`
   is on Option+3). So stock Miryoku is already correct for everything except
   `#`. The only non-stock symbol keycode is `SYM #` → `LA(N3)` (Option+3);
   `NUM`/`SYM` are otherwise upstream-stock. Earlier multi-key remap attempts
   were reverted once the real layout was identified. See `custom_config.h`.

8. **Mouse layer enabled** — Miryoku's MOUSE layer uses ZMK pointing
   behaviors (`&mmv`/`&msc`/`&mkp`), gated behind `CONFIG_ZMK_POINTING`
   (off by default; the local build only read ZMK's stock `prj.conf`). Added
   `config/pointing.conf` (`CONFIG_ZMK_POINTING=y` + smooth scrolling), wired
   into `build-all.sh` via `-DEXTRA_CONF_FILE` for all four images. Mouse is
   processed on the central (left) half.

9. **Clipboard cluster fixed for macOS** — undo/cut/copy/paste/redo on the
   NAV/MOUSE/BUTTON layers defaulted to `K_UNDO`/`LS(INS)`/`LC(INS)`/
   `LS(DEL)`/`K_AGAIN`, which macOS ignores. Set `#define MIRYOKU_CLIPBOARD_MAC`
   in `custom_config.h` → these become `⌘Z`/`⌘X`/`⌘C`/`⌘V`/`⇧⌘Z`.

10. **Local build environment** — set up west + Zephyr + a sudo-free ARM
   toolchain, resolved HWMv2 board naming and ZMK board-root registration, and
   built all four images. Committed on branch `miryoku-hrm-tuning`.

11. **Outer-column app hotkeys + Hyper modifier.** The 42-key Corne/Zen leave
   the outer pinky column (6 keys: 3 rows × 2 halves) as `&none` on every
   layer — Miryoku only uses the inner 36 keys. We fill all six, on **all
   layers** so they're reachable regardless of the held layer:
   - top row: `U_WITCH_L` (F16) / `U_WITCH_R` (F17) — e.g. Witch prev / next
   - home row: `U_MOUSELESS_L` (F18) / `U_MOUSELESS_R` (F19) — Mouseless, L/R
   - bottom row: `U_HYPER_MOD` on **both halves** — a bare Hyper modifier

   The four app keys (top/home) are **Hyper chords** (`U_HYPER(key)` = `⌃⌥⌘⇧` +
   an F-key, F16–F19), with a distinct key per half so each is an independent
   trigger; bind each app's global shortcut(s) to the matching Hyper+F-key in
   its own settings. Rationale: a bare F13–F24 that an app fails to **consume**
   falls through to the front app, and terminals interpret high F-keys as CSI
   escape sequences (the Mouseless "leak" — its activation only observed the key
   instead of swallowing it). A Hyper-modified key can't render as a stray
   terminal escape and is captured reliably. F16–F19 are macOS-visible and
   unbound by default.

   The bottom-row `U_HYPER_MOD` (`&kp LC(LA(LG(LSHIFT)))`) **holds**
   Ctrl+Alt+Gui+Shift while pressed and emits nothing on its own — use it as the
   modifier for a tiling WM (Amethyst etc.): set the WM's mod to the Hyper combo
   and chord Hyper+`<key>` with the other hand. It's on both halves so either
   pinky works. The outer positions are excluded from the HRM trigger lists, so
   none of this affects home-row mods. Retune in `custom_config.h`.

12. **ZMK pinned to v0.3.0 (battery-drain fix).** Both the CI clone and the
   local-setup clone now use `--branch v0.3.0` instead of tracking `main`. ZMK
   `main` has a deep-sleep regression where a split's **central half fails to
   wake and drains the battery** (zmkfirmware/zmk#3207), not present in v0.3;
   it was hitting both keyboards (worst on the Zen, central + display). v0.3.0
   is the latest tagged release (Aug 2025), predates the regression, and keeps
   the HWMv2 board names, so no build-arg changes were needed. Bonus: builds
   are now reproducible. To move off the pin, drop `--branch v0.3.0` once #3207
   is confirmed fixed.

13. **Base layout swapped to QWERTY.** `custom_config.h` now sets
   `MIRYOKU_ALPHAS_QWERTY` + `MIRYOKU_EXTRA_COLEMAKDH` + `MIRYOKU_TAP_QWERTY`,
   so BASE is QWERTY (everyday typing), EXTRA is Colemak-DH, and the no-HRM TAP
   layer matches QWERTY. The QWERTY base block already carries the timeless
   positional home-row mods (`U_MT_L`/`U_MT_R` + bottom-row `RALT`), so mod
   behavior is unchanged; `TAP_QWERTY` stays plain `&kp`.

14. **Browser controls on the MEDIA layer.** The MEDIA top row was RGB
   underglow control (`U_RGB_*`), inert on these boards (no underglow LEDs).
   Repurposed those 5 keys, plus the external-power toggle (`U_EP_TOG`) directly
   below — also removed so it can't be hit by accident — as macOS browser
   controls (right hand; hold the left-thumb Esc/MEDIA key):

   ```
   top:  Back(⌘[)  Forward(⌘])  Prev-tab(⌃⇧Tab)  Next-tab(⌃Tab)  Refresh(⌘R)
   mid:  Address-bar(⌘L) | Prev  Vol-  Vol+  Next   (media transport kept)
   ```

   Keycodes are editable `#define`s in `custom_config.h` (`U_BRO_*`), wired into
   the non-flip `MIRYOKU_ALTERNATIVES_MEDIA` block in
   `miryoku_babel/miryoku_layer_alternatives.h`. The bottom-row Bluetooth/output
   keys are untouched.

### Known tradeoff
Positional HRMs mean **same-hand modified keypresses no longer register the
mod** (e.g. a left-hand Ctrl HRM + another left-hand key). Use the mirrored
mod on the opposite hand for those shortcuts. This is the intended behavior
that makes misfires ~zero.

### Tuning guide
- Still getting accidental mods at speed → raise `U_HRM_PRIOR_IDLE_MS` (try 175)
- Deliberate mods need too long a pause → lower it (try 125)
- Keep `U_HRM_FLAVOR` at `"balanced"`
- Edit in `custom_config.h`, run `build-all.sh`, reflash
