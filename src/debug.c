#include "debug.h"

#include "simulator.h"
#include "segfreelist.h"

void ye_snapshot() {
    ye_header *head;
    for (int index = 0; index < NUM_LISTS; index++) {
        head = seglist[index].head;
        if (head != NULL) {
            info("FREE LIST %d:", index);
            for (ye_header *hdr = head; hdr != NULL; hdr = hdr->next) {
                ye_blockprint(hdr);
            }
        }
    }
}

void ye_blockprint(ye_header *hdr) {
    info("Block at %p: \n\
         size: %u \n\
         allocated: %s \n\
         next: %p \n\
         prev: %p",
         hdr, hdr->size, hdr->alloc, hdr->next, hdr->prev);
}

void ye_varprint(void *data) {
    ye_blockprint(data - sizeof(ye_header));
}