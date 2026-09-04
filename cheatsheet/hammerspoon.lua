-- Miryoku keymap HUD for Hammerspoon.
--
-- Install:
--   brew install --cask hammerspoon
--   mkdir -p ~/.hammerspoon
--   cp cheatsheet/hammerspoon.lua ~/.hammerspoon/miryoku.lua
--   cp cheatsheet/miryoku-cheatsheet.html ~/.hammerspoon/
--   echo 'require("miryoku")' >> ~/.hammerspoon/init.lua
-- Then open Hammerspoon and Reload Config.
--
-- Hotkey: Ctrl+Alt+Cmd+K  (change HOTKEY below).
-- Press it to toggle. Esc inside the panel closes it.

local M = {}

local HOTKEY_MODS = { "ctrl", "alt", "cmd" }
local HOTKEY_KEY = "k"
local HTML = os.getenv("HOME") .. "/.hammerspoon/miryoku-cheatsheet.html"

-- Panel size as a fraction of the current screen.
local W_FRAC, H_FRAC = 0.72, 0.78

local webview = nil

local function frame()
  local s = hs.screen.mainScreen():frame()
  local w, h = s.w * W_FRAC, s.h * H_FRAC
  return hs.geometry.rect(s.x + (s.w - w) / 2, s.y + (s.h - h) / 2, w, h)
end

local function build()
  if not hs.fs.attributes(HTML) then
    hs.alert.show("miryoku-cheatsheet.html not found in ~/.hammerspoon")
    return nil
  end
  local v = hs.webview.new(frame())
  v:windowStyle({ "titled", "closable", "resizable", "utility" })
  v:windowTitle("Miryoku Keymap")
  v:level(hs.drawing.windowLevels.floating)
  v:allowTextEntry(true)
  v:shadow(true)
  v:darkMode(true)
  -- let the page close itself on Esc
  v:navigationCallback(function() return true end)
  v:url("file://" .. HTML)
  return v
end

function M.hide()
  if webview then webview:hide() end
end

function M.toggle()
  if webview and webview:hswindow() and webview:hswindow():isVisible() then
    webview:hide()
    return
  end
  if not webview then
    webview = build()
    if not webview then return end
  end
  webview:frame(frame())
  webview:show()
  -- focus so you can type into the search box immediately
  local win = webview:hswindow()
  if win then win:focus() end
end

hs.hotkey.bind(HOTKEY_MODS, HOTKEY_KEY, M.toggle)

-- Pre-warm the panel.
--
-- Without this the webview is built on the FIRST hotkey press, so that press
-- pays for spinning up a WebKit process and parsing the page -- a visible
-- delay exactly when you are in a hurry to look something up. Building it
-- up front (hidden) makes every press, including the first, a plain show().
--
-- Deferred by a second so it never slows Hammerspoon's own config load, and
-- guarded so a missing HTML file just leaves it to be retried on first use.
hs.timer.doAfter(1, function()
  if not webview then
    webview = build()
  end
end)

-- Escape closes the panel while it is focused.
M.escWatcher = hs.hotkey.bind({}, "escape", function()
  local win = webview and webview:hswindow()
  if win and win:isVisible() and win == hs.window.focusedWindow() then
    webview:hide()
  else
    M.escWatcher:disable()
    hs.eventtap.keyStroke({}, "escape", 0)
    M.escWatcher:enable()
  end
end)

return M
