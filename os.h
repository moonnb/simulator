#ifndef OS_H_
#define OS_H_

#include <stddef.h>

// returns the size of the file, or 0 if the file does not exist
extern size_t fs_file_size(char const *filename);

#if __unix__
#include <time.h>
typedef struct timespec Time;
#else
#error "@TODO"
#endif

extern Time time_now(void);
// returns a value in seconds
extern double time_sub(Time a, Time b);

#endif // OS_H_
