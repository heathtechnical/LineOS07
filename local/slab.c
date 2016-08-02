#include<stdio.h>
#include<list.h>
#include<string.h>

typedef kmem_cache{
	char *name;
	unsigned long size;
	struct list_head free;
	struct list_head partial;
	struct list_head full;
}kmem_cache_t;

typedef kmem_slab{
	struct list_head list;
	void *s_mem;
 
}kmem_slab_t;

struct name{
	struct list_head list;
	char *name;
};

main(){
	struct name name1,name2,name3;
	struct list_head name_list, *ptr;
	
	name1.name="Dan";
	name2.name="Becc";
	name3.name="Sue";

	/* Initialise name_list head */
	INIT_LIST_HEAD(&name_list);

	/* Add Items to the list, 1st param = the embedded listin our object, p2 = the list head */
	list_add(&name1.list,&name_list);
	list_add(&name2.list,&name_list);
	list_add(&name3.list,&name_list);

	/* Iterate Over List */
	list_for_each(ptr,&name_list){
		if(!strcmp(list_entry(ptr,struct name,list)->name,"Dan")){
			list_del(ptr);
		}
		printf("The Name Is %s\n",list_entry(ptr,struct name,list)->name);
	}

	printf("Iteration 2:\n");
	
	list_for_each(ptr,&name_list){
		if(!strcmp(list_entry(ptr,struct name,list)->name,"Dan")){
			list_del(ptr);
		}
		printf("The Name Is %s\n",list_entry(ptr,struct name,list)->name);
	}
}
