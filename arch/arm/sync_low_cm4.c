#include "sync_low.h"

#define ATOMIC(op) do {							\
	uint32_t v;							\
	uint32_t strex_failed = 1U;					\
	while (strex_failed) {						\
		__asm volatile("LDREX %0, [%1]\n\t"			\
			: "=r" (v)					\
			: "r" (value));					\
		(op);							\
		__asm volatile("STREX %0, %1, [%2]\n\t"			\
			: "=r" (strex_failed)				\
			: "r" (v), "r" (value));			\
	}								\
	return v;							\
} while (0)

uint32_t sync_low_atomic_inc(uint32_t *value)
{
	ATOMIC(++v);
}

uint32_t sync_low_atomic_dec(uint32_t *value)
{
	ATOMIC(--v);
}
