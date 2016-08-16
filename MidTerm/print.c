#include "print.h"
#include "ls.h"

#define DEBUG 1

void print_entry(FTSENT *p) {
  struct stat *sp;
  sp = p->fts_statp;
  LEN *c = (LEN *)p->fts_pointer;
  if (f_inode)
    printf("%*lld ", c->l_inode, (LL)sp->st_ino);
  if (f_blocks)
    printf("%*lld ", c->l_blocks, (LL)sp->st_blocks);
  (void)print_name(p, c->l_name);
}

void print_long(FTSENT *p) {
  struct stat *sp;
  sp = p->fts_statp;
  LEN *c = (LEN *)p->fts_pointer;
  char perm[20];

  if (f_inode)
    printf("%*lld ", c->l_inode, (LL)sp->st_ino);
  if (f_blocks)
    printf("%*lld ", c->l_blocks, (LL)sp->st_blocks);

  (void)strmode(sp->st_mode, perm);
  (void)printf("%s ", perm);
  (void)printf("%*d ", c->l_links, (int)sp->st_nlink);
  /*- u(g)id/u(g)name solution -*/
  if (f_numid) {
    (void)printf("%*d  ", c->l_uid, (int)sp->st_uid);
    (void)printf("%*d  ", c->l_gid, (int)sp->st_gid);
  } else {
    struct passwd *u_st;
    struct group *g_st;
    u_st = getpwuid(sp->st_uid);
    g_st = getgrgid(sp->st_gid);
    (void)printf("%*s  ", c->l_uname, u_st->pw_name);
    (void)printf("%*s  ", c->l_gname, g_st->gr_name);
  }
  /*- size solution -*/
  if (f_kbyte) {
    (void)print_kilobyte((LL)sp->st_size, c->l_kb);
  } else if (f_human) {
    (void)print_human_size((LL)sp->st_size, c->l_human);
  } else if (f_blocks) {
    (void)printf("%*d ", c->l_blocks, (int)sp->st_blocks);
  } else {
    (void)printf("%*d ", c->l_size, (int)sp->st_size);
  }
  /*- time solution -*/
  if (f_accesstime) {
    (void)print_time(sp->st_atime);
  } else if (f_statustime) {
    (void)print_time(sp->st_ctime);
  } else {
    (void)print_time(sp->st_mtime);
  }
  /*- name solution -*/
  (void)print_name(p, c->l_name);
  if ((sp->st_mode & S_IFMT) == S_IFLNK) {
    char lnk[SIZE], tmp[SIZE];
    int res;
    sprintf(lnk, "%s/%s", p->fts_path, p->fts_accpath);
    if ((res = readlink(lnk, tmp, sizeof(lnk))) == -1)
      sys_err("readlink err");
    else
      printf(" -> %s", tmp);
  }
}

void print_time(time_t tm) {
  char *time_buf;
  time_t now = time(0);
  /* WWW MMM DD HH:MM:SS YYYY  24bytes */
  /* 012345678901234567890123 */
  if ((time_buf = ctime(&tm)) == NULL)
    time_buf = "????????????????????????";

  for (size_t i = 4; i < 11; ++i)
    putchar(time_buf[i]);

  if (now + HALFYEAR > tm && now - HALFYEAR < tm) {
    for (size_t i = 11; i < 16; ++i)
      putchar(time_buf[i]);
  } else {
    putchar(' ');
    for (size_t i = 20; i < 24; ++i)
      putchar(time_buf[i]);
  }
  putchar(' ');
}

char print_type(mode_t mod) {
  switch (mod & S_IFMT) {
  case S_IFDIR:
    return '/';
  case S_IFLNK:
    return '@';
  case S_IFWHT:
    return '%';
  case S_IFSOCK:
    return '=';
  case S_IFIFO:
    return '|';
  }
  if (mod & (S_IXUSR | S_IXGRP | S_IXOTH)) {
    return '*';
  } else
    return ' ';

  fprintf(stderr, "Unrecognized file type  0x%x\n", mod & S_IFMT);
  exit(EXIT_FAILURE);
}

void print_human_size(LL size, int len) {
  int i = 0;
  char hs_buf[SIZE_LEN];
  const char units[] = "BKMGT";
  while (size > 1024) {
    int tail = ((size - size % 1024) > 512) ? 1 : 0;
    size = size / 1024 + tail;
    i++;
  }
  sprintf(hs_buf, "%*lld%c ", i, size, units[i]);
  printf("%*s ", len, hs_buf);
}

void print_kilobyte(LL size, int len) {
  char ks_buf[SIZE_LEN];
  int tail = ((size - size % 1024) > 512) ? 1 : 0;
  size = size / 1024 + tail;
  sprintf(ks_buf, "%lld", size);
  printf("%*s ", len, ks_buf);
  ;
}

void print_name(FTSENT *p, int len) {
  int i;
  char tmp[SIZE];
  strcpy(tmp, p->fts_name); /*- maybe bcopy is more safe -*/

  if (f_nonprint) {
    for (i = 0; i < strlen(tmp); ++i) {
      tmp[i] = isprint(tmp[i]) ? tmp[i] : '?';
    }
  }
  if (f_type) {
    char r[SIZE];
    sprintf(r, "%s%c", tmp, print_type(p->fts_statp->st_mode));
    // printf("%-*s",len+1, r); 	/*- +1 for type -*/
    printf("%s", r);
  } else {
    // printf("%-*s", len, tmp);
    printf("%s", tmp);
  }
}
