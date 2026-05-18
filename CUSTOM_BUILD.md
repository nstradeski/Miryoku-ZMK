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
| `miryoku/miryoku_babel/miryoku_layer_alternatives.h` | `U_MT` → `U_MT_L`/`U_MT_R` in the active **Colemak-DH (BASE)** and **QWERTY (EXTRA)** blocks |
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
| `corne_left` | `nice_nano/nrf52840/zmk` | `corne_left nice_view_adapter nice_view` |
| `corne_right` | `nice_nano/nrf52840/zmk` | `corne_right nice_view_adapter nice_view` |
| `corneish_zen_v2_left` | `corneish_zen_left/nrf52840/zmk` | *(none — onboard)* |
| `corneish_zen_v2_right` | `corneish_zen_right/nrf52840/zmk` | *(none — onboard)* |

> HWMv2 board default revision is `2.0.0` = the v2 hardware (nice!nano v2,
> Corne-ish Zen v2). Board roots must be passed explicitly — ZMK's
> `app/module` (base boards) and `app` (variants/shields) are not
> auto-registered in this workspace.

### Fresh environment setup (only if rebuilding the toolchain from scratch)

```sh
brew install cmake ninja gperf ccache qemu dtc libusb wget
python3 -m venv /Users/nstradeski/Projects/zmk-build/.venv
/Users/nstradeski/Projects/zmk-build/.venv/bin/pip install --upgrade pip west
cd /Users/nstradeski/Projects/zmk-build && git clone https://github.com/zmkfirmware/zmk.git
cd zmk && ../.venv/bin/west init -l app && ../.venv/bin/west update && ../.venv/bin/west zephyr-export
../.venv/bin/pip install -r zephyr/scripts/requirements.txt
# ARM toolchain (no sudo): download + extract the macOS x86_64 tarball
#   arm-gnu-toolchain-14.2.rel1-darwin-x86_64-arm-none-eabi.tar.xz
#   into /Users/nstradeski/Projects/zmk-build/arm/
```
> Do **not** use `brew install --cask gcc-arm-embedded` here — its `.pkg`
> installer needs an interactive sudo password and will fail. Use the tarball.

---

## 3. Flashing instructions

Both keyboards use UF2 bootloader flashing over USB. **Each half is flashed
separately** with its own image. Order doesn't matter; flash all four.

### Typeractive Corne Wireless (nice!nano v2)

For each half (`corne_left.uf2` to the left half, `corne_right.uf2` to the right):

1. Plug the half into USB.
2. Double-tap the **reset button** on the nice!nano (quick double press).
3. It mounts as a USB drive named `NICENANO`.
4. Drag/copy the matching `.uf2` onto that drive. It will flash and reboot
   automatically (the drive disappears).
5. Repeat for the other half.

### Corne-ish Zen v2

For each half (`corneish_zen_v2_left.uf2` / `corneish_zen_v2_right.uf2`):

1. Plug the half into USB.
2. Double-tap the **reset button** on that half.
3. It mounts as a USB drive (Zen bootloader volume).
4. Drag/copy the matching `.uf2` onto it; it flashes and reboots.
5. Repeat for the other half.

### After flashing

- Pair/connect over Bluetooth as usual; the split halves re-pair to each other
  automatically.
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

6. **Extra-layer (QWERTY) alignment** — Miryoku's stock QWERTY puts the
   apostrophe `'` on the home-right pinky and `P` on the top-right, whereas the
   Colemak-DH base (what the printed guide shows) has `'` on the top-right
   pinky. Every other non-letter key (`,` `.` `/`, thumb layer-taps, home-row
   mod positions) was already identical. The active `BASE_QWERTY` and the
   `TAP_QWERTY` fallback were patched so `'` sits in the Colemak-DH position
   and `P` takes the freed home-pinky slot — so the extra layer now matches the
   base layer in every position except the letters themselves, and the guide
   photo is valid in both modes.

7. **macOS "British" (Apple) compatibility** — the host MacBook stays on the
   Apple "British" input source (shared with its built-in keyboard). ZMK emits
   US-ANSI key codes; six of them render wrong under British. The active
   SYM/NUM blocks were remapped so the printed guide stays correct without
   switching input source: `@`→`Shift+'`, `#`→`Option+3`, and `~ ` ` ` \ |`
   onto the two ISO keys (left of Z / left of Return). Every other symbol is
   identical between US-ANSI and Apple-British and is unchanged. Details and
   the one-line fix for any mis-mapped ISO key are in `custom_config.h`.

8. **Local build environment** — set up west + Zephyr + a sudo-free ARM
   toolchain, resolved HWMv2 board naming and ZMK board-root registration, and
   built all four images. Committed on branch `miryoku-hrm-tuning`.

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
