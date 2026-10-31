// Differential test: the optimized SoundexEnglish must produce exactly what the
// original in-place erase/insert implementation produced. frozen_encode (frozen.h) is
// a verbatim copy of that original. Inputs: every string of length <= 4 over a
// small alphabet (letters of each class, digits, space, high bytes, NUL), plus
// random strings and every dictionary-like random word.
//
// Usage: soundex_diff_test [random_strings]   (default 2,000,000)
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <string>
#include <string_view>

#include "english_soundex.h"
#include "frozen.h"

static size_t failures = 0;

static void check(const std::string& s) {
	std::string want = frozen_encode(s);
	std::string got = SoundexEnglish(s).encode();
	std::string got2 = SoundexEnglish().encode(s);
	if ((want != got || want != got2) && ++failures <= 10) {
		std::printf("MISMATCH on \"%s\": want \"%s\" got \"%s\"\n", s.c_str(), want.c_str(), got.c_str());
	}
}

int main(int argc, char** argv) {
	size_t n = argc > 1 ? std::strtoull(argv[1], nullptr, 10) : 2000000;
	size_t count = 0;

	const std::string alpha = std::string("aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ0123 -'") + '\x80' + '\xff' + '\x01';
	std::string s;
	for (int len = 0; len <= 4; ++len) {
		size_t total = 1;
		for (int i = 0; i < len; ++i) total *= alpha.size();
		for (size_t k = 0; k < total; ++k) {
			s.assign(len, ' ');
			size_t kk = k;
			for (int i = 0; i < len; ++i) { s[i] = alpha[kk % alpha.size()]; kk /= alpha.size(); }
			check(s);
			++count;
		}
	}

	std::mt19937_64 rng(777);
	const std::string small = "abcfgkmnprstvxyzAEHW1 -";
	for (size_t i = 0; i < n; ++i) {
		size_t len = rng() % 24;
		s.resize(len);
		for (auto& c : s) {
			c = (i & 1) ? small[rng() % small.size()] : static_cast<char>(rng());
		}
		check(s);
		++count;
	}
	std::string nul("ab\0cd", 5);
	check(nul);
	check(std::string("\0abc", 4));

	std::printf("checked %zu, failures %zu\n", count, failures);
	return failures ? 1 : 0;
}
