#ifndef _ARCH_H_
#define _ARCH_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/* These structures and routines are architecture
 * specific and need to be implemented separately */

#include "MK22F12.h"

extern void assert(bool cond);

#endif /* _ARCH_H_ */
