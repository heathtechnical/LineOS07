#include<ds/list.h>

typedef struct dev{
	struct list_head list;
	char *name;
	u16 major;
	u16 minor;
	void *devinfo;
	u32 devinfo_size;
} dev_t;

void init_devices(void);
u8 register_device(char *name, u16 major, u16 minor, void *devinfo, u32 devinfo_size);
void dump_devices(void);
void foreachdevice(u16 major,void (*do_func)(dev_t *dev));
