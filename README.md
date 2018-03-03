This is a dynamic memory allocator. It is modeled after glibc, and borrows
some optimizations from it. 

The supported operations are ye_malloc, ye_free, ye_calloc, and ye_realloc,
which mimics their glibc counterparts. 

The allocator obtains memory by allocating a "heap" of adjustable size using
glibc's malloc. It then allocates blocks of memory on request, storing free 
blocks using a segmented free-list. The free-list acts with the best-fit
policy for blocks of small size (currently 64, but this is adjustable) and
as a first-fit allocator for larger blocks. 
