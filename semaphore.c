#include "semaphore.h"

#include "task.h"
#include "sync_low.h"

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
	uint32_t value = sync_low_atomic_dec((uint32_t *)&sem->value);
	if (*(int32_t *)&value < 0)
		task_wait_queue_wait(&sem->wait_queue);
}

void semaphore_post(struct semaphore *sem)
{
	uint32_t value = sync_low_atomic_inc((uint32_t *)&sem->value);
	if (*(int32_t *)&value <= 0)
		task_wait_queue_signal(&sem->wait_queue);
}
