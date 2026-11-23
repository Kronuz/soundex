// The French, German and Spanish encoders as they were before their tables became
// constant arrays (soundex:2bddab7), in namespace frozen: the oracle for
// test/diff_languages.cc. Keep it unchanged.
#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "soundex.h"
#include "strings.hh"

namespace frozen {

static const std::unordered_map<std::string, std::string> french_accents({
	{ "Á", "A" }, { "À", "A" }, { "Ä", "A" }, { "Â", "A" }, { "Ã", "A" },
	{ "É", "E" }, { "È", "E" }, { "Ë", "E" }, { "Ê", "E" }, { "Œ", "E" },
	{ "Í", "I" }, { "Ì", "I" }, { "Ï", "I" }, { "Î", "I" },
	{ "Ó", "O" }, { "Ò", "O" }, { "Ö", "O" }, { "Ô", "O" }, { "Õ", "O" },
	{ "Ú", "U" }, { "Ù", "U" }, { "Ü", "U" }, { "Û", "U" },
	{ "á", "A" }, { "à", "A" }, { "ä", "A" }, { "â", "A" }, { "ã", "A" },
	{ "é", "E" }, { "è", "E" }, { "ë", "E" }, { "ê", "E" }, { "œ", "E" },
	{ "í", "I" }, { "ì", "I" }, { "ï", "I" }, { "î", "I" },
	{ "ó", "O" }, { "ò", "O" }, { "ö", "O" }, { "ô", "O" }, { "õ", "O" },
	{ "ú", "U" }, { "ù", "U" }, { "ü", "U" }, { "û", "U" },
	{ "Ñ", "N" }, { "Ç", "S" }, { "ñ", "N" }, { "ç", "S" }
});


static const std::vector<std::pair<std::string, std::string>> french_composed({
	{ "GUI", "KI" }, { "GUE", "KE" }, { "GA",  "KA" },
	{ "GO",  "KO" }, { "GU",  "K"  }, { "CA",  "KA" },
	{ "CO",  "KO" }, { "CU",  "KU" }, { "Q",   "K"  },
	{ "CC",  "K"  }, { "CK",  "K"  }
 });


static const std::vector<std::pair<std::string, std::string>> french_prefixes({
	{ "KN",  "NN"  }, { "PF",  "FF"  }, { "PH",  "FF"  },
	{ "ASA", "AZA" }, { "SCH", "SSS" }, { "MAC", "MCC" }
});


/*
 * Soundex for French language based in Soundex 2:
 *  http://sqlpro.developpez.com/cours/soundex/
 *
 * Length of the result is not truncated, so the code does not have a fixed length.
 */
class SoundexFrench : public Soundex<SoundexFrench> {

	friend class Soundex<SoundexFrench>;

	std::string _encode(std::string_view view) const {
		std::string str(view);
		if (str.empty()) {
			return str;
		}

		// 1. Replace accents.
		replace(str, 0, french_accents);

		// 2. Pass to upper case.
		strings::inplace_upper(str);

		// 3. Keep only alphabet characters.
		for (auto it = str.begin(); it != str.end(); ) {
			if (*it >= 'A' && *it <= 'Z') {
				++it;
			} else {
				it = str.erase(it);
			}
		}

		if (str.empty()) {
			return str;
		}

		// 4. Replace primary consonants.
		replace(str, 0, french_composed);

		// 5. Replace vowels except the fisrt.
		const auto it_e = str.end();
		for (auto it = ++str.begin(); it != it_e; ++it) {
			switch (*it) {
				case 'E':
				case 'I':
				case 'O':
				case 'U':
					*it = 'A';
					break;
				default:
					break;
			}
		}

		// 6. Replace prefix.
		replace_prefix(str, french_prefixes);

		// 7. Replace complementary substitutions.
		replace(str, 1, french_prefixes.begin(), --french_prefixes.end());

		// 8. Remove 'H' unless it is prededed by a 'C' or an 'S' and 'Y' unless it is preceded by 'A'.
		for (auto it = str.begin(); it != str.end(); ) {
			switch (*it) {
				case 'H':
					if (it == str.begin() || (*(it - 1) != 'C' && *(it - 1) != 'S')) {
						it = str.erase(it);
					} else {
						++it;
					}
					break;
				case 'Y':
					if (it == str.begin() || *(it - 1) != 'A') {
						it = str.erase(it);
					} else {
						++it;
					}
					break;
				default:
					++it;
					break;
			}
		}

		if (str.empty()) {
			return str;
		}

		// 9. Remove the terminations 'A', 'T', 'D', 'S'.
		if (str.length() > 1) {
			switch (str.back()) {
				case 'A':
				case 'T':
				case 'D':
				case 'S':
					str.pop_back();
					break;
				default:
					break;
			}
		}

		if (str.empty()) {
			return str;
		}

		// 10. Remove all 'A' except if it is at the beginning of the string. (We don't do this step).
		// str.erase(std::remove(++str.begin(), str.end(), 'A'), str.end());

		// 11. Remove repetitive letters.
		for (auto it = ++str.begin(); it != str.end(); ) {
			if (*it == *(it - 1)) {
				it = str.erase(it);
			} else {
				++it;
			}
		}

		return str;
	}

	std::string_view _name() const noexcept {
		return "SoundexFrench";
	}

	std::string _description() const noexcept {
		return "Soundex for French Language";
	}

public:
	SoundexFrench() = default;

	template <typename T>
		requires (!soundex_detail::is_encoder_v<T, SoundexFrench>)
	SoundexFrench(T&& str)
		: Soundex<SoundexFrench>(_encode(std::string(std::forward<T>(str)))) { }
};

static const std::unordered_map<std::string, std::string> german_accents({
	{ "Ä",  "A" }, { "ä",  "A" }, { "Ö" , "O" }, { "ö",  "O" },
	{ "Ü",  "U" }, { "ü",  "U" }, { "ß",  "S" }
});


// Applied in order: Kölner Phonetik codes C after S or Z, and D or T before C, S, or Z, as
// 8 before C before A, H, K, O, Q, U, or X as 4 ("Schmidt" is 8602).
static const std::vector<std::pair<std::string, std::string>> german_composed({
	{ "DC", "8" }, { "DS", "8" }, { "DZ", "8" }, { "TC", "8" },
	{ "TS", "8" }, { "TZ", "8" }, { "SC", "8" }, { "ZC", "8" },
	{ "KX", "8" }, { "QX", "8" }, { "PH", "3" }, { "CA", "4" },
	{ "CH", "4" }, { "CK", "4" }, { "CO", "4" }, { "CQ", "4" },
	{ "CU", "4" }, { "CX", "4" }
});


/*
 * Soundex for German based in Kölner Phonetik:
 *  https://de.wikipedia.org/wiki/Kölner_Phonetik
 *
 * Length of the result is not truncated, so the code does not have a fixed length.
 */
class SoundexGerman : public Soundex<SoundexGerman> {

	friend class Soundex<SoundexGerman>;

	std::string _encode(std::string_view view) const {
		std::string str(view);
		if (str.empty()) {
			return str;
		}

		// 1. Replace accents.
		replace(str, 0, german_accents);

		// 2. Pass to upper case.
		strings::inplace_upper(str);

		// 3. Remove all non alphabetic characters at the begin.
		auto it = str.begin();
		while (it != str.end()) {
			if (*it >= 'A' && *it <= 'Z') {
				break;
			} else {
				it = str.erase(it);
			}
		}

		if (str.empty()) {
			return str;
		}

		// 4. Replace prefix.
		if (str.length() > 1 && *it == 'C') {
			switch (*(it + 1)) {
				case 'A':
				case 'H':
				case 'K':
				case 'L':
				case 'O':
				case 'Q':
				case 'R':
				case 'U':
				case 'X':
					it = str.erase(it);
					*it = '4';
					break;
			}
		}


		// 5. Replace "composed letters".
		replace(str, 0, german_composed);

		// 6. Starts the calculation of Soundex.
		it = str.begin();
		while (it != str.end()) {
			switch (*it) {
				case 'A':
				case 'E':
				case 'I':
				case 'J':
				case 'O':
				case 'U':
				case 'Y':
					if (it == str.begin() || *(it - 1) != '0') {
						*it++ = '0';
					} else {
						it = str.erase(it);
					}
					break;
				case 'B':
				case 'P':
					if (it == str.begin() || *(it - 1) != '1') {
						*it++ = '1';
					} else {
						it = str.erase(it);
					}
					break;
				case 'D':
				case 'T':
					if (it == str.begin() || *(it - 1) != '2') {
						*it++ = '2';
					} else {
						it = str.erase(it);
					}
					break;
				case 'F':
				case 'V':
				case 'W':
					if (it == str.begin() || *(it - 1) != '3') {
						*it++ = '3';
					} else {
						it = str.erase(it);
					}
					break;
				case 'G':
				case 'K':
				case 'Q':
					if (it == str.begin() || *(it - 1) != '4') {
						*it++ = '4';
					} else {
						it = str.erase(it);
					}
					break;
				case 'L':
					if (it == str.begin() || *(it - 1) != '5') {
						*it++ = '5';
					} else {
						it = str.erase(it);
					}
					break;
				case 'M':
				case 'N':
					if (it == str.begin() || *(it - 1) != '6') {
						*it++ = '6';
					} else {
						it = str.erase(it);
					}
					break;
				case 'R':
					if (it == str.begin() || *(it - 1) != '7') {
						*it++ = '7';
					} else {
						it = str.erase(it);
					}
					break;
				case 'C':
				case 'S':
				case 'Z':
					if (it == str.begin() || *(it - 1) != '8') {
						*it++ = '8';
					} else {
						it = str.erase(it);
					}
					break;
				case 'X':
					if (it == str.begin() || *(it - 1) != '4') {
						*it++ = '4';
						it = ++str.insert(it, '8');
					} else {
						*it++ = '8';
					}
					break;
				case '3':
				case '4':
				case '8':
					++it;
					break;
				default:
					it = str.erase(it);
					break;
			}
		}

		return str;
	}

	std::string_view _name() const noexcept {
		return "SoundexGerman";
	}

	std::string _description() const noexcept {
		return "Soundex for German Language";
	}

public:
	SoundexGerman() = default;

	template <typename T>
		requires (!soundex_detail::is_encoder_v<T, SoundexGerman>)
	SoundexGerman(T&& str)
		: Soundex<SoundexGerman>(_encode(std::string(std::forward<T>(str)))) { }
};

static const std::unordered_map<std::string, std::string> spanish_accents({
	{ "Ñ",  "N" }, { "Á",  "A" }, { "É" , "E" }, { "Í",  "I" },
	{ "Ó",  "O" }, { "Ú",  "U" }, { "À",  "A" }, { "È",  "E" },
	{ "Ì",  "I" }, { "Ò",  "O" }, { "Ù",  "U" }, { "Ü",  "U" },
	{ "ñ",  "N" }, { "á",  "A" }, { "é" , "E" }, { "í",  "I" },
	{ "ó",  "O" }, { "ú",  "U" }, { "à",  "A" }, { "è",  "E" },
	{ "ì",  "I" }, { "ò",  "O" }, { "ù",  "U" }, { "ü",  "U" }
});


// Applied in this order, so overlapping rules ("NY" and "YA") code the same on every platform.
static const std::vector<std::pair<std::string, std::string>> spanish_composed({
	{ "CH", "V" }, { "QU", "K" }, { "LL", "J" }, { "CE", "S" },
	{ "CI", "S" }, { "YA", "J" }, { "YE", "J" }, { "YI", "J" },
	{ "YO", "J" }, { "YU", "J" }, { "GE", "J" }, { "GI", "J" },
	{ "NY", "N" }
});


/*
 * Soundex for Spanish based in:
 *  https://wiki.postgresql.org/wiki/SoundexESP.
 *  http://oraclenotepad.blogspot.mx/2008/03/soundex-en-espaol.html
 *
 * Length of the result is not truncated, so the code does not have a fixed length.
 */
class SoundexSpanish : public Soundex<SoundexSpanish> {

	friend class Soundex<SoundexSpanish>;

	std::string _encode(std::string_view view) const {
		std::string str(view);
		if (str.empty()) {
			return str;
		}

		// 1.  Replace accents.
		replace(str, 0, spanish_accents);

		// 2. Pass to upper case.
		strings::inplace_upper(str);

		// 3. Remove all non alphabetic characters and 'H's at the begin.
		auto it = str.begin();
		for (it = str.begin(); it != str.end(); ) {
			if (*it >= 'A' && *it <= 'Z' && *it != 'H') {
				break;
			} else {
				it = str.erase(it);
			}
		}

		if (str.empty()) {
			return str;
		}

		/*
		 * 4. First letter is important, We must associate similar.
		 * e.g. 'vaca' becomes 'baca' and 'zapote' becomes 'sapote'
		 * An important phenomenon is 'GE' and 'GI' become 'JE' and 'JI';
		 * 'CA' becomes 'KA', etc.
		 */
		switch (*it) {
			case 'V':
				*it = 'B';
				break;
			case 'Z':
			case 'X':
				*it = 'S';
				break;
			case 'G':
				try {
					auto c = str.at(1);
					if (c == 'E' || c == 'I') {
						*it = 'J';
					}
				} catch (const std::out_of_range&) { }
				break;
			case 'C':
				try {
					auto c = str.at(1);
					if (c != 'H' || c != 'E' || c != 'I') {
						*it = 'K';
					}
				} catch (const std::out_of_range&) { }
				break;
			default:
				break;
		}

		// 5. Replace "composed letters".
		replace(str, 0, spanish_composed);

		// 6. Starts the calculation of Soundex.
		it = str.begin();
		it = ++str.insert(it, *it);
		while (it != str.end()) {
			switch (*it) {
				case 'B':
				case 'P':
				case 'F':
				case 'V':
					if (*(it - 1) != '1') {
						*it++ = '1';
					} else {
						it = str.erase(it);
					}
					break;
				case 'C':
				case 'G':
				case 'K':
				case 'S':
				case 'X':
				case 'Z':
					if (*(it - 1) != '2') {
						*it++ = '2';
					} else {
						it = str.erase(it);
					}
					break;
				case 'D':
				case 'T':
					if (*(it - 1) != '3') {
						*it++ = '3';
					} else {
						it = str.erase(it);
					}
					break;
				case 'L':
					if (*(it - 1) != '4') {
						*it++ = '4';
					} else {
						it = str.erase(it);
					}
					break;
				case 'M':
				case 'N':
					if (*(it - 1) != '5') {
						*it++ = '5';
					} else {
						it = str.erase(it);
					}
					break;
				case 'R':
					if (*(it - 1) != '6') {
						*it++ = '6';
					} else {
						it = str.erase(it);
					}
					break;
				case 'Q':
				case 'J':
					if (*(it - 1) != '7') {
						*it++ = '7';
					} else {
						it = str.erase(it);
					}
					break;
				case 'A':
				case 'E':
				case 'H':
				case 'I':
				case 'O':
				case 'U':
				case 'W':
				case 'Y':
					if (*(it - 1) != '0') {
						*it++ = '0';
					} else {
						it = str.erase(it);
					}
					break;
				default:
					it = str.erase(it);
			}
		}

		return str;
	}

	std::string_view _name() const noexcept {
		return "SoundexSpanish";
	}

	std::string _description() const noexcept {
		return "Soundex for Spanish Language";
	}

public:
	SoundexSpanish() = default;

	template <typename T>
		requires (!soundex_detail::is_encoder_v<T, SoundexSpanish>)
	SoundexSpanish(T&& str)
		: Soundex<SoundexSpanish>(_encode(std::string(std::forward<T>(str)))) { }
};

}  // namespace frozen
