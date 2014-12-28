#include "task_low.h"

uint32_t *task_low_msp;

void PendSV_Handler()
{
	task_low_msp = task_low_get_msp();

	task_switch();
}

void SysTick_Handler()
{
	task_low_msp = task_low_get_msp();

	task_switch();
}

void task_low_set_exc_return(uint32_t exc_return)
{
	*task_low_msp = exc_return;
}

void task_low_systick_enable(void)
{
	// TODO
}

void task_low_systick_disable(void)
{
	// TODO
}
