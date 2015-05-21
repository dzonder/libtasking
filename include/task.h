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
