# Miryoku Keymap Cheat Sheet

A searchable, always-a-hotkey-away replacement for the printed Miryoku chart.

Generated **from the real firmware source** — `build.py` expands the actual layer
macros through the C preprocessor, so the sheet always matches what is flashed.
Change the keymap, re-run the build, and the sheet follows. No hand-maintained
tables to drift out of date.

## What it does

- **Search first.** Type `hash`, `undo`, `fullscreen`, `new tab`, `bluetooth`,
  `move tab` and it tells you the whole recipe:
  `⌥3 — hold RIGHT-inner thumb (Enter), then left index, bottom row`.
- **Layer browser.** Tab through all 11 layers as a 42-key diagram.
- **Shift actions are searchable.** The mod-morph keys on MEDIA (Forward, Reopen
  tab, New tab, Move tab, Hard refresh) are indexed separately, and the recipe
  tells you to hold Shift as well.
- **Duplicates collapse.** Clipboard keys live on Nav/Mouse/Button; you get one
  result with an "also on" note instead of five.

### Keys

| Key | Action |
|---|---|
| type | search |
| `↑` `↓` | move through results |
| `⏎` | reveal that key highlighted on the layer map |
| `⇥` / `⇧⇥` | next / previous layer |
| `esc` | clear the search, then close the panel |

## Build

`miryoku-cheatsheet.html` is generated output, but it **is** committed — kept
current by CI rather than by hand, so it can never disagree with the keymap
beside it:

- **CI** — the `Build Firmware` workflow's `cheatsheet` job regenerates it,
  **commits it back to the branch** if it changed, and the bundle step drops it
  into the **`miryoku-firmware`** artifact alongside the four `.uf2` files. One
  download gives you the firmware and the reference that matches it. The
  commit-back uses `GITHUB_TOKEN`, whose pushes do not re-trigger workflows, so
  it cannot loop; a failed push is a warning and never blocks the firmware.
- **Local** — `local-build/build-all.sh` regenerates it after the four images,
  copies it next to the `.uf2` output, and refreshes `~/.hammerspoon/` in place
  if the HUD is installed.

You therefore never need to run the generator to keep the repo honest. If you
edit the keymap locally and commit without building, CI regenerates and commits
the sheet for you on the next run.

To run it by hand:

```sh
python3 cheatsheet/build.py
```

Writes `cheatsheet/miryoku-cheatsheet.html` — one self-contained file, no
network, no dependencies (just `cpp`). Open it in a browser and it works as-is.

Wording lives in `keymap_labels.py` (labels, search synonyms, layer blurbs);
layout and logic in `build.py`; the UI in `template.html`. Edit the template,
never the generated HTML.

## Install as a floating HUD (Hammerspoon)

```sh
brew install --cask hammerspoon
mkdir -p ~/.hammerspoon
python3 cheatsheet/build.py                          # generate the HTML first
cp cheatsheet/hammerspoon.lua         ~/.hammerspoon/miryoku.lua
cp cheatsheet/miryoku-cheatsheet.html ~/.hammerspoon/
echo 'require("miryoku")' >> ~/.hammerspoon/init.lua
```

Open Hammerspoon → **Reload Config**. Grant Accessibility permission when asked.

Hotkey is **`⌃⌥⌘K`** — press to toggle the panel over whatever you are doing,
`esc` to dismiss. Change `HOTKEY_MODS` / `HOTKEY_KEY` at the top of
`hammerspoon.lua` to rebind.

After a keymap change you normally do nothing: `build-all.sh` refreshes
`~/.hammerspoon/` for you (reload Hammerspoon to pick it up). If you flashed from
a CI artifact instead, copy the `miryoku-cheatsheet.html` out of the same
`miryoku-firmware` download:

```sh
cp ~/Downloads/miryoku-firmware/miryoku-cheatsheet.html ~/.hammerspoon/
```

## A note on live layer highlighting

The sheet cannot follow your layers automatically. Layers are resolved **inside
the keyboard firmware** — a Miryoku layer-tap sends the host nothing at all while
held, so macOS never learns which layer is active and no app can read it.

This is why the design is search-first: when you cannot remember a key you are
not holding its layer yet, so the useful answer is the complete recipe rather
than a highlight of a layer you are already on.
