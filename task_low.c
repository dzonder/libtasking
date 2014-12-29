#include "task_low.h"

extern void task_switch(void);
extern void task_run(void);

struct sw_stack_frame {
	uint32_t r4, r5, r6, r7, r8, r9, r10, r11;
};

struct hw_stack_frame {
	uint32_t r0, r1, r2, r3;
	uint32_t r12;
	uint32_t lr;
	uint32_t pc;
	uint32_t xpsr;
};

struct stack_frame {
	struct sw_stack_frame sw;
	struct hw_stack_frame hw;
};

static uint32_t *task_low_msp;

// TODO floating point registers?

__attribute__ ((always_inline))
static inline void task_low_context_save(void)
{
	uint32_t scratch;
	__asm volatile ("MRS %0, psp\n\t"
			"STMDB %0!, {r4-r11}\n\t"
			"MSR psp, %0\n\t" : "=r" (scratch));
}

__attribute__ ((always_inline))
static inline void task_low_context_restore(void)
{
	uint32_t scratch;
	__asm volatile ("MRS %0, psp\n\t"
			"LDMFD %0!, {r4-r11}\n\t"
			"MSR psp, %0\n\t" : "=r" (scratch));
}

void PendSV_Handler()
{
	/* We need to save MSP in order to set correct exception
	   return behaviour. See 'task_low_set_exc_return'. */
	task_low_msp = task_low_get_msp();

	/* No need to save/restore context for MSP.
	   Master stack is left intact by software */

	if (task_low_get_psp() != NULL)
		task_low_context_save();

	task_switch();

	if (task_low_get_psp() != NULL)
		task_low_context_restore();
}

void SysTick_Handler() {
	// TODO
}

void task_low_set_exc_return(uint32_t exc_return)
{
	/* Modify LR stack saved value in order to set the
	   correct stack pointer and mode during exception
	   return. */
	*(task_low_msp + 1) = exc_return;
}

void task_low_systick_irq_enable(void)
{
	// TODO
}

void task_low_systick_irq_disable(void)
{
	// TODO
}

void task_low_pendsv_trigger(void)
{
	SCB_ICSR |= SCB_ICSR_PENDSVSET_MASK;
}

void task_low_setup_stack(struct task_info *task_info)
{
	assert(task_info != NULL);
	assert(task_info->state == TASK_STATE_SPAWNED);

	/* We need room for at least two stack frames on the stack */
	assert(task_info->opt.stack_size >= 2 * sizeof(struct stack_frame));

	/* Stack starts here */
	task_info->stack_top = (void *)task_info->stack + task_info->opt.stack_size;

	struct stack_frame *frame = (struct stack_frame *)task_info->stack_top - 1;

	frame->hw.r0 = 0U;
	frame->hw.r1 = 0U;
	frame->hw.r2 = 0U;
	frame->hw.r3 = 0U;
	frame->hw.r12 = 0U;
	frame->hw.lr = 0U;
	frame->hw.pc = (uint32_t)task_run;
	frame->hw.xpsr = 0x01000000;

	frame->sw.r4 = 0U;
	frame->sw.r5 = 0U;
	frame->sw.r6 = 0U;
	frame->sw.r7 = 0U;
	frame->sw.r8 = 0U;
	frame->sw.r9 = 0U;
	frame->sw.r10 = 0U;
	frame->sw.r11 = 0U;

	/* Push frame onto the stack */
	task_info->stack_top = (uint32_t *)frame;

	assert(task_info->stack_top >= task_info->stack);
}
