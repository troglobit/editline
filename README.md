Minix Editline
==============
[![Build Status](https://travis-ci.org/troglobit/editline.png?branch=master)](https://travis-ci.org/troglobit/editline)
[![Coverity Scan Status](https://scan.coverity.com/projects/2982/badge.svg)](https://scan.coverity.com/projects/2982)


Introduction
------------

This is a small line editing library.  It can be linked into almost any
program to provide command line editing and history functions.  It is
call compatible with the FSF readline library, but at a fraction of the
size, and as a result fewer features.

The small size (<30k), lack of dependencies (no ncurses needed!), and
the free license should make this library interesting to many embedded
developers.

Editline has several optional build-time features that can be enabled by
by supplying different options to the GNU configure script.  See the
output from <kbd>configure --help</kbd> for details.  In the `examples/`
directory you can find some small code snippets used for testing.


API
---

*TODO*


Example
-------

*TODO*


Origin & References
--------------------

The editline library was created by Simmule Turner and Rich Salz back in
in 1992.  It is distributed under a "C News-like" license, similar to
the [BSD License].  For details, see the file LICENSE.

This version of the editline library is forked from the [Minix 3] tree.
Other known versions, often based off of the original comp.sources.unix
posting, are:

* Debian [libeditline]
* [Heimdal]
* [Festival] speech-tools
* [Steve Tell]'s editline patches

The most intersting patches and bug fixes from each fork have been
merged here.  Outstanding issues are listed in the TODO file.

An explanation of the version numbering may be in order.  I didn't know
about the Debian version for quite some time, so I kept a different name
for the package and a different versioning scheme.  In June 2009, I
decided to line up alongside Debian, with the intent of eventually
merging the efforts.  However, despite several attempts, the Debian
maintainer has not responded to my emails.


[Minix 3]:     http://www.minix3.org/
[BSD License]: http://en.wikipedia.org/wiki/BSD_licenses
[libeditline]: http://packages.qa.debian.org/e/editline.html
[Heimdal]:     http://www.h5l.org
[Festival]:    http://festvox.org/festival/
[Steve Tell]:  http://www.cs.unc.edu/~tell/dist.html
