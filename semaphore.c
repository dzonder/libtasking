/* Current implementation uses two mutexes. Based on the following algorithm:
   http://www.csee.wvu.edu/~jdm/classes/cs550/notes/tech/mutex/semimp3.html */

#include "semaphore.h"

#include "arch.h"
#include "mutex.h"

struct semaphore {
	int32_t value;

	struct mutex *mutex_access;
	struct mutex *mutex_delay;
};

struct semaphore * semaphore_init(int32_t value)
{
	struct semaphore *sem = malloc(sizeof(*sem));

	assert(sem != NULL);
	assert(value > 0);

	sem->value = value;

	sem->mutex_access = mutex_init();
	sem->mutex_delay = mutex_init();

	mutex_lock(sem->mutex_delay);

	return sem;
}

void semaphore_free(struct semaphore *sem)
{
	//assert(sem->value == _initial_sem_value_);

	mutex_unlock(sem->mutex_delay);

	mutex_free(sem->mutex_access);
	mutex_free(sem->mutex_delay);

	free(sem);
}

void semaphore_wait(struct semaphore *sem)
{
	mutex_lock(sem->mutex_access);

	if (--sem->value < 0) {
		mutex_unlock(sem->mutex_access);
		mutex_lock(sem->mutex_delay);
	} else {
		mutex_unlock(sem->mutex_access);
	}
}

void semaphore_post(struct semaphore *sem)
{
	mutex_lock(sem->mutex_access);

	if (++sem->value < 0)
		mutex_unlock(sem->mutex_delay);

	mutex_unlock(sem->mutex_access);
}
