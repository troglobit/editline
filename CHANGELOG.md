Change Log
==========

All notable changes to the project are documented in this file.

[UNRELEASED]
------------

### Changes
* Rename NEWS.md --> CHANGELOG.md, with symlinks for `make install`
* Attempt to align with http://keepachangelog.com/ for this file.


[1.14.2] - 2014-09-14
---------------------

Bug fixes only.

### Fixes
  - Fix `el_no_echo` bug causing secrets to leak when disabling no-echo
  - Handle `EINTR` in syscalls better


[1.14.1] - 2014-09-14
---------------------

Minor fixes and additions.

### Changes
* Don't print status message on `stderr` in key binding funcions
* Export `el_del_char()`
* Check for and return pending signals when detected
* Allow custom key bindings ...

### Fixes
* Bug fixes ...


[1.14.0] - 2010-08-10
---------------------

Major cleanups and further merges with Debian editline package.

### Changes
* Merge in changes to `debian/` from `editline_1.12-6.debian.tar.gz`
* Migrate to use libtool
* Make `UNIQUE_HISTORY` configurable
* Make scrollback history (`HIST_SIZE`) configurable
* Configure options for toggling terminal bell and `SIGSTOP` (Ctrl-Z)
* Configure option for using termcap to read/control terminal size
* Rename Signal to `el_intr_pending`, from Festival speech-tools
* Merge support for capitalizing words (`M-c`) from Festival
  speech-tools by Alan W Black <awb()cstr!ed!ac!uk>
* Fallback backspace handling, in case `tgetstr("le")` fails

### Fixes
* Cleanups and fixes thanks to the Sparse static code analysis tool
* Merge `el_no_echo` patch from Festival speech-tools
* Merge fixes from Heimdal project
* Completely refactor `rl_complete()` and `rl_list_possib()` with
  fixes from the Heimdal project.  Use `rl_set_complete_func()` and
  `rl_set_list_possib_func()`.  Default completion callbacks are now
  available as a configure option `--enable-default-complete`
* Memory leak fixes
* Actually fix 8-bit handling by reverting old Debian patch
* Merge patch to improve compatibility with GNU readline, thanks to
  Steve Tell from way back in 1997 and 1998


[1.13.0] - 2010-03-09
---------------------

Adaptations to Debian editline package.

### Changes
* Major version number bump, adapt to Jim Studt's v1.12
* Import `debian/` directory and adapt it to configure et al.
* Change library name to libeditline to distinguish it from BSD libedit


[0.3.0] - 2009-02-08
--------------------

### Changes
* Support for ANSI arrow keys using <kbd>configure --enable-arrow-keys</kbd>


[0.2.x] - 2008-12-02
--------------------

### Changes
* Patches from Debian package merged
* Support for custom command completion

[UNRELEASED]: https://github.com/troglobit/finit/compare/1.14.2...HEAD
[1.14.2]:     https://github.com/troglobit/finit/compare/1.14.1...1.14.2
[1.14.1]:     https://github.com/troglobit/finit/compare/1.14.0...1.14.1
[1.14.0]:     https://github.com/troglobit/finit/compare/1.13.0...1.14.0
[1.13.0]:     https://github.com/troglobit/finit/compare/0.3.0...1.13.0
[0.3.0]:      https://github.com/troglobit/finit/compare/0.2.3...0.3.0
[0.2.x]:      https://github.com/troglobit/finit/compare/0.0.0...0.2.3
