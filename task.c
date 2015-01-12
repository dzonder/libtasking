#include "task.h"

#include "task_low.h"
#include "task_structs.h"

static struct task_info task_info_pool[TASK_MAX_TASKS];
static struct task_info *task_info_list_head_free;

struct task_info *task_main;
static struct task_info *task_current;

static struct scheduler *scheduler = NULL;

static struct task_opt default_task_opt = {
	.priority	= TASK_DEFAULT_PRIORITY,
	.stack_size	= TASK_DEFAULT_STACK_SIZE,
	.user_stack	= NULL,
};

static struct task_info *task_alloc_info(void)
{
	task_low_irq_disable();

	struct task_info *task_info = task_info_list_head_free;

	assert(task_info != NULL);
	assert(task_info->state == TASK_STATE_UNUSED);

	task_info_list_head_free = task_info->list_next;

	task_info->list_next = NULL;
	task_info->state = TASK_STATE_RUNNABLE;

	task_low_irq_enable();

	return task_info;
}

static void task_free_info(struct task_info *task_info)
{
	task_low_irq_disable();

	assert(task_info->state == TASK_STATE_TERMINATED);

	task_info->state = TASK_STATE_UNUSED;
	task_info->list_next = task_info_list_head_free;

	task_info_list_head_free = task_info;

	task_low_irq_enable();
}

static void task_scheduler_enqueue(struct task_info *task_info)
{
	task_low_irq_disable();

	assert(scheduler != NULL);
	scheduler->enqueue(task_info);

	task_low_irq_enable();
}

static struct task_info *task_scheduler_dequeue(void)
{
	task_low_irq_disable();

	assert(scheduler != NULL);
	struct task_info *task_info = scheduler->dequeue();

	task_low_irq_enable();

	return task_info;
}

void task_init(struct scheduler *_scheduler)
{
	scheduler = _scheduler;

	assert(scheduler != NULL);

	/* Initialize task_info pool as 'unused' list */
	for (uint16_t i = 0U; i < TASK_MAX_TASKS; ++i) {
		struct task_info *task_info = task_info_pool + i;

		task_info->state = TASK_STATE_UNUSED;
		task_info->list_next = task_info + 1;
	}
	task_info_pool[TASK_MAX_TASKS - 1].list_next = NULL;

	task_info_list_head_free = task_info_pool;

	if (scheduler->init != NULL)
		scheduler->init();

	assert(scheduler->dequeue != NULL);
	assert(scheduler->enqueue != NULL);

	task_main = task_alloc_info();

	task_main->task = NULL;
	task_main->arg = NULL;

	task_main->opt.priority = TASK_DEFAULT_PRIORITY;
	task_main->opt.stack_size = 0;
	task_main->opt.user_stack = NULL;

	task_main->state = TASK_STATE_RUNNING;

	task_current = task_main;

	/* This should be at the end */
	task_low_init();
}

void task_spawn_opt(task_t task, void *arg, struct task_opt *opt)
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
}

void task_spawn(task_t task, void *arg)
{
	task_spawn_opt(task, arg, &default_task_opt);
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

		task_free_info(task_current);
		task_current = NULL;
		break;

	default:
		assert(false);

	}

	/* Choose new task */
	task_current = task_scheduler_dequeue();

	// TODO: this could be NULL if all tasks are sleeping (enter low-power mode).
	assert(task_current != NULL);

	assert(task_current->state == TASK_STATE_RUNNABLE);

	task_current->state = TASK_STATE_RUNNING;

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

void task_wait(struct task_info **list_head_wait_queue,
		struct task_info **list_tail_wait_queue)
{
	task_low_irq_disable();

	assert(list_head_wait_queue != NULL);
	assert(list_tail_wait_queue != NULL);

	task_current->state = TASK_STATE_WAITING;
	task_current->list_next = NULL;

	if (*list_tail_wait_queue == NULL) {
		*list_head_wait_queue = task_current;
	} else {
		(*list_tail_wait_queue)->list_next = task_current;
	}
	*list_tail_wait_queue = task_current;

	task_low_irq_enable();

	task_yield();
}

void task_signal(struct task_info **list_head_wait_queue,
		struct task_info **list_tail_wait_queue)
{
	task_low_irq_disable();

	assert(list_head_wait_queue != NULL);
	assert(list_tail_wait_queue != NULL);

	struct task_info *waiting_task = *list_head_wait_queue;

	if (waiting_task != NULL) {
		assert(waiting_task->state == TASK_STATE_WAITING);

		*list_head_wait_queue = waiting_task->list_next;
		if (*list_head_wait_queue == NULL)
			*list_tail_wait_queue = NULL;

		waiting_task->state = TASK_STATE_RUNNABLE;
		waiting_task->list_next = NULL;
	}

	task_low_irq_enable();

	if (waiting_task != NULL)
		task_scheduler_enqueue(waiting_task);
}
