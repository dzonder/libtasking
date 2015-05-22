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

#ifndef _TASK_H_
#define _TASK_H_

#include "arch.h"
#include "sched_structs.h"
#include "task_conf.h"

#define TID_INVALID		(tid_t)(-1)

/* TID type */
typedef uint64_t tid_t;

/* Task function signature */
typedef void (*task_t)(void *arg);

/* Task spawning options */
struct task_opt {
	task_t task;
	void *arg;
	uint8_t priority;
	uint32_t stack_size;
	uint32_t *user_stack;
	bool privileged;
};

/* Should be executed before using any other task functions */
void task_init(struct scheduler *scheduler, void *scheduler_conf);

/* Adds a task to the scheduler queue */
tid_t task_spawn(task_t task, void *arg);

/* Adds a task to the scheduler queue with specified priority */
tid_t task_spawn_prio(task_t task, void *arg, uint8_t priority);

/* Adds a task to the scheduler queue with specific options */
tid_t task_spawn_opt(struct task_opt *opt);

/* Checks if a specified task has already terminated */
bool task_terminated(tid_t tid);

/* Waits for a specified task to finish */
void task_join(tid_t tid);

/* Currently running task can use yield to relinquish MCU */
void task_yield(void);

/* Wait queues */
struct wait_queue {
	uint32_t signals;
	struct task_info *list_head;
	struct task_info *list_tail;
};

void task_wait_queue_init(struct wait_queue *wait_queue);
void task_wait_queue_wait(struct wait_queue *wait_queue);
void task_wait_queue_signal(struct wait_queue *wait_queue);

#endif /* _TASK_H_ */
