// Frozen copy of the pre-optimization SoundexEnglish::_encode (in-place
// erase/insert). The reference for the differential test and the benchmark;
// do not "improve" it.
#pragma once

#include <cctype>
#include <string>

inline std::string frozen_encode(std::string str) {
	// 1. Remove all non alphabetic characters at the begin.
	auto it = str.begin();
	while (it != str.end()) {
		if ((*it >= 'a' && *it <= 'z') || (*it >= 'A' && *it <= 'Z')) {
			break;
		} else {
			it = str.erase(it);
		}
	}

	if (str.empty()) {
		return str;
	}

	// 2. Starts the calculation of Soundex.
	it = ++str.insert(it, std::toupper(*it));
	while (it != str.end()) {
		switch (*it) {
			case 'b':
			case 'p':
			case 'B':
			case 'P':
				if (*(it - 1) != '1') {
					*it++ = '1';
				} else {
					it = str.erase(it);
				}
				break;
			case 'f':
			case 'v':
			case 'F':
			case 'V':
				if (*(it - 1) != '2') {
					*it++ = '2';
				} else {
					it = str.erase(it);
				}
				break;
			case 'c':
			case 'k':
			case 's':
			case 'C':
			case 'K':
			case 'S':
				if (*(it - 1) != '3') {
					*it++ = '3';
				} else {
					it = str.erase(it);
				}
				break;
			case 'g':
			case 'j':
			case 'G':
			case 'J':
				if (*(it - 1) != '4') {
					*it++ = '4';
				} else {
					it = str.erase(it);
				}
				break;
			case 'q':
			case 'x':
			case 'z':
			case 'Q':
			case 'X':
			case 'Z':
				if (*(it - 1) != '5') {
					*it++ = '5';
				} else {
					it = str.erase(it);
				}
				break;
			case 'd':
			case 't':
			case 'D':
			case 'T':
				if (*(it - 1) != '6') {
					*it++ = '6';
				} else {
					it = str.erase(it);
				}
				break;
			case 'l':
			case 'L':
				if (*(it - 1) != '7') {
					*it++ = '7';
				} else {
					it = str.erase(it);
				}
				break;
			case 'm':
			case 'n':
			case 'M':
			case 'N':
				if (*(it - 1) != '8') {
					*it++ = '8';
				} else {
					it = str.erase(it);
				}
				break;
			case 'r':
			case 'R':
				if (*(it - 1) != '9') {
					*it++ = '9';
				} else {
					it = str.erase(it);
				}
				break;
			case 'a':
			case 'e':
			case 'h':
			case 'i':
			case 'o':
			case 'u':
			case 'w':
			case 'y':
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
				break;
		}
	}

	return str;
}


