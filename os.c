#include "os.h"

#if __unix__

#include <sys/stat.h>

size_t fs_file_size(char const *filename) {
	struct stat statbuf = {0};
	stat(filename, &statbuf);
	return (size_t)statbuf.st_size;
}

Time time_now(void) {
	Time ts = {0};
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts;
}

double time_sub(Time a, Time b) {
	return (double)(a.tv_sec - b.tv_sec)
		+ 1e-9 * (double)(a.tv_nsec - b.tv_nsec);
}

#else
#error "@TODO"

#endif
