#ifndef _TASK_SVC_H_
#define _TASK_SVC_H_

void task_svc_spawn_opt(void *arg, void *res);
void task_svc_yield(void *arg, void *res);
void task_svc_terminated(void *arg, void *res);
void task_svc_join(void *arg, void *res);
void task_svc_wait_queue_wait(void *arg, void *res);
void task_svc_wait_queue_signal(void *arg, void *res);

#endif /* _TASK_SVC_H_ */
