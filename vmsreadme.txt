This is the info file for inclusion with the VMS port of TCSH.

----
System Requirements:

  o  VAX running VMS version V5.5 or above.

  o  VMS/Posix installed and configured with a container file system.
       (^ comes free with the operating system)

  o  An /etc/termcap file that contains entries for vtxxx_series

  o  A friendly system manager.

  o  About an hour. (VS 4000/60)

----
Installation:

  1. untar the file.
  2. copy the config.vms file from config/ to the directory that 
     holds the tcsh source, rename to be config.h 
  3. rename Makefile.vms to be Makefile
  4. Type 
	psx> make
  5. Wait for an hour. 
	a) The construction of tc.const.h will take about 20 mins 
           on a vs4000/60
        b) Read the man pages.(see note about man pages)

  6) If you want to install TCSH as the default shell instead of ksh add the 
	following line to POSIX$USER_PARAMS.DAT    
	
	SHELL /usr/local/bin/tcsh 

	You may not include flags on the command line. This line is used
 	to create the executive mode logical name POSIX$SHELL.
	In creating this logical name flags are ignored, so a login shell 
         is not possible. 

  7) TCSH-VMS will use the file /etc/csh.cshrc  to provide the equivalent
 	of a sys$sylogin.com, suggested contents are below.
        You will probably need to make this file using the SYSTEM account.

----
Notes:

  This is not a definition of the capabilities of VMS/Posix. 
  Refer to `Guide to using VMS/Posix' etc...

  VMS/Posix is the OPEN part of OPEN/VMS. It consists of three
  main sections. 

	- The POSIX Libraries, which are system interfaces 
	  adhering to the Callable Interface standards IEEE 1003.1, 
	  1003.1a and 1003.2 and most of 1003.4 (real time). 
	- The POSIX Commands and utilities, 1003.2 (Classic Utilities)
	  some of the Software Development utilities. 
	- The VMS/Posix Shell, a Korn Shell variant.

  TCSH has been ported to be POSIX Conformant. The one thing preventing
  `Strict POSIX Conformance' is the use of Termcap routines that
  require the presence of a /dev/termcap.  Once code has been written
  to use the library routines tcgetattr etc, the source will be
  strictly POSIX conformant.

  TCSH has certain features missing or disabled.

	- UTMP facilities, this is related to most of the
	  inter-user stuff like watchlog.
	- Resource limit access.
	- Access to passwd tag in the passwd structure. 
	  (use DCL for user admin functions)
	- Other items may be missing or behave in an unexpected
	  manner. It has not been possible to test and debug all the
	  features of TCSH on VMS as yet.

----
Features and known bugs:

1)  The VMS/Posix MAN command is a drop through to DCL help. 
  It seems to assume it will always be running from ksh. To use define
  the alias 
	alias man '/bin/sh /bin/man \!*' 
  in your /etc/csh.cshrc
   This fixes the problem.
 
2)   There is a problem with the way TCSH initialises the current working 
   directory ($cwd, environment variable PWD)
     Put the command 
	cd 
     in /etc/csh.cshrc as a workaround.
   For the curious;
      The version of getwd i use is the subroutine in TCSH rather than the
   getcwd from the posix library.
      The tcsh version xgetcwd in tc.os.c will search up the tree, by 
   stat()'ing and opening the parent directory, to build up the CWD path.
   All works well until it hits the root directory. At this point it is 
   usually looking for a file with st_ino =0 and st_dev = 0, (the default
   numbers assigned to the mount point of a VMS filesystem in the '/'
   directory.) which is the first file it comes across. This, due to the 
   way UCX starts is /vms. 
    eg. I am in /u1/atp/fred.
        xgetwd looks at '.' gets the st_ino and st_dev. goes to '..' opens 
   it and gets the name associated with inode and dev from before. CWD now
   holds  "fred". I now repeat the process one layer up. CWD now becomes 
   "atp/fred". The problems now start with the stat of '.' at this point.
   stat() returns a st_ino and st_dev of 0. - i don't know why. When i open
   '..' and read it i get the first name that matches an inode of '0' and 
   a device of '0' - which is /vms. now my CWD is set to be "/vms/atp/fred"
   This is a problem, as /vms/atp/fred does not exist. That is my understanding
   of the problem. 
    This throws the history mechanism, terminal handling, and man pages 
   for some reason. I worked around it by putting a 
   'cd' in my /etc/csh.cshrc which effectively copies the contents of $HOME
   into $PWD.

3) termcap problem 
    If the value of term is not a recognised one (vt200_series, vt300_series)
   then shell utilities like 'more' will complain. Unless your /etc/termcap
   has entries for vt300_series, you will not be able to use %U %B et al, and
    anything else that uses the termcap entries.

4) Line editing problem
    Occasionally the line editing becomes corrupted. This is probably due
   to a conflict in the use of both termios and termcap routines.

5)  TCSH expects to be installed in /usr/local/bin - you may change this
  in Makefile.vms or create /usr/local/bin.

6)  If you have problems with reading things on the path - substitute
  /bin/ for /bin in pathnames.h - VMS/Posix uses symbolic links for all
  the top level directories.

7)  If the shell complains that a command is not found check to see if
  there are multiple version numbers in /bin/ eg. tar..1 tar..2 . if
  there are, get the sysmanager to purge this directory.

---------
suggested csh.cshrc
---
# model csh.cshrc
	alias man '/bin/sh /bin/man \!*' 
        cd 
        set history 50
        stty sane
#end
---

  I read vmsnet.vms-posix, It is a low volume newsgroup, so i tend to
  read all articles, if E-mail fails to work for you get me there.

  I Release the work I have done in porting this to VMS/Posix into the
  public domain under the conditions imposed by the headers contained
  in the source code. I accept no Liability either express or implied
  for any damage or loss arising from the use or failure of this code.
  Have a nice day.

  Andy Phillips 
  Mullard Space Science Lab.     92/10/07
  Holmbury St. Mary.		 19709::atp
  Surrey.			 atp@uk.ac.ucl.mssl 
  England.			  

