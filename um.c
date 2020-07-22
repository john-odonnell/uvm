#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "assert.h"
#include "run_um.h"

// opens filestream and begins runnning universal machine
int main(int argc, char *argv[]) {
	if (argc > 2) {
		// display error if too many arguments
		fprintf(stderr, "Usage: %s [filename].um\n", argv[0]);
		exit(1);
	} else if (argc == 2) {
		// open file stream and run machine
		FILE *stream = fopen(argv[1], "r");
		assert(stream);
		
		// clock_t start, end;
		// double cpu_time;
		
		// start = clock();
		run(stream);
		// end = clock();

		// cpu_time = ((double)(end-start)) / CLOCKS_PER_SEC;

		// printf("cpu time: %f\n", cpu_time);
		fclose(stream);
	} else {
		// run machine from standard input
		run(stdin);
	}

	return 0;
}
