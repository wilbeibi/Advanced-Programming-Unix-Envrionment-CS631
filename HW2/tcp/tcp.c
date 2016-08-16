/* 	tcpm.c 	trivial file copy via read/write
 *	@AUTHOR: Hongyi Shen
 *  September 13, 2013
 */
#include <bsd/stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#define BUFSIZE 1024

#ifdef DEBUG
#define ASSERT(value, msg)                                                     \
  if (!(value)) {                                                              \
    fprintf(stderr, "## Line %d:\nMessage: %s\n", __LINE__, msg);              \
  }
#endif

void usage();
void sys_err(const char *msg);
int is_dir(const char *path);
int tcp(const char *src, const char *dst); /*- return 0 if success -*/

void usage() {
  (void)fprintf(stderr, "usage: %s source_file target_file\n", getprogname());
  exit(EXIT_FAILURE);
}

void sys_err(const char *msg) {
  perror(msg);
  exit(1);
}

int is_dir(const char *path) {
  struct stat st;
  if (stat(path, &st) == 0) {
    if (st.st_mode & S_IFDIR)
      return 1; /*- is directory -*/
    else if (st.st_mode & S_IFREG)
      return 0; /*- is regular file -*/
    else
      sys_err("Neither directory nor regular file");
  }
  return 0; /*- just to avoid warning -*/
}

int tcp(const char *src, const char *dst) {
  int sdf, ddf;      /*- src, dst file descriptor -*/
  int nr, nw;        /*- R/W n bytes each time -*/
  char buf[BUFSIZE]; /*- temp buf to R/W -*/
  struct stat per;   /*- get permission  -*/
  char *ndst =
      (char *)malloc /*- ndst: new dst, for dst might be a directory -*/
      ((strlen(src) + strlen(dst) + 1) * sizeof(char *));
  /*---------- open file ----------*/
  if ((sdf = open(src, O_RDONLY)) < 0)
    sys_err("Fail to open source_file");
  fstat(sdf, &per); /*- get src file permission -*/
  /*- is target directory? -*/
  if (is_dir(dst)) {
    (void)sprintf(ndst, "%s/%s", dst, src);
  } else {
    strcpy(ndst, dst);
  }
  if ((ddf = open(ndst, O_WRONLY | O_CREAT | O_TRUNC, per.st_mode)) < 0) {
    sys_err("Fail to open target_file");
  }

  /*---------- copy ----------*/
  while ((nr = read(sdf, buf, BUFSIZE)) > 0) {
    if ((nw = write(ddf, buf, nr)) > 0 && nw) {
      /*- write ok -*/;
    } else if (nw == 0) {
      sys_err("EOF while writing");
    }
    if (nw < 0) { /*- write error -*/
      close(sdf);
      close(ddf);
      sys_err("Unable to write");
    }
  }
  if (nr < 0) /*- read error -*/
  {
    close(sdf);
    close(ddf);
    sys_err("Unable to read");
  }

  close(sdf);
  close(ddf);
  return EXIT_SUCCESS;
}

int main(int argc, char const *argv[]) {
  const char *src, *dst;
  int flag;
  setprogname(argv[0]);

  if (argc != 3)
    usage();

  src = argv[1], dst = argv[2];

  if ((flag = tcp(src, dst)) != 0)
    exit(EXIT_FAILURE);

  return EXIT_SUCCESS;
}