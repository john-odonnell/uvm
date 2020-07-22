# Universal Virtual Machine
## A Turing Complete VM

**Contributors: John O’Donnell and Marcus Comolli**

All aspects of the UVM have been implemented correctly to the assignment specifications.

**Architecture**

8 general purpose registers.\
A dynamic address space of memory segments, each with a unique 32 bit ID.\
A 32 bit program counter, which points to program instructions in the most recently loaded program, or the "0 Segment".

**Modules**\
*um.c*\
Contains the main function of the universal machine.\
Here, the file in question is opened, and then passed to the run_um.h module.

*run_um.h*\
This module loads the program into the zero segment using seg.h functions.\ 
It pulls each instruction from the zero segment, and executes them by way of the module execute.h.

*execute.h*\
This module is responsible for the work of the instructions\
It uses the seg.h functions in order to perform universal machine abstraction-specific functions.

*seg.h*\
This is our representation of segmented memory. The abstraction is a Hanson sequence of Hanson sequences.\
The overarching sequence is representative of the overall address space, while the sequences within are representative of each segment.\
The interior Hanson sequences contain uint32_t's, which are all words within the segment.


The routine that takes the most time in the final version of our Universal Machine is Hanson’s Seq_get. This is due to our design, where the memory is a Hanson sequence of Hanson sequences. This cannot be improved because the implementation is not available to us.

**Timing**\
Our UM executes the 85,070,522 instructions of midmark.umz in 8.176539 seconds.\
This shows an instruction/second rate of about 10.5 million instructions per second.\
This means our UM could execute 50 million instructions in about 4.7 seconds.

**Work Hours**\
Hours spent analyzing: 3\
Hours spent preparing: 4\
Hours spent solving problems: 15

<!--
ARCHITECTURE
          bitpack.h
             |
um.c --- run_um.h ---- execute.h
                \       / 
                 \     /
                  seg.h
 
All aspects of the universal machine have been correctly implemented.
No significant departures from our design were made.
-->
