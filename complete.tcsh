#
# $Id: complete.tcsh,v 1.1 1992/02/21 23:34:59 christos Exp $
# example file using the new completion code
#

if ($?tcsh) then
    if ($tcsh != 1) then
   	set rev=$tcsh:r
	set rel=$rev:e
	set pat=$tcsh:e
	set rev=$rev:r
    endif
    if ($rev > 5 && $rel > 0 && $pat > 1) then
	set complete=1
    endif
    unset rev rel pat
endif

if ($?complete) then
    set hosts=(hyperion phaeton guillemin theory.tc vangogh.cs.berkeley.edu)
    set ftphosts=(ftp.uu.net prep.ai.mit.edu export.lcs.mit.edu \
		  labrea.stanford.edu sumex-aim.stanford.edu \
		  tut.cis.ohio-state.edu)

    complete rsh '=$hosts'		# get argument from list in $hosts
    complete ywho '=$hosts'
    complete rlogin '-l ,=u' '-,=(l 8 e)' '=$hosts' # expand usernames
    complete xrsh '-l ,=u' '-,=(l 8 e)' '=$hosts' # expand usernames
    complete ftp '=$ftphosts'
    complete cd =d			# Directories only
    complete chdir =d
    complete pushd =d
    complete popd =d
    complete pu =d
    complete po =d
    complete mkdir =d
    complete rmdir =d
    complete man =c			# only commands are valid
    complete complete =c
    complete uncomplete =c
    complete exec =c
    complete trace =c
    complete strace =c
    complete which =c
    complete where =c
    complete skill =c
    complete dde =c
    complete adb =c
    complete sdb =c
    complete dbx =c
    complete xdb =c
    complete gdb =c 
    complete ups =c '^ups,=(-a)'
    complete set =s			# only shell variables are valid
    complete unset =s
    complete setenv =e			# only environment variables are valid
    complete unsetenv =e
    complete alias =a			# only aliases are valid
    complete unalias =a
    complete finger =u			# Only usernames are valid
    complete xdvi '=f:*.dvi'		# Only files that match *.dvi are valid
    complete dvips '=f:*.dvi'
    complete latex '=f:*.tex'
    complete tex '=f:*.tex'
    complete cc '-I,=d' '-L,=d' '-,=(o l c g L I D U)' '=f:*.[coa]'
    complete acc '-I,=d' '-L,=d' '-,=(o l c g L I D U)' '=f:*.[coa]'
    complete gcc '-I,=d' '-L,=d' \
		 '-f,=(caller-saves cse-follow-jumps delayed-branch \
		       elide-constructors expensive-optimizations float-store \
		       force-addr force-mem inline inline-functions \
		       keep-inline-functions memoize-lookups no-default-inline \
		       no-defer-pop no-function-cse omit-frame-pointer \
		       rerun-cse-after-loop schedule-insns schedule-insns2 \
		       strength-reduce thread-jumps unroll-all-loops \
		       unroll-loops syntax-only all-virtual cond-mismatch \
		       dollars-in-identifiers enum-int-equiv no-asm \
		       no-builtin no-strict-prototype signed-bitfields \
		       signed-char this-is-variable unsigned-bitfields \
		       unsigned-char writable-strings call-saved-reg \
		       call-used-reg fixed-reg no-common no-gnu-binutils \
		       nonnull-objects pcc-struct-return pic PIC shared-data \
		       short-enums short-double volatile)' \
		 '-W,=(all aggregate-return cast-align cast-qual \
		       comment conversion enum-clash error format \
		       id-clash-len implicit missing-prototypes \
		       no-parentheses pointer-arith return-type shadow \
		       strict-prototypes switch  \
		       uninitialized unused write-strings)' \
		 '-m,=(68000 68020 68881 bitfield fpa nobitfield rtd short \
		       c68000 c68020 soft-float g gnu unix fpu no-epilogue)' \
		 '-d,=(D M N)' \
		 '-,=(f W vspec v vpath ansi traditional traditional-cpp \
		      trigraphs pedantic x o l c g L I D U O O2 C E H B b V \
		      M MD MM i dynamic nodtdlib static nostdinc undef)' \
		      '=f:*.[coa]' \
		 '-l,=f:*.a'
    complete g++ '=f:*.{C,cc,o}'		
    complete CC '=f:*.{C,cc,o}'		
    complete rm '=f:^*.{c,cc,C,h}'	# Protect precious files
    complete vi '=f:^*.o'
    complete emacs '=f:^*~'		# usually don't want to edit a backup
    complete mail =u			# trade files for users--debatable
    complete bindkey =b
    complete find '-fstype ,=(nfs 4.2)' '-name ,=f' '-type ,=(c b d f p l s)' \
		  '-user ,=u' '-exec ,=c' '-ok ,=c' '-cpio ,=f' '-ncpio ,=f' \
		  '-newer, =f' '-,=(fstype name perm prune type user nouser \
		  group nogroup size inum atime mtime ctime exec ok print ls \
		  cpio ncpio newer xdev depth)' =d
    unset complete
endif
