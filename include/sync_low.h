#ifndef _SYNC_LOW_H_
#define _SYNC_LOW_H_

#include "arch.h"

/* The atomic routines below return the new value which was stored in the
 * memory location. */
uint32_t sync_low_atomic_inc(volatile uint32_t *value);
uint32_t sync_low_atomic_dec(volatile uint32_t *value);

#endif /* _SYNC_LOW_H_ */
