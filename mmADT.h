
typedef struct mmCDT * mmADT;

mmADT init_mm(void * start, uint64 size);

void * mm_alloc(mmADT mm, uint64_t size);

void mm_free(mmADT mm, void * mem);