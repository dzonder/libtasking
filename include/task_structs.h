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

#ifndef _TASK_STRUCTS_H_
#define _TASK_STRUCTS_H_

#include "task.h"

enum task_state {
	TASK_STATE_UNUSED,
	TASK_STATE_RUNNABLE,
	TASK_STATE_WAITING,
	TASK_STATE_RUNNING,
	TASK_STATE_TERMINATED,
};

struct task_info {
	enum task_state state;

	tid_t tid;
	struct wait_queue terminate_event;

	/* Options set on spawning */
	struct task_opt opt;

	uint32_t *stack;	/* This is stack limiting address.
				   As supplied by user or allocated. */
	uint32_t *stack_top;	/* Last known top of the stack
				   (valid for non-running tasks). */

	uint8_t fp_used;

	struct task_info *list_next;
};

extern struct task_info *task_main;

extern void task_switch(void);
extern void task_run(struct task_info *task_info);

#endif /* _TASK_STRUCTS_H_ */
