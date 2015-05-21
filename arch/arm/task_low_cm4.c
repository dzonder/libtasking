#include "task_low.h"

#include "task_structs.h"

#if (__FPU_PRESENT == 1)
#include "ARMCM4_FP.h"
#else
#include "ARMCM4.h"
#endif

#include "core_cm4.h"

#define EXC_RETURN_MAIN		(uint32_t)(0xFFFFFFF1)
#define EXC_RETURN_MSP		(uint32_t)(0xFFFFFFF9)
#define EXC_RETURN_PSP		(uint32_t)(0xFFFFFFFD)

#define IS_MAIN_TASK(task)	((task) == task_main)

struct sw_fp_registers {
	uint32_t s16_r31[16];
};

struct hw_fp_registers {
	uint32_t s0_r15[16];
	uint32_t fpscr;
};

struct sw_stack_frame {
#if (__FPU_PRESENT == 1)
	struct sw_fp_registers fp;
#endif
	uint32_t r4_r11[8];
};

struct hw_stack_frame {
	uint32_t r0, r1, r2, r3;
	uint32_t r12;
	uint32_t lr;
	uint32_t pc;
	uint32_t xpsr;
#if (__FPU_PRESENT == 1)
	struct hw_fp_registers fp;
#endif
};

struct stack_frame {
	struct sw_stack_frame sw;
	struct hw_stack_frame hw;
};

/* This is used to SW preserve main task registers (we can not
   use MSP stack because it is used by exception handlers). */
struct sw_stack_frame task_main_sw_stack_frame;

__attribute__ ((always_inline))
static inline void task_low_get_psp(uint32_t **stack)
{
	__asm volatile ("MRS %0, psp\n\t" : "=r" (*stack));
}

__attribute__ ((always_inline))
static inline void task_low_set_psp(uint32_t *stack)
{
	__asm volatile ("MSR psp, %0\n\t" : : "r" (stack));
}

/* Note: Lazy Stacking - if FP is not used in the exception handler then
 * hardware-preserved registers will not be saved/restored. */

static void task_low_context_save_fp(bool fp_used)
{
#if (__FPU_PRESENT == 1)
	uint32_t *stack;

	task_low_get_psp(&stack);

	void *stack_new = (void *)stack - sizeof(struct sw_fp_registers);

	if (fp_used) {
		__asm ("MOV r0, %0\n\t"
			"VSTM r0, {s16-s31}\n\t"
			:
			: "r" (stack_new)
			: "r0");
	}

	task_low_set_psp(stack_new);
#endif
}

static void task_low_context_restore_fp(bool fp_used)
{
#if (__FPU_PRESENT == 1)
	uint32_t *stack;

	task_low_get_psp(&stack);

	if (fp_used) {
		__asm ("MOV r0, %0\n\t"
			"VLDM r0, {s16-s31}\n\t"
			:
			: "r" (stack)
			: "r0", "r1");
	}

	task_low_set_psp((void *)stack + sizeof(struct sw_fp_registers));
#endif
}

void task_low_exc_return_lr_addr_set(uint32_t *stack)
{
	task_main->stack = stack;
}

static inline uint32_t * task_low_exc_return_lr_addr_get(void)
{
	return task_main->stack;
}

static inline uint8_t task_low_exc_return_fp_used(void)
{
	return (*task_low_exc_return_lr_addr_get() & (1U << 4U)) == 0U;
}

static inline void task_low_exc_return_set(uint32_t exc_return, uint8_t fp_used)
{
	/* Set FP bit according to current value */
	exc_return &= ~(fp_used << 4U);

	/* Modify LR stack saved value in order to set the
	   correct stack pointer and mode during exception
	   return. */
	*task_low_exc_return_lr_addr_get() = exc_return;
}

static void task_low_preemption_init(void)
{
	uint32_t ticks = 120 * TASK_PREEMPTION_TIMESLICE_US;

	assert((ticks - 1) <= SysTick_LOAD_RELOAD_Msk);

	SysTick->LOAD = ticks - 1;
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
}

void task_low_preemption_enable(void)
{
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
}

void task_low_preemption_disable(void)
{
	SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
}

void task_low_irq_enable(void)
{
	__enable_irq();
}

void task_low_irq_disable(void)
{
	__disable_irq();
}

void task_low_enter_low_power_mode(void)
{
	__asm volatile ("WFI\n\t");
}

void task_low_init(void)
{
	task_low_set_psp((uint32_t *)(&task_main_sw_stack_frame + 1));

	/* Set PendSV priority to low-urgency */
	NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);

	task_low_preemption_init();
}

void task_low_yield(void)
{
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

void task_low_pendsv_clear(void)
{
	SCB->ICSR |= SCB_ICSR_PENDSVCLR_Msk;
}

void task_low_stack_setup(struct task_info *task_info)
{
	assert(task_info != NULL);
	assert(task_info->state == TASK_STATE_RUNNABLE);

	/* We need room for at least two stack frames on the stack */
	assert(task_info->opt.stack_size >= 2 * sizeof(struct stack_frame));

	/* Stack starts here */
	task_info->stack_top = (void *)task_info->stack + task_info->opt.stack_size;

	struct stack_frame *frame = (struct stack_frame *)task_info->stack_top - 1;

	/* Initialize hardware-saved fp registers */
	memset(&frame->hw.fp, 0, sizeof(frame->hw.fp));

	frame->hw.r0 = (uint32_t)task_info;
	frame->hw.r1 = 0U;
	frame->hw.r2 = 0U;
	frame->hw.r3 = 0U;
	frame->hw.r12 = 0U;
	frame->hw.lr = 0U;
	frame->hw.pc = (uint32_t)task_run;
	frame->hw.xpsr = 0x01000000;

	/* Initialize software-saved registers */
	memset(&frame->sw, 0, sizeof(frame->sw));

	/* Push frame onto the stack */
	task_info->stack_top = (uint32_t *)frame;

	assert(task_info->stack_top >= task_info->stack);
}

void task_low_stack_save(struct task_info *task_info)
{
	assert(task_info->state == TASK_STATE_RUNNING || task_info->state == TASK_STATE_WAITING);

	task_info->fp_used = task_low_exc_return_fp_used();

	task_low_context_save_fp(task_info->fp_used);

	task_low_get_psp(&task_info->stack_top);

	if (!IS_MAIN_TASK(task_info)) {
		/* Stack overflow checking */
		assert(task_info->stack_top >= task_info->stack);
	}
}

void task_low_stack_restore(struct task_info *task_info)
{
	assert(task_info->state == TASK_STATE_RUNNING);

	uint32_t exc_return = IS_MAIN_TASK(task_info)
				? EXC_RETURN_MSP
				: EXC_RETURN_PSP;

	task_low_exc_return_set(exc_return, task_info->fp_used);

	task_low_set_psp(task_info->stack_top);

	task_low_context_restore_fp(task_info->fp_used);
}

void SysTick_Handler(void)
{
	task_low_yield();
}
