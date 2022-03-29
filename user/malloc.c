// #include "malloc.h"
#include <link.h>
#include <debug.h>
#include <malloc.h>

#define MAX_MALLOC_NODES 256

__USER_DATA unsigned int mem_list_n = 0;
/* List of mem_node, representing all current memory allocations */
__USER_DATA mem_node mem_list[MAX_MALLOC_NODES];

__USER_TEXT int mem_list_insert_i(unsigned int index, mem_node mem_node) {
    for (int i = mem_list_n; i > index; i--) {
        mem_list[i] = mem_list[i - 1];
    }
    mem_list[index] = mem_node;
    mem_list_n++;
    return index;
}

__USER_TEXT int mem_list_remove_i(unsigned int index) {
    for (int i = index; i < mem_list_n; i++) {
        mem_list[i] = mem_list[i + 1];
    }
    mem_list_n--;
    return index;
}

__USER_TEXT int mem_list_remove(char *base) {
    for (int i = 0; i < mem_list_n; i++) {
        if (mem_list[i].base == base) {
            return mem_list_remove_i(i);
        }
    }
    return -1;
}

/**
   Memory allocate

   Assumes mem_list is sequential

   TODO: Debug some info on error
*/
__USER_TEXT void *malloc(unsigned int size) {
    uint32_t *heap_start = &user_thread_heap_start;
    uint32_t *heap_end = &user_thread_heap_end;

    if (!heap_start || !heap_end)
        return (void *)NULL;
    if (size <= 0)
        return (void *)NULL;
    if (mem_list_n >= MAX_MALLOC_NODES - 1)
        return (void *)NULL;
    unsigned int heap_size = (char *)heap_end - (char *)heap_start;
    if (size > heap_size)
        return (void *)NULL;

    /* Handle the free space before the first mem_node */
    if (mem_list_n == 0) {
        char *free_base = (char *)heap_start;
        mem_node m = {.size = size, .base = free_base};
        mem_list_insert_i(0, m);
        return free_base;
    }

    for (int i = 0; i < mem_list_n - 1; i++) {
        char *free_base = mem_list[i].base + mem_list[i].size;
        unsigned int free_size = mem_list[i + 1].base - free_base;
        if (free_size > size) {
            mem_node m = {.size = size, .base = free_base};
            mem_list_insert_i(i + 1, m);
            return free_base;
        }
    }

    /* Handle the free space after the last mem node */
    if (mem_list_n != 0) {
        mem_node node = mem_list[mem_list_n - 1];
        char *free_base = node.base + node.size;
        unsigned int free_size = (char *)heap_end - free_base;
        if (free_size > size) {
            mem_node m = {.size = size, .base = free_base};
            mem_list_insert_i(mem_list_n, m);
            return free_base;
        }
    }

    /* TODO: debug some info */
    return (void *)NULL;
}

/**
   Free previously allocated memory
*/
__USER_TEXT void free(void *ptr) {
    mem_list_remove(ptr);
}
