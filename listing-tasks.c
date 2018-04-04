#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/module.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("gavinic - garayf");
MODULE_DESCRIPTION("Show all tasks kernel module");
MODULE_VERSION("1.0");


int listing_tasks_init(void)
{
	printk(KERN_INFO "Loading module listing-tasks\n");
	struct task_struct *task;

	for_each_process(task) {
		printk(KERN_INFO "[pid: %d] pname: %s [state: %ld]\n", task->pid, task->comm, task->state);
	}
	printk(KERN_INFO "Module loaded.\n");
	return 0;	// Success
}

void listing_tasks_exit(void)
{
	printk(KERN_INFO "Removing module listing-tasks\n");
	printk(KERN_INFO "Module removed.\n");
}

module_init(listing_tasks_init);
module_exit(listing_tasks_exit);

