#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/list.h> //In order to use lists
#include <linux/slab.h> //In order to use kmalloc

struct birthday {
	int day;
	int month;
	int year;
	struct list_head list;
};

struct list_head global_head;

void create_list(void){
	struct birthday *most_important_person;

	most_important_person = kmalloc(sizeof(struct birthday *), GFP_KERNEL);

	most_important_person->day = 15;
	most_important_person->month = 3;
	most_important_person->year = 1998;

	INIT_LIST_HEAD(&global_head);

	list_add_tail(&most_important_person->list, &global_head);

	return;
}

void add_person_tail(int day, int month, int year){
	struct birthday *person;

	person = kmalloc(sizeof(struct birthday *), GFP_KERNEL);

	person->day = day;
	person->month = month;
	person->year = year;

	list_add_tail(&person->list, &global_head);
}

void seed_list(void){
	int i;

	add_person_tail(5, 3, 2004);	//Sister's birthday
	add_person_tail(11, 1, 1995);	//Brother's birthday

	for(i = 0; i < 10; i++){
		add_person_tail(i, i, i);
	}
}

void list_list(void){
	struct birthday *actual;
	struct list_head *pointer;
	list_for_each(pointer, &global_head){
		actual = list_entry(pointer, struct birthday, list);
		printk(KERN_INFO "Birthday %d day, %d month, %d year", actual->day, actual->month, actual->year);
	}
}

void remove_list(void){
	struct birthday *actual, *next;
	list_for_each_entry_safe(actual, next, &global_head, list){
		printk(KERN_INFO "Cleared birthday %d day, %d month, %d year", actual->day, actual->month, actual->year);
		list_del(&actual->list);
		kfree(actual);
	}
}

int simple_init(void){
	printk(KERN_INFO "Loading Module\n");

	printk(KERN_INFO "Creating List\n");
	create_list();
	printk(KERN_INFO "Seeding List\n");
	seed_list();
	printk(KERN_INFO "Listing List\n");
	list_list();
	printk(KERN_INFO "Cleaning List\n");
	remove_list();
	return 0;
}

void simple_exit(void){
	printk(KERN_INFO "Removing Module\n");
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SIMPLE MODULE");
MODULE_AUTHOR("SGG");

