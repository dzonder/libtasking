#ifndef _TASK_LOW_H_
#define _TASK_LOW_H_

#define EXC_RETURN_MAIN		(uint32_t)(0xFFFFFFF1)
#define EXC_RETURN_MSP		(uint32_t)(0xFFFFFFF9)
#define EXC_RETURN_PSP		(uint32_t)(0xFFFFFFFD)

#define EXC_RETURN_FP_MAIN	(uint32_t)(0xFFFFFFE1)
#define EXC_RETURN_FP_MSP	(uint32_t)(0xFFFFFFE9)
#define EXC_RETURN_FP_PSP	(uint32_t)(0xFFFFFFED)

void task_low_set_exc_return(uint32_t exc_return);

void task_low_systick_enable(void);
void task_low_systick_disable(void);

static inline uint32_t * task_low_get_msp(void)
{
}

static inline uint32_t * task_low_get_psp(void)
{
}

static inline void task_low_set_psp(uint32_t *stack)
{
}

#endif /* _TASK_LOW_H_ */
