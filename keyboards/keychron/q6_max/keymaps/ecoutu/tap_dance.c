/* Copyright 2024 @ Keychron (https://www.keychron.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "layers.h"
#include "tap_dance.h"

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

static td_state_t q_tap_state = TD_NONE;
static bool       q_held      = false;

static void td_q_each(tap_dance_state_t *state, void *user_data) {
    if (state->count == 1 && state->pressed && !q_held) {
        register_code(KC_Q);
    }
}

static void td_q_finished(tap_dance_state_t *state, void *user_data) {
    q_tap_state = cur_dance(state);
    switch (q_tap_state) {
        case TD_SINGLE_TAP:
            if (!q_held) unregister_code(KC_Q);
            break;
        case TD_SINGLE_HOLD:
            break;
        case TD_DOUBLE_TAP:
            if (q_held) {
                unregister_code(KC_Q);
                q_held = false;
            } else {
                q_held = true;
            }
            break;
        default:
            if (!q_held) unregister_code(KC_Q);
            break;
    }
}

static void td_q_reset(tap_dance_state_t *state, void *user_data) {
    if (q_tap_state == TD_SINGLE_HOLD && !q_held) {
        unregister_code(KC_Q);
    }
    q_tap_state = TD_NONE;
}

static td_state_t w_tap_state = TD_NONE;
static bool       w_held      = false;

static void td_w_each(tap_dance_state_t *state, void *user_data) {
    // Register W immediately on first press to eliminate hold delay
    if (state->count == 1 && state->pressed && !w_held) {
        register_code(KC_W);
    }
}

static void td_w_finished(tap_dance_state_t *state, void *user_data) {
    w_tap_state = cur_dance(state);
    switch (w_tap_state) {
        case TD_SINGLE_TAP:
            // W was registered in td_w_each; unregister to complete the tap
            if (!w_held) unregister_code(KC_W);
            break;
        case TD_SINGLE_HOLD:
            // W already registered in td_w_each; nothing to do
            break;
        case TD_DOUBLE_TAP:
            if (w_held) {
                unregister_code(KC_W);
                w_held = false;
            } else {
                // W already registered in td_w_each; just set the held state
                w_held = true;
            }
            break;
        default:
            if (!w_held) unregister_code(KC_W);
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
    [TD_Q] = ACTION_TAP_DANCE_FN_ADVANCED(td_q_each, td_q_finished, td_q_reset),
    [TD_W] = ACTION_TAP_DANCE_FN_ADVANCED(td_w_each, td_w_finished, td_w_reset),
};

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    if (!q_held && !w_held) return true;
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint8_t led_i = g_led_config.matrix_co[row][col];
            if (led_i == NO_LED || led_i < led_min || led_i >= led_max) continue;
            keypos_t pos     = {.row = row, .col = col};
            uint16_t keycode = keymap_key_to_keycode(WIN_BASE, pos);
            if ((q_held && keycode == TD(TD_Q)) || (w_held && keycode == TD(TD_W))) {
                rgb_matrix_set_color(led_i, 0, 255, 0);
            }
        }
    }
    return true;
}

#endif // TAP_DANCE_ENABLE
