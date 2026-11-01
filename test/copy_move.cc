// Copies and moves of the encoders. An encoder built from a word is a value: a copy or
// a move, from a non-const lvalue, a const lvalue, an rvalue or a const rvalue, holds
// the same code as the original, and keeps it after the original is overwritten or
// destroyed. The forwarding constructor that takes a word must not capture any of
// these (it would try to encode the encoder itself). The expected codes are the ones
// pinned in test/test.cc.
// Named checks: copy_<encoder>.

#include <cstdio>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "english_soundex.h"
#include "french_soundex.h"
#include "german_soundex.h"
#include "spanish_soundex.h"

static int failures = 0;
static size_t checks = 0;

static void expect(const std::string& got, const std::string& want, const char* what, const char* how) {
	++checks;
	if (got != want) {
		if (++failures < 30) std::printf("FAIL %s (%s): got '%s' want '%s'\n", what, how, got.c_str(), want.c_str());
	}
}

// A class derived from an encoder is copied through the same constructors.
template <typename Encoder>
struct Derived : Encoder {
	using Encoder::Encoder;
};

template <typename Encoder>
static void check_copies(const char* what, const std::string& word, const std::string& want, const std::string& other,
                         const std::string& other_want) {
	static_assert(std::is_copy_constructible_v<Encoder> && std::is_move_constructible_v<Encoder>);
	static_assert(std::is_copy_assignable_v<Encoder> && std::is_move_assignable_v<Encoder>);

	// Construction from the word still works for every kind of string argument.
	expect(Encoder(word).encode(), want, what, "from a std::string lvalue");
	expect(Encoder(word.c_str()).encode(), want, what, "from a C string");
	expect(Encoder(std::string_view(word)).encode(), want, what, "from a string_view");
	expect(Encoder(std::string(word)).encode(), want, what, "from a std::string rvalue");

	Encoder original(word);
	expect(original.encode(), want, what, "original");

	{
		Encoder copy = original;  // non-const lvalue
		expect(copy.encode(), want, what, "copy of a non-const lvalue");
	}
	{
		Encoder copy(original);  // non-const lvalue, direct initialization
		expect(copy.encode(), want, what, "direct copy of a non-const lvalue");
	}
	{
		const Encoder& ref = original;
		Encoder copy = ref;
		expect(copy.encode(), want, what, "copy of a const lvalue");
	}
	{
		const Encoder constant(word);
		Encoder copy = std::move(constant);  // const rvalue
		expect(copy.encode(), want, what, "copy of a const rvalue");
	}
	{
		Encoder source(word);
		Encoder moved = std::move(source);  // non-const rvalue
		expect(moved.encode(), want, what, "move");
	}
	{
		Encoder source(word);
		Encoder copy = source;
		source = Encoder(other);  // the copy is independent of the original
		expect(copy.encode(), want, what, "copy after the original is overwritten");
		expect(source.encode(), other_want, what, "the overwritten original");
	}
	{
		Encoder target(other);
		target = original;  // copy assignment from a non-const lvalue
		expect(target.encode(), want, what, "copy assignment");
		Encoder target2(other);
		target2 = Encoder(word);
		expect(target2.encode(), want, what, "move assignment");
	}
	{
		auto destroyed = std::make_unique<Encoder>(word);
		Encoder copy = *destroyed;  // non-const lvalue of a heap object
		destroyed.reset();
		expect(copy.encode(), want, what, "copy after the original is destroyed");
	}

	// The library paths that construct in place from a lvalue go through the forwarding
	// constructor.
	{
		std::vector<Encoder> v;
		for (int i = 0; i < 20; ++i) {
			v.emplace_back(original);  // non-const lvalue, with regrowth
		}
		for (auto& e : v) expect(e.encode(), want, what, "emplace_back of a non-const lvalue");
		std::vector<Encoder> w = v;
		for (auto& e : w) expect(e.encode(), want, what, "vector copy");
	}
	{
		auto p = std::make_unique<Encoder>(original);
		expect(p->encode(), want, what, "make_unique of a non-const lvalue");
		std::optional<Encoder> o(original);
		expect(o->encode(), want, what, "optional of a non-const lvalue");
		std::pair<Encoder, int> pr(original, 1);
		expect(pr.first.encode(), want, what, "pair of a non-const lvalue");
	}

	// A derived class copies and moves too.
	{
		Derived<Encoder> d(word);
		expect(d.encode(), want, what, "derived");
		Derived<Encoder> dc = d;
		expect(dc.encode(), want, what, "derived copy of a non-const lvalue");
		Encoder base(d);  // slices a non-const derived lvalue into the base
		expect(base.encode(), want, what, "base from a non-const derived lvalue");
		const Derived<Encoder> cd(word);
		Encoder base2(cd);
		expect(base2.encode(), want, what, "base from a const derived lvalue");
		Derived<Encoder> dm = std::move(d);
		expect(dm.encode(), want, what, "derived move");
	}

	// The default-constructed encoder is empty and still encodes a fresh word.
	{
		Encoder empty;
		Encoder copy = empty;  // non-const lvalue of an empty encoder
		expect(copy.encode(), "", what, "copy of a default-constructed encoder");
		expect(copy.encode(word), want, what, "encode(word) on a copy");
	}
}

int main() {
	check_copies<SoundexEnglish>("copy_english", "Robert", "R901096", "Rubin", "R90108");
	check_copies<SoundexFrench>("copy_french", "Bonjour", "BANJAR", "Chien", "CHAN");
	check_copies<SoundexGerman>("copy_german", "Wikipedia", "30401020", "Schmidt", "8602");
	check_copies<SoundexSpanish>("copy_spanish", "Gonzalez", "G20520402", "Vaca", "B1020");

	// The CRTP base copies too, from a non-const lvalue.
	{
		Soundex<SoundexEnglish> base;
		Soundex<SoundexEnglish> copy = base;
		expect(copy.encode(), "", "copy_base", "copy of a non-const lvalue base");
		SoundexEnglish robert("Robert");
		Soundex<SoundexEnglish>& as_base = robert;
		Soundex<SoundexEnglish> sliced = as_base;
		expect(sliced.encode(), "R901096", "copy_base", "copy of a non-const lvalue base holding a code");
	}

	std::printf("%zu checks, %d failures\n", checks, failures);
	if (failures) {
		return 1;
	}
	std::printf("all soundex copy tests passed\n");
	return 0;
}
