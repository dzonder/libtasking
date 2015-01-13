#ifndef _SCHED_STRUCTS_H_
#define _SCHED_STRUCTS_H_

struct task_info;

struct scheduler {
	void (*init)(void *user_data);
	void (*enqueue)(struct task_info *);
	struct task_info * (*dequeue)(void);
};

#endif /* _SCHED_STRUCTS_H_ */
