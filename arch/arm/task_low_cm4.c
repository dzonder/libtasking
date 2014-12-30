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

#define IS_MAIN_TASK(task)	((task) == task_main)

struct sw_stack_frame {
//#if (__FPU_PRESENT == 1)
//	uint32_t s16, s17, s18, s19, s20, s21, s22, s23;
//	uint32_t s24, s25, s26, s27, s28, s29, s30, s31;
//#endif
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

/* This is used to SW preserve main task registers (we can not
   use MSP stack because it is used by exception handlers). */
struct sw_stack_frame task_main_sw_stack_frame;

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

static inline void task_low_get_msp(uint32_t **stack)
{
	__asm volatile ("MRS %0, msp\n\t" : "=r" (*stack));
}

static inline void task_low_get_psp(uint32_t **stack)
{
	__asm volatile ("MRS %0, psp\n\t" : "=r" (*stack));
}

static inline void task_low_set_msp(uint32_t *stack)
{
	__asm volatile ("MSR msp, %0\n\t" : : "r" (stack));
}

static inline void task_low_set_psp(uint32_t *stack)
{
	__asm volatile ("MSR psp, %0\n\t" : : "r" (stack));
}

static inline uint32_t * task_low_exc_return_addr(void)
{
	return task_main->stack_top + 1;
}

static uint8_t task_low_exc_return_fp_used(void)
{
	return (*task_low_exc_return_addr() & (1U << 4U)) == 0U;
}

static void task_low_exc_return_set(uint32_t exc_return, uint8_t fp_used)
{
	/* Set FP bit according to current value */
	exc_return &= ~(fp_used << 4U);

	/* Modify LR stack saved value in order to set the
	   correct stack pointer and mode during exception
	   return. */
	*task_low_exc_return_addr() = exc_return;
}

void task_low_init(void)
{
	task_low_set_psp((uint32_t *)(&task_main_sw_stack_frame + 1));
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
	assert(task_info->state == TASK_STATE_RUNNING);

	if (!IS_MAIN_TASK(task_info)) {
		task_low_get_psp(&task_info->stack_top);

		/* Stack overflow checking */
		assert(task_info->stack_top >= task_info->stack);
	}

	task_info->fp_used = task_low_exc_return_fp_used();
}

void task_low_stack_restore(struct task_info *task_info)
{
	assert(task_info->state == TASK_STATE_RUNNING);

	if (IS_MAIN_TASK(task_info)) {
		task_low_exc_return_set(EXC_RETURN_MSP, task_info->fp_used);
		task_low_set_msp(task_info->stack_top);

		/* Here we use a trick to save SW preserved registers.
		   We use PSP for this which holds a special allocated
		   memory location for MSP SW preserved context. */
		task_low_set_psp((uint32_t *)(&task_main_sw_stack_frame + 1));
	} else {
		task_low_exc_return_set(EXC_RETURN_PSP, task_info->fp_used);
		task_low_set_psp(task_info->stack_top);
	}
}

void PendSV_Handler()
{
	/* We need to save address of return value on the MSP stack
	   in order to set correct exception return behaviour.
	   See 'task_low_set_exc_return'. */
	task_low_get_msp(&task_main->stack_top);

	task_low_context_save();

	/* No need to save/restore context for MSP.
	   Master stack is left intact by software */

	/* Trigger hardware FP context save (see Lazy Stacking).
	   Lazy stacking could also be disabled - but it would
	   influence all exception handlers and we care only about
	   scheduling related exceptions. */
	__asm ("VMOV.F32 s1, s1\n\t");

	task_switch();

	task_low_context_restore();
}

void SysTick_Handler() {
	// TODO
}
