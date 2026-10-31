/*
 * Copyright © 2026 Germán Méndez Bravo (Kronuz)
 * Copyright (c) 2015-2019 Dubalu LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include "soundex.h"

#include <array>                // for std::array
#include <cctype>                // for std::toupper
#include <string>                // for std::string
#include <string_view>           // for std::string_view
#include <utility>               // for std::forward


/*
 * Refined Soundex for English.
 *   http://ntz-develop.blogspot.mx/2011/03/phonetic-algorithms.html
 *
 * Length of the result is not truncated, so the code does not have a fixed length.
 */
class SoundexEnglish : public Soundex<SoundexEnglish> {

	friend class Soundex<SoundexEnglish>;

	// Code class of each byte: '0'..'9' for the letter classes, 0 for any
	// byte that is not an ASCII letter (those are dropped).
	static constexpr std::array<char, 256> _classes = [] {
		std::array<char, 256> t{};
		auto set = [&t](const char* letters, char code) {
			for (; *letters; ++letters) {
				t[static_cast<unsigned char>(*letters)] = code;
				t[static_cast<unsigned char>(*letters) - 'a' + 'A'] = code;
			}
		};
		set("bp", '1');
		set("fv", '2');
		set("cks", '3');
		set("gj", '4');
		set("qxz", '5');
		set("dt", '6');
		set("l", '7');
		set("mn", '8');
		set("r", '9');
		set("aehiouwy", '0');
		return t;
	}();

	std::string _encode(std::string_view str) const {
		// 1. Skip all non alphabetic characters at the begin.
		size_t i = 0;
		while (i < str.size() && !_classes[static_cast<unsigned char>(str[i])]) {
			++i;
		}

		std::string res;
		if (i == str.size()) {
			return res;
		}

		// 2. Starts the calculation of Soundex: the upper case first letter,
		// then one code per letter, dropping non letters and any code equal to
		// the one right before it.
		res.reserve(str.size() - i + 1);
		res.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(str[i]))));
		char last = res.back();
		for (; i < str.size(); ++i) {
			char code = _classes[static_cast<unsigned char>(str[i])];
			if (code && code != last) {
				res.push_back(code);
				last = code;
			}
		}

		return res;
	}

	std::string_view _name() const noexcept {
		return "SoundexEnglish";
	}

	std::string _description() const noexcept {
		return "Soundex for English Language";
	}

public:
	SoundexEnglish() = default;

	template <typename T>
	SoundexEnglish(T&& str)
		: Soundex<SoundexEnglish>(_encode(std::string_view(std::forward<T>(str)))) { }
};
