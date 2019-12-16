# Developer Tools

You need the following tools to fully work on tcsh development.

- `autoconf`
- `gettext` (for `AM_ICONV` in `share/aclocal/intl.m4`)

## Debian

The `autoconf` scripts on Debian add a `--runstatedir` option
to `configured` that we don't have in the `tcsh` repository,
due to running `autoreconf` on NetBSD.

Install `gettext` to get `AM_ICONV`. The `gettext-base` package
does not include the needed files in `/usr/share/aclocal`.

## NetBSD

Install `pkgsrc/devel/gettext-m4` to get `AM_ICONV`.
