#!/usr/bin/env python3
"""
Generate a searchable Miryoku keymap cheat sheet from the ACTUAL firmware source.

It expands the real layer macros through the C preprocessor, so the output always
matches what is flashed -- no hand-transcribed tables to drift out of date.

    python3 cheatsheet/build.py

Writes cheatsheet/miryoku-cheatsheet.html (self-contained: open in a browser, or
let Hammerspoon host it -- see cheatsheet/hammerspoon.lua).
"""

import json
import os
import re
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
sys.path.insert(0, HERE)

from keymap_labels import (  # noqa: E402
    KEYS, MODS, CHORD_MEANINGS, BEHAVIORS, SHIFT_MORPHS, OUTER, LAYERS,
    LAYER_ORDER, FINGERS_L, FINGERS_R, ROWS, THUMBS_L, THUMBS_R,
)

LAYER_NAMES = ["BASE", "EXTRA", "TAP", "BUTTON", "NAV", "MOUSE", "MEDIA",
               "NUM", "SYM", "FUN", "WINDOW"]

STUB = r"""
#define U_NP  &none
#define U_NA  &none
#define U_NU  &none
#define U_BOOT &bootloader
#define U_MT(a,b)   &u_mt a b
#define U_MT_L(a,b) &u_mt a b
#define U_MT_R(a,b) &u_mt a b
#define U_LT(a,b)   &u_lt a b
#include "miryoku/custom_config.h"
#include "miryoku/miryoku_clipboard.h"
#include "miryoku/miryoku_mousekeys.h"
#include "miryoku/miryoku_babel/miryoku_layer_list.h"
#include "miryoku/miryoku_babel/miryoku_layer_selection.h"
#include "miryoku/miryoku_babel/miryoku_layer_alternatives.h"
"""


def expand_layers():
    """Run cpp over the real headers and return {LAYER: [40 binding strings]}."""
    src = STUB + "".join(
        f"@@{n}@@ MIRYOKU_LAYER_{n}\n" for n in LAYER_NAMES
    )
    with tempfile.NamedTemporaryFile("w", suffix=".c", delete=False) as fh:
        fh.write(src)
        path = fh.name
    try:
        out = subprocess.run(
            ["cpp", "-P", "-I", ROOT, path],
            capture_output=True, text=True, check=True,
        ).stdout
    finally:
        os.unlink(path)

    parts = re.split(r"@@([A-Z]+)@@", out)
    layers = {}
    for i in range(1, len(parts), 2):
        name = parts[i]
        body = " ".join(parts[i + 1].split())
        items = [x.strip() for x in body.split(",") if x.strip()]
        if len(items) != 40:
            raise SystemExit(f"{name}: expected 40 bindings, got {len(items)}")
        layers[name] = items
    missing = set(LAYER_NAMES) - set(layers)
    if missing:
        raise SystemExit(f"missing layers: {missing}")
    return layers


def pretty_key(code):
    """LC(LA(LG(LS(Q)))) -> '⌃⌥⌘⇧Q';  LA(N3) -> '⌥3';  N7 -> '7'."""
    mods = ""
    while True:
        m = re.fullmatch(r"(L[GACS]|RA)\((.*)\)", code)
        if not m:
            break
        mods += MODS[m.group(1)]
        code = m.group(2)
    # canonical mod order ⌃⌥⇧⌘
    mods = "".join(c for c in "⌃⌥⇧⌘" if c in mods)
    if code in KEYS:
        base = KEYS[code][0]
    elif re.fullmatch(r"N\d", code):
        base = code[1]
    elif re.fullmatch(r"F\d{1,2}", code):
        base = code
    elif len(code) == 1:
        base = code
    else:
        base = code.replace("_", " ").title()
    return mods + base


def search_words_for(code):
    words = []
    stripped = code
    while True:
        m = re.fullmatch(r"(L[GACS]|RA)\((.*)\)", stripped)
        if not m:
            break
        stripped = m.group(2)
    if stripped in KEYS:
        words.append(KEYS[stripped][1])
    return " ".join(words)


def resolve(binding):
    """binding string -> dict(label, sub, desc, search)"""
    b = binding.strip()
    tok = b.split()

    def out(label, sub="", desc="", search=""):
        return {"label": label, "sub": sub, "desc": desc, "search": search}

    if b == "&none":
        return None

    if tok[0] == "&kp":
        code = " ".join(tok[1:])
        label = pretty_key(code)
        meaning = CHORD_MEANINGS.get(label)
        words = search_words_for(code)
        if meaning:
            return out(label, meaning[0], meaning[0], f"{meaning[1]} {words} {label}")
        return out(label, "", "", f"{words} {label}")

    if tok[0] == "&u_mt":
        mod, key = tok[1], tok[2]
        return out(pretty_key(key), f"hold = {KEYS.get(mod, (mod,''))[0]}",
                   f"Tap for {pretty_key(key)}, hold for {KEYS.get(mod,(mod,''))[0]}",
                   f"{search_words_for(key)} {pretty_key(key)} modifier hold {mod}")

    if tok[0] == "&u_lt":
        idx, key = int(tok[1]), tok[2]
        lname = LAYER_NAMES[idx] if idx < len(LAYER_NAMES) else str(idx)
        pretty = LAYERS.get(lname, {}).get("name", lname)
        return out(pretty_key(key), f"hold = {pretty}",
                   f"Tap for {pretty_key(key)}, hold for the {pretty} layer",
                   f"{search_words_for(key)} {pretty_key(key)} layer {pretty} hold thumb")

    m = re.fullmatch(r"&u_to_U_([A-Z]+)", b)
    if m:
        pretty = LAYERS.get(m.group(1), {}).get("name", m.group(1))
        return out(f"→ {pretty}", "lock layer", f"Lock the keyboard to the {pretty} layer",
                   f"lock layer switch {pretty}")

    m = re.fullmatch(r"&u_bt_sel_(\d)", b)
    if m:
        n = m.group(1)
        return out(f"BT {n}", "profile", f"Select Bluetooth profile {n} (shift = clear then select)",
                   f"bluetooth bt profile {n} pair connect device")

    if b == "&u_out_tog":
        lbl, desc, words = BEHAVIORS["u_out_tog"]
        return out(lbl, "BT / USB", desc, words)

    name = b.lstrip("&").split()[0]
    if name in SHIFT_MORPHS:
        tl, tc, sl, sc, words = SHIFT_MORPHS[name]
        return out(tl, f"⇧ {sl}", f"{tl} ({tc}) — with Shift: {sl} ({sc})", f"{words} {tc} {sc}")

    if name in BEHAVIORS:
        lbl, desc, words = BEHAVIORS[name]
        return out(lbl, "", desc, words)

    if tok[0] == "&mmv":
        d = tok[1].replace("MOVE_", "").lower()
        arrow = {"left": "←", "right": "→", "up": "↑", "down": "↓"}[d]
        return out(f"Mouse {arrow}", "", f"Move the mouse cursor {d}",
                   f"mouse cursor move {d} pointer")
    if tok[0] == "&msc":
        d = tok[1].replace("SCRL_", "").lower()
        arrow = {"left": "←", "right": "→", "up": "↑", "down": "↓"}[d]
        return out(f"Scroll {arrow}", "", f"Scroll {d}", f"scroll wheel {d}")
    if tok[0] == "&mkp":
        n = {"MB1": ("Left Click", "left click primary"),
             "MB2": ("Right Click", "right click secondary context"),
             "MB3": ("Middle Click", "middle click")}[tok[1]]
        return out(n[0], "", n[0], f"mouse button click {n[1]}")

    return out(b, "", "", b)


# ---- physical layout -------------------------------------------------------

def positions():
    """Return the 42 slots as (row, col, hand, finger, kind, source_index)."""
    slots = []
    for r in range(3):
        # outer-left
        slots.append((r, 0, "L", FINGERS_L[0], "outer", None))
        for c in range(5):
            slots.append((r, c + 1, "L", FINGERS_L[c + 1], "main", r * 10 + c))
        for c in range(5):
            slots.append((r, c + 6, "R", FINGERS_R[c], "main", r * 10 + 5 + c))
        slots.append((r, 11, "R", FINGERS_R[5], "outer", None))
    for i in range(3):
        slots.append((3, i, "L", THUMBS_L[i], "thumb", 32 + i))
    for i in range(3):
        slots.append((3, 3 + i, "R", THUMBS_R[i], "thumb", 35 + i))
    return slots


OUTER_BY_ROW = {
    0: ("U_WITCH_L", "U_WITCH_R"),
    1: ("U_MOUSELESS_L", "U_MOUSELESS_R"),
    2: ("U_WIN_MO", "U_WIN_MO"),
}


def where(row, hand, finger, kind):
    if kind == "thumb":
        return f"{'left' if hand=='L' else 'right'} {finger}"
    if kind == "outer":
        return f"{'left' if hand=='L' else 'right'} outer pinky, {ROWS[row]} row"
    return f"{'left' if hand=='L' else 'right'} {finger}, {ROWS[row]} row"


def build():
    raw = expand_layers()
    slots = positions()
    layers_out = []
    index = []

    for lname in LAYER_ORDER:
        meta = LAYERS[lname]
        keys = []
        # where does Shift live on this layer? (for mod-morph "shift" actions)
        shift_where = None
        for (row, col, hand, finger, kind, src) in slots:
            if kind == "main" and raw[lname][src].strip() == "&kp LSHFT":
                shift_where = where(row, hand, finger, kind)
                break

        for (row, col, hand, finger, kind, src) in slots:
            if kind == "outer":
                oname = OUTER_BY_ROW[row][0 if hand == "L" else 1]
                lbl, desc, words = OUTER[oname]
                entry = {"label": lbl, "sub": "", "desc": desc, "search": words}
            else:
                entry = resolve(raw[lname][src])
            cell = {"row": row, "col": col, "hand": hand, "kind": kind}
            if entry:
                cell.update(entry)
                cell["where"] = where(row, hand, finger, kind)
                index.append({
                    "layer": lname,
                    "label": entry["label"],
                    "sub": entry["sub"],
                    "desc": entry["desc"],
                    "search": f"{entry['search']} {entry['label']} {entry['sub']}".lower(),
                    "where": cell["where"],
                    "row": row, "col": col, "shift": False,
                })
                # mod-morph: make the SHIFT action independently searchable
                bname = raw[lname][src].strip().lstrip("&").split()[0] if src is not None else ""
                if bname in SHIFT_MORPHS:
                    tl, tc, sl, sc, words = SHIFT_MORPHS[bname]
                    index.append({
                        "layer": lname,
                        "label": sl,
                        "sub": sc,
                        "desc": f"{sl} ({sc}) — hold Shift with this key",
                        "search": f"{words} {sl} {sc}".lower(),
                        "where": cell["where"], "shiftWhere": shift_where,
                        "row": row, "col": col, "shift": True,
                    })
            keys.append(cell)
        layers_out.append({
            "id": lname,
            "name": meta["name"],
            "hold": meta["hold"],
            "blurb": meta["blurb"],
            "keys": keys,
        })

    # collapse identical actions that exist on several layers (clipboard etc.)
    seen = {}
    merged = []
    for it in index:
        key = (it["label"], it["desc"], it["shift"])
        if key in seen:
            prim = seen[key]
            name = LAYERS[it["layer"]]["name"]
            if name != LAYERS[prim["layer"]]["name"] and name not in prim["also"]:
                prim["also"].append(name)
            continue
        it["also"] = []
        seen[key] = it
        merged.append(it)

    return {"layers": layers_out, "index": merged}


def main():
    data = build()
    tpl = open(os.path.join(HERE, "template.html"), encoding="utf-8").read()
    html = tpl.replace("/*__DATA__*/null", json.dumps(data, ensure_ascii=False))
    dest = os.path.join(HERE, "miryoku-cheatsheet.html")
    with open(dest, "w", encoding="utf-8") as fh:
        fh.write(html)
    n = len(data["index"])
    print(f"wrote {dest}  ({len(data['layers'])} layers, {n} searchable keys)")


if __name__ == "__main__":
    main()
