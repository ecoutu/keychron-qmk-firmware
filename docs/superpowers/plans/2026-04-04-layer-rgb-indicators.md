# Layer RGB Indicators Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Show solid red on base layers (MAC_BASE, WIN_BASE) and solid blue on function layers (MAC_FN, WIN_FN), with the W key showing green when held via double-tap.

**Architecture:** `layer_state_set_user` drives the background color by switching RGB Matrix into solid color mode whenever the active layer changes. `keyboard_post_init_user` initializes the same red on boot. The existing `rgb_matrix_indicators_advanced_user` (inside `TAP_DANCE_ENABLE`) is trimmed down to only handle the W key green override when `w_held`.

**Tech Stack:** QMK firmware C, `rgb_matrix_mode_noeeprom`, `rgb_matrix_sethsv_noeeprom`, `g_led_config`, `layer_state_set_user`

---

### Task 1: Add `keyboard_post_init_user` and `layer_state_set_user`

**Files:**
- Modify: `keyboards/keychron/q6_max/keymaps/ecoutu/keymap.c`

No automated tests exist in this project. Build verification is the test.

- [ ] **Step 1: Add `keyboard_post_init_user` after `process_record_user`**

In `keymap.c`, append after the closing `}` of `process_record_user` (currently the last function, ending at line 154):

```c
void keyboard_post_init_user(void) {
  rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
  rgb_matrix_sethsv_noeeprom(0, 255, RGB_MATRIX_MAXIMUM_BRIGHTNESS);
}
```

- [ ] **Step 2: Add `layer_state_set_user` immediately after `keyboard_post_init_user`**

```c
layer_state_t layer_state_set_user(layer_state_t state) {
  uint8_t hue;
  switch (get_highest_layer(state)) {
    case MAC_FN:
    case WIN_FN:
      hue = 170;
      break;
    default:
      hue = 0;
      break;
  }
  rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
  rgb_matrix_sethsv_noeeprom(hue, 255, rgb_matrix_get_val());
  return state;
}
```

Note: hue 0 = red, hue 170 ≈ blue on QMK's 0–255 scale. `rgb_matrix_get_val()` preserves user brightness. `_noeeprom` avoids flash writes on every layer change.

- [ ] **Step 3: Build to verify it compiles**

```bash
make build
```

Expected: build completes with no errors. Warnings about unused variables are acceptable; errors are not.

- [ ] **Step 4: Commit**

```bash
git add keyboards/keychron/q6_max/keymaps/ecoutu/keymap.c
git commit -m "feat: set RGB to red on base layers, blue on fn layers"
```

---

### Task 2: Update `rgb_matrix_indicators_advanced_user` to W-key-only green

**Files:**
- Modify: `keyboards/keychron/q6_max/keymaps/ecoutu/keymap.c` (inside `TAP_DANCE_ENABLE` guard)

- [ ] **Step 1: Replace the body of `rgb_matrix_indicators_advanced_user`**

Find the existing function (currently lines 130–137):

```c
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
  if (w_held) {
    for (uint8_t i = led_min; i < led_max; i++) {
      rgb_matrix_set_color(i, 0, 0, 255);
    }
  }
  return true;
}
```

Replace the entire function body with:

```c
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
  if (w_held) {
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
      for (uint8_t col = 0; col < MATRIX_COLS; col++) {
        uint8_t led_i = g_led_config.matrix_co[row][col];
        if (led_i == NO_LED || led_i < led_min || led_i >= led_max) continue;
        keypos_t pos = {.row = row, .col = col};
        if (keymap_key_to_keycode(WIN_BASE, pos) == TD(TD_W)) {
          rgb_matrix_set_color(led_i, 0, 255, 0);
        }
      }
    }
  }
  return true;
}
```

How this works: `g_led_config.matrix_co[row][col]` gives the LED index for each key position. We skip `NO_LED` entries (keys with no LED) and entries outside the current LED range. `keymap_key_to_keycode(WIN_BASE, pos)` looks up what keycode is at that position on the WIN_BASE layer — when it matches `TD(TD_W)`, we set that LED green (RGB 0,255,0). Returns `true` so QMK continues processing other indicator hooks.

- [ ] **Step 2: Build to verify it compiles**

```bash
make build
```

Expected: build completes with no errors.

- [ ] **Step 3: Commit**

```bash
git add keyboards/keychron/q6_max/keymaps/ecoutu/keymap.c
git commit -m "feat: show green on W key only when w_held, replace all-blue override"
```

---

### Task 3: Flash and manually verify

- [ ] **Step 1: Put keyboard in bootloader mode**

Hold the `Boot` button on the underside of the keyboard while plugging in USB (or hold `Fn + Space` for 3 seconds).

- [ ] **Step 2: Flash**

```bash
make flash
```

Expected: firmware flashes successfully, keyboard reconnects.

- [ ] **Step 3: Verify layer colors**

| Action | Expected RGB |
|--------|-------------|
| Keyboard powers on | All keys red |
| Hold Mac Fn key | All keys blue |
| Release Mac Fn key | All keys red |
| Hold Win Fn key | All keys blue |
| Release Win Fn key | All keys red |

- [ ] **Step 4: Verify W-held indicator**

| Action | Expected RGB |
|--------|-------------|
| On WIN_BASE (red background), double-tap W | W key turns green; rest stays red |
| Double-tap W again to release | W key returns to red |
| Switch to WIN_FN (blue background) while W is held | W key stays green; rest is blue |
