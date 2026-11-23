#!/bin/sh
# What including the four headers adds to one TU: its sections; the static
# initializer is __StaticInit (macOS) or .text.startup (Linux). Release, so configure the build dir with
# -G Ninja -DCMAKE_BUILD_TYPE=Release. Usage: test/init_size.sh BUILD_DIR
set -eu
build=$1
cmake --build "$build" --target soundex_include_all >/dev/null
obj=$(find "$build" -name include_all.cc.o | head -1)
if [ "$(uname)" = Darwin ]; then
	size -m "$obj"
else
	size -A "$obj"
fi
