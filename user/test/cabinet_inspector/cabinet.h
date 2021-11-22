#include <unistd.h>
#include <linux/cabinet.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#ifndef CABINET_H
#define CABINET_H

#define strbool(b) (b) ? "yes" : "no"

#define die(...) \
	do { \
		if (errno) \
			perror("errno"); \
		fprintf(stderr, __VA_ARGS__); \
		exit(1); \
	} while (0)

#define __NR_inspect_cabinet 505

static inline int inspect_cabinet(pid_t pid, unsigned long vaddr,
		struct cab_info *inventory)
{
	return syscall(__NR_inspect_cabinet, pid, vaddr, inventory);
}


static inline void pcab_info(struct cab_info *info)
{
	printf("paddr: %p\n",		(void *) info->paddr);
	printf("pf_paddr: %p\n",	(void *) info->pf_paddr);
	printf("pte_paddr: %p\n",	(void *) info->pte_paddr);
	printf("pmd_paddr: %p\n",	(void *) info->pmd_paddr);
	printf("pud_paddr: %p\n",	(void *) info->pud_paddr);
	printf("p4d_paddr: %p\n",	(void *) info->p4d_paddr);
	printf("pgd_paddr: %p\n",	(void *) info->pgd_paddr);
	printf("dirty: %s\n",		strbool(info->dirty));
	printf("refcount: %d\n",	info->refcount);
}

#endif
