# Layer RGB Indicators Design

**Date:** 2026-04-04
**Status:** Approved

## Overview

Add layer-aware RGB background colors to the Keychron Q6 Max custom keymap. Base layers (MAC_BASE, WIN_BASE) show red; function layers (MAC_FN, WIN_FN) show blue. When the W key is held via double-tap, the W key LED shows green as a per-key override, regardless of the active layer.

## Architecture

Two QMK callbacks handle this feature:

### 1. `layer_state_set_user` — background color

Sets the solid background color whenever the active layer changes.

```c
layer_state_t layer_state_set_user(layer_state_t state) {
    uint8_t hue;
    switch (get_highest_layer(state)) {
        case MAC_FN:
        case WIN_FN:
            hue = 170; // blue
            break;
        default:
            hue = 0; // red
            break;
    }
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
    rgb_matrix_sethsv_noeeprom(hue, 255, rgb_matrix_get_val());
    return state;
}
```

- `_noeeprom` variants avoid writing to EEPROM on every layer change.
- `rgb_matrix_get_val()` respects the user's current brightness setting.
- Hue 0 = red, hue 170 ≈ blue on the 0–255 QMK scale.

### 2. `keyboard_post_init_user` — startup initialization

Calls the same color logic on boot so the keyboard starts red (MAC_BASE is layer 0).

```c
void keyboard_post_init_user(void) {
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
    rgb_matrix_sethsv_noeeprom(0, 255, RGB_MATRIX_MAXIMUM_BRIGHTNESS);
}
```

### 3. `rgb_matrix_indicators_advanced_user` — W key override

Remains inside the `TAP_DANCE_ENABLE` guard. When `w_held`, scans the key matrix to find the LED index for `TD(TD_W)` in WIN_BASE and sets it green.

```c
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    if (w_held) {
        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
            for (uint8_t col = 0; col < MATRIX_COLS; col++) {
                uint8_t led_i = g_led_config.matrix_co[row][col];
                if (led_i == NO_LED || led_i < led_min || led_i >= led_max) continue;
                keypos_t pos = {.row = row, .col = col};
                if (keymap_key_to_keycode(WIN_BASE, pos) == TD(TD_W)) {
                    rgb_matrix_set_color(led_i, 0, 255, 0); // green
                }
            }
        }
    }
    return true;
}
```

- Returns `true` — background is already handled by the solid color mode.
- Matrix scan avoids hardcoding the LED index, so it stays correct if the key layout changes.
- Green (RGB 0,255,0) is visually distinct from both the red base layer and blue FN layer.

## Layer → Color mapping

| Layer | Index | Color | HSV hue |
|-------|-------|-------|---------|
| MAC_BASE | 0 | red | 0 |
| MAC_FN | 1 | blue | 170 |
| WIN_BASE | 2 | red | 0 |
| WIN_FN | 3 | blue | 170 |
| W held (key only) | any | green | 85 |

## Per-key override pattern

Any future per-key override follows the same pattern in `rgb_matrix_indicators_advanced_user`: find the LED index via `g_led_config.matrix_co` and call `rgb_matrix_set_color` after the background has been set by the solid color mode.

## Files changed

- `keyboards/keychron/q6_max/keymaps/ecoutu/keymap.c` — add `layer_state_set_user`, `keyboard_post_init_user`, update `rgb_matrix_indicators_advanced_user`

No changes to `config.h`, `rules.mk`, or any other file.
