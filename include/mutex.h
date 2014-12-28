#ifndef _MUTEX_H_
#define _MUTEX_H_

#include <stdint.h>

typedef uint32_t mutex_t;

void mutex_init(mutex_t *mutex);

void mutex_lock(mutex_t *mutex);
void mutex_unlock(mutex_t *mutex);

#endif /* _MUTEX_H_ */
