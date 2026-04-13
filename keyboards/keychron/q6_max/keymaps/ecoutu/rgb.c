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

#include QMK_KEYBOARD_H
#include "layers.h"

void keyboard_post_init_user(void) {
  rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
  rgb_matrix_sethsv(0, 255, RGB_MATRIX_MAXIMUM_BRIGHTNESS);
}

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
