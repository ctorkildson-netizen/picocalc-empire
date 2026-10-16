#!/bin/bash
set -e

# Apply the patch
patch -p1 < ./patches/empire.patch

# Force a fresh mtime on every empire source/header file. Ninja decides
# whether to recompile a .c file by comparing mtimes (its own, and the
# headers it #includes) -- never by looking at file content. This
# custom command deletes and recreates empire/ from modules/empire on
# every run, but that copy can preserve the original files' old
# timestamps, so a re-patch with genuinely different content can still
# look "unchanged" to Ninja -- which is exactly what caused a stale
# binary to link silently after empire.h was edited. Touching every
# file here guarantees Ninja always sees them as newer than whatever
# object files it already built, so a real rebuild always happens.
touch empire/*.c empire/*.h

echo "Empire patches applied successfully."
