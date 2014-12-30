#include "task_low.h"

#include "task_structs.h"

#ifdef __FPU_PRESENT
#include "ARMCM4_FP.h"
#else
#include "ARMCM4.h"
#endif

#include "core_cm4.h"

#define EXC_RETURN_MAIN		(uint32_t)(0xFFFFFFF1)
#define EXC_RETURN_MSP		(uint32_t)(0xFFFFFFF9)
#define EXC_RETURN_PSP		(uint32_t)(0xFFFFFFFD)

#define EXC_RETURN_FP_MAIN	(uint32_t)(0xFFFFFFE1)
#define EXC_RETURN_FP_MSP	(uint32_t)(0xFFFFFFE9)
#define EXC_RETURN_FP_PSP	(uint32_t)(0xFFFFFFED)

#define IS_MAIN_TASK(task)	((task) == task_main)

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

void task_low_set_exc_return(uint32_t exc_return)
{
	/* Modify LR stack saved value in order to set the
	   correct stack pointer and mode during exception
	   return. */
	*(task_main->stack_top + 1) = exc_return;
}

void task_low_preemption_enable(void)
{
	// TODO
}

void task_low_preemption_disable(void)
{
	// TODO
}

void task_low_yield(void)
{
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

void task_low_stack_setup(struct task_info *task_info)
{
	assert(task_info != NULL);
	assert(task_info->state == TASK_STATE_SPAWNED);

	/* We need room for at least two stack frames on the stack */
	assert(task_info->opt.stack_size >= 2 * sizeof(struct stack_frame));

	/* Stack starts here */
	task_info->stack_top = (void *)task_info->stack + task_info->opt.stack_size;

	struct stack_frame *frame = (struct stack_frame *)task_info->stack_top - 1;

	frame->hw.r0 = (uint32_t)task_info;
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

void task_low_stack_save(struct task_info *task_info)
{
	if (!IS_MAIN_TASK(task_info)) {
		task_info->stack_top = task_low_get_psp();

		/* Stack overflow checking */
		assert(task_info->stack_top >= task_info->stack);
	}
}

void task_low_stack_restore(struct task_info *task_info)
{
	if (IS_MAIN_TASK(task_info)) {
		task_low_set_exc_return(EXC_RETURN_MSP);
		task_low_set_msp(task_info->stack_top);
		task_low_set_psp(NULL);
	} else {
		task_low_set_exc_return(EXC_RETURN_PSP);
		task_low_set_psp(task_info->stack_top);
	}
}

void PendSV_Handler()
{
	/* We need to save address of return value on the MSP stack
	   in order to set correct exception return behaviour.
	   See 'task_low_set_exc_return'. */
	task_main->stack_top = task_low_get_msp();

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
