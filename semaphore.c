#include "semaphore.h"

#include "arch.h"
#include "task.h"

struct semaphore {
	int32_t value;
	struct wait_queue wait_queue;
};

struct semaphore * semaphore_init(int32_t value)
{
	struct semaphore *sem = malloc(sizeof(*sem));

	assert(sem != NULL);
	assert(value > 0);

	sem->value = value;

	task_wait_queue_init(&sem->wait_queue);

	return sem;
}

void semaphore_free(struct semaphore *sem)
{
	//assert(sem->value == _initial_sem_value_);
	//assert(sem->wait_queue->list_head == NULL);
	//assert(sem->wait_queue->list_tail == NULL);

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
		task_wait_queue_wait(&sem->wait_queue);

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
		task_wait_queue_signal(&sem->wait_queue);
}
