#define DAYS_OF_YEAR 366
#define SECS_OF_DAY 86400
#define HALFYEAR ((DAYS_OF_YEAR / 2) * SECS_OF_DAY)
#define SIZE_LEN 20
#ifndef S_IFWHT
#define S_IFWHT 0160000
#endif

#ifdef __linux__
#include <bsd/stdio.h>
#include <bsd/stdlib.h>
#include <bsd/string.h>
#endif

#ifdef __APPLE__
#include <uuid/uuid.h>
#endif

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <ctype.h>
#include <errno.h>
#include <fts.h>
#include <getopt.h>
#include <grp.h>
#include <pwd.h> /*- user_from_uid -*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#define LL long long
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define UPDATE(b, f, v, l)                                                     \
  sprintf((b), (f), (v));                                                      \
  (l) = MAX(strlen(b), (l));

#define WATCH(fmt, ...)                                                        \
  if (DEBUG)                                                                   \
    printf(fmt, __VA_ARGS__);

void sys_err(const char *msg);

int f_accesstime; /*- last access time -*/
int f_blocks;     /*- ls size, short -*/
int f_human;      /*- humanlize format -*/
int f_inode;      /*- print inode -*/
int f_kbyte;      /*- print size in kbytes -*/
int f_long;       /*- print long format -*/
int f_lsalldot;   /*- ls . and .. -*/
int f_lsdot;      /*- ls hidden file begin with . -*/
int f_lsdir;      /*- ls directories -*/
int f_monocol;    /*- print in one signal column -*/
int f_multicol;   /*- print in multiple columns -*/
int f_nosort;     /*- non-sort order -*/
int f_nonprint;   /*- print non-printable ch as ? -*/
int f_numid;      /*- show uid/gid instead of uname/gname -*/
int f_recursive;  /*- ls recursively -*/
int f_reverse;    /*- ls order reversely -*/
int f_statustime; /*- last change time -*/
int f_type;       /*- ls file type -*/

int fts_opts; /*- fts options -*/

typedef struct { /* 15 */
  int total;
  int l_name; /*- useless when single column -*/
  int l_inode;
  int l_links;
  int l_uid;
  int l_gid;
  int l_uname;
  int l_gname;
  int l_size;
  int l_kb;
  int l_human;
  int l_blocks;
  int cnt_total;
  int l_major;
  int l_minor;
} LEN;
