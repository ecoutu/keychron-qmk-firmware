---
name: add-layer
description: Scaffold a new QMK layer in keymap.c following the existing pattern
---

The user wants to add a new layer to keyboards/keychron/q6_max/keymaps/ecoutu/keymap.c.

Steps:
1. Read the current keymap.c and rules.mk
2. Ask the user: layer name, which base layer to copy from (transparent vs explicit keys), and what key (MO, TG, etc.) will activate it
3. Add the layer to the `enum layers` block
4. Add a fully-populated `LAYOUT_109_ansi(...)` entry — copy the closest existing layer, replace non-transparent keys as requested
5. If ENCODER_MAP_ENABLE is set in rules.mk, add an encoder_map entry for the new layer
6. Remind the user to add an activation key (e.g. `MO(NEW_LAYER)`) on a reachable key in an existing layer
