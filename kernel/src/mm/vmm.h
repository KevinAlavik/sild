#pragma once

#include <stdint.h>

#define PTE_ADDR_MASK 0x000ffffffffff000
#define PTE_GET_ADDR(VALUE) ((VALUE) & PTE_ADDR_MASK)
#define PTE_GET_FLAGS(VALUE) ((VALUE) & ~PTE_ADDR_MASK)

#define VMM_PRESENT  (1 << 0)
#define VMM_WRITABLE (1 << 1)
#define VMM_USER     (1 << 2)
#define VMM_NX       (1ULL << 63) 

typedef char sym[];

extern sym reqs_start_ld;
extern sym reqs_end_ld;

extern sym text_start_ld;
extern sym text_end_ld;

extern sym rodata_start_ld;
extern sym rodata_end_ld;

extern sym data_start_ld;
extern sym data_end_ld;


typedef struct pagemap {
    uint64_t *toplevel;
} pagemap_t;

extern pagemap_t *vmm_kernel_pm;
extern pagemap_t *vmm_current_pm;

pagemap_t *vmm_alloc_pm();
void vmm_init();
void vmm_release_pm(pagemap_t *pm);
void vmm_load_pagemap(pagemap_t *pm);
uint64_t vmm_get_flags(pagemap_t* pm, uint64_t vaddr);
uint64_t virt_to_phys(pagemap_t *pagemap, uint64_t virt);
void vmm_map(pagemap_t *pm, uint64_t vaddr, uint64_t paddr, uint64_t flags);
void vmm_unmap(pagemap_t *pm, uint64_t vaddr) ;