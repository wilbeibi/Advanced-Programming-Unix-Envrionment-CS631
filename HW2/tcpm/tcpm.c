/* 	tcpm.c 	trivial file copy via memory mapping
 *	@AUTHOR: Hongyi Shen
 *  September 14, 2013
 */ 
#include <bsd/stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 1024
#ifdef DEBUG
#define ASSERT(value, msg)  if (!(value)) \
{fprintf(stderr, "## Line %d:\nMessage: %s\n", __LINE__, msg); }
#endif

void usage();
void sys_err(const char *msg);
int is_dir(const char *path);
int tcpm(const char *src, const char *dst);	/*- return 0 if success -*/	

void usage(){
	(void)fprintf(stderr, "usage: %s source_file target_file\n", getprogname());
		exit(EXIT_FAILURE);
}

void sys_err(const char *msg){
	perror(msg);
	exit(1);
}

/*- return 1 if *path is a directory, 0 if it is regualr file -*/
int is_dir(const char *path){
	struct stat st;
	if ( stat(path, &st) == 0 ){
		if (st.st_mode & S_IFDIR)
			return 1;
		else if (st.st_mode & S_IFREG)
			return 0;
		else
			sys_err("Neither directory nor regular file:");
	}
	return 0;				/*- just to avoid warning -*/
}


int tcpm(const char *src, const char *dst){
	int sdf, ddf;						/*- src, dst file descriptor -*/
	void *mr, *mw; 						/*- memory map -*/
	struct stat per;					/*- get permission  -*/
	char *ndst = (char*) malloc 		/*- ndst: new dst, for dst might be a directory -*/
		( (strlen(src) + strlen(dst) + 1) * sizeof(char*));
	/*---------- open file ----------*/
	if ( (sdf = open(src, O_RDONLY)) < 0)
		sys_err("Fail to open source_file");
	fstat(sdf, &per);					/*- get src file permission -*/
	/*- is target directory? -*/
	if (is_dir(dst)){
		(void)sprintf(ndst, "%s/%s", dst, src);
	}
	else{
		strcpy(ndst, dst);
	}
	
	if ( (ddf = open(ndst, O_RDWR | O_CREAT | O_TRUNC, per.st_mode)) < 0)  /*- change O_RD_ONLY to O_RDWR -*/
		sys_err("Fail to open target_file");
		
	/*- mmap need to pre-sizing dst size, otherwise cause bus error  40mins costs there -*/
	if (ftruncate(ddf, per.st_size) < 0 ){
		sys_err("Fail to ftruncate");
	}	
	
	/*---------- copy in mmap's way----------*/
	/*- FIXME: this code would work only when memory is enough -*/
	if ((mr = mmap(0, per.st_size, PROT_READ, MAP_PRIVATE, sdf, 0)) == MAP_FAILED){
		sys_err("source file mmap err");
	}
	if ((mw = mmap(0, per.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, ddf, 0)) == MAP_FAILED){
		sys_err("target file mmap err");
	}

	if (memcpy(mw, mr, per.st_size) == NULL){
		sys_err("memory copy err");
	}

	munmap(mr, per.st_size);
	munmap(mw, per.st_size);
	close(sdf);
	close(ddf);	
	return EXIT_SUCCESS;
}


int main (int argc, char const *argv[])
{
	const char *src, *dst;
	int flag;
	setprogname(argv[0]);
	if (argc!=3)
		usage();
		
	src = argv[1], dst = argv[2];
	
	if ((flag = tcpm(src, dst)) != 0)
		exit(EXIT_FAILURE);
		
	return EXIT_SUCCESS;
}