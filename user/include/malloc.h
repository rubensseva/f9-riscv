#ifndef MALLOC_H_
#define MALLOC_H_

typedef struct mem_node_t {
    char *base;
    unsigned int size;
} mem_node;

int mem_list_insert(mem_node mem_node);
int mem_list_insert_i(unsigned int index, mem_node mem_node);
int mem_list_remove_i(unsigned int index);
int mem_list_remove(char *base);
void *malloc(unsigned int size);
void free(void *base);

#endif // MALLOC_H_
