#!/bin/bash

# Apply the patch
patch -p1 < ./patches/empire.patch

echo "Empire patches applied successfully."
