# system-wide .cshrc file for csh(1) and tcsh(1)

if ($?tcsh && $?prompt) then
    bindkey "\e[1~" beginning-of-line	# Home
    bindkey "\e[7~" beginning-of-line	# Home rxvt
    bindkey "\e[2~" overwrite-mode	# Ins
    bindkey "\e[3~" delete-char		# Delete
    bindkey "\e[4~" end-of-line		# End
    bindkey "\e[8~" end-of-line		# End rxvt

    set prompt="%U%N@%m%u:%B%c02%b%# "
endif

# allow for other packages/system admins to customize the shell environment
set _dir=/etc/csh/cshrc.d
if ( -d "${_dir}" ) then
    foreach _file ( `run-parts --list "${_dir}"` )
	source "${_file}"
    end
endif
unset _dir
unset _file
