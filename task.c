#include "task.h"

#include "task_low.h"
#include "task_structs.h"

static struct task_info task_info_pool[TASK_MAX_TASKS];
static struct task_info *task_info_list_head_free;

struct task_info *task_main;
struct task_info *task_current;

static struct scheduler *scheduler = NULL;
static void *scheduler_desc = NULL;

static struct task_opt default_task_opt = {
	.priority	= TASK_DEFAULT_PRIORITY,
	.stack_size	= TASK_DEFAULT_STACK_SIZE,
	.user_stack	= NULL,
};

static struct task_info *task_get_info(tid_t tid)
{
	return &task_info_pool[tid % TASK_MAX_TASKS];
}

static struct task_info *task_alloc_info(void)
{
	task_low_irq_disable();

	struct task_info *task_info = task_info_list_head_free;

	assert(task_info != NULL);
	assert(task_info->state == TASK_STATE_UNUSED);

	task_info_list_head_free = task_info->list_next;

	task_info->list_next = NULL;
	task_info->state = TASK_STATE_RUNNABLE;

	task_wait_queue_init(&task_info->terminate_event);

	/* Allocate next TID incrementing by number of task_info
	   structures available to easily get task_info from TID.
	   Should never overflow. */
	task_info->tid += TASK_MAX_TASKS;

	task_low_irq_enable();

	return task_info;
}

static void task_free_info(struct task_info *task_info)
{
	task_low_irq_disable();

	assert(task_info->state == TASK_STATE_TERMINATED);

	task_info->state = TASK_STATE_UNUSED;
	task_info->list_next = task_info_list_head_free;

	//assert(task_info->terminate_event wait queue is empty);

	task_info_list_head_free = task_info;

	task_low_irq_enable();
}

static void task_scheduler_enqueue(struct task_info *task_info)
{
	task_low_irq_disable();

	assert(scheduler != NULL);
	scheduler->enqueue(scheduler_desc, task_info);

	task_low_irq_enable();
}

static struct task_info *task_scheduler_dequeue(void)
{
	task_low_irq_disable();

	assert(scheduler != NULL);
	struct task_info *task_info = scheduler->dequeue(scheduler_desc);

	task_low_irq_enable();

	return task_info;
}

void task_init(struct scheduler *_scheduler, void *scheduler_conf)
{
	scheduler = _scheduler;

	assert(scheduler != NULL);

	/* Initialize task_info pool as 'unused' list */
	for (uint16_t i = 0U; i < TASK_MAX_TASKS; ++i) {
		struct task_info *task_info = task_info_pool + i;

		task_info->list_next = task_info + 1;
		task_info->state = TASK_STATE_UNUSED;
		task_info->tid = i;
	}
	task_info_pool[TASK_MAX_TASKS - 1].list_next = NULL;

	task_info_list_head_free = task_info_pool;

	assert(scheduler->init != NULL);

	scheduler_desc = scheduler->init(scheduler_conf);

	/* All methods should be set now */
	assert(scheduler->free != NULL);
	assert(scheduler->dequeue != NULL);
	assert(scheduler->enqueue != NULL);
	assert(scheduler->preempt != NULL);

	task_main = task_alloc_info();

	task_main->task = NULL;
	task_main->arg = NULL;

	task_main->opt.priority = TASK_DEFAULT_PRIORITY;
	task_main->opt.stack_size = 0;
	task_main->opt.user_stack = NULL;

	task_main->state = TASK_STATE_RUNNING;

	task_current = task_main;

	task_low_init();

	/* Decide if preemption should be enabled for the main task */
	if (scheduler->preempt(scheduler_desc, task_current))
		task_low_preemption_enable();
}

tid_t task_spawn_opt(task_t task, void *arg, struct task_opt *opt)
{
	struct task_info *task_info = task_alloc_info();

	task_info->task = task;
	task_info->arg = arg;
	task_info->opt = *opt;

	if (task_info->opt.user_stack == NULL) {
		/* User did not provide a stack */
		task_low_irq_disable();
		task_info->stack = malloc(task_info->opt.stack_size);
		task_low_irq_enable();
	} else {
		task_info->stack = task_info->opt.user_stack;
	}

	assert(task_info->stack != NULL);

	task_low_stack_setup(task_info);

	task_scheduler_enqueue(task_info);

	return task_info->tid;
}

tid_t task_spawn(task_t task, void *arg)
{
	return task_spawn_opt(task, arg, &default_task_opt);
}

tid_t task_spawn_prio(task_t task, void *arg, uint8_t priority)
{
	struct task_opt task_opt = default_task_opt;

	task_opt.priority = priority;

	return task_spawn_opt(task, arg, &task_opt);
}

void task_yield(void)
{
	task_low_yield();
}

void task_switch(void)
{
	assert(task_current != NULL);

	switch (task_current->state) {

	case TASK_STATE_WAITING:
		task_low_stack_save(task_current);
		break;

	case TASK_STATE_RUNNING:
		task_low_stack_save(task_current);

		task_current->state = TASK_STATE_RUNNABLE;
		task_scheduler_enqueue(task_current);
		break;

	case TASK_STATE_TERMINATED:
		/* Main task should never have a terminated state */
		assert(task_current != task_main);

		/* User did not supply stack - it was malloced - lets free it */
		if (task_current->opt.user_stack == NULL) {
			task_low_irq_disable();
			free(task_current->stack);
			task_low_irq_enable();
		}

		task_wait_queue_signal(&task_current->terminate_event);

		task_free_info(task_current);
		task_current = NULL;
		break;

	default:
		assert(false);

	}

	do {
		/* Choose new task */
		task_current = task_scheduler_dequeue();

		if (task_current == NULL)
			task_low_enter_low_power_mode();
	} while (task_current == NULL);

	assert(task_current->state == TASK_STATE_RUNNABLE);

	task_current->state = TASK_STATE_RUNNING;

	if (scheduler->preempt(scheduler_desc, task_current)) {
		task_low_preemption_enable();
	} else {
		task_low_preemption_disable();
	}

	task_low_stack_restore(task_current);
}

void task_run(struct task_info *task_info)
{
	assert(task_info != NULL);
	assert(task_info->state == TASK_STATE_RUNNING);
	assert(task_info == task_current);

	/* Execute task */
	task_info->task(task_info->arg);

	task_info->state = TASK_STATE_TERMINATED;

	task_yield();

	for (;;); /* RIP - task shall be removed in 'task_switch' */
}

static inline bool task_terminated_low(struct task_info *task_info, tid_t tid)
{
	return task_info->state == TASK_STATE_UNUSED || task_info->tid != tid;
}

bool task_terminated(tid_t tid)
{
	task_low_irq_disable();

	struct task_info *task_info = task_get_info(tid);

	bool terminated = task_terminated_low(task_info, tid);

	task_low_irq_enable();

	return terminated;
}

void task_join(tid_t tid)
{
	task_low_irq_disable();

	struct task_info *task_info = task_get_info(tid);

	assert(task_info != task_current);

	if (task_terminated_low(task_info, tid)) {
		/* Task has already finished */
		task_low_irq_enable();
	} else {
		/* Inherit disabled IRQs */
		task_wait_queue_wait(&task_info->terminate_event);
	}
}

void task_wait_queue_init(struct wait_queue *wait_queue)
{
	assert(wait_queue != NULL);

	wait_queue->signals = 0;
	wait_queue->list_head = NULL;
	wait_queue->list_tail = NULL;
}

void task_wait_queue_wait(struct wait_queue *wait_queue)
{
	task_low_irq_disable();

	if (wait_queue->signals > 0) {
		--wait_queue->signals;
		task_low_irq_enable();
		return;
	}

	assert(wait_queue != NULL);
	assert(task_current != NULL);

	task_current->state = TASK_STATE_WAITING;
	task_current->list_next = NULL;

	if (wait_queue->list_tail == NULL) {
		wait_queue->list_head = task_current;
	} else {
		wait_queue->list_tail->list_next = task_current;
	}
	wait_queue->list_tail = task_current;

	task_low_irq_enable();

	task_low_yield();
}

void task_wait_queue_signal(struct wait_queue *wait_queue)
{
	task_low_irq_disable();

	assert(wait_queue != NULL);

	struct task_info *waiting_task = wait_queue->list_head;

	if (waiting_task != NULL) {
		assert(waiting_task->state == TASK_STATE_WAITING);

		wait_queue->list_head = waiting_task->list_next;
		if (wait_queue->list_head == NULL)
			wait_queue->list_tail = NULL;

		waiting_task->state = TASK_STATE_RUNNABLE;
		waiting_task->list_next = NULL;

		task_scheduler_enqueue(waiting_task); /* Enables IRQ */
	} else {
		++wait_queue->signals;
		task_low_irq_enable();
	}
}
