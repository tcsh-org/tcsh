# system-wide .login file for csh(1) and tcsh(1)

# allow for other packages/system admins to customize the shell environment
set _dir=/etc/csh/login.d
if ( -d "${_dir}" ) then
    foreach _file ( `run-parts --list "${_dir}"` )
	source "${_file}"
    end
endif
unset _dir
unset _file
