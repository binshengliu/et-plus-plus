#ifndef storage_First
#define storage_First

extern "C" {
    void storage_init();
    void *storage_malloc(int);
    void *storage_malloc_atomic(int);
    void storage_free(void*, int);
    void *storage_realloc(void*, int, int);
    int storage_size(void*);
}

#endif storage_First

