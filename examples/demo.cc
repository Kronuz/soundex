// A runnable tour of the phonetic Soundex encoders.
//
// Build (when this repo is the top-level project):
//   cmake -B build && cmake --build build && ./build/phonetic_demo
//
// The one idea worth taking away: each SoundexXxx class turns a word into a
// language-specific phonetic code, so two words that sound alike collapse to the
// same string. Construct one from a word and read it back with encode(), or call
// encode(word) on an instance to encode another word with the same rules. The
// code length is not truncated, so it is not the classic fixed 4-char Soundex.
#include <cstdio>
#include <string>

#include "english_soundex.h"
#include "french_soundex.h"
#include "german_soundex.h"
#include "spanish_soundex.h"

static void rule(const char* title) {
	std::printf("\n\033[1m-- %s --\033[0m\n", title);
}

static void show_en(const char* word) {
	std::printf("  %-12s -> %s\n", word, SoundexEnglish(word).encode().c_str());
}
static void show_fr(const char* word) {
	std::printf("  %-12s -> %s\n", word, SoundexFrench(word).encode().c_str());
}
static void show_de(const char* word) {
	std::printf("  %-12s -> %s\n", word, SoundexGerman(word).encode().c_str());
}
static void show_es(const char* word) {
	std::printf("  %-12s -> %s\n", word, SoundexSpanish(word).encode().c_str());
}

int main() {
	std::puts("phonetic demo");

	rule("English (refined soundex): similar-sounding names collapse");
	show_en("Robert");
	show_en("Rupert");
	show_en("Rubin");
	show_en("Ashcraft");

	rule("French (Soundex 2)");
	show_fr("Bonjour");
	show_fr("Martin");
	show_fr("Phonetique");

	rule("German (Koelner Phonetik)");
	show_de("Wikipedia");
	show_de("Mueller");
	show_de("Breschnew");

	rule("Spanish");
	show_es("Vaca");
	show_es("Baca");
	show_es("Gonzalez");

	std::puts("\ndone.");
	return 0;
}
