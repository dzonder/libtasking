#ifndef _TASK_LOW_H_
#define _TASK_LOW_H_

#include "arch.h"
#include "task_structs.h"

#define EXC_RETURN_MAIN		(uint32_t)(0xFFFFFFF1)
#define EXC_RETURN_MSP		(uint32_t)(0xFFFFFFF9)
#define EXC_RETURN_PSP		(uint32_t)(0xFFFFFFFD)

#define EXC_RETURN_FP_MAIN	(uint32_t)(0xFFFFFFE1)
#define EXC_RETURN_FP_MSP	(uint32_t)(0xFFFFFFE9)
#define EXC_RETURN_FP_PSP	(uint32_t)(0xFFFFFFED)

void task_low_set_exc_return(uint32_t exc_return);

void task_low_systick_irq_enable(void);
void task_low_systick_irq_disable(void);

void task_low_pendsv_trigger(void);

void task_low_setup_stack(struct task_info *task_info);

static inline uint32_t * task_low_get_msp(void)
{
	uint32_t *stack = NULL;

	__asm volatile ("MRS %0, msp\n\t" : "=r" (stack));

	return stack;
}

static inline uint32_t * task_low_get_psp(void)
{
	uint32_t *stack = NULL;

	__asm volatile ("MRS %0, psp\n\t" : "=r" (stack));

	return stack;
}

static inline void task_low_set_msp(uint32_t *stack)
{
	__asm volatile ("MSR msp, %0\n\t" : : "r" (stack));
}

static inline void task_low_set_psp(uint32_t *stack)
{
	__asm volatile ("MSR psp, %0\n\t" : : "r" (stack));
}

#endif /* _TASK_LOW_H_ */
