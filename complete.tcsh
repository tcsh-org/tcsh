#
# $Id: complete.tcsh,v 1.20 1993/06/07 22:21:41 christos Exp $
# example file using the new completion code
#

if ($?tcsh) then
    if ($tcsh != 1) then
   	set rev=$tcsh:r
	set rel=$rev:e
	set pat=$tcsh:e
	set rev=$rev:r
    endif
    if ($rev > 5 && $rel > 1) then
	set complete=1
    endif
    unset rev rel pat
endif

if ($?complete) then
    set noglob
    set hosts=(hyperion.ee.cornell.edu phaeton.ee.cornell.edu \
	       guillemin.ee.cornell.edu theory.tc.cornell.edu \
	       vangogh.cs.berkeley.edu)

    complete rsh	n/*/\$hosts/	# argument from list in $hosts
    complete ywho  	n/*/\$hosts/
    complete cd  	p/1/d/		# Directories only
    complete chdir 	p/1/d/
    complete pushd 	p/1/d/
    complete popd 	p/1/d/
    complete pu 	p/1/d/
    complete po 	p/1/d/
    complete mkdir 	p/1/d/
    complete rmdir 	p/1/d/
    complete complete 	p/1/C/
    complete uncomplete p/1/C/
    complete exec 	p/1/c/
    complete trace 	p/1/c/
    complete strace 	p/1/c/
    complete which 	p/1/c/
    complete where 	p/1/c/
    complete skill 	p/1/c/
    complete dde	p/1/c/ 
    complete adb	p/1/c/ 
    complete sdb	p/1/c/
    complete dbx	p/1/c/
    complete xdb	p/1/c/
    complete gdb	p/1/c/
    complete ups	p/1/c/
    complete set	'c/*=/f/' 'p/1/s/=' 'n/=/f/'
    complete unset	p/1/s/
    complete unsetenv 	p/1/e/
    complete alias 	p/1/a/		# only aliases are valid
    complete unalias 	p/1/a/
    complete xdvi 	n/*/f:*.dvi/	# Only files that match *.dvi
    complete dvips 	n/*/f:*.dvi/
    complete latex 	n/*/f:*.tex/
    complete tex 	n/*/f:*.tex/
    complete rlogin 	c/-/"(l 8 e)"/ n/-l/u/ n/*/\$hosts/ 
    complete telnet 	n/*/\$hosts/ 
    complete su		c/-/"(f c)"/ n/-c/c/ n/*/u/
    complete xrsh   	c/-/"(l 8 e)"/ n/-l/u/ n/*/\$hosts/ 
    complete cc 	c/-I/d/ c/-L/d/ \
              c@-l@'`\ls -1 /usr/lib/lib*.a | sed s%^.\*/lib%%\;s%\\.a\$%%`'@ \
			c/-/"(o l c g L I D U)"/ n/*/f:*.[coa]/
    complete acc 	c/-I/d/ c/-L/d/ \
       c@-l@'`\ls -1 /usr/lang/SC1.0/lib*.a | sed s%^.\*/lib%%\;s%\\.a\$%%`'@ \
			c/-/"(o l c g L I D U)"/ n/*/f:*.[coa]/
    complete gcc 	c/-I/d/ c/-L/d/ \
		 	c/-f/"(caller-saves cse-follow-jumps delayed-branch \
		               elide-constructors expensive-optimizations \
			       float-store force-addr force-mem inline \
			       inline-functions keep-inline-functions \
			       memoize-lookups no-default-inline \
			       no-defer-pop no-function-cse omit-frame-pointer \
			       rerun-cse-after-loop schedule-insns \
			       schedule-insns2 strength-reduce \
			       thread-jumps unroll-all-loops \
			       unroll-loops syntax-only all-virtual \
			       cond-mismatch dollars-in-identifiers \
			       enum-int-equiv no-asm no-builtin \
			       no-strict-prototype signed-bitfields \
			       signed-char this-is-variable unsigned-bitfields \
			       unsigned-char writable-strings call-saved-reg \
			       call-used-reg fixed-reg no-common \
			       no-gnu-binutils nonnull-objects \
			       pcc-struct-return pic PIC shared-data \
			       short-enums short-double volatile)"/ \
		 	c/-W/"(all aggregate-return cast-align cast-qual \
		      	       comment conversion enum-clash error format \
		      	       id-clash-len implicit missing-prototypes \
		      	       no-parentheses pointer-arith return-type shadow \
		      	       strict-prototypes switch uninitialized unused \
		      	       write-strings)"/ \
		 	c/-m/"(68000 68020 68881 bitfield fpa nobitfield rtd \
			       short c68000 c68020 soft-float g gnu unix fpu \
			       no-epilogue)"/ \
		 	c/-d/"(D M N)"/ \
		 	c/-/"(f W vspec v vpath ansi traditional \
			      traditional-cpp trigraphs pedantic x o l c g L \
			      I D U O O2 C E H B b V M MD MM i dynamic \
			      nodtdlib static nostdinc undef)"/ \
		 	c/-l/f:*.a/ \
		 	n/*/f:*.{c,C,cc,o,a}/
    complete g++ 	n/*/f:*.{C,cc,o}/
    complete CC 	n/*/f:*.{C,cc,o}/
    complete rm 	n/*/f:^*.{c,cc,C,h,in}/	# Protect precious files
    complete vi 	n/*/f:^*.o/
    complete bindkey    N/-a/b/ N/-c/c/ n/-[ascr]/'x:<key-sequence>'/ \
			n/-[svedl]/n/ c/-[vedl]/n/ c/-/"(a s k c v e d l r)"/ \
			n/-k/"(left right up down)"/ p/2-/b/ \
			p/1/'x:<key-sequence or option>'/

    complete find 	n/-fstype/"(nfs 4.2)"/ n/-name/f/ \
		  	n/-type/"(c b d f p l s)"/ n/-user/u/ n/-exec/c/ \
		  	n/-ok/c/ n/-cpio/f/ n/-ncpio/f/ n/-newer/f/ \
		  	c/-/"(fstype name perm prune type user nouser \
		  	     group nogroup size inum atime mtime ctime exec \
			     ok print ls cpio ncpio newer xdev depth)"/ \
			n/*/d/

    complete kill	c/-/S/ c/%/j/
    complete -%*	c/%/j/			# fill in the jobs builtin
    complete fg		c/%/j/
    complete bg		c/%/j/
    complete stop	c/%/j/

    complete limit	c/-/"(h)"/ n/*/l/
    complete unlimit	c/-/"(h)"/ n/*/l/

    complete -co*	p/0/"(compress)"/	# make compress completion
						# not ambiguous
    complete zcat	n/*/f:*.Z/
    complete nm		n/*/f:^*.{h,C,c,cc}/

    complete finger	c/*@/\$hosts/ p/1/u/@ 
    complete ping	p/1/\$hosts/
    complete traceroute	p/1/\$hosts/

    complete {talk,ntalk,phone}	p/1/'`users | tr " " "\012" | uniq`'/ \
		n/*/\`who\ \|\ grep\ \$:1\ \|\ awk\ \'\{\ print\ \$2\ \}\'\`/

    if ( -f $HOME/.netrc ) then
	complete ftp    p@1@'`cat $HOME/.netrc | awk '"'"'{print $2 }'"'"\`@
    else
	set ftphosts=(ftp.uu.net prep.ai.mit.edu export.lcs.mit.edu \
		      labrea.stanford.edu sumex-aim.stanford.edu \
		      tut.cis.ohio-state.edu)
	complete ftp 	n/*/\$ftphosts/
    endif

    # this one is simple...
    #complete rcp c/*:/f/ C@[./]*@f@ n/*/\$hosts/:
    # From Michael Schroeder: 
    # This one will rsh to the file to fetch the list of files!
    complete rcp 'c%*:%`set q=$:-0;set q="$q:s/:/ /";set q=($q " ");rsh $q[1] ls -dp $q[2]\*`%%' 'C@[./]*@f@' 'n/*/$hosts/:'


    complete dd c/if=/f/ c/of=/f/ \
		c/conv=*,/"(ascii ebcdic ibm block unblock \
			    lcase ucase swap noerror sync)"/,\
		c/conv=/"(ascii ebcdic ibm block unblock \
			  lcase ucase swap noerror sync)"/,\
	        c/*=/x:'<number>'/ \
		n/*/"(if of conv ibs obs bs cbs files skip file seek count)"/=

    complete nslookup   p/1/x:'<host>'/ p/2/\$hosts/

    complete ar c/[dmpqrtx]/"(c l o u v a b i)"/ p/1/"(d m p q r t x)"// \
		p/2/f:*.a/ p/*/f:*.o/

    complete {refile,sprev,snext,scan,pick,rmm,inc,folder,show} \
		c@+@p:$HOME/Mail/@

    # More completions from waz@quahog.nl.nuwc.navy.mil (Tom Warzeka)
    # this one works but is slow and doesn't descend into subdirectories
    # complete	cd	C@[./]*@d@ \
    #			p@1@'`\ls -1F . $cdpath | grep /\$ | sort -u`'@ n@*@n@

    if ( -r /etc/shells ) then
        complete setenv	p@1@e@ n@DISPLAY@\$hosts@: n@SHELL@'`cat /etc/shells`'@
    else
	complete setenv	p@1@e@ n@DISPLAY@\$hosts@:
    endif



    # replace "/usr/local/emacs" with your GNU Emacs main directory
    complete emacs	c/-/"(batch d f funcall i insert kill l load \
			no-init-file q t u user)"/ c/+/x:'<line_number>'/ \
			n/-d/x:'<display>'/ n/-f/x:'<lisp_function>'/ n/-i/f/ \
			n@-l@p:/usr/local/emacs/lisp@ n/-t/x:'<terminal>'/ \
			n/-u/u/ n/*/f:^*[\#~]/

    # if you're running SysV, change "/var/spool" to "/usr"
    complete mail       c/-/"(e i f n s u v)"/ c/*@/\$hosts/ \
			c@+@p:$HOME/Mail@ C@[./]@f@ n/-s/x:'<subject>'/ \
			n@-u@p:/var/spool/mail@ n/-f/f/ n/*/u/

    complete man	    n@1@'`\ls -1 /usr/man/man1 | sed s%\\.1.\*\$%%`'@ \
			    n@2@'`\ls -1 /usr/man/man2 | sed s%\\.2.\*\$%%`'@ \
			    n@3@'`\ls -1 /usr/man/man3 | sed s%\\.3.\*\$%%`'@ \
			    n@4@'`\ls -1 /usr/man/man4 | sed s%\\.4.\*\$%%`'@ \
			    n@5@'`\ls -1 /usr/man/man5 | sed s%\\.5.\*\$%%`'@ \
			    n@6@'`\ls -1 /usr/man/man6 | sed s%\\.6.\*\$%%`'@ \
			    n@7@'`\ls -1 /usr/man/man7 | sed s%\\.7.\*\$%%`'@ \
			    n@8@'`\ls -1 /usr/man/man8 | sed s%\\.8.\*\$%%`'@ \
    n@9@'`[ -r /usr/man/man9 ] && \ls -1 /usr/man/man9 | sed s%\\.9.\*\$%%`'@ \
    n@0@'`[ -r /usr/man/man0 ] && \ls -1 /usr/man/man0 | sed s%\\.0.\*\$%%`'@ \
  n@new@'`[ -r /usr/man/mann ] && \ls -1 /usr/man/mann | sed s%\\.n.\*\$%%`'@ \
  n@old@'`[ -r /usr/man/mano ] && \ls -1 /usr/man/mano | sed s%\\.o.\*\$%%`'@ \
n@local@'`[ -r /usr/man/manl ] && \ls -1 /usr/man/manl | sed s%\\.l.\*\$%%`'@ \
n@public@'`[ -r /usr/man/manp ] && \ls -1 /usr/man/manp| sed s%\\.p.\*\$%%`'@ \
		c/-/"(- f k P s t)"/ n/-f/c/ n/-k/x:'<keyword>'/ n/-P/d/ n/*/c/

    complete touch 	c/-/"(a c f m)"/ n/*/f/
    complete xhost	c/[+-]/\$hosts/ n/*/\$hosts/

    complete gzcat	c/--/"(force help license quiet version)"/ \
			c/-/"(f h L q V)"/ n/*/f:*.{gz,Z,z,zip}/
    complete gzip	c/--/"(stdout to-stdout decompress uncompress \
			force help list license no-name quiet recurse \
			suffix test verbose version fast best)"/ \
			c/-/"(c d f h l L n q r S t v V 1 2 3 4 5 6 7 8 9)"/ \
			n/{-S,--suffix}/x:'<file_name_suffix>'/ \
			n/{-d,--{de,un}compress}/f:*.{gz,Z,z,zip,taz,tgz}/ \
			N/{-d,--{de,un}compress}/f:*.{gz,Z,z,zip,taz,tgz}/ \
			n/*/f:^*.{gz,Z,z,zip,taz,tgz}/
    complete {gunzip,ungzip} c/--/"(stdout to-stdout force help list license \
			no-name quiet recurse suffix test verbose version)"/ \
			c/-/"(c f h l L n q r S t v V)"/ \
			n/{-S,--suffix}/x:'<file_name_suffix>'/ \
			n/*/f:*.{gz,Z,z,zip,taz,tgz}/
    complete zgrep	c/-*A/x:'<#_lines_after>'/ c/-*B/x:'<#_lines_before>'/\
			c/-/"(A b B c C e f h i l n s v V w x)"/ \
			p/1/x:'<limited_regular_expression>'/ \
			n/-*e/x:'<limited_regular_expression>'/ n/-*f/f/ n/*/f/
    complete zegrep	c/-*A/x:'<#_lines_after>'/ c/-*B/x:'<#_lines_before>'/\
			c/-/"(A b B c C e f h i l n s v V w x)"/ \
			p/1/x:'<full_regular_expression>'/ \
			n/-*e/x:'<full_regular_expression>'/ n/-*f/f/ n/*/f/
    complete zfgrep	c/-*A/x:'<#_lines_after>'/ c/-*B/x:'<#_lines_before>'/\
			c/-/"(A b B c C e f h i l n s v V w x)"/ \
			p/1/x:'<fixed_string>'/ \
			n/-*e/x:'<fixed_string>'/ n/-*f/f/ n/*/f/

    complete znew	c/-/"(f t v 9 P K)"/ n/*/f:*.Z/
    complete zmore	n/*/f:*.{gz,Z,z,zip}/
    complete ztouch	n/*/f:*.{gz,Z,z,zip,taz,tgz}/
    complete zforce	n/*/f:^*.{gz,tgz}/

    complete grep	c/-*A/x:'<#_lines_after>'/ c/-*B/x:'<#_lines_before>'/\
			c/-/"(A b B c C e f h i l n s v V w x)"/ \
			p/1/x:'<limited_regular_expression>'/ \
			n/-*e/x:'<limited_regular_expression>'/ n/-*f/f/ n/*/f/
    complete egrep	c/-*A/x:'<#_lines_after>'/ c/-*B/x:'<#_lines_before>'/\
			c/-/"(A b B c C e f h i l n s v V w x)"/ \
			p/1/x:'<full_regular_expression>'/ \
			n/-*e/x:'<full_regular_expression>'/ n/-*f/f/ n/*/f/
    complete fgrep	c/-*A/x:'<#_lines_after>'/ c/-*B/x:'<#_lines_before>'/\
			c/-/"(A b B c C e f h i l n s v V w x)"/ \
			p/1/x:'<fixed_string>'/ \
			n/-*e/x:'<fixed_string>'/ n/-*f/f/ n/*/f/


    complete users	p/1/x:'<accounting_file>'/
    complete who	p/1/x:'<accounting_file>'/ n/am/"(i)"/ n/are/"(you)"/
    complete ps	        c/-t/x:'<tty>'/ c/-/"(a c C e g k l S t u v w x)"/ \
			n/-k/x:'<kernel>'/ N/-k/x:'<core_file>'/ n/*/x:'<PID>'/

    complete chgrp	c/-/"(f R)"/ n/-*/x:'<group>'/ p/1/x:'<group>'/ n/*/f/
    complete chown	c/-/"(f R)"/ n/-*/u/           p/1/u/           n/*/f/

    complete cat	c/-/"(b e n s t u v)"/ n/*/f/
    complete mv		c/-/"(f i)"/ n/-*/f/ N/-*/d/ p/1/f/ p/2/d/ n/*/f/
    complete cp		c/-/"(i p r)"/ n/-*r*/d/ n/-*/f/ N/-*/d/ \
			p/1/f/ p/2/d/ n/*/f/

    complete ln		c/-/"(f s)"/ n/-*/f/ N/-*/x:'<link_name>'/ \
			p/1/f/ p/2/x:'<link_name>'/

    complete tar 	c/-[cru]*/"(b B C f F FF h i l m o p v w)"/ \
			c/-[tx]*/"(   B C f F FF h i l m o p v w)"/ \
			p/1/"(-c -r -t -u -x c r t u x)"/ \
			c/-/"(b B C f F FF h i l m o p v w)"/ \
			n/-c*f/x:'<new_tar_file or "-">'/ n/-*f/f:*.tar/ \
			n/-[cru]*b/x:'<block_size>'/ n/-b/x:'<block_size>'/ \
			n/-C/d/ N/-C/x:'<file or ".">'/ n/*/f/

    complete compress	c/-/"(c f v b)"/ n/-b/x:'<max_bits>'/ n/*/f:^*.Z/
    complete uncompress	c/-/"(c f v)"/                        n/*/f:*.Z/

    unset noglob
    unset complete
endif
