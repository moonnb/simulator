#ifndef CORE_H_
#define CORE_H_

#if !DEBUG
#define NDEBUG 1
#endif

#include <stdio.h>
#include <assert.h>

typedef struct SDL_Window SDL_Window;

#ifndef SDL_MESSAGEBOX_ERROR
#define SDL_MESSAGEBOX_ERROR 0x00000010
extern int SDL_ShowSimpleMessageBox(
	unsigned, const char *, const char *, SDL_Window *); 
#endif

// show a message box, and if that fails, print the message instead, then exit
#define die(...) do { \
	char _str[1024] = {}; \
	snprintf(_str, sizeof _str - 1, __VA_ARGS__); \
	if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", _str, NULL) < 0) \
		fprintf(stderr, "%s\n", _str); \
	exit(-1); \
} while (0)

#if DEBUG
#define debug_print printf
#else
#define debug_print(...) ((void)0)
#endif

// allocate memory; die on failure
extern void *memory_alloc_bytes(size_t bytes);
extern void *memory_realloc_bytes(void *mem, size_t bytes);
#define memory_allocate(type, n) ((type *)memory_alloc_bytes((n) * sizeof(type)))
#define memory_reallocate(memory, new_n) ((memory) = memory_realloc_bytes((memory), sizeof *(memory) * (new_n)))

#define join3(a, b) a##b
#define join2(a, b) join3(a, b)
#define join(a, b) join2(a, b)

#endif // CORE_H_
