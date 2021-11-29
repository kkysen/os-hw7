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

void lookup_cab_info(struct task_struct *task, unsigned long vaddr,
		     struct cab_info *info)
{
	pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;
	struct page *page;

#define next_offset_using(cur, next, offset, check)                                \
	do {                                                                       \
		unsigned long pfn;                                                 \
		phys_addr_t phys_addr;                                             \
		next = offset(cur, vaddr);                                         \
		if (!(check))                                                      \
			return;                                                    \
		pfn = next##_pfn(*next);                                           \
		phys_addr = (phys_addr_t)pfn << PAGE_SHIFT;                        \
		info->next##_paddr = (unsigned long)phys_addr;                     \
		/*info->next##_paddr = next##_val(*next);*/                        \
		/*info->next##_paddr = */                                          \
			/*(next##_val(*next) & next##_pfn_mask(*next)) +*/         \
			/*next##_index(vaddr);*/ \
	} while (false)

#define next_offset(cur, next)                                                 \
	next_offset_using(cur, next, next##_offset,                            \
			  !next##_none(*next) &&                               \
				  !unlikely(next##_bad(*next)) &&              \
				  next##_present(*next))

	/* zero everything in case we return early */
	*info = (struct cab_info){ 0 };
	next_offset(task->mm, pgd);
	next_offset(pgd, p4d);
	next_offset(p4d, pud);
	next_offset(pud, pmd);
	next_offset_using(pmd, pte, pte_offset_map, pte);

#undef next_offset
#undef next_offset_using

	info->pf_paddr =
		(unsigned long)((phys_addr_t)pte_pfn(*pte) << PAGE_SHIFT);
	info->paddr = pte_val(*pte);
	info->dirty = pte_dirty(*pte);
	page = pte_page(*pte);
	info->refcount = page_ref_count(page);
}

long inspect_cabinet(int pid, unsigned long vaddr, struct cab_info *inventory)
{
	kuid_t euid;
	bool is_root;
	struct task_struct *task;
	struct cab_info info;

	euid = current_cred()->euid;
	is_root = uid_eq(euid, GLOBAL_ROOT_UID);
	if (!is_root)
		return -EACCES;
	/**
	 * Should we check root directly or check `CAP_SYS_RAWIO`,
	 * which similarly allows accessing `/dev/mem`?
	 */
	//if (!capable(CAP_SYS_RAWIO))
	//	return -EACCES;
	if (pid < -1)
		return -EINVAL;
	if (pid == -1) {
		pid = current->pid;
		task = current;
	} else {
		struct pid *pid_struct;

		pid_struct = find_vpid(pid);
		if (!pid_struct)
			return -ESRCH;
		task = pid_task(pid_struct, PIDTYPE_PID);
		if (!task)
			return -ESRCH;
	}

	/* TODO: check vaddr is a valid addr first? */

	lookup_cab_info(task, vaddr, &info);
	if (copy_to_user(inventory, &info, sizeof(info)) != 0)
		return -EFAULT;

	return 0;
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
