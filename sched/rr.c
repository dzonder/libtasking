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

/* Round Robin scheduler.

   Like FCFS but with preemption enabled. */

#include "sched/rr.h"

#include "sched/fcfs.h"

static void * rr_init(void *conf)
{
	void *desc = fcfs_scheduler.init(NULL);

	rr_scheduler.free = fcfs_scheduler.free;
	rr_scheduler.enqueue = fcfs_scheduler.enqueue;
	rr_scheduler.dequeue = fcfs_scheduler.dequeue;

	return desc;
}

static bool rr_preempt(void *desc, struct task_info *task_info)
{
	return true;
}

struct scheduler rr_scheduler = {
	.init		= rr_init,
	.free		= NULL,
	.enqueue	= NULL,
	.dequeue	= NULL,
	.preempt	= rr_preempt,
};
