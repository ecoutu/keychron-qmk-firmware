# Via live remapping support
VIA_ENABLE = yes

# Tap dance support
TAP_DANCE_ENABLE = yes

# Encoder mapping support
ENCODER_MAP_ENABLE = yes

# RGB Matrix enabled - Keychron wireless_playground fork has the required SPI drivers
RGB_MATRIX_ENABLE = yes

SRC += tap_dance.c rgb.c
