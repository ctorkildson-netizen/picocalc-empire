#!/bin/bash
set -e

# Apply the patch
patch -p1 < ./patches/empire.patch

echo "Empire patches applied successfully."
