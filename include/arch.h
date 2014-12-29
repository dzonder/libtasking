#ifndef _ARCH_H_
#define _ARCH_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/* The following functions should be set as interrupt handlers respectively:
      1. SysTick_Handler()
      2. PendSV_Handler()
   in order for the scheduling to work. */

/* These structures and routines are architecture
   specific and need to be implemented separately */

extern void assert(bool cond);

#endif /* _ARCH_H_ */
