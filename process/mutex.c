#include<process.h>
#include<mutex.h>

void mutex_init(mutex_t *mutex)
{
	mutex->owner=NULL;
	mutex->state=MUTEX_UNLOCKED;
}

void mutex_lock(mutex_t *mutex)
{
	/* Mutexes only make sense in a preemptable environment */
	if(!is_kernel_preempt()) return;

	/* Return of we already own the lock */
	if(mutex->owner==get_current_process() && mutex->state==MUTEX_LOCKED) return;

	/* Disable kernel preemption */
	disable_kernel_preempt();

	/* Spin while Mutex is locked */
	while(mutex->state==MUTEX_LOCKED){
		mutex_wait();
	}

	/* Finally lock the Mutex */
	mutex->state=MUTEX_LOCKED;
	mutex->owner=get_current_process();

	/* Now we can safely re-enable preemption */
	enable_kernel_preempt();
}

void mutex_wait(mutex_t *mutex)
{
	disable_ints();

	enable_kernel_preempt();

	enable_ints();
}
