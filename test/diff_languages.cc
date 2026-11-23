// Differential test: the French, German and Spanish encoders must produce exactly
// what the frozen originals (frozen_languages.h, tables as maps and vectors) produce.
// Inputs per language: every string of up to 3 tokens from a token alphabet (letters
// of each class, the language's accents in UTF-8, digits, separators, stray bytes),
// plus random strings of up to 20 tokens.
//
// Usage: soundex_diff_languages_test [random_strings]   (default 300,000 per language)
#include <cstdio>
#include <cstdlib>
#include <random>
#include <string>
#include <vector>

#include "french_soundex.h"
#include "german_soundex.h"
#include "spanish_soundex.h"
#include "frozen_languages.h"

static size_t failures = 0;

template <typename New, typename Old>
static void check(const char* lang, const std::string& s) {
	std::string want = Old().encode(s);
	std::string got = New().encode(s);
	std::string got2 = New(s).encode();
	if ((want != got || want != got2) && ++failures <= 10) {
		std::printf("MISMATCH [%s] on \"%s\": want \"%s\" got \"%s\"\n", lang, s.c_str(), want.c_str(), got.c_str());
	}
}

template <typename New, typename Old>
static void run(const char* lang, const std::vector<std::string>& tokens, size_t n, std::mt19937_64& rng) {
	size_t count = 0;
	std::vector<size_t> idx;
	std::string s;
	for (size_t len = 0; len <= 3; ++len) {
		idx.assign(len, 0);
		for (;;) {
			s.clear();
			for (auto i : idx) s += tokens[i];
			check<New, Old>(lang, s);
			++count;
			size_t k = 0;
			while (k < len && ++idx[k] == tokens.size()) idx[k++] = 0;
			if (k == len) break;
		}
	}
	for (size_t i = 0; i < n; ++i) {
		s.clear();
		for (size_t len = rng() % 21; len--;) s += tokens[rng() % tokens.size()];
		check<New, Old>(lang, s);
		++count;
	}
	std::string nul("ab\0cd", 5);
	check<New, Old>(lang, nul);
	std::printf("%s: checked %zu\n", lang, count);
	if (count < 10000) ++failures;
}

int main(int argc, char** argv) {
	size_t n = argc > 1 ? std::strtoull(argv[1], nullptr, 10) : 300000;
	std::mt19937_64 rng(777);

	// Letters that start, end or join the rules of each language, both cases.
	std::vector<std::string> common = {" ", "-", "'", "1", "0", "\x80", "\xff", "\xc3", "h", "H"};
	auto add = [&](std::vector<std::string> t, const std::vector<std::string>& more) {
		t.insert(t.end(), more.begin(), more.end());
		return t;
	};
	auto french = add(common, {"a", "e", "i", "o", "u", "y", "A", "E", "G", "g", "c", "C", "k", "K", "q", "Q", "p", "P", "f", "s", "S", "n", "N", "m", "M", "r", "t", "x", "z", "w", "b", "d", "l",
		"Á", "À", "Ä", "Â", "Ã", "É", "È", "Ë", "Ê", "Œ", "Í", "Ì", "Ï", "Î", "Ó", "Ò", "Ö", "Ô", "Õ", "Ú", "Ù", "Ü", "Û", "Ñ", "Ç",
		"á", "à", "ä", "â", "ã", "é", "è", "ë", "ê", "œ", "í", "ì", "ï", "î", "ó", "ò", "ö", "ô", "õ", "ú", "ù", "ü", "û", "ñ", "ç", "ß"});
	auto german = add(common, {"a", "e", "i", "o", "u", "y", "A", "E", "c", "C", "k", "K", "q", "Q", "x", "X", "d", "D", "t", "T", "s", "S", "z", "Z", "p", "P", "f", "v", "w", "l", "L", "m", "n", "r", "R", "j", "b", "g",
		"Ä", "ä", "Ö", "ö", "Ü", "ü", "ß", "é", "Ñ"});
	auto spanish = add(common, {"a", "e", "i", "o", "u", "y", "Y", "A", "E", "c", "C", "q", "Q", "l", "L", "n", "N", "g", "G", "b", "v", "z", "s", "x", "r", "m", "d", "j", "k", "w",
		"Ñ", "Á", "É", "Í", "Ó", "Ú", "À", "È", "Ì", "Ò", "Ù", "Ü", "ñ", "á", "é", "í", "ó", "ú", "à", "è", "ì", "ò", "ù", "ü", "ç", "ä"});

	run<SoundexFrench, frozen::SoundexFrench>("French", french, n, rng);
	run<SoundexGerman, frozen::SoundexGerman>("German", german, n, rng);
	run<SoundexSpanish, frozen::SoundexSpanish>("Spanish", spanish, n, rng);

	std::printf("failures %zu\n", failures);
	return failures ? 1 : 0;
}
