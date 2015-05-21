	.syntax unified

	.section .text


	.global PendSV_Handler

	.thumb
	.thumb_func
	.type PendSV_Handler, %function
PendSV_Handler:
	PUSH	{lr}

	# First save where LR is into task_main->stack_top
	MRS	r0, MSP
	BL	task_low_exc_return_lr_addr_set

	# Clear PendSV pending bit
	# See http://embeddedgurus.com/state-space/2011/09/whats-the-state-of-your-cortex/
	BL	task_low_pendsv_clear

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


	.global SVC_Handler

	.thumb
	.thumb_func
	.type SVC_Handler, %function
SVC_Handler:
	PUSH	{lr}

	# Save service function pointer
	MOV	r3, r0

	# Prepare service function arguments
	MOV	r0, r1
	MOV	r1, r2

	# Call service function
	BLX	r3

	POP	{pc}
