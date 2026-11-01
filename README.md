# soundex

Part of [Kronuz C++ Libraries](https://github.com/Kronuz/cpp-libraries), a collection of small, reusable C++ libraries.

[![CI](https://github.com/Kronuz/soundex/actions/workflows/ci.yml/badge.svg)](https://github.com/Kronuz/soundex/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)
![Header-only](https://img.shields.io/badge/header--only-yes-green.svg)

Soundex phonetic codes for C++20 in English, French, German and Spanish: words that sound alike
share one code. Each language has its own encoder over one CRTP base, with rules for its own
spelling, so "Robert" and "Rupert" both encode to `R901096`. Reach for it for name search, fuzzy
matching and deduplicating spellings.

## ✨ Highlights

- **Four languages.** Refined Soundex (English), Soundex 2 (French), Kölner Phonetik (German),
  and a Spanish Soundex.
- **Refined codes.** Codes keep vowels and full length, so they separate more words than classic
  four-character Soundex.
- **One interface.** Construct an encoder from a word and call `encode()`, or reuse one encoder
  with `encode(word)`.
- **Stable codes.** Codes are stored in indexes, so an encoding never changes; `test/diff_test.cc`
  checks the English encoder against a frozen reference over every string of up to four
  characters of a mixed alphabet, plus random strings.

## 🚀 Quick start

```cpp
#include "english_soundex.h"
#include "spanish_soundex.h"

#include <cstdio>
#include <string>

int main() {
	std::string robert = SoundexEnglish("Robert").encode();
	std::string rupert = SoundexEnglish("Rupert").encode();
	std::printf("%s %s\n", robert.c_str(), rupert.c_str());  // R901096 R901096

	SoundexSpanish es;
	std::printf("%s %s\n", es.encode("Vaca").c_str(), es.encode("Baca").c_str());  // B1020 B1020
}
```

[`examples/demo.cc`](examples/demo.cc) encodes word pairs in each language.

## 📦 Installation

Requires a C++20 compiler and CMake 3.14 or newer. CMake fetches
[strings](https://github.com/Kronuz/strings) at a pinned commit, which the French, German and
Spanish encoders use for upper-casing; the English encoder needs only the standard library. CI
builds and tests it on Linux (x86_64 with GCC and Clang, arm64), macOS (arm64, x86_64) and
FreeBSD, plus ASan + UBSan and TSan builds.

```cmake
include(FetchContent)
FetchContent_Declare(soundex
  GIT_REPOSITORY https://github.com/Kronuz/soundex.git
  GIT_TAG        407e7d199ab6edc9843196445b61a8f05614a49f)  # or main to track the tip
FetchContent_MakeAvailable(soundex)

target_link_libraries(my_app PRIVATE soundex::soundex)
```

```cpp
#include "english_soundex.h"  // SoundexEnglish
#include "french_soundex.h"   // SoundexFrench
#include "german_soundex.h"   // SoundexGerman
#include "spanish_soundex.h"  // SoundexSpanish
```

## 🎯 When to use it

Matching names and words by sound within one language, and phonetic scoring with
[string-similarity](https://github.com/Kronuz/string-similarity)'s `PhoneticMetric`.

Not for:

- Compatibility with classic Soundex codes (`R163`): these are the refined variants.
- English with more nuance: [double-metaphone](https://github.com/Kronuz/double-metaphone) handles
  more spelling rules and alternate pronunciations.
- Comparing codes across languages: each encoder maps letters to codes by its own rules.

## 📚 Documentation

- [docs/API.md](docs/API.md): the encoders, their references, and usage.
- [ARCHITECTURE.md](ARCHITECTURE.md): the CRTP base and each language's rules.
- [CHANGELOG.md](CHANGELOG.md): dated changes.
- [AGENTS.md](AGENTS.md): repository map and traps.
- [Catalog entry](https://github.com/Kronuz/cpp-libraries/blob/main/AGENTS.md#soundex) in Kronuz
  C++ Libraries.

## 🛠️ Building and testing

```sh
cmake -B build && cmake --build build && ctest --test-dir build
./build/soundex_demo
./build/soundex_bench
```

CTest runs the encoders' vectors, the differential test against `test/frozen.h`, and
`test/copy_move.cc`, which copies and moves every encoder, from non-const lvalues included.
`soundex_bench` times `SoundexEnglish` against the frozen reference.

## 🤝 Contributing

Issues and pull requests are welcome. Read [AGENTS.md](AGENTS.md) and the catalog's
[rules of engagement](https://github.com/Kronuz/cpp-libraries/blob/main/AGENTS.md#rules-of-engagement)
first; codes are stored in indexes, so a change must not alter any encoding.

## 📄 License

MIT. See [LICENSE](LICENSE). Extracted from [Xapiand](https://github.com/Kronuz/Xapiand)
(Copyright © 2015-2020 Dubalu LLC).
