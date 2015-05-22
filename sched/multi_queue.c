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

/* Multi-Queue scheduler.

  `struct multi_queue_conf` should be passed to init()
  in order to setup correct scheduler per priority. */

#include "sched/multi_queue.h"

#include "task_structs.h"

static struct multi_queue_conf *multi_queue_conf;

void multi_queue_conf_init(struct multi_queue_conf *multi_queue_conf, uint8_t max_priority)
{
	assert(multi_queue_conf != NULL);

	multi_queue_conf->max_priority = max_priority;
	multi_queue_conf->scheduler_setups = calloc(max_priority + 1, sizeof(*multi_queue_conf->scheduler_setups));
}

static void * multi_queue_init(void *conf)
{
	multi_queue_conf = (struct multi_queue_conf *)conf;

	assert(multi_queue_conf->scheduler_setups != NULL);

	for (uint8_t prio = 0; prio <= multi_queue_conf->max_priority; ++prio) {
		struct scheduler_setup *scheduler_setup = &multi_queue_conf->scheduler_setups[prio];
		assert(scheduler_setup->scheduler != NULL);
		scheduler_setup->desc = scheduler_setup->scheduler->init(scheduler_setup->conf);
	}

	return NULL;
}

static void multi_queue_free(void *desc)
{
	for (uint8_t prio = 0; prio <= multi_queue_conf->max_priority; ++prio) {
		struct scheduler_setup *scheduler_setup = &multi_queue_conf->scheduler_setups[prio];
		scheduler_setup->scheduler->free(scheduler_setup->desc);
	}
}

static void multi_queue_enqueue(void *desc, struct task_info *task_info)
{
	uint8_t prio = task_info->opt.priority;

	assert(prio <= multi_queue_conf->max_priority);

	struct scheduler_setup *scheduler_setup = &multi_queue_conf->scheduler_setups[prio];

	scheduler_setup->scheduler->enqueue(scheduler_setup->desc, task_info);
}

static struct task_info * multi_queue_dequeue(void *desc)
{
	struct task_info *task_info = NULL;

	uint8_t prio = multi_queue_conf->max_priority;
	do {
		struct scheduler_setup *scheduler_setup = &multi_queue_conf->scheduler_setups[prio];
		task_info = scheduler_setup->scheduler->dequeue(scheduler_setup->desc);
	} while (task_info == NULL && prio-- > 0);

	return task_info;
}

static bool multi_queue_preempt(void *desc, struct task_info *task_info)
{
	uint8_t prio = task_info->opt.priority;

	assert(prio <= multi_queue_conf->max_priority);

	struct scheduler_setup *scheduler_setup = &multi_queue_conf->scheduler_setups[prio];

	return scheduler_setup->scheduler->preempt(scheduler_setup->desc, task_info);
}

struct scheduler multi_queue_scheduler = {
	.init		= multi_queue_init,
	.free		= multi_queue_free,
	.enqueue	= multi_queue_enqueue,
	.dequeue	= multi_queue_dequeue,
	.preempt	= multi_queue_preempt,
};
