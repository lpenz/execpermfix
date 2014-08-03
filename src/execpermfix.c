/****************************************************************************/
/**
 * \file
 * \brief  Fixes executable permissions of files and directories.
 */
/****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#define PROGRAM_NAME "execpermfix"
#define PROGRAM_VERSION "1.0.5"

/****************************************************************************/

const char USAGE[] =
"Usage: execpermfix [-v] [-h] <paths...>\n"
"Fixes executable permission of files based on the file type and on the\n"
"current read permissions.\n\n"
"    -h    This help.\n"
"    -V    Version information.\n"
"    -v    Verbose mode.\n"
"    -n    Performs a trial run, no changes are made.\n";

/****************************************************************************/

/**
 * \brief  Check if a file should BE executable by its contents.
 * \param  name Path of entity to process.
 * \param  fd File descriptor of entity (result of open).
 * \param  st Stat structure of entity (result of stat).
 * \return 0 if not executable, 1 if executable, -1 if error.
 */
static int fileIsExec(const char *name, int fd, const struct stat *st)
{
	const char scriptMagic[] = "#!";
	struct {
		uint8_t ei_magic[4];
		uint8_t ei_class;
		uint8_t ei_data;
		uint8_t ei_version;
		uint8_t ei_pad[9];
		uint16_t e_type;
	} *eh;
	const char elfMagic[] = { 0x7f, 0x45, 0x4c, 0x46 };
	char buf[sizeof(*eh)];
	int r;
	int s;

	s = st->st_size > (signed)sizeof(*eh) ? (signed)sizeof(*eh) : st->st_size;

	if (s < 2)
		return 0;

	if ((r = read(fd, buf, s)) < 0) {
		fprintf(stderr, "%s: error in read\n", name);
		return -1;
	}
	
	if (r >= (signed)(sizeof scriptMagic - 1)
			&& memcmp(buf, scriptMagic, sizeof scriptMagic - 1) == 0) {
		return 1;
	}

	eh = (typeof(eh))buf;
	if (r >= (signed)sizeof(*eh)
			&& memcmp(eh->ei_magic, elfMagic, sizeof elfMagic) == 0
			&& ((eh->ei_data == 2 && eh->e_type == 0x0200) || (eh->ei_data == 1 && eh->e_type == 0x0002))) {
		return 1;
	}

	return 0;
}

/****************************************************************************/

/**
 * \brief  Processes a file or a directory recursively.
 * \param  verbose 1 to print actions.
 * \param  noop Do not perform any change.
 * \param  name Path of entity to process.
 * \return 0 if ok, 1 if error.
 */
static int doEntity(int verbose, int noop, const char *name)
{
	int fd = 0;
	struct stat st;
	char fullname[PATH_MAX+1];
	int exec;
	mode_t mode;

	if (realpath(name, fullname) == NULL) {
		fprintf(stderr, "%s: realpath error %s\n", name, strerror(errno));
		return 1;
	}

	if ((fd = open(fullname, O_RDONLY)) < 0) {
		fprintf(stderr, "%s: open error %s\n", fullname, strerror(errno));
		return 1;
	}

	if (fstat(fd, &st) != 0) {
		fprintf(stderr, "%s: fstat error %s\n", fullname, strerror(errno));
		close(fd);
		return 1;
	}

	if (!S_ISDIR(st.st_mode) && !S_ISREG(st.st_mode)) {
		if(verbose)
			printf("%s skipped, neither a file nor a directory\n", fullname);
		close(fd);
		return 0;
	}

	/* Decide if file is executable: */
	exec = 0;
	if (S_ISDIR(st.st_mode))
		exec = 1;
	else if (S_ISREG(st.st_mode))
		exec = fileIsExec(fullname, fd, &st);
	if (exec < 0) {
		close(fd);
		return -1;
	}

	if(verbose)
		printf("%s: file is %sexecutable\n", name, exec ? "" : "not ");

	/* Define mode: */
	mode = st.st_mode;
	if (exec)
		mode &= ~0111;
	else
		mode |= (mode & 0444) >> 2;

	if (mode == st.st_mode) {
		if(verbose)
			printf("%s: %o would not be changed\n", name, st.st_mode);
		close(fd);
		return 0;
	}

	/* Set permission: */
	if(noop) {
		if(verbose)
			printf("%s: %o would change to %o\n", name, st.st_mode, mode);
		close(fd);
		return 0;
	}
	if (fchmod(fd, mode) < 0) {
		fprintf(stderr, "%s: chmod error %s\n", name, strerror(errno));
		close(fd);
		return 1;
	}
	if(verbose)
		printf("%s: %o changed to %o\n", name, st.st_mode, mode);

	close(fd);

	return 0;
}

/****************************************************************************/

int main(int argc, char *argv[])
{
	int rv = 0;
	int i;
	int opt;
	const char options[] = "hvVn";
	int verbose = 0;
	int noop = 0;

	while ((opt = getopt(argc, argv, options)) != -1) {
		switch (opt) {
			case 'h':
				printf("%s", USAGE);
				return 0;
			case 'v':
				verbose = 1;
				break;
			case 'V':
				printf("%s %s\n", PROGRAM_NAME, PROGRAM_VERSION);
				exit(0);
				break;
			case 'n':
				noop = 1;
				break;
			case '?':
				fprintf(stderr, "Unknown option/command.\n%s", USAGE);
				return 1;
			default:
				fprintf(stderr, "Error: option/command not found!\n%s", USAGE);
				return 1;
		}
	}

	for (i = optind; i < argc; i++)
		rv |= doEntity(verbose, noop, argv[i]);

	return rv;
}

