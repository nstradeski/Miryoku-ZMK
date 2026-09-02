# Keycode -> human label + search synonyms.
# Kept separate from build.py so it is easy to tweak wording without touching logic.

# Plain ZMK keycodes -> (display label, extra search words)
KEYS = {
    # letters/digits handled generically; only specials listed here
    "SQT": ("'", "quote apostrophe single"),
    "DQT": ('"', "double quote"),
    "COMMA": (",", "comma"),
    "DOT": (".", "period dot full stop"),
    "SLASH": ("/", "slash forward divide"),
    "BSLH": ("\\", "backslash"),
    "SEMI": (";", "semicolon"),
    "COLON": (":", "colon"),
    "EQUAL": ("=", "equals equal"),
    "PLUS": ("+", "plus add"),
    "MINUS": ("-", "minus dash hyphen"),
    "UNDER": ("_", "underscore under"),
    "GRAVE": ("`", "grave backtick"),
    "TILDE": ("~", "tilde"),
    "LBKT": ("[", "left bracket square open"),
    "RBKT": ("]", "right bracket square close"),
    "LBRC": ("{", "left brace curly open"),
    "RBRC": ("}", "right brace curly close"),
    "LPAR": ("(", "left paren parenthesis open"),
    "RPAR": (")", "right paren parenthesis close"),
    "LT": ("<", "less than left angle"),
    "GT": (">", "greater than right angle"),
    "EXCL": ("!", "exclamation bang not"),
    "AT": ("@", "at sign"),
    "HASH": ("#", "hash pound number sign octothorpe"),
    "DLLR": ("$", "dollar"),
    "PRCNT": ("%", "percent"),
    "CARET": ("^", "caret hat circumflex"),
    "AMPS": ("&", "ampersand and"),
    "ASTRK": ("*", "asterisk star times"),
    "PIPE": ("|", "pipe vertical bar"),
    "QMARK": ("?", "question mark"),
    # navigation / editing
    "LEFT": ("←", "left arrow"),
    "RIGHT": ("→", "right arrow"),
    "UP": ("↑", "up arrow"),
    "DOWN": ("↓", "down arrow"),
    "HOME": ("Home", "home start line begin"),
    "END": ("End", "end line finish"),
    "PG_UP": ("PgUp", "page up"),
    "PG_DN": ("PgDn", "page down"),
    "INS": ("Insert", "insert ins"),
    "DEL": ("Del", "delete forward"),
    "BSPC": ("Bksp", "backspace delete back"),
    "RET": ("Enter", "enter return newline"),
    "ESC": ("Esc", "escape"),
    "TAB": ("Tab", "tab"),
    "SPACE": ("Space", "space spacebar"),
    "CAPS": ("Caps Lock", "caps lock capital"),
    "K_APP": ("Menu", "context menu application right click"),
    "PSCRN": ("Print Screen", "print screen prtsc screenshot"),
    "SLCK": ("Scroll Lock", "scroll lock"),
    "PAUSE_BREAK": ("Pause", "pause break"),
    # media
    "C_VOL_UP": ("Vol +", "volume up louder"),
    "C_VOL_DN": ("Vol −", "volume down quieter"),
    "C_MUTE": ("Mute", "mute silence volume"),
    "C_PP": ("Play / Pause", "play pause music"),
    "C_NEXT": ("Next Track", "next track skip forward song"),
    "C_PREV": ("Prev Track", "previous track back song"),
    "C_STOP": ("Stop", "stop playback"),
    # modifiers as keys
    "LGUI": ("⌘ Cmd", "command gui meta super win"),
    "LALT": ("⌥ Opt", "option alt"),
    "LCTRL": ("⌃ Ctrl", "control ctrl"),
    "LSHFT": ("⇧ Shift", "shift"),
    "RALT": ("⌥ AltGr", "right alt altgr option"),
}

# Modifier wrapper prefixes
MODS = {"LG": "⌘", "LA": "⌥", "LC": "⌃", "LS": "⇧", "RA": "⌥"}

# Well-known macOS chords -> what they actually do (search gold)
CHORD_MEANINGS = {
    "⌘Z": ("Undo", "undo revert back mistake"),
    "⇧⌘Z": ("Redo", "redo again forward"),
    "⌘X": ("Cut", "cut"),
    "⌘C": ("Copy", "copy"),
    "⌘V": ("Paste", "paste"),
    "⌘W": ("Close window / tab", "close window tab quit"),
    "⌘M": ("Minimize window", "minimize window hide shrink"),
    "⌃⌘F": ("Full screen", "fullscreen full screen maximize zoom"),
    "⌘T": ("New tab", "new tab open"),
    "⌘L": ("Address bar", "address bar url location omnibox focus"),
    "⌘R": ("Refresh", "refresh reload"),
    "⇧⌘R": ("Hard refresh", "hard refresh reload cache"),
    "⌘[": ("Back", "back previous history"),
    "⌘]": ("Forward", "forward next history"),
    "⇧⌘T": ("Reopen closed tab", "reopen restore closed tab undo close"),
    "⌃Tab": ("Next tab", "next tab right"),
    "⌃⇧Tab": ("Previous tab", "previous prev tab left"),
    "⌃⇧PgUp": ("Move tab left", "move tab left reorder"),
    "⌃⇧PgDn": ("Move tab right", "move tab right reorder"),
    "⌥3": ("# hash", "hash pound number sign"),
}

# Custom behaviors -> (label, description, search words)
BEHAVIORS = {
    "u_out_tog": ("Output BT/USB", "Toggle output between Bluetooth and USB (shift = force USB)",
                  "output usb bluetooth toggle connection"),
    "caps_word": ("Caps Word", "Capitalises the next word (shift = Caps Lock)", "caps word capital shout"),
    "u_caps_word": ("Caps Word", "Capitalises the next word (shift = Caps Lock)", "caps word capital shout"),
    "bootloader": ("Bootloader", "Reboot into UF2 bootloader for flashing", "bootloader flash reset dfu uf2 firmware"),
    "sys_reset": ("Reset", "Reset the keyboard", "reset reboot restart"),
}

# Mouse keys
MOUSE = {
    "U_MS_U": ("Mouse ↑", "mouse move up cursor"),
    "U_MS_D": ("Mouse ↓", "mouse move down cursor"),
    "U_MS_L": ("Mouse ←", "mouse move left cursor"),
    "U_MS_R": ("Mouse →", "mouse move right cursor"),
    "U_WH_U": ("Wheel ↑", "scroll wheel up"),
    "U_WH_D": ("Wheel ↓", "scroll wheel down"),
    "U_WH_L": ("Wheel ←", "scroll wheel left"),
    "U_WH_R": ("Wheel →", "scroll wheel right"),
    "U_BTN1": ("Left Click", "left click primary mouse button"),
    "U_BTN2": ("Right Click", "right click secondary context mouse button"),
    "U_BTN3": ("Middle Click", "middle click mouse button"),
}

# Shift-morph behaviours: tap action / shift action
SHIFT_MORPHS = {
    "u_bro_back":   ("Back", "⌘[", "Forward", "⌘]", "back forward history navigate browser"),
    "u_bro_close":  ("Close tab", "⌘W", "Reopen tab", "⇧⌘T", "close reopen restore tab browser"),
    "u_bro_tabp":   ("Prev tab", "⌃⇧Tab", "Move tab left", "⌃⇧PgUp", "previous tab left move reorder browser"),
    "u_bro_tabn":   ("Next tab", "⌃Tab", "Move tab right", "⌃⇧PgDn", "next tab right move reorder browser"),
    "u_bro_reload": ("Refresh", "⌘R", "Hard refresh", "⇧⌘R", "refresh reload hard cache browser"),
    "u_bro_url":    ("Address bar", "⌘L", "New tab", "⌘T", "address bar url new tab open browser"),
}

# Outer-column app hotkeys (from mapping/42/corne.h)
OUTER = {
    "U_WITCH_L":     ("Witch ◀", "Witch window switcher — previous", "witch window switcher app previous"),
    "U_WITCH_R":     ("Witch ▶", "Witch window switcher — next", "witch window switcher app next"),
    "U_MOUSELESS_L": ("Mouseless", "Mouseless — keyboard-driven pointing", "mouseless mouse pointer app"),
    "U_MOUSELESS_R": ("Mouseless", "Mouseless — keyboard-driven pointing", "mouseless mouse pointer app"),
    "U_WIN_MO":      ("WINDOW", "Hold for the WINDOW layer (Amethyst tiling)", "window layer tiling amethyst hold"),
}

# Layer metadata: how you reach each layer
LAYERS = {
    "BASE":   {"name": "Base",   "hold": None, "blurb": "Default QWERTY layer with home-row mods."},
    "EXTRA":  {"name": "Extra",  "hold": None, "blurb": "Alternate Colemak-DH alphas (locked via Nav)."},
    "TAP":    {"name": "Tap",    "hold": None, "blurb": "QWERTY with no home-row mods (locked via Nav)."},
    "BUTTON": {"name": "Button", "hold": "hold left-pinky Z or right-pinky /",
               "blurb": "Clipboard + mouse buttons, reachable from any hand position."},
    "NAV":    {"name": "Nav",    "hold": "hold LEFT-middle thumb (Space)",
               "blurb": "Arrows, Home/End/PgUp/PgDn and the clipboard cluster."},
    "MOUSE":  {"name": "Mouse",  "hold": "hold LEFT-inner thumb (Tab)",
               "blurb": "Cursor movement, scroll wheel and mouse buttons."},
    "MEDIA":  {"name": "Media",  "hold": "hold LEFT-outer thumb (Esc)",
               "blurb": "Browser controls, volume and track transport."},
    "NUM":    {"name": "Num",    "hold": "hold RIGHT-middle thumb (Bksp)",
               "blurb": "Number pad and the bracket/maths symbols."},
    "SYM":    {"name": "Sym",    "hold": "hold RIGHT-inner thumb (Enter)",
               "blurb": "Shifted symbols."},
    "FUN":    {"name": "Fun",    "hold": "hold RIGHT-outer thumb (Del)",
               "blurb": "F-keys plus the Bluetooth / output cluster."},
    "WINDOW": {"name": "Window", "hold": "hold either BOTTOM-OUTER pinky",
               "blurb": "Amethyst tiling: every letter sends its own Hyper chord."},
}

LAYER_ORDER = ["BASE", "NAV", "MEDIA", "SYM", "NUM", "FUN", "MOUSE", "BUTTON", "WINDOW", "EXTRA", "TAP"]

# Physical position naming
FINGERS_L = ["outer", "pinky", "ring", "middle", "index", "inner"]
FINGERS_R = ["inner", "index", "middle", "ring", "pinky", "outer"]
ROWS = ["top", "home", "bottom"]
THUMBS_L = ["outer thumb (Esc)", "middle thumb (Space)", "inner thumb (Tab)"]
THUMBS_R = ["inner thumb (Enter)", "middle thumb (Bksp)", "outer thumb (Del)"]
