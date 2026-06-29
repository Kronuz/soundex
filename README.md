# phonetic

A small, header-only **phonetic Soundex** library for C++20, extracted from
[Xapiand](https://github.com/Kronuz/Xapiand). It encodes a word into a
language-specific phonetic code so that words that sound alike collapse to the
same string.

## What it is

A CRTP base, `Soundex<Impl>`, plus four language encoders that subclass it:

- **`SoundexEnglish`** — refined Soundex
  ([reference](http://ntz-develop.blogspot.mx/2011/03/phonetic-algorithms.html)).
- **`SoundexFrench`** — Soundex 2
  ([reference](http://sqlpro.developpez.com/cours/soundex/)).
- **`SoundexGerman`** — Kölner Phonetik
  ([reference](https://de.wikipedia.org/wiki/Kölner_Phonetik)).
- **`SoundexSpanish`** —
  ([reference](https://wiki.postgresql.org/wiki/SoundexESP)).

These are the *refined* variants from Xapiand, not the textbook fixed-length
Soundex. The code is not truncated to four characters, and vowels are kept
(encoded, not dropped), so `SoundexEnglish("Robert")` is `R901096`, not the
classic `R163`. The homophone property still holds: `Robert` and `Rupert` share a
code, and in Spanish `Vaca` and `Baca` collapse (the leading `V` normalizes to
`B`).

The API is the same for every variant: construct one from a word and read the
code back with `encode()`, or call `encode(word)` on an instance to encode a
fresh word with the same rules.

## Install

This is header-only. There is nothing to compile; you put the headers on your
include path and `#include` the variants you need. It requires C++20.

Three of the four encoders (French, German, Spanish) include `"strings.hh"` for
`strings::inplace_upper` (their upper-casing step), so the library has one
dependency: the sibling [strings](https://github.com/Kronuz/strings) library.
CMake pulls it in for you. The English encoder and the base header need nothing
but standard headers.

With CMake `FetchContent`:

```cmake
include(FetchContent)
FetchContent_Declare(
  phonetic
  GIT_REPOSITORY https://github.com/Kronuz/phonetic.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(phonetic)

target_link_libraries(your_target PRIVATE phonetic::phonetic)
```

The `phonetic` target is a pure `INTERFACE` library: it compiles nothing,
requests `cxx_std_20`, puts the header directory on your include path, and links
`strings` so `"strings.hh"` resolves with no extra wiring. Then:

```cpp
#include "english_soundex.h"
#include "french_soundex.h"
#include "german_soundex.h"
#include "spanish_soundex.h"
```

The headers keep their original filenames, so a codebase that already
`#include`s them just needs this repo on its include path.

## Usage

```cpp
#include "english_soundex.h"
#include "spanish_soundex.h"

// Construct from a word, read the code back.
SoundexEnglish("Robert").encode();          // "R901096"
SoundexEnglish("Rupert").encode();          // "R901096"  (same: they rhyme)

// Reuse one encoder over many words with encode(word).
SoundexEnglish en;
en.encode("Ashcraft");                       // "A03039026"

// Spanish normalizes the first letter phonetically: V -> B.
SoundexSpanish("Vaca").encode();             // "B1020"
SoundexSpanish("Baca").encode();             // "B1020"  (collapse)

// Leading non-alphabetic characters are stripped; empty stays empty.
SoundexEnglish("123Robert").encode();        // "R901096"
SoundexEnglish("").encode();                 // ""
```

Each class also exposes `name()` and `description()` (e.g. `"SoundexEnglish"` /
`"Soundex for English Language"`).

## Build & test

```sh
cmake -B build && cmake --build build && ctest --test-dir build
```

The first configure fetches `strings` (and its own dependency chain) over the
network. The test encodes representative words in each of the four languages and
asserts the exact codes this implementation produces, including the homophone
collapse (`Robert`/`Rupert`, `Vaca`/`Baca`), leading-character stripping, and the
empty-input case. It prints `all phonetic tests passed` and exits 0.

## Examples

[`examples/demo.cc`](examples/demo.cc) is a runnable tour. A top-level CMake build
produces it next to the test:

```sh
cmake -B build && cmake --build build && ./build/phonetic_demo
```

It prints a handful of encodings per language.

## Provenance

Extracted from [Xapiand](https://github.com/Kronuz/Xapiand). The encoders were
copied as-is, with one decoupling delta: the base `Soundex` had `serialise()` /
`unserialise()` methods that persisted the encoder's configured code into a
Xapiand index, pulling in `"length.h"` (Xapiand's varint serializer) and
`"xapian/common/serialise-double.h"` (a GPL Xapian helper). Those methods and
both includes were removed; the standalone library is the pure phonetic
algorithm. The only remaining local include, `"strings.hh"`, is wired to the
standalone [strings](https://github.com/Kronuz/strings) library through CMake. See
[ARCHITECTURE.md](ARCHITECTURE.md) for the design and [AGENTS.md](AGENTS.md) for
the repo map and invariants.

## License

MIT, Copyright (c) 2015-2019 Dubalu LLC. See [LICENSE](LICENSE).
