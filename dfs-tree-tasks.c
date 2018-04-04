#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("garayf - gavinic");
MODULE_DESCRIPTION("Involves iterating over all tasks in the system using DFS tree");
MODULE_VERSION("1.0");

void dfs(struct task_struct *task)
{
	struct task_struct *next_task;
	struct list_head *list;
	list_for_each(list, &task->children) {
		next_task = list_entry(list, struct task_struct, sibling);
		printk(KERN_INFO "[pid: %d] pname: %s [state: %ld]\n",next_task->pid, next_task->comm, next_task->state);
		dfs(next_task);
	}
}
int dfs_tree_tasks_init(void)
{
	printk(KERN_INFO "Loading module dfs-tree-tasks\n");
	dfs(&init_task);
	printk(KERN_INFO "Module loaded.\n");
	return 0;
}	

void dfs_tree_tasks_exit(void)
{
	printk(KERN_INFO "Removing module dfs-tree-tasks\n");
	printk(KERN_INFO "Module removed.\n");
}

module_init(dfs_tree_tasks_init);
module_exit(dfs_tree_tasks_exit);


