ChangeLog
=========
Notable Changes

* v1.14.2 - Bug fixes only
  - Fix `el_no_echo` bug causing secrets to leak when disabling no-echo
  - Handle `EINTR` in syscalls better

* v1.14.1 - Minor fixes and additions
  - Don't print status message on `stderr` in key binding funcions
  - Export `el_del_char()`
  - Check for and return pending signals when detected
  - Allow custom key bindings ...
  - Bug fixes ...

* v1.14.0 - Major cleanups and further merges with Debian editline package
  - Merge in changes to `debian/` from editline_1.12-6.debian.tar.gz
  - Migrate to use libtool
  - Cleanups and fixes thanks to the Sparse static code analysis tool
  - Make `UNIQUE_HISTORY` configurable
  - Make scrollback history (`HIST_SIZE`) configurable
  - Configure options for toggling terminal bell and `SIGSTOP` (Ctrl-Z)
  - Configure option for using termcap to read/control terminal size
  - Merge `el_no_echo` patch from Festival speech-tools
  - Rename Signal to `el_intr_pending`, from Festival speech-tools
  - Merge support for capitalizing words (`M-c`) from Festival
    speech-tools by Alan W Black <awb()cstr!ed!ac!uk>
  - Merge fixes from Heimdal project
  - Completely refactor `rl_complete()` and `rl_list_possib()` with
    fixes from the Heimdal project.  Use `rl_set_complete_func()` and
    `rl_set_list_possib_func()`.  Default completion callbacks are now
    available as a configure option `--enable-default-complete`
  - Memory leak fixes
  - Fallback backspace handling, in case `tgetstr("le")` fails
  - Actually fix 8-bit handling by reverting old Debian patch
  - Merge patch to improve compatibility with GNU readline, thanks to
    Steve Tell from way back in 1997 and 1998

* v1.13.0 - Adaptations to Debian editline package
  - Major version number bump, adapt to Jum Studt's v1.12
  - Import `debian/` directory and adapt it to configure et al.
  - Change library name to libeditline to distinguish it from BSD libedit

* v0.3.0
  - Support for ANSI arrow keys using `--enable-arrow-keys`

* v0.2.x
  - Patches from Debian package merged
  - Support for custom command completion

