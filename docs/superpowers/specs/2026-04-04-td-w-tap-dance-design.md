# TD_W Tap Dance Design

**Date:** 2026-04-04
**Layer:** WIN_BASE (layer 2)
**File:** `keyboards/keychron/q6_max/keymaps/ecoutu/keymap.c`

## Summary

Replace `KC_W` in the `WIN_BASE` layer with a tap-dance key (`TD(TD_W)`) that supports three gestures:

| Gesture | Result |
|---|---|
| Single tap | Types W (fires immediately on interrupt, no delay) |
| Hold | Normal W hold; releases when key is released |
| Double-tap (W not held) | Holds W down indefinitely (`register_code`) |
| Double-tap (W held) | Releases the held W (`unregister_code`) |

`MAC_BASE` is unaffected — it keeps plain `KC_W`.

## Implementation

### New enum

```c
enum tap_dance_keycodes {
    TD_W,
};
```

### State types

```c
typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_TAP,
} td_state_t;
```

### `cur_dance()` helper

Classifies the current tap dance based on `state->count`, `state->interrupted`, and `state->pressed`. Returning `TD_SINGLE_TAP` when interrupted (count=1, another key pressed) is what allows the single-tap to fire immediately without waiting for the tap-dance timer.

```c
static td_state_t cur_dance(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) return TD_SINGLE_TAP;
        return TD_SINGLE_HOLD;
    }
    if (state->count == 2) return TD_DOUBLE_TAP;
    return TD_UNKNOWN;
}
```

### Static state

```c
static td_state_t w_tap_state = TD_NONE;
static bool       w_held      = false;
```

`w_held` persists across dances and tracks whether the toggle-hold is active.

### `td_w_finished` callback

```c
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
```

### `td_w_reset` callback

```c
static void td_w_reset(tap_dance_state_t *state, void *user_data) {
    if (w_tap_state == TD_SINGLE_HOLD && !w_held) {
        unregister_code(KC_W);
    }
    w_tap_state = TD_NONE;
}
```

The `!w_held` guard prevents the reset from cancelling an active toggle-hold when the user physically holds the W key while the toggle is on.

### `tap_dance_actions` registration

```c
tap_dance_action_t tap_dance_actions[] = {
    [TD_W] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_w_finished, td_w_reset),
};
```

### Keymap change

In `WIN_BASE`, replace `KC_W` with `TD(TD_W)`. All other layers unchanged.

## Files Changed

- `keyboards/keychron/q6_max/keymaps/ecoutu/keymap.c` — all changes above
- `keyboards/keychron/q6_max/keymaps/ecoutu/rules.mk` — already has `TAP_DANCE_ENABLE = yes`, no change needed

## Edge Cases

- **Double-tap while W is physically held by another means**: not a realistic scenario; ignored.
- **Triple-tap or more**: `cur_dance` returns `TD_UNKNOWN`; the switch falls through to no-op.
- **Single tap while `w_held` is true**: `tap_code(KC_W)` will briefly unregister/re-register W, causing a momentary stutter. Acceptable since mixing single-taps with the toggle-hold is an unusual interaction.
