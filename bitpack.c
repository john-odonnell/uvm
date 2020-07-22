#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "assert.h"
#include "except.h"

Except_T Bitpack_Overflow = { "Overflow Packing Bits" };

uint64_t shl(uint64_t word, unsigned bits) {
	assert(bits <= 64);
	if (bits == 64) {
		// shifting 64 results in 64 0s
		return 0;
	} else {
		// otherwise shift by bits
		return (word << bits);
	}
}

uint64_t shr(uint64_t word, unsigned bits) {
	assert(bits <= 64);	
	if (bits == 64) {
		// shifting 64 results in 64 0s
		return 0;
	} else {
		// otherwise shift by bits
		return (word >> bits);
	}
}

uint64_t sra(uint64_t word, unsigned bits) {
	assert(bits <= 64);
	if (bits == 64) {
		bits = 63;
	}

	return (int64_t)word >> bits;
}

bool Bitpack_fitsu(uint64_t n, unsigned width) {
	assert(width <= 64);
	
	// limit is equal to all 1 bits
	uint64_t limit = ~0;
	
	// limit = 2^63
	limit = shl(limit, 63);
	// limit = 2^(width+1);
	limit = shr(limit, 64-width-1);

	if (n < limit) {
		return true;
	} else {
		return false;
	}
}

bool Bitpack_fitss(int64_t n, unsigned width) {
	assert(width <= 64);
	
	// limit is equal to all 1 bits
	int64_t lower_limit = ~0;
	// shifted 1 bits in signed are lower limit
	lower_limit = (int64_t)shl(lower_limit, width-1);


	if (n >= lower_limit && n <= ~lower_limit) {
		return true;
	} else {
		return false;
	}
}

uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb) {
	assert(width <= 64);
	
	// isolate bits in desired region
	uint64_t n = ~0;
	n = shl(n, (64-width));
	n = shr(n, (64-width-lsb));
	
	// logical and isolates desired bits in word
	word = (word & n);
	// moves isolated bits to the least significant bits in word
	word = shr(word, lsb);

	return word;
}

int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb) {
	assert(width <= 64);
	
	// isolate bits in desired region
	uint64_t n = ~0;
	n = shl(n, (64-width));
	n = shr(n, (64-width-lsb));
	
	// logical and isolated bits in word
	word = (word & n);
	// shifts isolated bits to the most significant bits to appease signed notation
	word = shl(word, (64-width-lsb));
	// shifts the bits to the right
	word = sra(word, (64-width));

	return (uint64_t)word;
}

uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, uint64_t value) {
	assert(width <= 64);
	
	// if the value fits in the desired bits
	if (Bitpack_fitsu(value, width)) {
		// isolate desired bits
		int64_t n = ~0;
		n = shl(n, (64-width));
		n = shr(n, (64-width-lsb));
		
		// move value bits to desired range
		value = shl(value, lsb);

		// logical and isolates the desired bits
		// in the desired location in value
		value = (value & n);
		
		// logical and clears the desired bits in word
		word = (word & ~n);

		// logical or combines word and value into word
		word = (word | value);
	} else {
		RAISE(Bitpack_Overflow);
	}

	return word;
}

uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, int64_t value) {
	assert(width <= 64);
	
	// if the value fits in the desired bits
	if (Bitpack_fitss(value, width)) {
		// isolate desired bits
		int64_t n = ~0;
		n = shl(n, (64-width));
		n = shr(n, (64-width-lsb));
		
		// move value bits to desired range
		value = shl(value, lsb);

		// logical and isolates the desired bits
		// in the desired location in value
		value = (value & n);
		
		// logical and clears the desired bits in word
		word = (word & ~n);

		// logical or combines word and value into word
		word = (word | value);
	} else {
		RAISE(Bitpack_Overflow);
	}

	return word;
}
