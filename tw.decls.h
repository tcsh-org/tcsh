/* $Header$ */
/*
 * tw.decls.h: Tenex external declarations
 */
#ifndef _h_tw_decls
#define _h_tw_decls

/*
 * tw.help.c
 */
extern void do_help();

/*
 * tw.parse.c
 */
extern int tenematch();
extern int t_search();
extern int starting_a_command();
extern void copyn();
extern void catn();
extern int fcompare();
extern void print_by_column();

/*
 * tw.init.c
 */
extern void tw_clear_comm_list();
extern void tw_add_comm_name();
extern void tw_add_builtins();
extern void tw_add_aliases();
extern Char **tw_start_env_list();
extern Char *tw_next_env_var();
extern Char *tw_next_shell_var();
extern struct varent *tw_start_shell_list();
extern Char **tw_start_env_list();
extern Char *tw_next_env_var();
extern void tw_sort_comms();
extern Char *Getenv();
extern int StrQcmp();

/*
 * tw.spell.c
 */
extern int spell_me();
extern int spdir();
extern int spdist();

#endif /* _h_tw_decls */
