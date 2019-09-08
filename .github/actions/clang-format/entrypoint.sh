#!/bin/bash

set -e -x

find "$PWD" \( -name '*.[ch]' -o -name '*.cc' \) -exec clang-format -i {} +

