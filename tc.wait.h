/* $Header: /home/hyperion/mu/christos/src/sys/tcsh-5.20/RCS/ourwait.h,v 1.4 1990/12/04 03:07:42 christos Exp $ */
/*
 * ourwait.h: Wait.h for machines that don't have it or have it and
 *	      is incorrect.
 */
#ifndef _h_ourwait
#define _h_ourwait

/*
 *	This wait is for big-endians and little endians
 */
union wait	{
	int	w_status;		
#ifdef _SEQUENT_
        struct {
                unsigned short  w_Termsig:7;
                unsigned short  w_Coredump:1;
                unsigned short  w_Retcode:8;
	} w_T;
        struct {
                unsigned short  w_Stopval:8;
                unsigned short  w_Stopsig:8;
        } w_S;
};
#define w_termsig     w_T.w_Termsig
#define w_coredump    w_T.w_Coredump
#define w_retcode     w_T.w_Retcode
#define w_stopval     w_S.w_Stopval
#define w_stopsig     w_S.w_Stopsig
#else /* _SEQUENT_ */
#if defined(vax) || defined(i386) 
	union {
		struct {
			unsigned w_Termsig:7;
			unsigned w_Coredump:1;
			unsigned w_Retcode:8;
			unsigned w_Dummy:16;
		} w_T;
		struct {
			unsigned w_Stopval:8;
			unsigned w_Stopsig:8;
			unsigned w_Dummy:16;
		} w_S;
	} w_P;
#else /* mc68000 || sparc || ??? */
	union {
		struct {
			unsigned w_Dummy:16;
			unsigned w_Retcode:8;
			unsigned w_Coredump:1;
			unsigned w_Termsig:7;
		} w_T;
		struct {
			unsigned w_Dummy:16;
			unsigned w_Stopsig:8;
			unsigned w_Stopval:8;
		} w_S;
	} w_P;
#endif /* vax || i386 */
};
#define	w_termsig	w_P.w_T.w_Termsig 
#define w_coredump	w_P.w_T.w_Coredump
#define w_retcode	w_P.w_T.w_Retcode
#define w_stopval	w_P.w_S.w_Stopval
#define w_stopsig	w_P.w_S.w_Stopsig
#endif /* _SEQUENT_ */


#ifndef WNOHANG
#define WNOHANG		1	/* dont hang in wait */
#endif
#ifndef WUNTRACED
#define WUNTRACED	2	/* tell about stopped, untraced children */
#endif
#define	WSTOPPED	0177

#define WIFSTOPPED(x)	((x).w_stopval == WSTOPPED)
#define WIFSIGNALED(x)	(((x).w_stopval != WSTOPPED) && ((x).w_termsig != 0))

#endif /* _h_ourwait */
