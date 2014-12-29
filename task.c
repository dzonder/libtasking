#include "task.h"

#include "task_low.h"
#include "task_structs.h"

static struct task_info task_infos[TASK_MAX_TASKS];
static struct task_info *task_info_list_head_free;

static struct task_info *task_current;
static struct task_info *task_main;

static struct scheduler *scheduler = NULL;

static struct task_opt default_task_opt = {
	.priority	= TASK_DEFAULT_PRIORITY,
	.stack_size	= TASK_DEFAULT_STACK_SIZE,
	.user_stack	= NULL,
};

static struct task_info *task_alloc_info(void)
{
	struct task_info *task_info = task_info_list_head_free;

	assert(task_info != NULL);
	assert(task_info->state == TASK_STATE_UNUSED);

	task_info_list_head_free = task_info->list_next;

	task_info->list_next = NULL;
	task_info->state = TASK_STATE_SPAWNED;

	return task_info;
}

void task_yield(void)
{
	/* Just trigger PendSV */
	task_low_pendsv_trigger();
}

static void task_free_info(struct task_info *task_info)
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
	for (uint16_t i = 0U; i < TASK_MAX_TASKS; ++i) {
		struct task_info *task_info = task_infos + i;

		task_info->state = TASK_STATE_UNUSED;
		task_info->list_next = task_info + 1;
	}
	task_infos[TASK_MAX_TASKS - 1].list_next = NULL;

	task_info_list_head_free = task_infos;

	if (scheduler->init != NULL)
		scheduler->init();

	task_main = task_alloc_info();

	task_main->task = NULL;
	task_main->arg = NULL;

	task_main->opt.priority = TASK_DEFAULT_PRIORITY;
	task_main->opt.stack_size = 0;
	task_main->opt.user_stack = NULL;

	task_main->state = TASK_STATE_RUNNING;

	task_current = task_main;
}

void task_spawn_opt(task_t task, void *arg, struct task_opt *opt)
{
	task_low_systick_irq_disable();

	struct task_info *task_info = task_alloc_info();

	task_info->task = task;
	task_info->arg = arg;
	task_info->opt = *opt;

	if (task_info->opt.user_stack == NULL) {
		/* User did not provide a stack */
		task_info->stack = malloc(task_info->opt.stack_size);
	} else {
		task_info->stack = task_info->opt.user_stack;
	}

	assert(task_info->stack != NULL);

	task_low_setup_stack(task_info);

	assert(scheduler != NULL);

	scheduler->enqueue(task_info);

	task_low_systick_irq_enable();
}

void task_spawn(task_t task, void *arg)
{
	task_spawn_opt(task, arg, &default_task_opt);
}

void task_switch(void)
{
	assert(scheduler != NULL);
	assert(task_current != NULL);

	switch (task_current->state) {

	case TASK_STATE_RUNNING:
		task_current->state = TASK_STATE_SPAWNED;

		/* Save top of the stack */
		if (task_current == task_main) {
			task_current->stack_top = task_low_get_msp();
		} else {
			task_current->stack_top = task_low_get_psp();

			/* Stack overflow checking */
			assert(task_current->stack_top >= task_current->stack);
		}

		scheduler->enqueue(task_current);

		break;

	case TASK_STATE_TERMINATED:
		/* Main task should never have a terminated state */
		assert(task_current != task_main);

		/* User did not supply stack - is was mallocd - lets free it */
		if (task_current->opt.user_stack == NULL)
			free(task_current->stack);

		task_free_info(task_current);
		task_current = NULL;

		break;

	default:
		assert(false);

	}

	/* Choose new task */
	task_current = scheduler->dequeue();

	assert(task_current != NULL);
	assert(task_current->state == TASK_STATE_SPAWNED);

	task_current->state = TASK_STATE_RUNNING;

	if (task_current == task_main) {
		task_low_set_exc_return(EXC_RETURN_MSP);
		task_low_set_msp(task_current->stack_top);
		task_low_set_psp(NULL);
	} else {
		task_low_set_exc_return(EXC_RETURN_PSP);
		task_low_set_psp(task_current->stack_top);
	}
}

void task_run(void)
{
	assert(task_current != NULL);
	assert(task_current->state == TASK_STATE_RUNNING);

	/* Execute task */
	task_current->task(task_current->arg);

	task_current->state = TASK_STATE_TERMINATED;

	task_yield();

	for (;;); /* RIP - task shall be removed in 'task_switch' */
}
