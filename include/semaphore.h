#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include <stdint.h>

struct semaphore;

struct semaphore * semaphore_init(int32_t value);
void semaphore_free(struct semaphore *sem);

void semaphore_wait(struct semaphore *sem);
void semaphore_post(struct semaphore *sem);

#endif /* _SEMAPHORE_H_ */
