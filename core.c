#include "core.h"

#include <stdlib.h>

void *memory_alloc_bytes(size_t bytes) {
	if (bytes == 0) return NULL;

	void *mem = calloc(1, bytes);
	if (mem) {
		return mem;
	} else {
		die("Out of memory (tried to allocate %zu bytes).", bytes);
		return mem;
	}
}

void *memory_realloc_bytes(void *mem, size_t bytes) {
	mem = realloc(mem, bytes);
	if (mem) {
		return mem;
	} else {
		die("Out of memory (tried to allocate %zu bytes).", bytes);
		return mem;
	}
}
