#ifndef SEGLIST_H
#define SEGLIST_H

#include <stdlib.h>
#include <stdint.h> // TODO: figure out which number types for data fields

#include "blocks.h"

/* Struct to store a free list */
typedef struct {
    ye_header *head;
    uint16_t min;
    uint16_t max;
} freelist;

void seg_init();

void seg_add(ye_header *hdr);

void seg_rm(ye_header *hdr);

ye_header *seg_find(size_t rsize);

#endif /* SEGLIST_H */
