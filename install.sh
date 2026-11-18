#!/bin/sh
set -eu

cd "$(dirname "$0")"
if [ -f build-meson/meson-private/coredata.dat ]; then
    CC=clang CXX=clang++ meson setup --reconfigure build-meson --prefix="$HOME/.local"
else
    CC=clang CXX=clang++ meson setup build-meson --prefix="$HOME/.local"
fi
meson compile -C build-meson
meson install -C build-meson
