#include "mutex.h"

#include "arch.h"
#include "task_structs.h"

enum {
	MUTEX_LOCKED	= 1U,
	MUTEX_UNLOCKED	= 0U,
};

struct mutex {
	uint32_t lock;
	struct task_info *list_head_wait_queue;
	struct task_info *list_tail_wait_queue;
};

struct mutex * mutex_init(void)
{
	struct mutex *mutex = malloc(sizeof(*mutex));

	assert(mutex != NULL);

	mutex->lock = MUTEX_UNLOCKED;
	mutex->list_head_wait_queue = NULL;
	mutex->list_tail_wait_queue = NULL;

	return mutex;
}

void mutex_free(struct mutex *mutex)
{
	assert(mutex->lock == MUTEX_UNLOCKED);
	assert(mutex->list_head_wait_queue == NULL);
	assert(mutex->list_tail_wait_queue == NULL);

	free(mutex);
}

void mutex_lock(struct mutex *mutex)
{
	uint32_t lock;

	for (;;) {
		__asm volatile("LDREX %0, [%1]\n\t"
			: "=r" (lock)
			: "r" (&mutex->lock));

		if (lock == MUTEX_UNLOCKED) {
			uint32_t lock_val = MUTEX_LOCKED;
			uint32_t strex_failed = 0U;

			__asm volatile("STREX %0, %1, [%2]\n\t"
				: "=r" (strex_failed)
				: "r" (lock_val), "r" (&mutex->lock));

			if (strex_failed == 1U)
				continue;
			else
				break;
		}

		task_wait(&mutex->list_head_wait_queue, &mutex->list_tail_wait_queue);
	}

	__asm("dmb");
}

void mutex_unlock(struct mutex *mutex)
{
	__asm("dmb");

	assert(mutex->lock == MUTEX_LOCKED);

	mutex->lock = MUTEX_UNLOCKED;

	task_signal(&mutex->list_head_wait_queue, &mutex->list_tail_wait_queue);
}
