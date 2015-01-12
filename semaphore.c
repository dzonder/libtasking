#include "semaphore.h"

#include "arch.h"
#include "task_structs.h"

struct semaphore {
	int32_t value;
	struct task_info *list_head_wait_queue;
	struct task_info *list_tail_wait_queue;
};

struct semaphore * semaphore_init(int32_t value)
{
	struct semaphore *sem = malloc(sizeof(*sem));

	assert(sem != NULL);
	assert(value > 0);

	sem->value = value;

	sem->list_head_wait_queue = NULL;
	sem->list_tail_wait_queue = NULL;

	return sem;
}

void semaphore_free(struct semaphore *sem)
{
	//assert(sem->value == _initial_sem_value_);
	assert(sem->list_head_wait_queue == NULL);
	assert(sem->list_tail_wait_queue == NULL);

	free(sem);
}

void semaphore_wait(struct semaphore *sem)
{
	int32_t value;
	uint32_t strex_failed = 1U;

	while (strex_failed) {
		__asm volatile("LDREX %0, [%1]\n\t"
			: "=r" (value)
			: "r" (&sem->value));

		--value;

		__asm volatile("STREX %0, %1, [%2]\n\t"
			: "=r" (strex_failed)
			: "r" (value), "r" (&sem->value));
	}

	if (value < 0)
		task_wait(&sem->list_head_wait_queue, &sem->list_tail_wait_queue);

	__asm("dmb");
}

void semaphore_post(struct semaphore *sem)
{
	int32_t value;
	uint32_t strex_failed = 1U;

	__asm("dmb");

	while (strex_failed) {
		__asm volatile("LDREX %0, [%1]\n\t"
			: "=r" (value)
			: "r" (&sem->value));

		++value;

		__asm volatile("STREX %0, %1, [%2]\n\t"
			: "=r" (strex_failed)
			: "r" (value), "r" (&sem->value));
	}

	if (value <= 0)
		task_signal(&sem->list_head_wait_queue, &sem->list_tail_wait_queue);
}
