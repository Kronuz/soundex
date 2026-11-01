# Changelog

Notable changes to soundex, newest first. The format follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/); entries are dated because the
library has no releases.

## 2026-10-31

### Fixed

- A copy of an encoder from a non-const lvalue (`SoundexEnglish b = a;`, and the same
  for `SoundexFrench`, `SoundexGerman`, `SoundexSpanish` and the `Soundex<Impl>` base),
  or from a const rvalue, did not compile: the forwarding constructor that takes a word
  is a better match than the copy constructor there, and it tried to encode the encoder.
  It also broke `vector::emplace_back(a)`, `std::make_unique<E>(a)` and `std::optional<E>(a)`
  with a non-const `a`. The constructor now excludes the encoder's own type, its derived
  classes and their cv-ref forms (`soundex_detail::is_encoder_v`). Results are unchanged.
  `test/copy_move.cc` (`soundex_copy`) covers every encoder.

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
