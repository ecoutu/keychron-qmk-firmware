# QMK Userspace Base Project Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Scaffold a QMK userspace repo for the Keychron Q6 Max with Via support, a keymap, and three build workflows (QMK CLI, Makefile, Docker).

**Architecture:** This is a QMK userspace — a standalone repo containing only keyboard customizations. QMK firmware is installed globally via `qmk setup`; this repo is registered with QMK via `qmk.json`. The keymap lives at `keyboards/keychron/q6_max/keymaps/ecoutu/`.

**Tech Stack:** QMK firmware (mainline), QMK CLI (`qmk` Python tool), Docker (`qmkfm/qmk_cli` image), GNU Make, C (keymap), Via

---

## File Map

| File | Action | Responsibility |
|------|--------|----------------|
| `qmk.json` | Create | Registers this repo as a QMK userspace |
| `Makefile` | Create | Wraps build/flash/setup commands |
| `Dockerfile` | Create | Containerized build environment |
| `keyboards/keychron/q6_max/keymaps/ecoutu/keymap.c` | Create | Keymap layers and custom keycodes |
| `keyboards/keychron/q6_max/keymaps/ecoutu/config.h` | Create | Per-keymap config overrides |
| `keyboards/keychron/q6_max/keymaps/ecoutu/rules.mk` | Create | Feature flags (Via, tap-dance, etc.) |
| `.gitignore` | Create | Ignore build artifacts |

---

## Task 1: Install QMK CLI and set up the toolchain

**Files:** none (system-level setup)

- [ ] **Step 1: Install QMK CLI via pip**

```bash
python3 -m pip install --user qmk
```

Expected output: `Successfully installed qmk-...`

- [ ] **Step 2: Verify QMK CLI is on PATH**

```bash
qmk --version
```

Expected output: a version string like `1.1.5`. If `qmk: command not found`, add `~/.local/bin` to your PATH:

```bash
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.zshrc && source ~/.zshrc
```

- [ ] **Step 3: Run QMK setup**

```bash
qmk setup --yes
```

This clones `qmk/qmk_firmware` into `~/qmk_firmware` and installs the ARM/AVR toolchain. Answer "yes" to all prompts. This may take several minutes.

Expected final line: `QMK is ready to go`

- [ ] **Step 4: Verify the Keychron Q6 Max keyboard definition exists in QMK**

```bash
ls ~/qmk_firmware/keyboards/keychron/q6_max/
```

Expected: a directory listing with files like `info.json`, `keymaps/`, etc. If this path doesn't exist, the keyboard may be under a variant path — check `ls ~/qmk_firmware/keyboards/keychron/` and adjust the keyboard name used in subsequent tasks accordingly.

---

## Task 2: Initialize the repo and register the userspace

**Files:**
- Create: `qmk.json`
- Create: `.gitignore`

- [ ] **Step 1: Confirm you are in the project directory**

```bash
pwd
```

Expected: `/home/ecoutu/ecoutu/src/keychron-firmware`

- [ ] **Step 2: Create `qmk.json`**

```bash
cat > qmk.json << 'EOF'
{
    "userspace_version": "1",
    "build_targets": [
        ["keychron/q6_max", "ecoutu"]
    ]
}
EOF
```

This tells QMK CLI which keyboard/keymap pair this userspace manages. The `build_targets` entry means `qmk compile` with no flags will build `keychron/q6_max` with the `ecoutu` keymap.

- [ ] **Step 3: Register this directory as the QMK userspace**

```bash
qmk config user.qmk_home=~/qmk_firmware
export QMK_USERSPACE=$(pwd)
```

To make the userspace path permanent, add it to your shell profile:

```bash
echo "export QMK_USERSPACE=/home/ecoutu/ecoutu/src/keychron-firmware" >> ~/.zshrc
source ~/.zshrc
```

- [ ] **Step 4: Create `.gitignore`**

```bash
cat > .gitignore << 'EOF'
# QMK build output
*.hex
*.bin
*.uf2
*.map
*.elf

# macOS
.DS_Store

# Editor
.vscode/
*.swp
EOF
```

- [ ] **Step 5: Commit**

```bash
git add qmk.json .gitignore
git commit -m "feat: initialize QMK userspace for Keychron Q6 Max"
```

---

## Task 3: Create the keymap directory and `rules.mk`

**Files:**
- Create: `keyboards/keychron/q6_max/keymaps/ecoutu/rules.mk`

- [ ] **Step 1: Create the keymap directory**

```bash
mkdir -p keyboards/keychron/q6_max/keymaps/ecoutu
```

- [ ] **Step 2: Create `rules.mk`**

```bash
cat > keyboards/keychron/q6_max/keymaps/ecoutu/rules.mk << 'EOF'
# Via live remapping support
VIA_ENABLE = yes

# Tap-dance (double-tap keys for alternate behavior)
TAP_DANCE_ENABLE = yes
EOF
```

`VIA_ENABLE = yes` enables the Via protocol so the Via desktop app can remap keys over USB without reflashing. `TAP_DANCE_ENABLE = yes` enables the tap-dance feature for keys that behave differently on single vs. double tap.

- [ ] **Step 3: Commit**

```bash
git add keyboards/
git commit -m "feat: add keymap directory and rules.mk with Via and tap-dance"
```

---

## Task 4: Create `config.h`

**Files:**
- Create: `keyboards/keychron/q6_max/keymaps/ecoutu/config.h`

- [ ] **Step 1: Create `config.h`**

```c
// keyboards/keychron/q6_max/keymaps/ecoutu/config.h
#pragma once

// Debounce: time in ms to wait after a key state change before registering it.
// Default is 5. Lower = more responsive, higher = fewer false triggers.
#define DEBOUNCE 5

// Tapping term: max ms between press and release to count as a tap (not a hold).
// Used by tap-dance and mod-tap keys. Default is 200.
#define TAPPING_TERM 200

// RGB brightness cap (0-255). Limits max brightness to reduce heat and power draw.
#define RGB_MATRIX_MAXIMUM_BRIGHTNESS 180
```

- [ ] **Step 2: Commit**

```bash
git add keyboards/keychron/q6_max/keymaps/ecoutu/config.h
git commit -m "feat: add config.h with debounce, tapping term, and RGB brightness cap"
```

---

## Task 5: Create `keymap.c`

**Files:**
- Create: `keyboards/keychron/q6_max/keymaps/ecoutu/keymap.c`

- [ ] **Step 1: Look up the Q6 Max layer count and layout macro**

```bash
cat ~/qmk_firmware/keyboards/keychron/q6_max/info.json | python3 -m json.tool | grep -E '"layouts"|"LAYOUT"'
```

Note the layout macro name (e.g., `LAYOUT_ansi_98`) — you'll use it in Step 2.

- [ ] **Step 2: Create `keymap.c` with a base layer and a function layer**

The Q6 Max is a 98-key (96%) layout. The base layer mirrors the stock Keychron layout. The function layer (`FN`) activates via the Fn key and provides media controls and RGB controls on the top row.

```c
// keyboards/keychron/q6_max/keymaps/ecoutu/keymap.c
#include QMK_KEYBOARD_H

enum layers {
    MAC_BASE,
    MAC_FN,
    WIN_BASE,
    WIN_FN,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [MAC_BASE] = LAYOUT_ansi_98(
        KC_ESC,   KC_BRID,  KC_BRIU,  KC_MCTL,  KC_LPAD,  RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,            KC_DEL,   KC_INS,   KC_PGUP,  KC_PGDN,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,  KC_NUM,   KC_PSLS,  KC_PAST,  KC_PMNS,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,  KC_P7,    KC_P8,    KC_P9,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,            KC_ENT,   KC_P4,    KC_P5,    KC_P6,    KC_PPLS,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,            KC_RSFT,  KC_P1,    KC_P2,    KC_P3,
        KC_LCTL,  KC_LOPT,  KC_LCMD,                                KC_SPC,                                 KC_RCMD,  KC_ROPT,  MO(MAC_FN), KC_RCTL,  KC_P0,              KC_PDOT,  KC_PENT
    ),

    [MAC_FN] = LAYOUT_ansi_98(
        _______,  KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,             _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  _______,  _______,  _______,  _______,  _______,  _______,            _______,  _______,  _______,  _______,  _______,
        _______,            _______,  _______,  _______,  _______,  _______,  NK_TOGG,  _______,  _______,  _______,  _______,            _______,  _______,  _______,  _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,  _______,            _______,  _______
    ),

    [WIN_BASE] = LAYOUT_ansi_98(
        KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,             KC_DEL,   KC_INS,   KC_PGUP,  KC_PGDN,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,  KC_NUM,   KC_PSLS,  KC_PAST,  KC_PMNS,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,  KC_P7,    KC_P8,    KC_P9,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,            KC_ENT,   KC_P4,    KC_P5,    KC_P6,    KC_PPLS,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,            KC_RSFT,  KC_P1,    KC_P2,    KC_P3,
        KC_LCTL,  KC_LWIN,  KC_LALT,                                KC_SPC,                                 KC_RALT,  KC_RWIN,  MO(WIN_FN), KC_RCTL,  KC_P0,              KC_PDOT,  KC_PENT
    ),

    [WIN_FN] = LAYOUT_ansi_98(
        _______,  KC_BRID,  KC_BRIU,  KC_TASK,  KC_FILE,  RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,            _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  _______,  _______,  _______,  _______,  _______,  _______,            _______,  _______,  _______,  _______,  _______,
        _______,            _______,  _______,  _______,  _______,  _______,  NK_TOGG,  _______,  _______,  _______,  _______,            _______,  _______,  _______,  _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,  _______,            _______,  _______
    ),
};
```

> **Note:** `LAYOUT_ansi_98` is the expected macro name — verify it against the output of Step 1. If the macro name differs, update all four layer definitions. `KC_LOPT`/`KC_ROPT` are macOS Option keys; `KC_LWIN`/`KC_RWIN` are Windows keys.

- [ ] **Step 3: Commit**

```bash
git add keyboards/keychron/q6_max/keymaps/ecoutu/keymap.c
git commit -m "feat: add keymap with Mac and Windows base/fn layers"
```

---

## Task 6: Create the Makefile

**Files:**
- Create: `Makefile`

- [ ] **Step 1: Create `Makefile`**

```makefile
# Makefile — convenience wrapper for QMK build and flash commands

KEYBOARD  := keychron/q6_max
KEYMAP    := ecoutu
IMAGE     := qmkfm/qmk_cli
USERSPACE := $(shell pwd)
QMK_HOME  := $(HOME)/qmk_firmware

.PHONY: setup build flash docker-build docker-flash

## Install or update QMK globally
setup:
	qmk setup --yes

## Compile firmware using local QMK CLI
build:
	QMK_USERSPACE=$(USERSPACE) qmk compile -kb $(KEYBOARD) -km $(KEYMAP)

## Compile and flash firmware using local QMK CLI
flash:
	QMK_USERSPACE=$(USERSPACE) qmk flash -kb $(KEYBOARD) -km $(KEYMAP)

## Compile firmware inside Docker (no local toolchain required)
docker-build:
	docker run --rm \
		-v $(USERSPACE):/qmk_userspace \
		-v $(QMK_HOME):/qmk_firmware \
		-e QMK_USERSPACE=/qmk_userspace \
		-w /qmk_firmware \
		$(IMAGE) \
		qmk compile -kb $(KEYBOARD) -km $(KEYMAP)

## Flash firmware built via Docker (requires keyboard to be in bootloader mode)
docker-flash:
	docker run --rm --privileged \
		-v $(USERSPACE):/qmk_userspace \
		-v $(QMK_HOME):/qmk_firmware \
		-v /dev:/dev \
		-e QMK_USERSPACE=/qmk_userspace \
		-w /qmk_firmware \
		$(IMAGE) \
		qmk flash -kb $(KEYBOARD) -km $(KEYMAP)
```

- [ ] **Step 2: Verify the Makefile syntax**

```bash
make --dry-run build
```

Expected output: the `qmk compile` command printed but not executed. No errors.

- [ ] **Step 3: Commit**

```bash
git add Makefile
git commit -m "feat: add Makefile with build, flash, and docker targets"
```

---

## Task 7: Create the Dockerfile

**Files:**
- Create: `Dockerfile`

- [ ] **Step 1: Create `Dockerfile`**

```dockerfile
# Dockerfile — containerized QMK build environment
# Based on the official QMK CLI image which includes QMK firmware
# and the full ARM/AVR toolchain.
#
# Usage (via Makefile):
#   make docker-build
#   make docker-flash
#
# Usage (manual):
#   docker build -t qmk-ecoutu .
#   docker run --rm \
#     -v $(pwd):/qmk_userspace \
#     -v ~/qmk_firmware:/qmk_firmware \
#     -e QMK_USERSPACE=/qmk_userspace \
#     -w /qmk_firmware \
#     qmk-ecoutu \
#     qmk compile -kb keychron/q6_max -km ecoutu

FROM qmkfm/qmk_cli

# Set default working directory to QMK firmware root (mounted at runtime)
WORKDIR /qmk_firmware

# Default command: compile the ecoutu keymap
CMD ["qmk", "compile", "-kb", "keychron/q6_max", "-km", "ecoutu"]
```

- [ ] **Step 2: Verify Docker is available**

```bash
docker --version
```

Expected: `Docker version 24.x.x` or similar. If Docker is not installed, install it from https://docs.docker.com/engine/install/.

- [ ] **Step 3: Pull the base image to verify it exists**

```bash
docker pull qmkfm/qmk_cli
```

Expected: image layers downloaded, ending with `Status: Downloaded newer image for qmkfm/qmk_cli:latest`.

- [ ] **Step 4: Commit**

```bash
git add Dockerfile
git commit -m "feat: add Dockerfile based on qmkfm/qmk_cli"
```

---

## Task 8: Verify the full build

This task confirms everything works end-to-end before calling the project complete.

- [ ] **Step 1: Run a local build**

```bash
make build
```

Expected: QMK compiles and produces a `.bin` or `.hex` file. The last line will be something like:
```
Copying keychron_q6_max_ecoutu.bin to /home/ecoutu/ecoutu/src/keychron-firmware/
```

If you see errors about the layout macro, revisit Task 5 Step 1 and correct the macro name in `keymap.c`.

- [ ] **Step 2: Run a Docker build**

```bash
make docker-build
```

Expected: same `.bin`/`.hex` output produced inside the container and written to the project directory.

- [ ] **Step 3: Confirm the output file exists**

```bash
ls *.bin *.hex *.uf2 2>/dev/null
```

Expected: at least one firmware file listed.

- [ ] **Step 4: Commit any fixes discovered during build verification**

```bash
git add -p
git commit -m "fix: correct layout macro / build issues found during verification"
```

(Skip this step if no fixes were needed.)

---

## Flashing to the Keyboard

> This is a reference section, not a task. Flash only when you're ready to update the keyboard.

1. Put the Q6 Max into bootloader mode: hold the key on the bottom of the keyboard labeled `Boot` (or `Reset`) while plugging in USB, OR hold `Fn + Space` for 3 seconds (check your keyboard's manual).
2. Run `make flash` (or `make docker-flash`).
3. QMK will detect the keyboard in bootloader mode and flash the firmware.
4. The keyboard will restart with the new firmware.

---

## Via Setup Reference

After flashing:
1. Download the Via app from https://www.caniusevia.com/ or use the web version at https://usevia.app/
2. Plug in the keyboard.
3. Via should automatically detect the Keychron Q6 Max — no JSON sideload needed.
4. Remap keys, configure layers, and save — changes persist on the keyboard's EEPROM.
