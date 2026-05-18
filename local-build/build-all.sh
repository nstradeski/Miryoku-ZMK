#!/bin/zsh
# Build all four Miryoku ZMK images (Typeractive Corne + Corne-ish Zen v2).
#
# Portable: paths come from env vars with defaults matching the original
# local setup, so it works as-is here and is overridable elsewhere.
#   ZMK_DIR        west topdir (the zmk checkout). default: $HOME/Projects/zmk-build/zmk
#   WEST           west binary. default: $HOME/Projects/zmk-build/.venv/bin/west
#   ARM_TOOLCHAIN  gnuarmemb toolchain root (contains bin/arm-none-eabi-gcc)
#   OUT            output dir for .uf2s. default: $HOME/Projects/zmk-build/firmware
# MIRYOKU_DIR (this repo) is derived from the script location.
#
# Hardened so failures are unmissable even when piped through `tail`
# (which discards exit codes).
set -euo pipefail

SCRIPT_DIR=${0:A:h}
MIRYOKU_DIR=${MIRYOKU_DIR:-${SCRIPT_DIR:h}}          # repo root = local-build/..
ZMK_DIR=${ZMK_DIR:-$HOME/Projects/zmk-build/zmk}
WEST=${WEST:-$HOME/Projects/zmk-build/.venv/bin/west}
ARM_TOOLCHAIN=${ARM_TOOLCHAIN:-$HOME/Projects/zmk-build/arm/arm-gnu-toolchain-14.2.rel1-darwin-x86_64-arm-none-eabi}
OUT=${OUT:-$HOME/Projects/zmk-build/firmware}

export ZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb
export GNUARMEMB_TOOLCHAIN_PATH=$ARM_TOOLCHAIN
export PATH="$ARM_TOOLCHAIN/bin:$PATH"
CFG=$MIRYOKU_DIR/config
cd "$ZMK_DIR"
mkdir -p "$OUT"

CURRENT=""   # which target is in flight, for the failure banner

fail() {
  echo ""
  echo "##############################################################"
  echo "########## BUILD FAILED${CURRENT:+ during: $CURRENT} ##########"
  echo "##############################################################"
  echo "(no/partial firmware written; existing .uf2 files are stale)"
  exit 1
}
trap fail ERR

build() {  # name  board  shield(optional)
  local name=$1 board=$2 shield=${3:-}
  CURRENT=$name
  echo "=================== BUILDING $name ==================="
  local args=(-p -s app -d build/$name -b $board --
    -DZMK_CONFIG=$CFG
    -DBOARD_ROOT=$ZMK_DIR/app/module
    -DSHIELD_ROOT=$ZMK_DIR/app
    -DDTS_ROOT=$ZMK_DIR/app/module
    -DEXTRA_CONF_FILE=$CFG/pointing.conf)
  [[ -n $shield ]] && args+=(-DSHIELD="$shield")
  $WEST build "${args[@]}"
  [[ -f build/$name/zephyr/zmk.uf2 ]] || { echo "missing zmk.uf2 for $name"; return 1; }
  cp build/$name/zephyr/zmk.uf2 "$OUT/$name.uf2"
  echo ">>> $name -> $OUT/$name.uf2"
}

build corne_left             nice_nano/nrf52840/zmk          "corne_left nice_view_adapter nice_view"
build corne_right            nice_nano/nrf52840/zmk          "corne_right nice_view_adapter nice_view"
build corneish_zen_v2_left   corneish_zen_left/nrf52840/zmk
build corneish_zen_v2_right  corneish_zen_right/nrf52840/zmk

trap - ERR
echo ""
echo "############################################"
echo "########## BUILD SUCCESS (4/4) #############"
echo "############################################"
ls -lh "$OUT"
