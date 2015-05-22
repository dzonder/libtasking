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

uint32_t sync_low_atomic_inc(volatile uint32_t *value)
{
	ATOMIC(++v);
}

uint32_t sync_low_atomic_dec(volatile uint32_t *value)
{
	ATOMIC(--v);
}
