/****************************************************************************/
/**
 * \file
 * \brief  Fixes executable permissions of files and directories - header file.
 */
/****************************************************************************/

#ifndef EXECPERMFIX_H_INCLUDED
#define EXECPERMFIX_H_INCLUDED

#include <sys/stat.h>

/**
 * \brief  Check if a file should BE executable by its contents.
 * \param  name Path of entity to process.
 * \param  fd File descriptor of entity (result of open).
 * \param  st Stat structure of entity (result of stat).
 * \return 0 if not executable, 1 if executable, -1 if error.
 */
int fileIsExec(const char *name, int fd, const struct stat *st);

/**
 * \brief  Processes a file or a directory recursively.
 * \param  name Path of entity to process.
 * \param  noop Do not perform any change.
 * \param  verbose 1 to print actions.
 * \return 0 if ok, 1 if error.
 */
int execpermfix(const char *name, int noop, int verbose);

#endif /* EXECPERMFIX_H_INCLUDED */
