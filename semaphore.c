/*
Copyright (c) 2015 Michal Olech.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "semaphore.h"

#include "task.h"
#include "sync_low.h"

struct semaphore {
	volatile int32_t value;
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
	uint32_t value = sync_low_atomic_dec((volatile uint32_t *)&sem->value);
	if (*(int32_t *)&value < 0)
		task_wait_queue_wait(&sem->wait_queue);
}

void semaphore_post(struct semaphore *sem)
{
	uint32_t value = sync_low_atomic_inc((volatile uint32_t *)&sem->value);
	if (*(int32_t *)&value <= 0)
		task_wait_queue_signal(&sem->wait_queue);
}
