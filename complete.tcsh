#
# $Id$
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
    complete rlogin '=$hosts' -l =u	# in addition expand usernames
    complete xrsh '=$hosts' -l =u	# in addition expand usernames
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
    complete dde =c core
    complete adb =c core
    complete sdb =c core
    complete dbx =c core
    complete xdb =c core
    complete gdb =c core
    complete ups =c -a 
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
    complete cc '=f:*.[co]'		
    complete acc '=f:*.[co]'		
    complete gcc '=f:*.{c,C,cc,o}'		
    complete g++ '=f:*.{C,cc,o}'		
    complete CC '=f:*.{C,cc,o}'		
    complete rm '=f:^*.{C,cc,C,h}'	# Protect precious files
    complete vi '=f:^*.o'
    complete emacs '=f:^*~'		# usually don't want to edit a backup
    complete mail =u			# trade files for users--debatable
    complete bindkey =b
    unset complete
endif
