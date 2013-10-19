void print_entry(FTSENT *p);
void print_long(FTSENT *p);
char print_type(mode_t mod);
void print_time(time_t tm);
void print_human_size( LL size, int len);
void print_kilobyte( LL size, int len);
void print_name(FTSENT *p, int len);

#define SIZE 1024