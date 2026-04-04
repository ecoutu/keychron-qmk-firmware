# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this repo is

A QMK **userspace** for the Keychron Q6 Max (ANSI encoder variant). It contains only customizations — not QMK itself. QMK firmware lives at `~/qmk_firmware` (installed via `qmk setup`) and reads this repo via `qmk.json`.

## Build commands

```bash
make build          # compile via local QMK CLI
make flash          # compile + flash via local QMK CLI (keyboard must be in bootloader mode)
make docker-build   # compile inside Docker (no local toolchain needed)
make docker-flash   # flash via Docker
make setup          # install/update QMK CLI and toolchain globally
```

The keyboard target is `keychron/q6_max/ansi_encoder` with keymap `ecoutu`. Both the Makefile and `qmk.json` encode this. There are no tests.

## Flashing

Put the Q6 Max in bootloader mode first: hold the `Boot` button on the underside while plugging in USB, or hold `Fn + Space` for 3 seconds. Then run `make flash`.

## Architecture

The keymap lives entirely in three files:

| File | Purpose |
|------|---------|
| `keyboards/keychron/q6_max/keymaps/ecoutu/keymap.c` | Layers, tap-dance keys, encoder map |
| `keyboards/keychron/q6_max/keymaps/ecoutu/config.h` | Numeric overrides: `DEBOUNCE` (5ms, down from default 20ms), `TAPPING_TERM`, `RGB_MATRIX_MAXIMUM_BRIGHTNESS` |
| `keyboards/keychron/q6_max/keymaps/ecoutu/rules.mk` | Feature flags: `VIA_ENABLE`, `TAP_DANCE_ENABLE`, `ENCODER_MAP_ENABLE`, `RGB_MATRIX_ENABLE` |

### Layer structure

Four layers defined in `enum layers`:
- `MAC_BASE` / `MAC_FN` — macOS layout; Fn key is `MO(MAC_FN)`
- `WIN_BASE` / `WIN_FN` — Windows layout; Fn key is `MO(WIN_FN)`

The layout macro is `LAYOUT_109_ansi` (109 keys, includes numpad). All four layers must have an entry in `encoder_map` when `ENCODER_MAP_ENABLE = yes`. The encoder is mapped to volume down/up (`KC_VOLD`/`KC_VOLU`) on all four layers.

### Tap-dance key: `TD_W`

`WIN_BASE` uses `TD(TD_W)` instead of `KC_W`. Three gestures:
- **Single tap** — types W immediately (fires on interrupt, no timer wait)
- **Hold** — normal W hold; releases when key released
- **Double-tap** — toggles W held-down indefinitely; double-tap again to release

Implemented in `keymap.c` via `ACTION_TAP_DANCE_FN_ADVANCED`. A `cur_dance()` helper classifies the gesture; `td_w_finished` / `td_w_reset` callbacks handle the three cases. A `static bool w_held` tracks the toggle state across dances. `MAC_BASE` uses plain `KC_W` and is unaffected.

**Important:** `process_record_user` must call `process_record_keychron_common(keycode, record)` first and return false if it returns false. This handles Keychron-specific keycodes (Mac/Win toggle, etc.).

### QMK upstream

Since this is a userspace (not a fork), QMK updates are consumed with `qmk setup` — no submodule to manage. The keyboard definition (`info.json`, matrix layout) lives in `~/qmk_firmware/keyboards/keychron/q6_max/`.

## Code style

`.clang-format` is checked in. A PostToolUse hook auto-runs `clang-format -i` on any `.c` or `.h` file edited in this session (restart Claude Code to activate it if new to the session). The keymap matrix rows use `// clang-format off` guards to preserve alignment.
