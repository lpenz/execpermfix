/****************************************************************************/
/**
 * \file
 * \brief  Fixes exec permissions of files.
 */
/****************************************************************************/

#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/mman.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <getopt.h>

#define DEBUG 0

#define S_IX (S_IXUSR | S_IXGRP | S_IXOTH)
#define S_DEF (0644)

/****************************************************************************/

/**
 * \brief  Processes a file or a directory recursively.
 * \param  verbose 1 to print actions.
 * \param  name Path of entity to process.
 * \return 0 if ok, 1 if error.
 */
static int doEntity(int verbose, const char *name);

/****************************************************************************/

/**
 * \brief  Processes a directory recursively.
 * \param  verbose 1 to print actions.
 * \param  name Path of entity to process.
 * \param  fd File descriptor of entity (result of open).
 * \param  st Stat structure of entity (result of stat).
 * \return 0 if ok, 1 if error.
 */
static int doDir(int verbose, const char *name, int fd, const struct stat *st)
{
	DIR *dir;
	struct dirent entry;
	struct dirent *result;
	int rv = 0;

	dir = fdopendir(fd);

	while (readdir_r(dir, &entry, &result) == 0 && result) {
		char *namesub;
		int namesubSize;

		if (strcmp(entry.d_name, ".") == 0
				|| strcmp(entry.d_name, "..") == 0)
			continue;

		namesubSize = strlen(name) + strlen(entry.d_name) + 2;
		namesub = malloc(namesubSize);
		snprintf(namesub, namesubSize, "%s/%s", name, entry.d_name);
		rv |= doEntity(verbose, namesub);
		free(namesub);

		if ((st->st_mode & S_IX) == S_IX)
			continue;

		if (!DEBUG) {
			if (chmod(name, st->st_mode | S_IX) < 0) {
				fprintf(stderr, "Error in chmod of %s: %s\n", name, strerror(errno));
				rv |= 1;
			}
			else if (verbose)
				printf("%-4s %-80s | %8o -> %o\n", "dir", name, st->st_mode, st->st_mode | S_IX);
		}
		else if (verbose)
			printf("%-4s %-80s | %8o would change to 0%o\n", "dir", name, st->st_mode, st->st_mode | S_IX);
	}

	closedir(dir);

	return rv;
}

/****************************************************************************/

/**
 * \brief  Check if a file should NOT BE executable by its name/path.
 * \param  name Path of entity to process.
 * \param  dir Directory of file.
 * \param  bname Basename of file.
 * \param  ext Extension of file.
 * \return 0 if not executable, 1 if executable.
 */
static int filenameIsNoExec(const char *name, const char *dir, const char *bname, const char *ext)
{
	const static char *extsNoExec[] = {
		"h", "c", "o", "a", "d",
		"t2t", "mk", "txt", "dox",
	};
	int i;

	if (strcmp(bname, "Makefile") == 0)
		return 1;

	for (i = 0; i < sizeof(extsNoExec) / sizeof(*extsNoExec); i++)
		if (strcmp(ext, extsNoExec[i]) == 0)
			return 1;

	if (strcmp(dir, "CVS") == 0)
		return 1;
	if (strcmp(dir, ".git") == 0)
		return 1;

	return 0;
}


/**
 * \brief  Check if a file should BE executable by its name/path.
 * \param  name Path of entity to process.
 * \param  dir Directory of file.
 * \param  bname Basename of file.
 * \param  ext Extension of file.
 * \return 0 if executable, 1 if not executable.
 */
static int filenameIsExec(const char *name, const char *dir, const char *bname, const char *ext)
{
	const static char *extsExec[] = {
		"py", "pl", "sh"
	};
	int i;

	for (i = 0; i < sizeof(extsExec) / sizeof(*extsExec); i++)
		if (strcmp(ext, extsExec[i]) == 0)
			return 1;
	return 0;
}


/**
 * \brief  Check if a file should BE executable by its contents.
 * \param  name Path of entity to process.
 * \param  fd File descriptor of entity (result of open).
 * \param  st Stat structure of entity (result of stat).
 * \return 0 if not executable, 1 if executable, -1 if error.
 */
static int fileIsMagicExec(const char *name, int fd, const struct stat *st)
{
	const char scriptMagic[] = "#!/";
	struct {
		uint8_t ei_magic[4];
		uint8_t ei_class;
		uint8_t ei_data;
		uint8_t ei_version;
		uint8_t ei_pad[9];
		uint16_t e_type;
	} *eh;
	const char elfMagic[] = { 0x7f, 0x45, 0x4c, 0x46 };
	void *f;

	if ((f = mmap(NULL, st->st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
		return -1;

	if (st->st_size >= sizeof scriptMagic - 1
			&& memcmp(f, scriptMagic, sizeof scriptMagic - 1) == 0) {
		munmap(f, st->st_size);
		return 1;
	}

	eh = f;
	if (st->st_size >= sizeof(*eh)
			&& memcmp(eh->ei_magic, elfMagic, sizeof elfMagic) == 0
			&& ((eh->ei_data == 2 && eh->e_type == 0x0200) || (eh->ei_data == 1 && eh->e_type == 0x0002))) {
		munmap(f, st->st_size);
		return 1;
	}

	munmap(f, st->st_size);

	return 0;
}


/**
 * \brief  Check if a file should BE executable.
 * \param  name Path of entity to process.
 * \param  fd File descriptor of entity (result of open).
 * \param  st Stat structure of entity (result of stat).
 * \return 0 if not executable, 1 if executable, -1 if error.
 */
static int fileIsExec(const char *name, int fd, const struct stat *st)
{
	const char *dirI;
	char dir[PATH_MAX];
	const char *bname;
	const char *ext;

	if (fileIsMagicExec(name, fd, st) == 1)
		return 1;

	if ((bname = strrchr(name, '/')) == NULL)
		return -1;
	bname++;

	dirI = bname - 2;
	while (*dirI != '/') {
		dirI--;
		if (dirI < name)
			return -1;
	}
	memcpy(dir, dirI + 1, bname - dirI - 2);
	dir[bname-dirI-2] = 0;

	if ((ext = strrchr(bname, '.')) != NULL && ext != bname)
		ext++;
	else
		ext = "";

	if (filenameIsNoExec(name, dir, bname, ext))
		return 0;
	else if (filenameIsExec(name, dir, bname, ext))
		return 1;

	return 0;
}


/**
 * \brief  Processes a file.
 * \param  verbose 1 to print actions.
 * \param  name Path of entity to process.
 * \param  fd File descriptor of entity (result of open).
 * \param  st Stat structure of entity (result of stat).
 * \return 0 if ok, 1 if error.
 */
static int doFile(int verbose, const char *name, int fd, const struct stat *st)
{
	int rv = 0;
	int x = 0;
	int wanted;

	if ((x = fileIsExec(name, fd, st)) < 0)
		return 1;

	if (x)
		wanted = st->st_mode | S_IX;
	else
		wanted = st->st_mode & ~S_IX;

	if (!DEBUG && st->st_mode != wanted) {
		if (chmod(name, wanted) < 0) {
			fprintf(stderr, "Error in chmod of %s: %s\n", name, strerror(errno));
			rv |= 1;
		}
		else if (verbose)
			printf("%-4s %-80s | %8o -> %o\n", "file", name, st->st_mode, wanted);
	}

	if (DEBUG && x != ((st->st_mode & S_IX) == S_IX) && verbose)
		printf("%-4s %-80s | %8o would change to 0%o\n", "file", name, st->st_mode, wanted);

	return 0;
}

/****************************************************************************/

static int doEntity(int verbose, const char *name)
{
	int rv = 0;
	int fd = 0;
	struct stat st;
	char fullname[PATH_MAX+1];

	if (realpath(name, fullname) == NULL) {
		fprintf(stderr, "Error in realpath of %s: %s\n", name, strerror(errno));
		return 1;
	}

	if (stat(fullname, &st) != 0) {
		fprintf(stderr, "Error in stat of %s: %s\n", fullname, strerror(errno));
		return 1;
	}
	if (!S_ISDIR(st.st_mode) && !S_ISREG(st.st_mode))
		return 0;

	if ((fd = open(fullname, O_RDONLY)) < 0) {
		fprintf(stderr, "Error opening %s: %s\n", fullname, strerror(errno));
		return 1;
	}
	if (fstat(fd, &st) != 0) {
		fprintf(stderr, "Error in fstat of %s: %s\n", fullname, strerror(errno));
		return 1;
	}
	if (S_ISDIR(st.st_mode))
		rv |= doDir(verbose, fullname, fd, &st);
	else if (S_ISREG(st.st_mode))
		rv |= doFile(verbose, fullname, fd, &st);

	close(fd);

	return rv;
}

/****************************************************************************/

int main(int argc, char *argv[])
{
	int rv = 0;
	int i;
	int opt;
	const char options[] = "v";
	int verbose = 0;

	while ((opt = getopt(argc, argv, options)) != -1) {
		switch (opt) {
			case 'v':
				verbose = 1;
				break;
			case '?':
				printf("Unknown option/command.\n");
				return 1;
			default:
				printf("Error: option/command not found!\n");
				return 1;
		}
	}

	for (i = optind; i < argc; i++)
		rv |= doEntity(verbose, argv[i]);

	return rv;
}

