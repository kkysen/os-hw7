#include <stdio.h>
#include "cabinet.h"

int inspect_cabinet(pid_t pid, unsigned long vaddr, struct cab_info *inventory)
{
	return syscall(__NR_inspect_cabinet, pid, vaddr, inventory);
}

int ref_inspect_cabinet(pid_t pid, unsigned long vaddr,
		struct cab_info *inventory)
{
	return syscall(__NR_ref_inspect_cabinet, pid, vaddr, inventory);
}

void pcab_info(struct cab_info *info)
{
	printf("paddr: %p\n",		(void *) info->paddr);
	printf("pf_paddr: %p\n",	(void *) info->pf_paddr);
	printf("pte_paddr: %p\n",	(void *) info->pte_paddr);
	printf("pmd_paddr: %p\n",	(void *) info->pmd_paddr);
	printf("pud_paddr: %p\n",	(void *) info->pud_paddr);
	printf("pgd_paddr: %p\n",	(void *) info->pgd_paddr);
	printf("modified: %s\n",	info->modified ? "yes" : "no");
	printf("refcount: %d\n",	info->refcount);
}

#define ref reference_module
#define sub submission_module
#define diff diff_module
#define ret return_val

#define diff_set(f)		(diff->f = (ref->f == sub->f))

//#define CMP_PEDANTIC		/* booleans _must_ be bitwise same */

#ifdef CMP_PEDANTIC
#define diff_set_bool(f)	(diff->f = (ref->f == sub->f))
#else
#define diff_set_bool(f)	(diff->f = (!!ref->f == !!sub->f))
#endif

#define ror(f)			(ret |= !(f))

int cmp_cab(struct cab_info *ref, struct cab_info *sub, struct cab_info *diff)
{
	int ret = 0;

	ror(diff_set(paddr));
	ror(diff_set(pf_paddr));
	ror(diff_set(pte_paddr));
	ror(diff_set(pmd_paddr));
	ror(diff_set(pud_paddr));
	ror(diff_set(pgd_paddr));
	ror(diff_set_bool(modified));
	ror(diff_set(refcount));
	return ret;
}
