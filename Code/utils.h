#pragma once

#include "assert.h"

// streams
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

// structures
#include <string>
#include <vector>

// algo
#include <algorithm>

// random
#include <random>

std::mt19937_64 rnd(time(NULL));

using s8 = int8_t;
using u8 = uint8_t;
using s16 = int16_t;
using u16 = uint16_t;
using s32 = int32_t;
using u32 = uint32_t;
using s64 = int64_t;
using u64 = uint64_t;

using point_t = float;

using text_t = const char*;

// random range [0, 1]
point_t get_urnd() {
	return (point_t)rnd() / ULLONG_MAX;
}

// random range [-1, 1]
point_t get_rnd() {
	return ((point_t)rnd() - LLONG_MAX) / ULLONG_MAX * 2;
}

// random range [left, right]
point_t get_rnd_range(point_t left, point_t right) {
	ASSERT(left <= right, "get_rnd_range: wrong border [left, right]");
	point_t len = right - left;
	return left + len * get_urnd();
}

s64 get_rnd_range_int(s64 left, s64 right) {
	ASSERT(left <= right, "get_rnd_range_int: wrong border [left, right]");
	return left + (rnd() % u64(right - left + 1));
}

template<typename T>
T quare(const T& val) {
	return val * val;
}

template<typename T>
T clamp(const T& min, const T& val, const T& max) {
	ASSERT(min <= max, "bad border");

	if (val < min) {
		return min;
	}
	else if (val > max) {
		return max;
	}
	else {
		return val;
	}
}

template<typename T>
bool is_between(const T& min, const T& val, const T& max) {
	ASSERT(min <= max, "bad border");

	return min <= val && val <= max;
}

// object -> string
template<typename T>
std::string to_string(const T& val) {

	std::ostringstream oss;
	oss << std::setprecision(4) << val;

	std::string s = oss.str();
	
	if (s == "nan(ind)" || s == "-nan(ind)") {
		s = "nan";
	}

	return s;
}

u64 round_two(u64 n) {
	n--;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	n |= n >> 32;
	return n + 1;
}

void fill(u32* dest, u32 val32, u32 len) {

#ifdef WINX32

	for (u32 i = 0; i < len; i++) {
		dest[i] = val32;
	}

#elif defined(WINX64)

	u64 val64 = (static_cast<u64>(val32) << 32) | val32;

	len--;
	for (u32 i = 0; i < len; i += 2) {
		*reinterpret_cast<u64*>(dest + i) = val64;
	}
	len++;

	if (len & 1) {
		dest[len - 1] = val32;
	}

#endif
}
