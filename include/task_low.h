#ifndef _TASK_LOW_H_
#define _TASK_LOW_H_

#include "arch.h"

struct task_info;

void task_low_init(void);

void task_low_preemption_enable(void);
void task_low_preemption_disable(void);

void task_low_yield(void);

void task_low_stack_setup(struct task_info *task_info);
void task_low_stack_save(struct task_info *task_info);
void task_low_stack_restore(struct task_info *task_info);

#endif /* _TASK_LOW_H_ */
