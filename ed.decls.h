/* $Header$ */
/*
 * ed.decls.h: Editor external definitions
 */
#ifndef _h_ed_decls
#define _h_ed_decls

/*
 * ed.chared.c
 */
extern int InsertStr();
extern void DeleteBack();

/*
 * ed.init.c
 */
extern void ed_Init();
extern int Cookedmode();
extern int Rawmode();
extern void ed_set_tty_eight_bit();
#ifdef SIG_WINDOW
extern sigret_t window_change();
#endif
extern void QuoteModeOn();
extern void QuoteModeOff();
extern void ResetInLine();
extern int Load_input_line();

/*
 * ed.screen.c
 */
extern void check_window_size();
extern void SetAttributes();
extern void so_write();
extern void ClearScreen();
extern void MoveToLine();
extern void MoveToChar();
extern void ClearEOL();
extern void Insert_write();
extern void DeleteChars();
extern void TellTC();
extern void SetTC();
extern void EchoTC();
extern void BindArrowKeys();
extern int CanWeTab();
extern void ChangeSize();
extern int GetSize();
extern void ClearToBottom();
extern void GetTermCaps();

/*
 * ed.defns.c
 */
extern void ed_InitNLSMaps();
#ifdef DEBUG_EDIT
extern void CheckMaps();
#endif
extern void ed_InitMaps();
extern void ed_InitEmacsMaps();
extern void ed_InitVIMaps();

/*
 * ed.inputl.c
 */
extern int Inputl();
extern int GetNextChar();

/*
 * ed.refresh.c
 */
extern void ClearLines();
extern void ClearDisp();
extern void Refresh();
extern void Beep();
extern void RefCursor();
extern void RefPlusOne();
extern void PastBottom();

/*
 * ed.xmap.c
 */
extern void AddXkeyCmd();
extern void AddXkey();
extern void ClearXkey();
extern int GetXkey();
extern void ResetXmap();
extern int DeleteXkey();
extern void PrintXkey();

#endif /* _h_ed_decls */
