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
