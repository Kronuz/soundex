# Architecture

The internal design of `soundex`: a CRTP base plus four per-language Soundex
encoders, all inline in headers. For usage see `README.md`; for the repo map and
invariants see `AGENTS.md`.

## Shape

Five headers, header-only:

```
  soundex.h           CRTP base Soundex<Impl> + the replace/replace_prefix helpers
  english_soundex.h   SoundexEnglish  (refined Soundex)
  french_soundex.h    SoundexFrench   (Soundex 2)
  german_soundex.h    SoundexGerman   (Kölner Phonetik)
  spanish_soundex.h   SoundexSpanish
```

The four language headers each `#include "soundex.h"`; a consumer includes
whichever variants it needs.

## The CRTP base

`Soundex<Impl>` in `soundex.h` is a curiously-recurring-template base. It holds
one member, `_code_str` (the encoded code), and forwards the public surface to
the derived class via `static_cast<const Impl*>(this)`:

- `encode(word)` calls the derived `_encode(word)` and returns the code for a
  fresh word.
- `encode()` returns the stored `_code_str` (set at construction).
- `name()` / `description()` call the derived `_name()` / `_description()`.

Each derived class declares `friend class Soundex<Impl>` so the base can reach its
private `_encode` / `_name` / `_description`, provides a default constructor and a
templated constructor that runs `_encode` on the input and seeds the base's
`_code_str`. So `SoundexEnglish("Robert").encode()` and
`SoundexEnglish().encode("Robert")` reach the same algorithm by two paths: the
former through the constructor (and reads the stored result back), the latter
straight through the base's `encode(word)`.

`soundex.h` also carries three free helpers the encoders share: two `replace`
overloads (a container of `{from, to}` pairs, or an iterator range) that do
in-place find/replace from a start position, and `replace_prefix`, which swaps a
matching prefix using `std::mismatch`. These are how the language tables
(accent folds, composed-letter maps, prefix rules) get applied.

## The four encoders

Each `_encode` is a straight-line transform over a `std::string`, returning the
code. They share a rough skeleton but differ in the tables and the digit map:

- **English** is the simplest: strip leading non-letters, keep the first letter
  upper-cased and literal, then walk the rest mapping each letter class to a
  digit (`b/p -> 1`, ... `r -> 9`, vowels/`h/w/y -> 0`), collapsing runs of the
  same digit. It is the *refined* map, so it has more buckets than classic
  Soundex and keeps vowels as `0`, which is why the codes are long.

- **French** (Soundex 2) folds accents from `french_accents`, upper-cases via
  `strings::inplace_upper`, keeps only `A-Z`, applies the `french_composed`
  consonant map, folds non-initial vowels to `A`, runs prefix and complementary
  substitutions from `french_prefixes`, drops `H`/`Y` except in specific
  contexts, trims a trailing `A/T/D/S`, and squeezes repeated letters. The output
  stays alphabetic (e.g. `Bonjour -> BANJAR`).

- **German** (Kölner Phonetik) folds umlauts/eszett, upper-cases, handles the
  leading-`C` rule, applies `german_composed`, then maps to digits per the Kölner
  table. The output is a pure digit string.

- **Spanish** folds accents, upper-cases, strips leading non-letters and `H`,
  normalizes the *first* letter to its phonetic equivalent (`V -> B`, `Z/X -> S`,
  context-sensitive `G`/`C`), applies `spanish_composed`, duplicates the first
  letter, then maps to digits. `V -> B` at the front is what makes `Vaca` and
  `Baca` collapse. This is the one encoder that uses a `try`/`catch` (around
  `str.at(1)`), catching `std::out_of_range` for one-character inputs.

The per-language tables (`french_accents`, `german_composed`, `spanish_composed`,
...) are `static const` namespace-scope maps/vectors defined in each header. They
are `static`, so each translation unit that includes the header gets its own copy;
that is the original Xapiand shape, preserved here.

## What was removed in extraction

The base `Soundex` in Xapiand had `virtual serialise()` / `unserialise()` methods
that persisted `_code_str` into a Xapiand index, using `serialise_string` /
`unserialise_string` from `"length.h"` (Xapiand's varint serializer) and pulling
in `"xapian/common/serialise-double.h"` (a GPL Xapian helper). None of that is
part of the phonetic algorithm: it is how Xapiand stored a configured encoder.
The two methods and both includes were dropped, so the base is now non-virtual
and depends only on `<string>`, `<string_view>`, `<utility>`, and `<algorithm>`
(for `std::mismatch`). Xapiand keeps its own serialise adapter when it eventually
de-vendors this; that is not this library's concern.
