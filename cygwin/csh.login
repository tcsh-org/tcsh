#
# Example /etc/csh.login for Cygwin
#
unsetenv TEMP
unsetenv TMP

set winpath = ( $path:q )
if ( ${?CYGWIN_NOWINPATH} ) then
  set path=( /usr/bin )
else
  set path=( /usr/bin $path:q )
endif
if ( -d /etc/profile.d ) then
  set nonomatch
  foreach _s ( /etc/profile.d/*.csh )
    if ( -r $_s ) then
      source $_s
    endif
  end
  unset _s nonomatch
endif

if ( ! ${?USER} ) then
  set user="`id -un`"
endif
if ( ! ${?HOME} ) then
  set home=/home/$USER
endif
if ( ! -d "$HOME" ) then
  mkdir -p "$HOME"
endif

if ( ! ${?term} || "$term" == "unknown"  || "$tty" == "conin" ) then
  set term=cygwin
endif

setenv MAKE_MODE unix

setenv SHELL /bin/tcsh

umask 022

cd
