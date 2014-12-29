#ifndef _MUTEX_H_
#define _MUTEX_H_

struct mutex;

struct mutex * mutex_init(void);
void mutex_free(struct mutex *mutex);

void mutex_lock(struct mutex *mutex);
void mutex_unlock(struct mutex *mutex);

#endif /* _MUTEX_H_ */
