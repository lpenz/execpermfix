/****************************************************************************/
/**
 * \file
 * \brief  execpermfix test helper utility.
 */
/****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "execpermfix.h"

int main(int argc, char *argv[])
{
	const char *filename;
	mode_t pre;
	mode_t pos;
	struct stat st;

	(void)argc;

	filename = argv[1];
	pre = strtol(argv[2], NULL, 0);
	pos = strtol(argv[3], NULL, 0);

	chmod(filename, pre);
	execpermfix(filename, 0, 0);
	stat(filename, &st);
	printf("filename %s, pre 0%o, pos 0%o, curr 0%o\n", filename, pre, pos, st.st_mode);

	return pos == (0777 & st.st_mode) ? 0 : 1;
}

