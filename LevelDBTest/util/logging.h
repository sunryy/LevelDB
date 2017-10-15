#pragma once
#include <stdio.h>
#include <stdint.h>
#include <string>
#include "port/port.h"

namespace{

	class Slice;
	class WritableFile;

	//Append a human-readable printout of "num" to * str
	extern void AppendNumberTo(std::string* str, uint64_t num);

	//Append a human-readable printout of "value" to *str.
	extern void AppendEscapedStringTo(std::string* str, const Slice& value);

	//Return a human-readable printout of "num"
	extern std::string NumberToString(uint64_t num);

	// Return a human-readable version of "value".
	// Escapes any non-printable characters found in "value".
	extern std::string EscapeString(const Slice& value);

	// If *in starts with "c", advances *in past the first character and
	// returns true.  Otherwise, returns false.
	extern bool ConsumeChar(Slice* in, char c);

	// Parse a human-readable number from "*in" into *value.  On success,
	// advances "*in" past the consumed number and sets "*val" to the
	// numeric value.  Otherwise, returns false and leaves *in in an
	// unspecified state.
	extern bool ConsumeDecimalNumber(Slice* in, uint64_t* val);

}