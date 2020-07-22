#include <stdint.h>
#include "seq.h"
#include "array.h"

#ifndef SEG_INCLUDED
#define SEG_INCLUDED

// Segs struct :: universal machine memory
// contains a Hanson sequence of segments,
// a Hanson sequence of unmapped segment IDs,
// and a highest mapped segment value
typedef struct Segs {
	Seq_T segments;
	Seq_T unmapped;
	int highest;
} *Segs;

// creates and returns a new Segs address space
extern Segs     seg_new   ();

// frees the given Segs address space
extern void     seg_free  (Segs segments);

// a new segment is created with size words
// the new segment is either located at the most recently unmapped segment
// or at the end of the segment sequence
inline uint32_t seg_map   (Segs segments, uint32_t size) {
	uint32_t x = 0;


	Array_T this_seg = Array_new((int)size, sizeof(uint32_t));
	for (int i = 0; i < (int)size; i++) {
		*(uint32_t*)Array_get(this_seg, i) = x;
	}

	uint32_t idx;
	
	if (Seq_length(segments->unmapped) == 0) {
		segments->highest += 1;
		idx = segments->highest;
		Seq_addhi(segments->segments, (void*)this_seg);
	} else {
		idx = (uint32_t)(uintptr_t)Seq_remlo(segments->unmapped);
		Array_T current_seg = Seq_get(segments->segments, (int)idx);
		Array_free(&current_seg);
		Seq_put(segments->segments, (int)idx, (void*)this_seg);
	}
	return idx;
}

// unmaps the segment at the given seg ID
inline void     seg_unmap (Segs segments, uint32_t id) {
	Seq_addlo(segments->unmapped, (void*)(uintptr_t)id);
	return;
}

// fills the offset word of the ID segment with value
inline void     seg_fill  (Segs segments, uint32_t values, uint32_t id, uint32_t offset) {
	Array_T this_seg = Seq_get(segments->segments, (int)id);
	*(uint32_t*)Array_get(this_seg, (int)offset) = values;
	return;
}

// returns the offset word of the ID segment
inline uint32_t seg_get   (Segs segments, uint32_t id, uint32_t offset) {
	Array_T this_seg = Seq_get(segments->segments, (int)id);
	return *(uint32_t*)Array_get(this_seg, (int)offset);
}

#endif
