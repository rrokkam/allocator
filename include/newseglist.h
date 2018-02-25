#ifndef NEWSEGLIST_H
#define NEWSEGLIST_H

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
void seg_find(size_t size);

#endif /* NEWSEGLIST_H */