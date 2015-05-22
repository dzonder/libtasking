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

#ifndef _SCHED_MULTI_QUEUE_H_
#define _SCHED_MULTI_QUEUE_H_

#include "sched_structs.h"

struct scheduler_setup {
	struct scheduler *scheduler;
	void *desc;
	void *conf;
};

struct multi_queue_conf {
	uint8_t max_priority;
	struct scheduler_setup *scheduler_setups;
};

extern struct scheduler multi_queue_scheduler;

extern void multi_queue_conf_init(struct multi_queue_conf *multi_queue_conf, uint8_t max_priority);

#endif /* _SCHED_MULTI_QUEUE_H_ */
