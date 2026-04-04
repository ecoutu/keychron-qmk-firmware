# Dockerfile — containerized QMK build environment
# Based on the official QMK CLI image which includes QMK firmware
# and the full ARM/AVR toolchain.
#
# Usage (via Makefile):
#   make docker-build
#   make docker-flash
#
# Usage (manual):
#   docker run --rm \
#     -v $(pwd):/qmk_userspace \
#     -v ~/qmk_firmware:/qmk_firmware \
#     -e QMK_USERSPACE=/qmk_userspace \
#     -w /qmk_firmware \
#     qmkfm/qmk_cli \
#     qmk compile -kb keychron/q6_max -km ecoutu

FROM qmkfm/qmk_cli

# Set default working directory to QMK firmware root (mounted at runtime)
WORKDIR /qmk_firmware

# Default command: compile the ecoutu keymap
CMD ["qmk", "compile", "-kb", "keychron/q6_max", "-km", "ecoutu"]
