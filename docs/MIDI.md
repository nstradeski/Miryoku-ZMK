# Toucan2 as a Bluetooth MIDI controller (iPad)

The Toucan2 firmware includes a **MIDI layer**. On that layer the keys send
MIDI over Bluetooth instead of keystrokes, and the trackpad works as an XY
pad. The keyboard connects to the iPad over the same Bluetooth link it
already uses for typing. You don't need a cable, a dongle, or a second pairing.

The firmware side is `modules/zmk-ble-midi`, a small ZMK module added for this
build. ZMK has no MIDI support of its own. The module adds the standard
Bluetooth LE MIDI service next to the keyboard service. iPadOS treats that as
a class-compliant Bluetooth MIDI device, so any MIDI app can use it:
GarageBand, Logic Pro, AUM, Loopy Pro, Drambo, Koala, and others.

Only the Toucan2 has this. The Corne and Corne-ish Zen builds are unchanged.

## One-time setup

1. **Flash both halves** with the new `toucan_left.uf2` / `toucan_right.uf2`.
   The MIDI service runs on the left (central) half.
2. **Let the iPad see the new service.** The iPad caches the list of services
   from when you paired the keyboard. The firmware tells it the list changed,
   but if the MIDI device never appears in step 4 below, re-pair from scratch:
   - iPad: Settings → Bluetooth → Toucan ⓘ → *Forget This Device*.
   - Keyboard: on the **FUN** layer, press the iPad's profile key (the
     `BT 0–3` keys on the bottom-right row) *shifted*. That clears the profile.
   - Pair again from Settings → Bluetooth.

## Connecting a MIDI app (each session)

iPadOS apps connect to Bluetooth MIDI devices themselves, from their own
"Bluetooth MIDI Devices" panel. The panel only lists devices that are
advertising MIDI. A keyboard that is already connected for typing isn't
advertising, so there's a **Pair** key that makes it advertise for 60 seconds:

1. Switch to the MIDI layer: press **both bottom outer pinky keys together**.
2. Open the app's Bluetooth MIDI panel:
   - **GarageBand:** open a song → Settings (gear) → Advanced →
     *Bluetooth MIDI Devices*.
   - **AUM / Loopy Pro / others:** look for *Bluetooth MIDI* in the app's
     settings or MIDI routing page.
3. Tap **Pair**, the bottom-right outer key on the MIDI layer. The keyboard
   shows up in the list under its Bluetooth name. Tap it to connect.
   Advertising stops by itself once the app connects.

After that, every MIDI app on the iPad sees the keyboard as a MIDI input until
it disconnects: sleep, walking away, or a profile switch. After a disconnect,
repeat step 3.

Press both bottom outer pinky keys together again to leave the MIDI layer.
The keyboard and trackpad work normally again, and the MIDI connection stays
up in the background.

## The MIDI layer

**Turn it on and off by pressing both bottom outer pinky keys together.** The
same gesture works both ways, and the layer stays on until you press it again.
Either key alone still does its normal job (hold for WINDOW on the base
layers; Panic or Pair on the MIDI layer). The combo only fires when both go
down within 50 ms.

Notes follow GarageBand's *Musical Typing*. The home row is the white keys,
starting with **C on the A key**. Each sharp is **directly above** its natural,
so the key above is one semitone higher. The keys above E and B are empty, as
on a piano.

```
         outer  Q/A   W/S   E/D   R/F   T/G  │  Y/H   U/J   I/K   O/L   P/'  outer
top       --    C#    D#    --    F#    G#   │  A#    --    C#'   D#'   --    F#'
home      B,    C     D     E     F     G    │  A     B     C'    D'    E'    F'
bottom  Panic  Oct-  Oct+  Vel-  Vel+  Mod   │ Tg20  Tg21  Tg22  Tg23  Tg24  Pair
thumbs               --  Sustain CC25       │  Rec  Play  Stop
```

| Key | Sends |
|---|---|
| C (A key) | Middle C, MIDI note 60. GarageBand labels it **C3**. |
| Oct− / Oct+ | Shift all notes an octave, ±4. Held notes still release correctly. |
| Vel− / Vel+ | Note velocity ±16. It starts at 100. |
| Mod | Mod wheel (CC 1) at full while held |
| Sustain | Sustain pedal (CC 64) while held. The left thumb, in the Space position. |
| CC25 | CC 25 at 127 while held, 0 on release. Use it with MIDI Learn for momentary actions. |
| Tg20–Tg24 | CC 20–24. Each press toggles 127 / 0. Use them with MIDI Learn for mutes, FX on/off, loop tracks. |
| Rec / Play / Stop | MIDI Machine Control (MMC) Record / Play / Stop. Play and Stop also send MIDI Start / Stop. |
| Panic | Releases every held note and sends All Notes Off and Sustain Off |
| Pair | Advertises the MIDI service for 60 s (see above) |
| Panic + Pair together | Leaves the MIDI layer (the toggle combo) |

### Trackpad = XY pad

On the MIDI layer the trackpad no longer moves the cursor. Instead:

- **left / right** sends **CC 16**, from 0 to 127
- **up / down** sends **CC 17**, from 0 to 127

Both start at 64. They work like two knobs you move with one finger: filter
cutoff on X and resonance on Y, for example. Map them with the app's MIDI
Learn. To change how far you swipe for the full 0–127 range, set `divisor` on
`&midi_xy` in `config/toucan.keymap`. The default is 4; higher gives finer
control and needs more travel.

### What works where

- **Notes** work in any instrument app. GarageBand plays whichever
  instrument is open.
- **CCs** need a host that maps MIDI CC, which most do through *MIDI Learn*:
  AUM, Loopy Pro, Logic Pro, Drambo, and most synth apps. GarageBand
  responds to sustain (CC 64) and mod wheel (CC 1) but has no general MIDI
  Learn.
- **Transport** (MMC / Start / Stop) depends on the app. If an app ignores
  it, map one of the toggle CCs to play/record with MIDI Learn instead.

## Customizing

The layer is defined in `miryoku/miryoku_midi.h`, one row per line. These
bindings are available:

```c
&midi_note MIDI_N(MN_FS, 4)   // a note: MN_C, MN_CS, MN_D, ... MN_B + octave
&midi_cc 74                   // CC 74 = 127 while held, 0 on release
&midi_cc_tog 80               // CC 80 toggles 127 / 0
&midi_ctl MIDI_OCT_UP         // see the list below
```

`&midi_ctl` commands: `MIDI_OCT_DN/UP/RST`, `MIDI_SEMI_DN/UP` (transpose),
`MIDI_VEL_DN/UP`, `MIDI_CH_DN/UP` (MIDI channel, which starts at 1),
`MIDI_PC_DN/UP` (previous/next program change), `MIDI_PANIC`, `MIDI_PLAY`,
`MIDI_STOP`, `MIDI_CONT`, `MIDI_REC`, `MIDI_PAIR`. The full list with comments
is in `modules/zmk-ble-midi/include/dt-bindings/zmk/midi.h`.

Tuning knobs, such as the default velocity and how long Pair advertises, are
Kconfig options in `modules/zmk-ble-midi/Kconfig`. Set them in
`config/toucan_left.conf`.

## Limits

- **Velocity is per layer, not per keystroke.** Key switches can't sense how
  hard you press, so every note uses the current Vel−/Vel+ setting.
- **Bluetooth only, to the active profile.** MIDI goes to the host on the
  currently selected Bluetooth profile. It doesn't go over USB.
- **Latency** is set by the Bluetooth connection interval the iPad chooses,
  typically 7.5–15 ms. That's fine for playing and triggering, but not for
  sample-tight drumming.
- **Not yet tested on hardware.** The firmware compiles and the layer is
  wired up, but the board hasn't arrived yet. The first things to check on a
  real iPad are the Pair → connect flow and the trackpad `divisor`.
