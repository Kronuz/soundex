// Benchmark: SoundexEnglish::encode against the frozen original (test/frozen.h)
// on pseudo-words of 3 to 12 letters.
//
// Build: cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build --target soundex_bench
// Run:   ./build/soundex_bench [words]
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <string>
#include <vector>

#include "../test/frozen.h"
#include "english_soundex.h"

int main(int argc, char** argv) {
	size_t n = argc > 1 ? std::strtoull(argv[1], nullptr, 10) : 50000;
	std::mt19937_64 rng(7);
	std::vector<std::string> words(n);
	for (auto& w : words) {
		w.resize(3 + rng() % 10);
		for (auto& c : w) c = static_cast<char>('a' + rng() % 26);
	}
	volatile size_t sink = 0;
	auto time = [&](auto&& fn) {
		double best = 1e300;
		for (int rep = 0; rep < 9; ++rep) {
			auto t0 = std::chrono::steady_clock::now();
			size_t s = 0;
			for (auto& w : words) s += fn(w).size();
			auto t1 = std::chrono::steady_clock::now();
			sink = sink + s;
			best = std::min(best, std::chrono::duration<double, std::nano>(t1 - t0).count() / n);
		}
		return best;
	};
	std::printf("SoundexEnglish::encode  frozen %6.1f ns   new %6.1f ns\n",
		time([](const std::string& w) { return frozen_encode(w); }),
		time([](const std::string& w) { return SoundexEnglish().encode(w); }));
	return 0;
}
