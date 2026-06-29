// Smoke test for the standalone phonetic Soundex encoders.
//
// Exercises the four language variants (English, French, German, Spanish) built
// on the CRTP base in soundex.h. These are the "refined" Soundex encoders from
// Xapiand: the code is not truncated to the classic 4 characters and vowels are
// kept (as '0'), so the expected codes below are what THIS implementation
// produces, traced from the headers and pinned here, not the textbook R163-style
// values. The headstone property still holds: words that sound alike collapse to
// the same code (Robert == Rupert, Vaca == Baca).
//
// Build via CMake: cmake -B build && cmake --build build && ctest --test-dir build
#include <cassert>
#include <cstdio>
#include <string>

#include "english_soundex.h"
#include "french_soundex.h"
#include "german_soundex.h"
#include "spanish_soundex.h"


// A tiny assert-with-message helper so a failure prints the word and the codes.
static void check(const char* lang, const char* word,
                  const std::string& got, const std::string& want) {
	if (got != want) {
		std::printf("FAIL [%s] %-12s got '%s' want '%s'\n",
		            lang, word, got.c_str(), want.c_str());
		assert(false && "soundex code mismatch");
	}
	std::printf("  ok [%s] %-12s -> %s\n", lang, word, got.c_str());
}


// ---------------------------------------------------------------------------
// English: refined soundex. Vowels survive as '0', codes are not truncated, so
// Robert encodes to R901096 (not the classic R163). The key invariant is the
// homophone collapse: Robert and Rupert sound alike and share a code.
// ---------------------------------------------------------------------------

static void test_english() {
	check("EN", "Robert",   SoundexEnglish("Robert").encode(),   "R901096");
	check("EN", "Rupert",   SoundexEnglish("Rupert").encode(),   "R901096");
	check("EN", "Rubin",    SoundexEnglish("Rubin").encode(),    "R90108");
	check("EN", "Ashcraft", SoundexEnglish("Ashcraft").encode(), "A03039026");

	// Robert and Rupert collapse to the same code.
	assert(SoundexEnglish("Robert").encode() == SoundexEnglish("Rupert").encode());

	// Leading non-alphabetic characters are stripped before encoding.
	check("EN", "123Robert", SoundexEnglish("123Robert").encode(), "R901096");

	// Empty input stays empty.
	check("EN", "(empty)", SoundexEnglish("").encode(), "");

	// encode(word) on an instance encodes a fresh word with the same rules; it
	// does not read back the stored code. A default-constructed encoder works.
	SoundexEnglish en;
	check("EN", "encode(Robert)", en.encode(std::string("Robert")), "R901096");

	// name()/description() come through the CRTP base unchanged.
	assert(SoundexEnglish().name() == "SoundexEnglish");
	assert(SoundexEnglish().description() == "Soundex for English Language");

	std::printf("english OK\n");
}


// ---------------------------------------------------------------------------
// French: Soundex 2. Vowels (after the first) fold to 'A', so Bonjour -> BANJAR.
// 'H' is kept only after 'C' or 'S', so Chien keeps its H -> CHAN.
// ---------------------------------------------------------------------------

static void test_french() {
	check("FR", "Bonjour",    SoundexFrench("Bonjour").encode(),    "BANJAR");
	check("FR", "Martin",     SoundexFrench("Martin").encode(),     "MARTAN");
	check("FR", "Phonetique", SoundexFrench("Phonetique").encode(), "FANATAKA");
	check("FR", "Chien",      SoundexFrench("Chien").encode(),      "CHAN");

	assert(SoundexFrench().name() == "SoundexFrench");
	std::printf("french OK\n");
}


// ---------------------------------------------------------------------------
// German: Koelner Phonetik. Output is a pure digit string. Accents fold (the
// umlaut/eszett map) before encoding.
// ---------------------------------------------------------------------------

static void test_german() {
	check("DE", "Wikipedia", SoundexGerman("Wikipedia").encode(), "30401020");
	check("DE", "Mueller",   SoundexGerman("Mueller").encode(),   "60507");
	check("DE", "Schmidt",   SoundexGerman("Schmidt").encode(),   "8602");

	assert(SoundexGerman().name() == "SoundexGerman");
	std::printf("german OK\n");
}


// ---------------------------------------------------------------------------
// Spanish: the first letter is normalized to its phonetic equivalent, so 'V'
// becomes 'B'. Vaca and Baca therefore collapse to the same code. 'X' at the
// start becomes 'S', so Xavier and (J-initial) Javier diverge as the rules say.
// ---------------------------------------------------------------------------

static void test_spanish() {
	check("ES", "Vaca",     SoundexSpanish("Vaca").encode(),     "B1020");
	check("ES", "Baca",     SoundexSpanish("Baca").encode(),     "B1020");
	check("ES", "Gonzalez", SoundexSpanish("Gonzalez").encode(), "G20520402");
	check("ES", "Xavier",   SoundexSpanish("Xavier").encode(),   "S20106");

	// Vaca and Baca collapse (V normalized to B).
	assert(SoundexSpanish("Vaca").encode() == SoundexSpanish("Baca").encode());

	assert(SoundexSpanish().name() == "SoundexSpanish");
	std::printf("spanish OK\n");
}


int main() {
	test_english();
	test_french();
	test_german();
	test_spanish();
	std::printf("all phonetic tests passed\n");
	return 0;
}
