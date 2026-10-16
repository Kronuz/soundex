# soundex API reference

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

## The encoders

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
