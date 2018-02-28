#include "debug.h"

#include "simulator.h"
#include "segfreelist.h"

void ye_snapshot() {
    ye_header *head;
    for (int index = 0; index < NUM_LISTS; index++) {
        head = seglist[index].head;
        if (head != NULL) {
            info("FREE LIST %d:", index);
            for (ye_header *block = head; block != NULL; block = block->next) {
                ye_blockprint(block);
            }
        }
    }
}

void ye_blockprint(ye_header *block) {
    info("Block at %p: \n\
         size: %u \n\
         allocated: %s \n\
         next: %p \n\
         prev: %p",
         block, block->size, block->alloc, block->next, block->prev);
}

void ye_varprint(void *data) {
    ye_blockprint(data - YE_HEADER_SIZE);
}