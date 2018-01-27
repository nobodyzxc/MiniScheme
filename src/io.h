#ifndef IO_H
#define IO_H


void clear_ctx(Obj pt);

#if I_MODE == rl
extern Obj rl_pt;
char *rl_raw_input(char *prompt);
char *rl_non_blank(char *p , char *prompt);
#endif

extern Obj read_pt;
char *read_raw_input(char *prompt);
char *read_non_blank(char *p , char *prompt);

void close_port(Obj pt_obj);
Obj open_port(char *name , char *mode);

#endif
