#ifndef _SCHED_STRUCTS_H_
#define _SCHED_STRUCTS_H_

struct scheduler {
	void (*init)(void);
	void (*schedule)(void);
};

#endif /* _SCHED_STRUCTS_H_ */
