	.syntax unified

	.section .text

	.global PendSV_Handler
	.global SysTick_Handler

	.thumb
	.thumb_func
	.type PendSV_Handler, %function
	.type SysTick_Handler, %function
PendSV_Handler:
SysTick_Handler:
	PUSH	{lr}

	# First save where LR is into task_main->stack_top
	MRS	r0, MSP
	BL	task_low_exc_return_lr_addr_set

	# Now save the context (for regs that are software-saved) at PSP
	MRS	r0, PSP
	STMDB	r0!, {r4-r11}
	MSR	PSP, r0

	# Switch current task
	BL	task_switch

	# Finally restore the context from PSP
	MRS	r0, PSP
	LDMFD	r0!, {r4-r11}
	MSR	PSP, r0

	# Exit exception handler
	POP	{pc}
