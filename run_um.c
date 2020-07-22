#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "assert.h"
#include "seq.h"
#include "mem.h"

#include "bitpack.h"
#include "seg.h"
#include "run_um.h"

extern uint32_t seg_map  (Segs segments, uint32_t size);
extern void     seg_unmap(Segs segments, uint32_t id);
extern void     seg_fill (Segs segments, uint32_t values, uint32_t id, uint32_t offset);
extern uint32_t seg_get  (Segs segments, uint32_t id, uint32_t offset);

void load_zero(FILE *stream, Segs memory) {	
	// initialize zero segment
	Array_T zero_seg = Array_new(1000, sizeof(uint32_t));

	int count = 0;
	int length = 1000;
	
	bool flag = true;

	// until the loop reaches the EOF
	while(flag){
		// initialize current word to 0
		int placehold;
		uint32_t word = 0;
		// scan for unsigned integer or break loop
		
		for (int i = 0; i < 4; i++) {
			placehold = getc(stream);
			if (placehold == EOF) {
				flag = false;
			} else {
				if (i == 0) {
					word = Bitpack_newu(word, 8, 24, (uint32_t)placehold);
				} else if (i == 1) {
					word = Bitpack_newu(word, 8, 16, (uint32_t)placehold);
				} else if (i == 2) {
					word = Bitpack_newu(word, 8,  8, (uint32_t)placehold);
				} else {
					word = Bitpack_newu(word, 8,  0, (uint32_t)placehold);
				}
			}
		}
		
		// add the word to the zero sequence
		// printf("WORD: %d: %x\n", count, word);
		*(uint32_t*)Array_get(zero_seg, count) = word;
		count++;

		if (count == length) {
			Array_resize(zero_seg, length*2);
			length = length * 2;
		}
	}
	
	Array_resize(zero_seg, count);

	// for the number of words in the zero segment
	// transfer each 32 bit word from the zero_seg sequence into memory
	// int words = Seq_length(zero_seg);
	// seg_map(memory, words);
	memory->highest += 1;
	Seq_addhi(memory->segments, (void*)zero_seg);

	return;
}

Op_Instruction get_opcode(uint32_t word) {
	// isolate opcode
	uint32_t opcode = word;
   	opcode = opcode >> 28;
	
	// initialize register id's and value
	// along with isolating ints for all
	uint32_t     a = 0;
	uint32_t     b = 0;
	uint32_t     c = 0;
       	uint32_t value = 0;
	uint32_t a_isolator;
	uint32_t b_isolator =  56;
       	uint32_t c_isolator =   7;
	if (opcode != 13) {
		// isolate a, b and c
		a_isolator = 448;
		a = (word & a_isolator); a = a >> 6;
		b = (word & b_isolator); b = b >> 3;
		c = (word & c_isolator);
	} else {
		// isolate a and value
		a_isolator = 234881024;
		a = (word & a_isolator); a = a >> 25;
		value = (word); value = value << 7; value = value >> 7;
	}
	
	// insert opcode, a, b, c and value into a struct and return
	Op_Instruction word_info;
	// NEW(word_info);
	word_info.opcode = opcode;
	word_info.a = a;
	word_info.b = b;
	word_info.c = c;
	word_info.value = value;

	return word_info;
}

void run(FILE *stream) {
	// create new address space and
	// load the program into the zero seg
	Segs memory = seg_new();
	load_zero(stream, memory);
	
	// int instruction_counter = 0;

	// initialize registers and program counter
	uint32_t reg[8]  = {0, 0, 0, 0, 0, 0, 0, 0};
	uint32_t pro_counter = 0;
	bool halted = false;
	
	// until the loop is told to break...
	while(!halted) {

		register Array_T zero_seg = Seq_get(memory->segments, 0);
		bool changed_zero = false;

		while(!changed_zero) {
			// pull word from the zero seg and break into components
			uint32_t word = *(uint32_t*)Array_get(zero_seg, pro_counter);
			Op_Instruction this_instr = get_opcode(word);

			// instruction_counter++;

			// pass the instruction to a function depending on opcode
			int opcode = this_instr.opcode;
			int a 	   = this_instr.a;
			int b	   = this_instr.b;
			int c	   = this_instr.c;
			int value  = this_instr.value;

			switch (opcode) {
				case 0:
					// conditional move
					if ((int)reg[c] != 0) { reg[a] = reg[b]; }
					break;
				case 1:
					// load segment from memory
					reg[a] = seg_get(memory, reg[b], reg[c]);
					break;
				case 2:
					// store segment in memory
					seg_fill(memory, reg[c], reg[a], reg[b]);
					break;
				case 3:
					// add
					reg[a] = (reg[b] + reg[c]) % 4294967296;
					break;
				case 4:
					// multiply
					reg[a] = (reg[b] * reg[c]) % 4294967296;
					break;
				case 5:
					// divide
					reg[a] = reg[b] / reg[c];
					break;
				case 6:
					// logical nand
					reg[a] = ~(reg[b] & reg[c]);
					break;
				case 7:
					// halt
					seg_free(memory);
					FREE(memory);
					break;
				case 8:
					// map segment
					reg[b] = seg_map(memory, reg[c]);
					break;
				case 9:
					// unmap segment
					seg_unmap(memory, reg[c]);
					break;
				case 10:
					// output
					fprintf(stdout, "%c", (char)reg[c]);
					break;
				case 11:
					// input
					reg[c] = (uint32_t)fgetc(stdin);
					break;
				case 12:
					// load program
					if (reg[b] != 0) {
						// seg_unmap(memory, 0);

						Array_T this_seg = Seq_get(memory->segments, reg[b]);
						int words = Array_length(this_seg);

						// seg_map(memory, words);
						
						Array_T zero_seg = Seq_get(memory->segments, 0);
						zero_seg = Array_copy(this_seg, words);
						Array_T old_zero = Seq_put(memory->segments, 0, zero_seg);
						Array_free(&old_zero);

						changed_zero = true;
					}
					pro_counter = reg[c];
					break;
				case 13:
					// load value
					reg[a] = value;
					break;
				default:
					break;
			};
			
			// if the program received the halt instruction,
			// break the execution loop
			if (opcode == 7) { halted = true; break; }

			// increment program counter
			if (opcode != 12) { pro_counter++; }
		}
	}

	// printf("instruction count: %d\n", instruction_counter);

	return;
}
