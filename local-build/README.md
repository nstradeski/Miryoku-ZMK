# Local build tooling

Scripts to build this customized Miryoku locally (no GitHub Actions) for the
Typeractive Corne Wireless (nice!nano v2) and Corne-ish Zen v2.

- `build-all.sh` — builds all four `.uf2` images. Paths are env-overridable
  (`ZMK_DIR`, `WEST`, `ARM_TOOLCHAIN`, `OUT`, `MIRYOKU_DIR`); defaults match a
  `~/Projects/zmk-build` layout. Hardened: a failure prints a loud banner as
  the last lines and exits non-zero (so it isn't masked when piped to `tail`).
- `md2html.py` — renders `CUSTOM_BUILD.md` → `CUSTOM_BUILD.html` (stdlib only).

Prereqilites (one-time): a ZMK west workspace + Zephyr deps + a gnuarmemb ARM
toolchain. See `../CUSTOM_BUILD.md` §2 ("Fresh environment setup") for the full
recipe, and the rest of that doc for flashing.

Flashing note (macOS): Finder/`cp` to the `NICENANO` UF2 volume is broken on
recent macOS (error -36, silent no-op). Use the raw-device method:
`sudo dd if=firmware/<img>.uf2 of=/dev/rdiskN bs=1m` (see `../CUSTOM_BUILD.md`).
