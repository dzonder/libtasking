#include "mutex.h"

#include "arch.h"
#include "semaphore.h"

struct mutex {
	struct semaphore *binsem;
};

struct mutex * mutex_init(void)
{
	struct mutex *mutex = malloc(sizeof(*mutex));

	assert(mutex != NULL);

	mutex->binsem = semaphore_init(1);

	return mutex;
}

void mutex_free(struct mutex *mutex)
{
	semaphore_free(mutex->binsem);

	free(mutex);
}

void mutex_lock(struct mutex *mutex)
{
	semaphore_wait(mutex->binsem);
}

void mutex_unlock(struct mutex *mutex)
{
	semaphore_post(mutex->binsem);
}
