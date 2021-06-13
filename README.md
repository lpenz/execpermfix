[![CI](https://github.com/lpenz/execpermfix/actions/workflows/ci.yml/badge.svg)](https://github.com/lpenz/execpermfix/actions/workflows/ci.yml)
[![coveralls](https://coveralls.io/repos/github/lpenz/execpermfix/badge.svg?branch=main)](https://coveralls.io/github/lpenz/execpermfix?branch=main)
[![packagecloud](https://img.shields.io/badge/deb-packagecloud.io-844fec.svg)](https://packagecloud.io/app/lpenz/debian/search?q=execpermfix)

execpermfix
===========

execpermfix is a tool that fixes executable permissions of files and
directories.

It first finds out if the file is executable or not by checking if it starts
with the ELF magic number or "#!".

If the file is executable, execpermfix sets the executable bit of
user/group/other only if the read bit is set. If the file is not executable,
all executable bits are cleared.

Directories are always considered executable.


## Usage

~~~[.sh]
execpermfix -h
execpermfix -V
execpermfix [-v] [-n] <files/directories>
~~~

### Options

**-h** Help.

**-V** Version information.

**-v** Verbose, show files changed.

**-n** Dry-run: do not change any permissions. Use with ``-v`` to check file with wrong permissions.


