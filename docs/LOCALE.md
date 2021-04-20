Locale Considerations
=====================

The editline library has basic support for locales via the use of the standard
POSIX functions `mbrtowc` and `wcwidth`.  As a result, the on-screen width of
characters in printed text should be calculated correctly, the so-called 
[East Asian ambiguous][] characters being a notable exception.

  [East Asian ambiguous]: http://www.unicode.org/reports/tr11/tr11-38.html

The input handling is not yet multibyte-aware. Specifically, the presence of
non-self-synchonizing encodings requires always segmenting the whole string
using `mbrlen` when deleting.

Programs wishing to use the feature need to link to a build of `editline`
configured with `--enable-locale`.  In addition, the program should call
`setlocale(LC_CTYPE, "")` to use the locale settings of the environment.
