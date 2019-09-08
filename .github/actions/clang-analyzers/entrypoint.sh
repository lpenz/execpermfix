#!/bin/bash

set -e -x

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .

find "$PWD" \( -name '*.[ch]' -o -name '*.cc' \) -exec "$INPUT_ANALYZER" -p "$PWD" {} +
