#include "ls.h"
#include "cmp.h"

int name_cp(const FTSENT *a, const FTSENT *b)
{
	return (strcmp(a->fts_name, b->fts_name));
}

int d_name_cp(const FTSENT *a, const FTSENT *b)
{
	return (strcmp(b->fts_name, a->fts_name));
}

int acc_time_cp(const FTSENT *a, const FTSENT *b)
{
	if (a->fts_statp->st_atime < b->fts_statp->st_atime)
		return 1;
	else if (a->fts_statp->st_atime > b->fts_statp->st_atime)
		return -1;
	else
		return (name_cp(a,b));
}

int d_acc_time_cp(const FTSENT *a, const FTSENT *b)
{
	if (a->fts_statp->st_atime > b->fts_statp->st_atime)
		return 1;
	else if (a->fts_statp->st_atime < b->fts_statp->st_atime)
		return -1;
	else
		return (d_name_cp(a,b));
}

int stat_time_cp(const FTSENT *a, const FTSENT *b)
{
	if (a->fts_statp->st_ctime < b->fts_statp->st_ctime)
		return 1;
	else if (a->fts_statp->st_ctime > b->fts_statp->st_ctime)
		return -1;
	else
		return (name_cp(a,b));
}

int d_stat_time_cp(const FTSENT *a, const FTSENT *b)
{
	if (a->fts_statp->st_ctime > b->fts_statp->st_ctime)
		return 1;
	else if (a->fts_statp->st_ctime < b->fts_statp->st_ctime)
		return -1;
	else
		return (d_name_cp(a,b));
}


int mod_time_cp(const FTSENT *a, const FTSENT *b)
{
	if (a->fts_statp->st_mtime < b->fts_statp->st_mtime)
		return 1;
	else if (a->fts_statp->st_mtime > b->fts_statp->st_mtime)
		return -1;
	else
		return (name_cp(a,b));
}

int d_mod_time_cp(const FTSENT *a, const FTSENT *b)
{
	if (a->fts_statp->st_mtime > b->fts_statp->st_mtime)
		return 1;
	else if (a->fts_statp->st_mtime < b->fts_statp->st_mtime)
		return -1;
	else
		return (d_name_cp(a,b));
}

int size_cp(const FTSENT *a, const FTSENT *b)
{
	if (a->fts_statp->st_size < b->fts_statp->st_size)
		return 1;
	else if (a->fts_statp->st_size > b->fts_statp->st_size)
		return -1;
	else
		return (name_cp(a,b));
}

int d_size_cp(const FTSENT *a, const FTSENT *b)
{
	if (a->fts_statp->st_size > b->fts_statp->st_size)
		return 1;
	else if (a->fts_statp->st_size < b->fts_statp->st_size)
		return -1;
	else
		return (d_name_cp(a,b));
}
