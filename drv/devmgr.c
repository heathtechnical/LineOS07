#include<types.h>
#include<system.h>
#include<page.h>
#include<heap.h>
#include<ds/list.h>
#include<process.h>
#include<devmgr.h>

struct list_head k_devs;

void init_devices(void)
{
	/* Initialise list head */
	INIT_LIST_HEAD(&k_devs);
}

u8 register_device(char *name, u16 major, u16 minor, void *devinfo, u32 devinfo_size)
{
	dev_t *dev=kmalloc(sizeof(dev_t),NULL);

	if(!dev){
		panic("Couldn't allocate memory while registering device");
	}

	dev->name=name;
	dev->major=major;
	dev->minor=minor;
	dev->devinfo=devinfo;
	dev->devinfo_size=devinfo_size;		

	list_add(&dev->list,&k_devs);
}

void dump_devices(void)
{
	struct list_head *this;
	dev_t *ptr;

        list_for_each(this,&k_devs){
		ptr=list_entry(this,dev_t,list);

		kprintf("Device Registered %d,%d %s\n",ptr->major,ptr->minor,ptr->name);
	}
}

void foreachdevice(u16 major,void (*do_func)(dev_t *dev))
{
	struct list_head *this;
	dev_t *ptr;

        list_for_each(this,&k_devs){
		ptr=list_entry(this,dev_t,list);
		if(ptr->major==major){
			do_func(ptr);
		}
	}	
}
