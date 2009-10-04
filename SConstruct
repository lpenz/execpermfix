
Program('execpermfix.c', CPPFLAGS='-g -Wall -Werror')
Command('manual.1', 'manual.t2t', 'txt2tags -t man -i $SOURCE -o $TARGET')
Default(['execpermfix', 'manual.1'])

Command('README.textile', 'manual.t2t', 'txt2tags -t textile -H -i $SOURCE -o $TARGET')

