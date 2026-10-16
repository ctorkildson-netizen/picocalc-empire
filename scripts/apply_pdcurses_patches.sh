#!/bin/bash
set -e

# Apply the patch
patch -p1 < ./patches/pdcurses.patch

# See the matching comment in apply_empire_patches.sh: force fresh
# mtimes so Ninja's normal mtime-based dependency tracking always
# notices a patch change, instead of relying on OBJECT_DEPENDS.
touch pdcurses/*.c pdcurses/*.h

echo "PDCurses patches applied successfully."
