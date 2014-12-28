#include "task.h"

#include "arch.h"
#include "task_structs.h"

static struct task_info task_infos[TASK_LIMIT_SPAWNED];
static struct task_info *task_info_list_head_free;

static struct task_info *current;
static struct task_info *main_task;

static struct scheduler *scheduler = NULL;

static struct task_opt default_task_opt = {
	.priority	= TASK_DEFAULT_PRIORITY,
	.stack_size	= TASK_DEFAULT_STACK_SIZE,
	.stack		= NULL,
};

struct task_info *task_alloc_info(void)
{
	struct task_info *task_info = task_info_list_head_free;

	assert(task_info != NULL);
	assert(task_info->state == TASK_STATE_UNUSED);

	task_info_list_head_free = task_info->list_next;

	task_info->list_next = NULL;
	task_info->state = TASK_STATE_SPAWNED;

	return task_info;
}

void task_free_info(struct task_info *task_info)
{
	assert(task_info->state == TASK_STATE_TERMINATED);

	task_info->state = TASK_STATE_UNUSED;
	task_info->list_next = task_info_list_head_free;

	task_info_list_head_free = task_info;
}

void task_init(struct scheduler *_scheduler)
{
	scheduler = _scheduler;

	assert(scheduler != NULL);
	assert(scheduler->dequeue != NULL);
	assert(scheduler->enqueue != NULL);

	/* Initialize task_info pool as 'unused' list */
	for (uint16_t i = 0U; i < TASK_LIMIT_SPAWNED; ++i) {
		struct task_info *task_info = task_infos + i;

		task_info->state = TASK_STATE_UNUSED;
		task_info->list_next = task_info + 1;
	}
	task_infos[TASK_LIMIT_SPAWNED - 1].list_next = NULL;

	task_info_list_head_free = task_infos;

	if (scheduler->init != NULL)
		scheduler->init();

	main_task = task_alloc_info();

	main_task->task = NULL;
	main_task->arg = NULL;

	main_task->opt.priority = TASK_DEFAULT_PRIORITY;
	main_task->opt.stack_size = 0;
	main_task->opt.stack = NULL;

	main_task->state = TASK_STATE_RUNNING;
}

void task_spawn_opt(task_t task, void *arg, struct task_opt *opt)
{
	task_low_systick_disable();

	struct task_info *task_info = task_alloc_info();

	task_info->task = task;
	task_info->arg = arg;
	task_info->opt = *opt;

	if (task_info->opt.stack == NULL) {
		/* User did not provide a stack */
		task_info->opt.stack = malloc(task_info->opt.stack_size);
		assert(task_info->opt.stack != NULL);
	}

	assert(scheduler != NULL);

	scheduler->enqueue(task_info);

	task_low_systick_enable();
}

void task_spawn(task_t task, void *arg)
{
	task_spawn_opt(task, arg, &default_task_opt);
}

void task_yield(void)
{
	assert(false); // TODO PendSV
}

void task_switch(void)
{
	assert(current->state == TASK_STATE_RUNNING);

	current->state = TASK_STATE_SPAWNED;

	if (current != main_task)
		current->stack_top = task_low_get_psp();

	assert(scheduler != NULL);

	/* Choose new task */
	current = scheduler->dequeue();

	assert(current != NULL);
	assert(current->state == TASK_STATE_SPAWNED);

	current->state = TASK_STATE_RUNNING;

	if (current == main_task) {
		task_low_set_exc_return(EXC_RETURN_PSP);
		task_low_set_psp(current->stack_top);
	} else {
		task_low_set_exc_return(EXC_RETURN_MSP);
	}
}

void task_run(struct task_info *task_info)
{
	assert(task_info != NULL);
	assert(task_info->state == TASK_STATE_SLEEPING);

	task_info->state = TASK_STATE_RUNNING;
}
