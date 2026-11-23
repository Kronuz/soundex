# Constant tables

The French, German and Spanish encoders keep their rules in tables
(`french_accents`, `french_composed`, `french_prefixes`, `german_accents`,
`german_composed`, `spanish_accents`, `spanish_composed`). Each is an
`inline constexpr` `std::array` of `std::pair<std::string_view, std::string_view>`
defined in its header:

- One definition program-wide, constant-initialized: including a header runs no
  static initializer and allocates nothing. A TU that uses an encoder references the
  table in read-only data (one copy after linking).
- Composed and prefix tables apply in declaration order, so overlapping rules code a
  word the same way everywhere. Accent tables hold distinct UTF-8 keys that map to
  ASCII, so their order does not matter.
- Keep new tables constant. A `static const` map or vector in a header brings back a
  static initializer and a heap copy in every including TU.

`test/init_size.sh BUILD_DIR` (Release build dir) builds `test/include_all.cc`, a TU
that only includes the four headers, and prints its section sizes.
`test/diff_languages.cc` compares the three encoders with frozen copies of the old
headers in `test/frozen_languages.h`.

## Measured

A TU that only includes the four headers, Release (`-O3 -DNDEBUG`), arm64 macOS,
`test/init_size.sh build` (`size -m`):

| Section | soundex:be2f07c (`static const` maps and vectors) | soundex:b2dc2ca (`inline constexpr` arrays) |
|---|---|---|
| `__StaticInit` | 12,272 B | 0 B |
| `__text` | 6,678 B | 0 B |
| `__mod_init_func` | 8 B | 0 B |
| object total | 22,136 B | 0 B |

A TU that uses every encoder (`test/test.cc`) has no `__StaticInit` and holds the
tables as 4,128 B of read-only data.
