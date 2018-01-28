#ifndef SEGLIST_H
#define SEGLIST_H

#include "helpers.h"
#include "allocator.h"

void seg_insert(sf_header *blockhdr);

void seg_remove(sf_header *blockhdr);

int seg_listindex(size_t size);

sf_header *seg_findspace(size_t size);

#endif /* SEGLIST_H */