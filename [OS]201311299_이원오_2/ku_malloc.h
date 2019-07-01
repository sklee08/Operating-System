#include <stdio.h>

#define KU_MAGIC 0x19311946U

#define _GNU_SOURCE

#define FREE_SIZE sizeof(hnode_t)

typedef struct _hnode_t{
	size_t size;
	struct _hnode_t* next;

}hnode_t;

hnode_t* free_list;

typedef struct _header_t{
	size_t size;
	unsigned long magic;
}hheader_t;

hnode_t* global_base;

void* malloc(size_t size);

void free(void* ptr);

void block_change(hheader_t* ptr, size_t size);

hnode_t* prev_search(hnode_t* ptr);

void coalescing();

hheader_t* expand_heap(size_t size);

hnode_t* next_find_fit(size_t size);

void free_change(hnode_t* ptr, size_t size);

void unlink_free(hnode_t* ptr);
