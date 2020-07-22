#include <stdio.h>
#include <stdint.h>

#include "seg.h"

#ifndef RUN_UM_INCLUDED
#define RUN_UM_INCLUDED

// used to break down instruction codes into its components
typedef struct Op_Instruction {
	uint32_t opcode;
	uint32_t a, b, c;
	uint32_t value;
} Op_Instruction;

// loads the program into the zero segment
// this is ran first, as to gather the program into the
// zero seg for manipulation
extern void 	      load_zero  (FILE *stream, Segs memory);

// returns a struct containing instruction details
// such as opcode and register IDs
extern Op_Instruction get_opcode (uint32_t word);

// runs the universal machine
// makes calls to the execute module and
// and manages memory
extern void	      run        (FILE *stream);

#endif
