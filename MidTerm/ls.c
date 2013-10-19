#include "ls.h"
#include "print.h"
#include "cmp.h"

#define DEBUG 1
#define IS_NOPRINT(p) ((p)->fts_number == NO_PRINT)

static void (*print_by)(FTSENT *);
static int (*sort_by)(const FTSENT *, const FTSENT *);
static void usage();

void build(int argc, char **argv);
void display(FTSENT *p, FTSENT *list);
void update_len(FTSENT *cur, LEN *len);
int compare(const FTSENT **, const FTSENT **);

#define BY_NAME 0x01
#define BY_SIZE 0x02
#define BY_TIME 0x03
#define NO_PRINT 0x04
int sortkey = BY_NAME;

int main (int argc, char *argv[])
{
	int ch;
	char *default_dir[] = {".", NULL};

	setprogname(argv[0]);
	if(!getuid())	/*- super-user uid is 0 -*/
		f_lsdot = 1;

	if(isatty(STDOUT_FILENO)) {
		f_multicol = f_nonprint = 1;
	}
	else
		f_monocol = 1;
		
	fts_opts |= FTS_PHYSICAL;
	while((ch = getopt(argc, argv, "AacdFfhiklnqRrSstuwl")) != -1){
		switch (ch) {
			case 'a':
				fts_opts |= FTS_SEEDOT;
			case 'A':	
				f_lsdot = 1;
			break;
			
			case 'd':
				f_lsdir = 1;
				f_recursive = 0;
			break;
			case 'c':
				f_statustime = 1;
				f_accesstime = 0;
			break;
			case 'u':
				f_accesstime = 1;
				f_statustime = 0;
			break;
			case 't':
				sortkey = BY_TIME;
			break;
			case 'r':
				f_reverse = 1;
			break;
			case 'i':
			 	f_inode = 1;
			break;
			case 'l':
				f_long = 1;
				f_numid = 0;
			break;
			case 'n':	/*- uid, gid rather than name -*/
				f_long = 1;
				f_numid = 1;
			break;
            case '1':
                f_monocol = 1;
			break;			
			case 'F':
				f_type = 1;	
			break;
			case 'f':
				f_nosort = 1;
			break;	
			case 'k':
				f_kbyte = 1;
			break;
			case 's':
				f_blocks = 1;
			break;
			case 'h':
				f_human = 1;
			break;	
            case 'q':  	/*-printing non-printable ch as '?' -*/
				f_nonprint = 1;
			break;
			case 'w':	/*- non-printable -*/
				f_nonprint = 0;
			break;
			case 'R':
				f_recursive = 1;
			break;
			case 'S':
				sortkey = BY_SIZE; 	/*- big first -*/
			break;
			
			default:
			case '?':
				usage();	/*- Other options -*/
		}
	}	
	argc -= optind;
	argv += optind;

	if(f_reverse)
	{
		switch(sortkey){
			case BY_NAME:
				sort_by = d_name_cp;
				break;
			case BY_SIZE:
				sort_by = d_size_cp;
				break;
			case BY_TIME:
				if(f_accesstime)
					sort_by = d_acc_time_cp;
				else if(f_statustime)
					sort_by = d_stat_time_cp;
				else
					sort_by = d_mod_time_cp;
				break;
		}
	} 
	else{	/*- not reverse -*/
		switch(sortkey){
			case BY_NAME:
				sort_by = name_cp;
				break;
			case BY_SIZE:
				sort_by = size_cp;
				break;
			case BY_TIME:
				if(f_accesstime)
					sort_by = acc_time_cp;
				else if(f_statustime)
					sort_by = stat_time_cp;
				else
					sort_by = mod_time_cp;
				break;		
		}
	}
	
	if(f_long)
		print_by = print_long;
	else
		print_by = print_entry;
		
	if(argc)
		build(argc, argv);
	else	/*- No parameter, cur dir -*/
		build(1, default_dir);
	return(EXIT_SUCCESS);

}

void build(int argc, char **argv)
{
	FTS *ftsp;
	FTSENT *p, *list;

	int has_print = 0;
	if((ftsp = fts_open(argv, fts_opts, f_nosort ? NULL : compare)) == NULL)
		sys_err("fts_open failed:");
	list = fts_children(ftsp,0);
	display(NULL, list);
	if(f_lsdir){
		(void)fts_close(ftsp);
		return ;
	}
	
	while ((p = fts_read(ftsp)) != NULL) {
		switch(p->fts_info) {
			case FTS_DC:
				(void)fprintf(stderr, "%s: Directory cycle detected %s.",
				  p->fts_name, strerror(errno));
				exit(1);	
				break;
			case FTS_ERR:
			case FTS_DNR:
				(void)fprintf(stderr, "%s: FTS directory error %s.",
				  p->fts_name, strerror(p->fts_errno));
				break;
			case FTS_D:
				/*- -a and -A option -*/
				if(p->fts_level != FTS_ROOTLEVEL && 
			   		p->fts_name[0] == '.' && !f_lsdot) {
					fts_set(ftsp, p, FTS_SKIP);
					break;
				}
				if(has_print)
					(void)printf("\n%s\n", p->fts_path);
				else if (argc > 1){
					(void)printf("%s\n", p->fts_path);
				}
				display(p, fts_children(ftsp, 0));
				if (!f_recursive) //&& list != NULL
					fts_set(ftsp, p, FTS_SKIP);
				has_print = 1;
				break;	
			default:
				break;
			}
			// fts_close(ftsp);
	}
	fts_close(ftsp);
	return ;
}

void display(FTSENT *p, FTSENT *list)
{
	LEN this_len = {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0};
	FTSENT *cur;
	for(cur = list; cur; cur = cur->fts_link){
		if(p == NULL && cur->fts_info ==FTS_D && !f_lsdir)
		{
			cur->fts_number = NO_PRINT;
			continue;
		}
		if( p && cur->fts_name[0] == '.' && !f_lsdot){ 
			cur->fts_number = NO_PRINT;
			continue;
		}
		update_len(cur, &this_len);
	}

	if((f_long || f_blocks) && p){
		printf("total %d\n", this_len.cnt_total);
	}
	for(cur = list; cur; cur = cur->fts_link){
		if(IS_NOPRINT(cur))
			continue;
		print_by(cur);
		putchar('\n');
	}
}

void update_len(FTSENT *cur, LEN *len)
{
	char buf[SIZE];
	cur->fts_pointer = len;
	struct stat *cs;
	cs = cur->fts_statp;
	
	UPDATE(buf, "%s", cur->fts_name, len->l_name);
	if(f_inode)
		UPDATE(buf, "%lld", (LL)cs->st_ino, len->l_inode);
	if(f_blocks)
		UPDATE(buf, "%lld", (LL)cs->st_blocks, len->l_blocks);
	
	UPDATE(buf, "%lld", (LL)cs->st_size, len->l_size);
	len->cnt_total += cs->st_blocks;
	
	if(f_long){
		UPDATE(buf, "%d", (int)cs->st_nlink, len->l_links);
		
		if(f_numid){
			UPDATE(buf, "%d", cs->st_uid, len->l_uid);
			UPDATE(buf, "%d", cs->st_gid, len->l_gid);
		}
		else {	/*- uname/gname -*/
			struct passwd *u_st;
		    struct group *g_st;
			u_st = getpwuid(cs->st_uid);
			g_st = getgrgid(cs->st_gid);
			UPDATE(buf, "%s", u_st->pw_name, len->l_uname);
			UPDATE(buf, "%s", g_st->gr_name, len->l_gname);
		}
		
		if(f_kbyte){
			LL kb = cs->st_size;
			kb = (kb % 1024) ? kb/1024 : (kb/1024 + 1);
			UPDATE(buf, "%lld", kb, len->l_kb);
		}
		if (f_human){
			int i = 0;
			LL size = (LL) cs->st_size;
			const char units[] = "BKMGT";
			while(size > 1024){
				size /= 1024;
				i++;
			}
			sprintf(buf, "%*lld%c", i, size, units[i]);
			len->l_human = MAX(strlen(buf), len->l_human);
		}
		if(S_ISCHR(cs->st_mode) || S_ISBLK(cs->st_mode)){
			UPDATE(buf, "%d", major(cs->st_rdev), len->l_major);
			UPDATE(buf, "%d", minor(cs->st_rdev), len->l_minor);
		}
	}
}

static void usage(){
	printf("usage: ls [AacdFfhiklnqRrSstuwl] [file ...]\n");
}

int compare(const FTSENT **a, const FTSENT **b)
{
	int a_info, b_info;
	a_info = (*a)->fts_info;
	b_info = (*b)->fts_info;
	if(a_info == FTS_ERR || b_info == FTS_ERR || 
	   a_info == FTS_DNR || b_info == FTS_DNR)
		return 0;
	if(a_info == FTS_NS && b_info == FTS_NS)
		return name_cp(*a, *b);
	else if(a_info == FTS_NS || b_info == FTS_NS)
		return (a_info==FTS_NS)?1:-1;
	
	return (sort_by(*a, *b));
}

void sys_err(const char *msg){
        perror(msg);
        exit(0);
}





