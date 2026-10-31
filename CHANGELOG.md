# Changelog

Notable changes to soundex, newest first. The format follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/); entries are dated because the
library has no releases.

## 2026-10-30

### Changed

- `SoundexEnglish` encodes in one pass: it takes a `std::string_view`, looks each byte
  up in a 256-entry class table, and writes the code once into a new string. Before,
  it copied the input and then erased and inserted in place. Output is identical.
  (`dd429fd`)

### Added

- `test/diff_test.cc` checks `SoundexEnglish` against the reference in-place encoder
  in `test/frozen.h` over every string up to four characters of a mixed alphabet plus
  random strings.
- `bench/bench.cc` (`soundex_bench`) times both encoders.

## 2026-10-14

### Fixed

- The German and Spanish two-letter rules lived in unordered maps and were applied in
  hash order, which differs between libc++ and libstdc++, so overlapping rules coded
  the same word differently by platform ("Schmidt" was 84602 on Linux and 8602 on
  macOS). They are ordered vectors. German follows Kölner Phonetik: C after S or Z,
  and D or T before C, S, or Z, code as 8 before C before A, H, K, O, Q, U, or X codes
  as 4. (`271e129`)
