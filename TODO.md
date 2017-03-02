TODO
====

Issues in need of work. Mostly compatibility with GNU readline, BSD
[libedit][], and usability improvements.

Remember, the general idea is to keep this library editline small with
no external dependencies, except a C library.


Add support for running in an event loop
----------------------------------------

To be able to use libeditline from within an event loop like [libuEv][]
there are few things to do:

- Refactor `editinput()` and `readline()`.  Break out the active code
  used for set up and teardown, and the character input logic
- Add bare necessities for external callbacks so that an event loop
  that monitors `el_infd` has something to call on events
- GNU Readline has its [alternate interface][gnu] which we should
  probably implement

Example usecase of the GNU alternate interface can be found here:
http://www.mcld.co.uk/blog/blog.php?274


Verify custom completion handlers
---------------------------------

Verify for v1.14.0 that custom completion handlers still work After
reverting a "fix" in v0.2.2 that made `rl_complete()` a function pointer
we need to make sure the same functionality is still available with the
new infrastructure.  Which is more inspired by BSD libedit and GNU
readline.


Check what's needed to run the fileman example
----------------------------------------------

The BSD libedit library has imported the GNU readline "fileman" example
into its tree to demonstrate the abilities of that library.  This would
also be quite useful for this library!

The first task is to investigate the depependencies and form TODO list
items detailing what is missing and, if possible, proposals how to
implement including any optional configure flags.


Other minor TODO's
------------------

- Instead of supporting multiline input, try the Emacs approach, line
  scrolling.
- Add support for `rl_bind_key()`, currently only en editline specific
  `el_bind_key()` exists.
- Make `char *rl_prompt;` globally visible.
- Add support for `rl_set_prompt()`
- Add support for `--enable-utf8` to configure script
- Use `strcmp(nl_langinfo(CODESET), "UTF-8")` to look for utf8 capable
  terminal
- Implement simple UTF-8 parser according to
  http://www.cl.cam.ac.uk/~mgk25/unicode.html#utf-8


[gnu]:     http://www.delorie.com/gnu/docs/readline/rlman_41.html#IDX288
[libuEv]:  https://github.com/troglobit/libuev/
[libedit]: http://www.thrysoee.dk/editline/
