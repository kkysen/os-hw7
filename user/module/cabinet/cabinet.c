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

void lookup_cab_info(unsigned long vaddr __always_unused, struct cab_info *info) {
	*info = (struct cab_info) {0};
}

long inspect_cabinet(int pid,
		     unsigned long vaddr,
		     struct cab_info *inventory)
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

	lookup_cab_info(vaddr, &info);
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
