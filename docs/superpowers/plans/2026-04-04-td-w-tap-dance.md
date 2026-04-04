# TD_W Tap Dance Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add a tap-dance W key to WIN_BASE that types W on single tap, holds W on physical hold, and toggles W held-down on double-tap (double-tap again to release).

**Architecture:** All changes are in `keymap.c`. A `cur_dance()` helper classifies the gesture; `td_w_finished` and `td_w_reset` callbacks handle the three cases. A static `w_held` bool persists the toggle state across dances.

**Tech Stack:** QMK firmware C, tap dance API (`ACTION_TAP_DANCE_FN_ADVANCED`), `TAP_DANCE_ENABLE = yes` already set in `rules.mk`.

---

### Task 1: Add the tap-dance enum and wire it into WIN_BASE

**Files:**
- Modify: `keyboards/keychron/q6_max/keymaps/ecoutu/keymap.c`

There are no automated tests for QMK keymaps — verification is a successful `make build`. We structure commits so each one compiles cleanly.

- [ ] **Step 1: Add `enum tap_dance_keycodes` after `enum layers`**

In `keymap.c`, find:

```c
enum layers {
  MAC_BASE,
  MAC_FN,
  WIN_BASE,
  WIN_FN,
};
```

Add immediately after (before the `// clang-format off` line):

```c
#if defined(TAP_DANCE_ENABLE)
enum tap_dance_keycodes {
    TD_W,
};
#endif
```

- [ ] **Step 2: Replace `KC_W` with `TD(TD_W)` in WIN_BASE**

In `keymap.c`, find this line inside `[WIN_BASE]`:

```c
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,    KC_BSLS,    KC_DEL,   KC_END,   KC_PGDN,  KC_P7,    KC_P8,    KC_P9,
```

Change `KC_W` to `TD(TD_W)`:

```c
        KC_TAB,   KC_Q,     TD(TD_W), KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,    KC_BSLS,    KC_DEL,   KC_END,   KC_PGDN,  KC_P7,    KC_P8,    KC_P9,
```

- [ ] **Step 3: Build to verify it compiles**

```bash
make build
```

Expected: successful build ending with a `.bin` / `.hex` file. A compile error here means the enum placement is wrong relative to the keymaps — the `TD_W` enum must appear before the `keymaps` array.

- [ ] **Step 4: Commit**

```bash
git add keyboards/keychron/q6_max/keymaps/ecoutu/keymap.c
git commit -m "feat: add TD_W enum and wire into WIN_BASE keymap"
```

---

### Task 2: Add the state types, helper, static vars, callbacks, and register the action

**Files:**
- Modify: `keyboards/keychron/q6_max/keymaps/ecoutu/keymap.c`

- [ ] **Step 1: Replace the existing `tap_dance_actions` block with the full implementation**

Find the existing block:

```c
#if defined(TAP_DANCE_ENABLE)
tap_dance_action_t tap_dance_actions[] = {
    // Tap dance actions can be added here
};
#endif // TAP_DANCE_ENABLE
```

Replace it entirely with:

```c
#if defined(TAP_DANCE_ENABLE)
typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_TAP,
} td_state_t;

static td_state_t cur_dance(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) return TD_SINGLE_TAP;
        return TD_SINGLE_HOLD;
    }
    if (state->count == 2) return TD_DOUBLE_TAP;
    return TD_UNKNOWN;
}

static td_state_t w_tap_state = TD_NONE;
static bool       w_held      = false;

static void td_w_finished(tap_dance_state_t *state, void *user_data) {
    w_tap_state = cur_dance(state);
    switch (w_tap_state) {
        case TD_SINGLE_TAP:
            tap_code(KC_W);
            break;
        case TD_SINGLE_HOLD:
            if (!w_held) register_code(KC_W);
            break;
        case TD_DOUBLE_TAP:
            if (w_held) {
                unregister_code(KC_W);
                w_held = false;
            } else {
                register_code(KC_W);
                w_held = true;
            }
            break;
        default:
            break;
    }
}

static void td_w_reset(tap_dance_state_t *state, void *user_data) {
    if (w_tap_state == TD_SINGLE_HOLD && !w_held) {
        unregister_code(KC_W);
    }
    w_tap_state = TD_NONE;
}

tap_dance_action_t tap_dance_actions[] = {
    [TD_W] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_w_finished, td_w_reset),
};
#endif // TAP_DANCE_ENABLE
```

- [ ] **Step 2: Build to verify it compiles**

```bash
make build
```

Expected: successful build. Common failure modes:
- `td_state_t` redefined → you accidentally left the old block in place; delete the duplicate.
- `TD_W` undeclared in `tap_dance_actions` → enum from Task 1 is inside a different `#if` scope; check that both `#if defined(TAP_DANCE_ENABLE)` guards are present.

- [ ] **Step 3: Commit**

```bash
git add keyboards/keychron/q6_max/keymaps/ecoutu/keymap.c
git commit -m "feat: implement TD_W tap dance callbacks and register action"
```

---

### Task 3: Manual verification checklist

**Files:** none (firmware flash + manual test)

No automated tests exist for keymap behavior. Flash the firmware and test each gesture.

- [ ] **Step 1: Put the keyboard in bootloader mode**

Hold the `Boot` button on the underside while plugging in USB, or hold `Fn + Space` for 3 seconds.

- [ ] **Step 2: Flash**

```bash
make flash
```

Expected: firmware flashes successfully and keyboard reconnects.

- [ ] **Step 3: Verify on WIN_BASE — single tap**

Switch to Windows layer (the keyboard defaults to WIN_BASE on power-up if last used layer was Windows). Open a text editor. Press W once quickly. Expected: one `w` character appears with no noticeable delay when followed by another key (the interrupt path fires immediately).

- [ ] **Step 4: Verify on WIN_BASE — physical hold**

Hold W for ~1 second. Expected: `wwwwww` repeating (auto-repeat). Release. Expected: W stops.

- [ ] **Step 5: Verify on WIN_BASE — double-tap hold toggle**

Double-tap W quickly. Expected: `w` starts repeating continuously without holding the key. Open a game or use a key-event viewer to confirm the key is held. Double-tap W again. Expected: W stops.

- [ ] **Step 6: Verify MAC_BASE is unaffected**

Toggle to Mac layer (Fn+1 or the layer toggle, per your physical setup). Type W. Expected: normal W with no tap-dance delay.

- [ ] **Step 7: Commit verification note**

```bash
git commit --allow-empty -m "chore: manually verified TD_W tap dance on WIN_BASE"
```
