$set 1 # Error messages
1 Syntax Error
2 %s is not allowed
3 Word too long
4 $< line too long
5 No file for $0
6 Incomplete [] modifier
7 $ expansion must end before ]
8 Bad : modifier in $ (%c)
9 Subscript error
10 Badly formed number
11 No more words
12 Missing file name
13 Internal glob error
14 Command not found
15 Too few arguments
16 Too many arguments
17 Too dangerous to alias that
18 Empty if
19 Improper then
20 Words not parenthesized
21 %s not found
22 Improper mask
23 No such limit
24 Argument too large
25 Improper or unknown scale factor
26 Undefined variable
27 Directory stack not that deep
28 Bad signal number
29 Unknown signal; kill -l lists signals
30 Variable name must begin with a letter
31 Variable name too long
32 Variable name must contain alphanumeric characters
33 No job control in this shell
34 Expression Syntax
35 No home directory
36 Can't change to home directory
37 Invalid null command
38 Assignment missing expression
39 Unknown operator
40 Ambiguous
41 %s: File exists
42 Argument for -c ends in backslash
43 Interrupted
44 Subscript out of range
45 Line overflow
46 No such job
47 Can't from terminal
48 Not in while/foreach
49 No more processes
50 No match
51 Missing %c
52 Unmatched %c
53 Out of memory
54 Can't make pipe
55 %s: %s
56 %s
57 Usage: jobs [ -l ]
58 Arguments should be jobs or process id's
59 No current job
60 No previous job
61 No job matches pattern
62 Fork nesting > %d; maybe `...` loop
63 No job control in subshells
64 Sunc fault: Process %d not found
65 %sThere are suspended jobs
66 %sThere are stopped jobs
67 No other directory
68 Directory stack empty
69 Bad directory
70 Usage: %s [-%s]%s
71 No operand for -h flag
72 Not a login shell
73 Division by 0
74 Mod by 0
75 Bad scaling; did you mean "%s"?
76 Can't suspend a login shell (yet)
77 Unknown user: %s
78 No $home variable set
79 Usage: history [-%s] [# number of events]
80 $, ! or < not allowed with $# or $?
81 Newline in variable name
82 * not allowed with $# or $?
83 $?<digit> or $#<digit> not allowed
84 Illegal variable name
85 Newline in variable index
86 Expansion buffer overflow
87 Variable syntax
88 Bad ! form
89 No previous substitute
90 Bad substitute
91 No previous left hand side
92 Right hand side too long
93 Bad ! modifier: %c
94 Modifier failed
95 Substitution buffer overflow
96 Bad ! arg selector
97 No prev search
98 %s: Event not found
99 Too many )'s
100 Too many ('s
101 Badly placed (
102 Missing name for redirect
103 Ambiguous output redirect
104 Can't << within ()'s
105 Ambiguous input redirect
106 Badly placed ()'s
107 Alias loop
108 No $watch variable set
109 No scheduled events
110 Usage: sched -<item#>.\nUsage: sched [+]hh:mm <command>
111 Not that many scheduled events
112 No command to run
113 Invalid time for event
114 Relative time inconsistent with am/pm
115 Out of termcap string space
116 Usage: settc %s [yes|no]
117 Unknown capability `%s'
118 Unknown termcap parameter `%%%c'
119 Too many arguments for `%s' (%d)
120 `%s' requires %d arguments
121 Usage: echotc [-v|-s] [<capability> [<args>]]
122 %s: %s. Wrong Architecture
123 !# History loop
124 Malformed file inquiry
125 Selector overflow
126 Unknown option: `-%s'\nUsage: tcsh [ -bcdefilmnqstvVxX -Dname[=value] ] [ argument ... ]
127 Unknown option: `-%s'\nUsage: tcsh [ -bcdefFilmnqstvVxX ] [ argument ... ]
128 Unknown option: `-%s'\nUsage: tcsh [ -bcdefilmnqstvVxX ] [ argument ... ]
129 \nInvalid completion: "%s"
130 \nInvalid %s: '%c'
131 \nMissing separator '%c' after %s "%s"
132 \nIncomplete %s: "%s"
133 No operand for -m flag
134 Usage: unlimit [-fh] [limits]
135 $%S is read-only
136 No such job
$set 2 # Signal names
1 Null signal
2 Hangup
3 Interrupt
4 Quit
5 Illegal instruction
6 Trace/BPT trap
7 Abort
8 IOT trap
9 System Crash Imminent
10 Error exit
11 EMT trap
12 Floating exception
13 Killed
14 User signal 1
15 User signal 2
16 Segmentation fault
17 Bus error
18 Program range error
19 Operand range error
20 Bad system call
21 Broken pipe
22 Alarm clock
23 Terminated
24 Child status change
25 Death of child
26 Apollo-specific fault
27 Child stopped or exited
28 Child exited
29 Power failure
30 Resource Lost
31 Break (Ctrl-Break)
32 Input/output possible signal
33 Asynchronous I/O (select)
34 Urgent condition on I/O channel
35 Multitasking wake-up
36 Multitasking kill
37 Fortran asynchronous I/O completion
38 Recovery
39 Uncorrectable memory error
40 CPU time limit exceeded
41 System shutdown imminent
42 micro-tasking group-no wakeup flag set
43 Thread error - (use cord -T for detailed info)
44 CRAY Y-MP register parity error
45 Information request
46 Suspended (signal)
47 Stopped (signal)
48 Suspended
49 Stopped
50 Continued
51 Suspended (tty input)
52 Stopped (tty input)
53 Suspended (tty output)
54 Stopped (tty output)
55 Window status changed
56 Window size changed
57 Phone status changed
58 Cputime limit exceeded
59 Filesize limit exceeded
60 Virtual time alarm
61 Profiling time alarm
62 DIL signal
63 Pollable event occured
64 Process's lwps are blocked
65 Special LWP signal
66 Special CPR Signal
67 Special CPR Signal
68 First Realtime Signal
69 Second Realtime Signal
70 Third Realtime Signal
71 Fourth Realtime Signal
72 Fourth Last Realtime Signal
73 Third Last Realtime Signal
74 Second Last Realtime Signal
75 Last Realtime Signal
76 LAN Asyncronous I/O
77 PTY read/write availability
78 I/O intervention required
79 HFT monitor mode granted
80 HFT monitor mode should be relinguished
81 HFT sound control has completed
82 Data in HFT ring buffer
83 Migrate process
84 Secure attention key
85 Reschedule
86 Signaling SS$_DEBUG
87 Priority changed
88 True deadlock detected
89 New input character
$set 3 # Editor function descriptions
1 Move back a character
2 Delete the character behind cursor
3 Cut from beginning of current word to cursor - saved in cut buffer
4 Cut from beginning of line to cursor - save in cut buffer
5 Move to beginning of current word
6 Move to beginning of line
7 Capitalize the characters from cursor to end of current word
8 Vi change case of character under cursor and advance one character
9 Vi change to end of line
10 Clear screen leaving current line on top
11 Complete current word
12 Tab forward through files
13 Tab backwards through files
14 Complete current word ignoring programmable completions
15 Copy current word to cursor
16 Copy area between mark and cursor to cut buffer
17 Delete character under cursor
18 Delete character under cursor or signal end of file on an empty line
19 Delete character under cursor or list completions if at end of line
20 Delete character under cursor, list completions or signal end of file
21 Cut from cursor to end of current word - save in cut buffer
22 Adds to argument if started or enters digit
23 Digit that starts argument
24 Move to next history line
25 Lowercase the characters from cursor to end of current word
26 Indicate end of file
27 Move cursor to end of line
28 Exchange the cursor and mark
29 Expand file name wildcards
30 Expand history escapes
31 Expand the history escapes in a line
32 Expand variables
33 Move forward one character
34 Move forward to end of current word
35 Exchange the two characters before the cursor
36 Search in history backwards for line beginning as current
37 Search in history forward for line beginning as current
38 Insert last item of previous command
39 Incremental search forward
40 Incremental search backwards
41 Clear line
42 Cut to end of line and save in cut buffer
43 Cut area between mark and cursor and save in cut buffer
44 Cut the entire line and save in cut buffer
45 List choices for completion
46 List choices for completion overriding programmable completion
47 List file name wildcard matches
48 List choices for completion or indicate end of file if empty line
49 Display load average and current process status
50 Expand history escapes and insert a space
51 Execute command
52 Expand pathnames, eliminating leading .'s and ..'s
53 Expand commands to the resulting pathname or alias
54 Switch from insert to overwrite mode or vice versa
55 Add 8th bit to next character typed
56 Add the next character typed to the line verbatim
57 Redisplay everything
58 Restart stopped editor
59 Look for help on current command
60 This character is added to the line
61 This character is the first in a character sequence
62 Set the mark at cursor
63 Correct the spelling of current word
64 Correct the spelling of entire line
65 Send character to tty in cooked mode
66 Toggle between literal and lexical current history line
67 Exchange the character to the left of the cursor with the one under
68 Exchange the two characters before the cursor
69 Tty delayed suspend character
70 Tty flush output character
71 Tty interrupt character
72 Tty quit character
73 Tty suspend character
74 Tty allow output character
75 Tty disallow output character
76 Indicates unbound character
77 Emacs universal argument (argument times 4)
78 Move to previous history line
79 Uppercase the characters from cursor to end of current word
80 Vi goto the beginning of next word
81 Vi enter insert mode after the cursor
82 Vi enter insert mode at end of line
83 Vi change case of character under cursor and advance one character
84 Vi change prefix command
85 Vi change to end of line
86 Enter vi command mode (use alternative key bindings)
87 Vi command mode complete current word
88 Vi move to previous character (backspace)
89 Vi delete prefix command
90 Vi move to the end of the current space delimited word
91 Vi move to the end of the current word
92 Vi move to the character specified backwards
93 Vi move to the character specified forward
94 Vi move up to the character specified backwards
95 Vi move up to the character specified forward
96 Enter vi insert mode
97 Enter vi insert mode at beginning of line
98 Vi repeat current character search in the same search direction
99 Vi repeat current character search in the opposite search direction
100 Vi repeat current search in the same search direction
101 Vi repeat current search in the opposite search direction
102 Vi replace character under the cursor with the next character typed
103 Vi replace mode
104 Vi search history backwards
105 Vi search history forward
106 Vi replace character under the cursor and enter insert mode
107 Vi replace entire line
108 Vi move to the previous word
109 Vi move to the next word
110 Vi undo last change
111 Vi goto the beginning of line
112 Perform which of current command
113 Paste cut buffer at cursor position
$set 4 # Termcap strings
1 add new blank line
2 audible bell
3 clear to bottom
4 clear to end of line
5 cursor to horiz pos
6 clear screen
7 delete a character
8 delete a line
9 start delete mode
10 end delete mode
11 end insert mode
12 cursor from status line
13 home cursor
14 insert character
15 start insert mode
16 insert padding
17 sends cursor down
18 sends cursor left
19 sends cursor right
20 sends cursor up
21 begin bold
22 end attributes
23 non destructive space
24 end standout
25 begin standout
26 cursor to status line
27 cursor up one
28 begin underline
29 end underline
30 visible bell
31 delete multiple chars
32 cursor down multiple
33 insert multiple chars
34 cursor left multiple
35 cursor right multiple
36 cursor up multiple
37 Has automatic margins
38 Can use physical tabs
39 Number of lines
40 Number of columns
41 Has meta key
42 Newline ignored at right margin
$set 5 # ed.chared.c
1 Load average unavailable\n
$set 6 # ed.inputl.c
1 ERROR: illegal command from key 0%o\r\n
2 yes\n
3 edit\n
4 abort\n
5 no\n
6 No matching command\n
7 Ambiguous command\n
8 *** editor fatal ERROR ***\r\n\n
$set 7 # ed.screen.c
1 \n\tTcsh thinks your terminal has the\n
2 \tfollowing characteristics:\n\n
3 \tIt has %d columns and %d lines\n
4 \tIt has %s meta key\n
5 a
6 no
7 \tIt can%s use tabs\n
8 not
9 \tIt %s automatic margins\n
10 has
11 does not have
12 \tIt %s magic margins\n
13 (empty)
14 yes
15 no
16 ERROR: cannot delete\r\n
17 DeleteChars: num is riduculous: %d\r\n
18 ERROR: cannot insert\r\n
19 StartInsert: num is riduculous: %d\r\n
20 tcsh: Cannot open /etc/termcap.\n
21 tcsh: No entry for terminal type "%s"\n
22 tcsh: using dumb terminal settings.\n
23 tcsh: WARNING: Your terminal cannot move up.\n
24 Editing may be odd for long lines.\n
25 no clear EOL capability.\n
26 no delete char capability.\n
27 no insert char capability.\n
$set 8 # ed.term.c
1 Unknown switch
2 Invalid argument
$set 9 # ed.xmap.c
1 AddXkey: Null extended-key not allowed.\n
2 AddXkey: sequence-lead-in command not allowed\n
3 DeleteXkey: Null extended-key not allowed.\n
4 Unbound extended key "%S"\n
5 Some extended keys too long for internal print buffer
6 Enumerate: BUG!! Null ptr passed\n!
7 no input
8 Something must follow: %c\n
9 Octal constant does not fit in a char.\n
$set 10 # ma.setp.c
1 setpath: invalid command '%s'.\n
2 setpath: insufficient arguments to command '%s'.\n
3 setpath: value missing in path '%s'\n
4 setpath: %s not found in %s\n
5 setpath: %d not valid position in %s\n
$set 11 # sh.c
1 Warning: no access to tty (%s).\n
2 Thus no job control in this shell.\n
3 You have %d mail messages.\n
4 You have %d mail messages in %s.\n
5 You have %smail.\n
6 new 
7 You have %smail in %s.\n
$set 12 # sh.dir.c
1 tcsh: Trying to start from "%s"\n
$set 13 # sh.exec.c
1 hash=%-4d dir=%-2d prog=%s\n
2 %d hash buckets of %d bits each\n
3 debug mask = 0x%08x\n
4 %d hits, %d misses, %d%%\n
5 %S: shell built-in command.\n
6 %S: Command not found.\n
7 where: / in command makes no sense\n
8 %S is aliased to 
9 %S is a shell built-in\n
10 hash miss: 
$set 14 # sh.file.c
1 \nYikes!! Too many %s!!\n
3 names in password file
3 files
$set 15 # sh.func.c
1 %s: %s: Can't %s%s limit\n
2 remove
3 set
4 \020hard
$set 16 # sh.lex.c
1 Reset tty pgrp from %d to %d\n
2 \nUse "logout" to logout.\n
3 \nUse "exit" to leave tcsh.\n
4 seek to eval %x %x\n
5 seek to alias %x %x\n
6 seek to file %x\n
7 Bad seek type %d\n
8 tell eval %x %x\n
9 tell alias %x %x\n
10 tell file %x\n
$set 17 # sh.proc.c
1 BUG: waiting for background job!\n
2 Exit %d\n
3 BUG: process flushed twice
4 Running 
5 Signal 
6 Exit %-25d
7 Done
8 BUG: status=%-9o
9 \020(core dumped)
10 \020(wd: 
11 wd now: 
12 %S: Already suspended\n
13 %S: Already stopped\n
$set 18 # sh.set.c
1 Warning: ridiculously long PATH truncated\n
$set 19 # tc.alloc.c
1 nbytes=%d: Out of memory\n
2 free(%lx) called before any allocations.
3 free(%lx) above top of memory.
4 free(%lx) below bottom of memory.
5 free(%lx) bad block.
6 free(%lx) bad range check.
7 free(%lx) bad block index.
8 tcsh current memory allocation:\nfree:\t
9 \nused:\t
10 \n\tTotal in use: %d, total free: %d\n
11 \tAllocated memory from 0x%lx to 0x%lx.  Real top at 0x%lx\n
12 Allocated memory from 0x%lx to 0x%lx (%ld).\n
$set 20 # tc.bind.c
1 Invalid key name `%S'\n
2 Bad key name: %S\n
3 Bad command name: %S\n
4 Bad key spec %S\n
5 Null string specification\n
6 Standard key bindings\n
7 Alternative key bindings\n
8 Multi-character bindings\n
9 Arrow key bindings\n
10 %-15s->  is undefined\n
11 BUG!!! %s isn't bound to anything.\n
12 Usage: bindkey [options] [--] [KEY [COMMAND]]\n
13     -a   list or bind KEY in alternative key map\n
14     -b   interpret KEY as a C-, M-, F- or X- key name\n
15     -s   interpret COMMAND as a literal string to be output\n
16     -c   interpret COMMAND as a builtin or external command\n
17     -v   bind all keys to vi bindings\n
18     -e   bind all keys to emacs bindings\n
19     -d   bind all keys to default editor's bindings\n
20     -l   list editor commands with descriptions\n
21     -r   remove KEY's binding\n
22     -k   interpret KEY as a symbolic arrow-key name\n
23     --   force a break from option processing\n
24     -u   (or any invalid option) this message\n
25 Without KEY or COMMAND, prints all bindings\n
26 Without COMMAND, prints the binding for KEY.\n
27 bad key specification -- null string\n
28 bad key specification -- empty string\n
29 Bad function-key specification.  Null key not allowed\n
30 bad key specification -- malformed hex number\n
31 bad key specification -- malformed octal number\n
32 bad key specification -- malformed decimal number\n
33 Bad function-key specification.\n
34 Null key not allowed\n
35 bad key specification -- unknown name "%S"\n
36 usage: bind [KEY | COMMAND KEY | "emacs" | "vi" | "-a"]\n
37 Invalid function
38  %s\t\tis undefined\n
$set 21 # tc.disc.c
1 Couldn't get local chars.\n
2 Couldn't set local chars.\n
$set 22 # tc.func.c
1 %S: \t aliased to 
2 \nIncorrect passwd for %s\n
3 Faulty alias 'precmd' removed.\n
4 Faulty alias 'cwdcmd' removed.\n
5 Faulty alias 'beepcmd' removed.\n
6 Faulty alias 'periodic' removed.\n
7 parsing command line\n
8 Do you really want to delete all files? [n/y] 
9 skipping deletion of files!\n
10 command line now is:\n
11 parsing command line\n
12 in one of the lists\n
13 command line now is:\n
$set 23 # tc.os.c
1 Bad cpu/site name
2 Site path too long
3 unknown
4 site: %s\n
5 %d: Site not found\n
6 setlocal: %s: %s\n
7 Site not found
8 You're trapped in a universe you never made
9 Getwarp failed
10 Invalid warp
11 Setwarp failed
12 Illegal universe
13 Unknown Error: %d
14 sysname:  %s\n
15 nodename: %s\n
16 release:  %s\n
17 version:  %s\n
18 machine:  %s\n
19 getwd: Cannot open ".." (%s)
20 getwd: Cannot chdir to ".." (%s)
21 getwd: Read error in ".." (%s)
22 getwd: Cannot change back to "." (%s)
23 getwd: Cannot stat "/" (%s)
24 getwd: Cannot stat directory "%s" (%s)
25 getwd: Cannot open directory "%s" (%s)
26 getwd: Cannot find "." in ".." (%s)
27 Invalid system type
28 System type is not set
$set 24 # tc.sched.c
1 kludge
$set 25 # tc.sig.c
1 our wait %d\n
2 error: bsd_signal(%d) signal out of range\n
3 error: bsd_signal(%d) - sigaction failed, errno %d\n
$set 26 # tc.who.c
1 cannot stat %s.  Please "unset watch".\n
2 %s cannot be opened.  Please "unset watch".\n
3 BUG! last element is not whotail!\n
4 backward: 
5 BUG! first element is not whohead!\n
6 new: %s/%s\n
7 %n has %a %l from %m.
8 %n has %a %l.
9 logged on
10 logged off
11 replaced %s on
12 local
$set 27 # tw.comp.c
1 command
2 separator
3 pattern
4 range
5 completion
$set 29 # tw.help.c
1 No help file for %S\n
$set 30 # tw.parse.c
1 starting_a_command %d\n
2 complete %d 
3 complete %d %S\n
4 tcsh: Internal match error.\n
5 items
6 rows)
7 There are %d %s, list them anyway? [n/y] 
8 looking = %d\n
9 \ntcsh internal error: I don't know what I'm looking for!\n
10 not a directory
11 not found
12 unreadable
$set 31 # vms.termcap.c
1 Can't open TERMCAP: [%s]\n
2 Can't open %s.\n
3 Found %s in %s.\n
4 No match found for %s in file %s\n
