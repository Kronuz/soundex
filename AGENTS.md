# AGENTS.md

Working notes for agents modifying this repository. For the design read
`ARCHITECTURE.md`; for usage read `README.md`. This file covers the repo layout,
how to build and test, the invariants you must not break, and the traps that are
easy to fall into.

## Repo map

```
soundex.h                    CRTP base Soundex<Impl> + replace/replace_prefix helpers. Header.
english_soundex.h            SoundexEnglish (refined Soundex). Header.
french_soundex.h             SoundexFrench (Soundex 2). Header.
german_soundex.h             SoundexGerman (Kölner Phonetik). Header.
spanish_soundex.h            SoundexSpanish. Header.
test/test.cc                 Runnable smoke test: encodes words in all four languages, asserts exact codes.
examples/demo.cc             Runnable tour (not a test): prints encodings per language.
CMakeLists.txt               INTERFACE library `soundex` (+ alias soundex::soundex); links strings; CTest test `soundex`.
LICENSE                      MIT, Copyright (c) 2015-2019 Dubalu LLC.
README.md                    What it is, install, quick start.
docs/API.md                  The four encoders, their references, and usage.
docs/soundex.d2              The README diagram's source (docs/soundex.svg is rendered from it).
ARCHITECTURE.md              Internal design: the CRTP base, the four encoders, what was stripped.
```

Everything is header-only. There is no `.cc` to compile except the test and the
demo. The CMake target is a pure `INTERFACE` library that adds the source dir to
the include path, requests `cxx_std_20`, and links `strings::strings`.

## Build and run the test

```sh
cmake -B build && cmake --build build && ctest --test-dir build
```

The first configure fetches `strings` (and its dependency chain) over the
network, so the configure step needs network access. Expected output ends with
`all soundex tests passed`, exit 0. The test target is `soundex_test`; the
registered CTest name is `soundex`. A `soundex_demo` target builds alongside.

## Conventions

- **C++20.** The target requests `cxx_std_20` to stay uniform with the sibling
  libraries.
- **Filenames are stable.** The headers keep their original Xapiand names
  (`soundex.h`, `english_soundex.h`, ...) so a consumer that already `#include`s
  them just needs this repo on the include path. Don't rename them.
- **One dependency.** `strings` (for `strings::inplace_upper`), pulled via
  FetchContent. The English encoder and the base header need only std headers;
  don't add dependencies they don't use.
- Tabs for indentation, double quotes in code, no em dashes in prose.

## Load-bearing invariants

- **These are the refined encoders, codes are not truncated.** The codes are long
  and keep vowels (English maps vowels to `0`), so `SoundexEnglish("Robert")` is
  `R901096`, not the classic `R163`. Don't "fix" this toward textbook Soundex;
  it would change every code and break the test. If you want classic fixed-length
  Soundex, that is a different algorithm, add it as a new variant.
- **The homophone collapse is the point.** Words that sound alike must share a
  code: `Robert`/`Rupert` in English, `Vaca`/`Baca` in Spanish (the leading `V`
  normalizes to `B`). The test pins both; keep them passing.
- **`encode()` vs `encode(word)` are different.** No-arg `encode()` returns the
  code stored at construction; `encode(word)` runs the algorithm on a fresh word.
  Don't collapse them.
- **The CRTP wiring is load-bearing.** Each encoder is `class SoundexX : public
  Soundex<SoundexX>` with `friend class Soundex<SoundexX>` and private
  `_encode` / `_name` / `_description`. The base reaches them via
  `static_cast<const Impl*>(this)`. Keep that shape when adding a variant.

## The persistence strip

The base `Soundex` in Xapiand carried `serialise()` / `unserialise()` methods
(and the `"length.h"` + `"xapian/common/serialise-double.h"` includes behind
them) purely to persist a configured encoder into a Xapiand index. Those were
removed in extraction; the standalone library is the pure algorithm. **Do not
re-add serialise/unserialise here**, and do not pull `length.h` or the GPL
`serialise-double.h` back in. If Xapiand needs to persist an encoder when it
de-vendors, it keeps that adapter on its own side.

## How to extend

- **Add a language.** Mirror an existing header: subclass `Soundex<SoundexX>`,
  `friend` the base, define `static const` tables at namespace scope, implement
  private `_encode` / `_name` / `_description`, and the two constructors
  (default + templated). If you need upper-casing, reuse
  `strings::inplace_upper`; don't add a new dependency for it.
- **Always extend the smoke test.** `test/test.cc` is the only executable check.
  Encode representative words and assert the exact codes. When you can't reason
  out the expected code by hand, trace it through the header (or run the demo)
  and pin the actual output, then assert it, don't guess a textbook value.

## Traps

- **`static const` tables are per-TU.** Each table (`french_accents`,
  `german_composed`, ...) is `static const` at namespace scope, so every
  translation unit including the header gets its own copy. That is the original
  Xapiand shape; it is fine for these small constant maps, but don't be surprised
  by the duplication, and don't drop the `static` (it would cause ODR clashes
  across TUs).
- **Spanish uses `try`/`catch`.** `spanish_soundex.h` catches `std::out_of_range`
  around `str.at(1)` for one-character inputs. That is the only exception path in
  the library and it is pure std, keep it.

## Standalone vs. Xapiand

This is a standalone extraction from
[Xapiand](https://github.com/Kronuz/Xapiand). The encoders were copied as-is; the
only decoupling delta is the removal of the base's serialise/unserialise (and
their `length.h` + GPL `serialise-double.h` includes), plus wiring the one local
include `"strings.hh"` to the standalone
[strings](https://github.com/Kronuz/strings) library through CMake. Any change
here should stay reconcilable with upstream.
