// keyboards/keychron/q6_max/keymaps/ecoutu/config.h
#pragma once

// Debounce: time in ms to wait after a key state change before registering it.
// The keyboard default is 20ms. Override here to tune responsiveness.
#undef DEBOUNCE
#define DEBOUNCE 5

// Tapping term: max ms between press and release to count as a tap (not a hold).
// Used by tap-dance and mod-tap keys. Default is 200.
#define TAPPING_TERM 200

// RGB brightness cap (0-255). Limits max brightness to reduce heat and power draw.
#define RGB_MATRIX_MAXIMUM_BRIGHTNESS 180
