# QMK Userspace Design — Keychron Q6 Max

**Date:** 2026-04-03
**Keyboard:** Keychron Q6 Max
**Approach:** QMK userspace (`QMK_USERSPACE`)

---

## Overview

This repo is a QMK userspace — it contains only customizations for the Keychron Q6 Max, not QMK itself. QMK is installed globally via `qmk setup` and reads this repo via `qmk.json`. The repo supports three build workflows: QMK CLI, a Makefile wrapper, and Docker.

---

## Directory Structure

```
keychron-firmware/
├── qmk.json                        # Declares userspace version and managed keyboards/keymaps
├── Makefile                        # Wraps qmk compile / qmk flash / docker build
├── Dockerfile                      # Containerized build environment (qmkfm/qmk_cli base)
├── keyboards/
│   └── keychron/
│       └── q6_max/
│           └── keymaps/
│               └── ecoutu/
│                   ├── keymap.c    # Keymap: layers, macros, custom keycodes
│                   ├── config.h    # Per-keymap config overrides
│                   └── rules.mk    # Feature flags (Via, tap-dance, encoders, etc.)
└── docs/
    └── superpowers/
        └── specs/
```

---

## Components

### `qmk.json`
Declares `userspace_version` and the keyboards/keymaps managed by this repo. Tells `qmk compile` to find the keymap here without needing the full `-kb`/`-km` flags each time.

### `keymap.c`
Defines all layers (base layer, function layer, Via-editable layers), macros, and any custom keycodes. This is the primary file for key behavior customization.

### `config.h`
Per-keymap configuration overrides: debounce timing, RGB brightness limits, mouse key speed, tapping term for tap-dance, etc.

### `rules.mk`
Feature flags enabling QMK modules:
- `VIA_ENABLE = yes` — enables Via live remapping
- `TAP_DANCE_ENABLE = yes` — enables tap-dance keys
- Encoder support flags as needed

### `Makefile`
Convenience wrapper with the following targets:
- `make setup` — runs `qmk setup` to install/update QMK globally
- `make build` — runs `qmk compile -kb keychron/q6_max -km ecoutu`
- `make flash` — runs `qmk flash -kb keychron/q6_max -km ecoutu`
- `make docker-build` — builds firmware inside a Docker container
- `make docker-flash` — flashes firmware built via Docker

### `Dockerfile`
Based on the official `qmkfm/qmk_cli` image, which includes QMK and the full ARM/AVR toolchain. Mounts this repo into the container at the expected userspace path and runs `qmk compile`. No local toolchain required beyond Docker.

---

## Via Support

Via is enabled via `VIA_ENABLE = yes` in `rules.mk`. The Keychron Q6 Max has an existing Via definition in the upstream QMK repo, so the Via app will recognize the keyboard automatically — no custom JSON sideload required.

Vial (open-source Via alternative) is explicitly excluded. Vial requires targeting the `vial-qmk` fork instead of mainline QMK, adding maintenance overhead not warranted for this use case.

---

## Build & Flash Workflows

| Method | Command | Requires |
|--------|---------|----------|
| QMK CLI | `qmk compile -kb keychron/q6_max -km ecoutu` | `qmk setup` run once |
| Makefile | `make build` / `make flash` | QMK CLI installed |
| Docker | `make docker-build` / `make docker-flash` | Docker only |

All three methods produce the same `.bin`/`.hex` output.

---

## Sync with Upstream QMK

Since this is a userspace (not a fork), upstream QMK updates are consumed by running `qmk setup` again or `qmk upgrade`. There is no submodule or subtree to manage. New QMK features and bug fixes become available automatically on the next `qmk setup`.
