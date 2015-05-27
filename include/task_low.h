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

#ifndef _TASK_LOW_H_
#define _TASK_LOW_H_

#include "arch.h"

struct task_info;

void task_low_init(void);

void task_low_preemption_enable(void);
void task_low_preemption_disable(void);

void task_low_irq_enable(void);
void task_low_irq_disable(void);

void task_low_enter_low_power_mode(void);

void task_low_yield(void);

void task_low_stack_setup(struct task_info *task_info);
void task_low_stack_save(struct task_info *task_info);
void task_low_stack_restore(struct task_info *task_info);

void task_low_set_privilege_level(struct task_info *task_info);

void task_low_svcall(uint8_t svc_func_num, void *arg, void *res);

#endif /* _TASK_LOW_H_ */
