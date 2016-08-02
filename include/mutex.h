#define MUTEX_UNLOCKED 0
#define MUTEX_LOCKED 1

typedef struct mutex{
	u8 state;
	struct process_t *owner;
}mutex_t;
