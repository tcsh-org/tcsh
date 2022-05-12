;;; 50tcsh.el -- debian emacs setups for tcsh

(if (not (file-exists-p "/usr/share/emacs/site-lisp/csh-mode.el"))
    (message "tcsh removed but not purged, skipping setup")

  (autoload 'csh-mode "csh-mode"
    "csh-mode 2.0 - Major mode for editing csh and tcsh scripts." t)

  ;; Emacs comes with `interpreter-mode-alist' settings to put
  ;; #!/usr/bin/csh and #!/usr/bin/tcsh files in `sh-mode'.
  ;; Whether you want `sh-mode' or `csh-mode' is a matter of personal
  ;; preference.  For now we leave the Emacs supplied sh-mode as the default.
  ;; Uncomment the lines below if you want csh-mode system-wide, or copy and
  ;; uncomment them in your .emacs for individual use.
  ;;
  ;; The lines are as recommended by docstring of `csh-mode', except
  ;;   * \' for end of string
  ;;   * .tcsh too, eg. /usr/share/doc/util-linux/examples/getopt-parse.tcsh
  ;; .login is allowed to be a suffix since it can appear as extension like
  ;; /etc/csh.login as well as whole name like ~/.login

  ;; (add-to-list 'auto-mode-alist '("\\.t?csh\\'" . csh-mode))
  ;; (add-to-list 'auto-mode-alist '("\\.login\\'" . csh-mode))
  ;; (add-to-list 'interpreter-mode-alist '("t?csh\\'" . csh-mode))
  )
