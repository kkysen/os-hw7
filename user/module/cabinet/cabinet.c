/*
 * cabinet.c
 */
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/cabinet.h>

extern long (*inspect_cabinet_ptr)(int pid, unsigned long vaddr,
		struct cab_info *inventory);
extern long (*inspect_cabinet_default)(int pid, unsigned long vaddr,
		struct cab_info *inventory);

long inspect_cabinet(int pid, unsigned long vaddr, struct cab_info *inventory)
{

	/* implement here */
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
MODULE_AUTHOR("John Hui");
