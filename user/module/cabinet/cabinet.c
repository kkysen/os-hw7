/*
 * cabinet.c
 */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include <linux/module.h>
#include <linux/printk.h>
#include <linux/cabinet.h>
#include <linux/cred.h>
#include <linux/uaccess.h>
#include <linux/pgtable.h>
#include <linux/page_ref.h>

#pragma GCC diagnostic pop

extern long (*inspect_cabinet_ptr)(int pid, unsigned long vaddr,
				   struct cab_info *inventory);
extern long (*inspect_cabinet_default)(int pid, unsigned long vaddr,
				       struct cab_info *inventory);

long lookup_cab_info(struct task_struct *task, unsigned long vaddr,
		     struct cab_info *info)
{
	pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;

#define check_pxd(x)                                                           \
	do {                                                                   \
		if (p##x##d_bad(*p##x##d) || p##x##d_none(*p##x##d))           \
			return -EINVAL;                                        \
		if (!p##x##d_present(*p##x##d))                                \
			return 0;                                              \
	} while (false)

	/* zero everything in case we return early */
	*info = (struct cab_info){ 0 };
	pgd = pgd_offset(task->mm, vaddr);
	check_pxd(g);
	info->pgd_paddr = __pa(task->mm->pgd);

	p4d = p4d_offset(pgd, vaddr);
	check_pxd(4);
	info->p4d_paddr = (pgd_val(*pgd) & PTE_PFN_MASK);

	pud = pud_offset(p4d, vaddr);
	check_pxd(u);
	info->pud_paddr = (p4d_val(*p4d) & p4d_pfn_mask(*p4d));

	pmd = pmd_offset(pud, vaddr);
	check_pxd(m);
	info->pmd_paddr = (pud_val(*pud) & pud_pfn_mask(*pud));

	pte = pte_offset_map(pmd, vaddr);
	if (!pte || !pte_present(*pte))
		return 0;
	info->pte_paddr = (pmd_val(*pmd) & pmd_pfn_mask(*pmd));

#undef check_pxd

	info->pf_paddr =
		(unsigned long)((phys_addr_t)pte_pfn(*pte) << PAGE_SHIFT);
	info->paddr = info->pf_paddr | (vaddr & ~PAGE_MASK);
	info->dirty = pte_dirty(*pte);
	info->refcount = page_ref_count(pte_page(*pte));
	return 0;
}

long inspect_cabinet(int pid, unsigned long vaddr, struct cab_info *inventory)
{
	long e;
	kuid_t euid;
	bool is_root;
	struct task_struct *task;
	struct cab_info info;

	e = 0;
	euid = current_cred()->euid;
	is_root = uid_eq(euid, GLOBAL_ROOT_UID);
	if (!is_root) {
		e = -EACCES;
		goto ret;
	}
	/**
	 * Should we check root directly or check `CAP_SYS_RAWIO`,
	 * which similarly allows accessing `/dev/mem`?
	 */
	//if (!capable(CAP_SYS_RAWIO)) {
	//	e = -EACCES;
	//	goto ret;
	//}
	if (pid < -1) {
		e = -EINVAL;
		goto ret;
	}
	if (pid == -1) {
		pid = current->pid;
		task = current;
	} else {
		struct pid *pid_struct;

		pid_struct = find_vpid(pid);
		if (!pid_struct) {
			e = -ESRCH;
			goto ret;
		}
		task = pid_task(pid_struct, PIDTYPE_PID);
		if (!task) {
			e = -ESRCH;
			goto ret;
		}
	}

	e = lookup_cab_info(task, vaddr, &info);
	if (e < 0)
		goto ret;
	if (copy_to_user(inventory, &info, sizeof(info)) != 0) {
		e = -EFAULT;
		goto ret;
	}

ret:
	return e;
}

int cabinet_init(void)
{
	pr_info("Installing cabinet\n");
	inspect_cabinet_ptr = inspect_cabinet;
	return 0;
}

void cabinet_exit(void)
{
	pr_info("Removing cabinet\n");
	inspect_cabinet_ptr = inspect_cabinet_default;
}

module_init(cabinet_init);
module_exit(cabinet_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Cabinet: a virtual to physical memory mapper");
MODULE_AUTHOR("FireFerrises");
