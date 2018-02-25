#ifndef SEGLIST_H
#define SEGLIST_H

#include "helpers.h"
#include "allocator.h"

/* Struct to store a free list */
typedef struct {
    ye_header *head;
    uint16_t min;
    uint16_t max;
} freelist;

void seg_init();
void seg_add(ye_header *blockhdr);
void seg_rm(ye_header *blockhdr);
ye_header *seg_find(size_t size);

#endif /* SEGLIST_H */
