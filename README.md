execpermfix
===========

[![Build Status](https://travis-ci.org/lpenz/execpermfix.png?branch=master)](https://travis-ci.org/lpenz/execpermfix) 
[![Coverage Status](https://coveralls.io/repos/lpenz/execpermfix/badge.png?branch=master)](https://coveralls.io/r/lpenz/execpermfix?branch=master)

# About

execpermfix is a tool that fixes executable permissions of files and
directories.

It first finds out if the file is executable or not by checking if it starts
with the ELF magic number or "#!".

If the file is executable, execpermfix sets the executable bit of
user/group/other only if the read bit is set. If the file is not executable,
all executable bits are cleared.

Directories are always considered executable.


# Usage

~~~[.sh]
execpermfix -h
execpermfix -V
execpermfix [-v] [-n] <files/directories>
~~~

## Options

**-h** Help.

**-V** Version information.

**-v** Verbose, show files changed.

**-n** Dry-run: do not change any permissions. Use with ``-v`` to check file with wrong permissions.


