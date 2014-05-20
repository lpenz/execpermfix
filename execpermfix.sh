#! /bin/sh

set -f

if [ $# != 1 ]; then
    echo "usage: $0 file-or-directory"
    echo "  chmod +x any files that look like executables"
    exit 1
fi

find "$1" \
    -type f \
    -execdir sh -c 'file --brief "$1" | grep -q executable' -- '{}' \; \
    -print0 \
    | xargs -0 chmod --verbose +x
