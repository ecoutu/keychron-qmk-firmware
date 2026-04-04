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
