/**
 * cabinet.c
 */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include <linux/module.h>
#include <linux/printk.h>
#include <linux/cabinet.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/cred.h>
#include <linux/uaccess.h>

#pragma GCC diagnostic pop

extern long (*inspect_cabinet_ptr)(int pid, unsigned long vaddr,
		struct cab_info *inventory);
extern long (*inspect_cabinet_default)(int pid, unsigned long vaddr,
		struct cab_info *inventory);

long inspect_cabinet(int pid,
		     unsigned long vaddr __always_unused,
		     struct cab_info *inventory __always_unused)
{
	kuid_t euid;
	bool is_root;
	struct task_struct *task;

	euid = current_cred()->euid;
	is_root = uid_eq(euid, GLOBAL_ROOT_UID);
	if (!is_root)
		return -EACCES;
	if (pid < -1)
		return -EINVAL;
	if (pid == -1) {
		pid = current->pid;
		task = current;
	} else {
		task = find_task_by_vpid(pid);
		if (!task)
			return -ESRCH;
	}

	return -ENOSYS;
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
