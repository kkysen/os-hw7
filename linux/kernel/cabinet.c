#include <linux/syscalls.h>
#include <linux/mm.h>
#include <asm/pgtable.h>
#include <asm/page.h>
#include <linux/printk.h>
#include <linux/cabinet.h>

long inspect_cabinet_default(pid_t pid, unsigned long vaddr,
		struct cab_info __user *inventory)
{
	pr_err("cabinet module not running.\n");
	return -ENOSYS;
}

long (*inspect_cabinet_ptr)(int pid, unsigned long vaddr,
		struct cab_info *inventory) = inspect_cabinet_default;

SYSCALL_DEFINE3(inspect_cabinet, pid_t, pid, unsigned long, vaddr,
		struct cab_info __user *, inventory)
{
	return inspect_cabinet_ptr(pid, vaddr, inventory);
}


EXPORT_SYMBOL(inspect_cabinet_default);
EXPORT_SYMBOL(inspect_cabinet_ptr);
