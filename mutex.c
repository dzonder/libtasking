#include "mutex.h"

#include "arch.h"

struct mutex {
	int todo;
};

struct mutex * mutex_init(void)
{
	struct mutex *mutex = malloc(sizeof(*mutex));

	assert(mutex != NULL);

	return mutex;
}

void mutex_free(struct mutex *mutex)
{
	//assert(mutex->lock == false);

	free(mutex);
}

void mutex_lock(struct mutex *mutex)
{
}

void mutex_unlock(struct mutex *mutex)
{
}
