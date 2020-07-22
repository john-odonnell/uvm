#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "seq.h"
#include "array.h"
#include "mem.h"
#include "assert.h"

#include "seg.h"
#include "bitpack.h"

// instantiates a new segs struct
// segs includes the segments address space
// along with mapped and unmapped addresses
Segs seg_new() {
	Segs new_seg;
	NEW(new_seg);

	// instatiate members and assign
	new_seg->segments = Seq_new(0);
	new_seg->unmapped = Seq_new(0);

	new_seg->highest = -1;

	return new_seg;
}

// frees segs struct and members
void seg_free(Segs segs) {
	// free memory for each segment sequence
	int len = Seq_length(segs->segments);
	for (int i = 0; i < len; i++) {
		Array_T this_seg = Seq_remlo(segs->segments);
		if (this_seg != NULL) {
			Array_free(&this_seg);
		}
	}

	// free member sequences
	Seq_free((Seq_T*)&segs->segments);
	Seq_free((Seq_T*)&segs->unmapped);

	return;
}

