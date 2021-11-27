#ifndef _UAPI_LINUX_CABINET_H
#define _UAPI_LINUX_CABINET_H

#include <linux/types.h>

struct cab_info {
	unsigned long paddr;  /* physical addr the virtual addr is mapped to */
	unsigned long pf_paddr;   /* physical address of its page frame */
	unsigned long pte_paddr;  /* physical addr of its PTE */
	unsigned long pmd_paddr;  /* physical addr of its PMD */
	unsigned long pud_paddr;  /* physical addr of its PUD */
	unsigned long p4d_paddr;  /* physical addr of its P4D */
	unsigned long pgd_paddr;  /* physical addr of its PGD */
	int dirty;  /* 1 if dirty, 0 otherwise */
	int refcount;  /* number of processes sharing the physical addr */
};
#endif
