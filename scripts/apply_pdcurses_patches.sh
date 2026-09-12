#!/bin/bash

# Apply the patch
patch -p1 < ./patches/pdcurses.patch

echo "PDCurses patches applied successfully."
