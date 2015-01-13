#ifndef _SCHED_MULTI_QUEUE_H_
#define _SCHED_MULTI_QUEUE_H_

#include "sched_structs.h"

struct scheduler_setup {
	struct scheduler *scheduler;
	void *desc;
	void *user_data;
};

struct multi_queue_conf {
	uint8_t max_priority;
	struct scheduler_setup *scheduler_setups;
};

extern struct scheduler multi_queue_scheduler;

extern void multi_queue_conf_init(struct multi_queue_conf *multi_queue_conf, uint8_t max_priority);

#endif /* _SCHED_MULTI_QUEUE_H_ */
