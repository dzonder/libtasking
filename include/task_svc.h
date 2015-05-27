/*
Copyright (c) 2015 Michal Olech.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef _TASK_SVC_H_
#define _TASK_SVC_H_

#define TASK_SVC_SPAWN_OPT		0
#define TASK_SVC_YIELD			1
#define TASK_SVC_TERMINATED		2
#define TASK_SVC_JOIN			3
#define TASK_SVC_WAIT_QUEUE_WAIT	4
#define TASK_SVC_WAIT_QUEUE_SIGNAL	5
#define TASK_SVC_UNDEFINED		6

typedef void (*svc_func_t)(void *arg, void *res);

extern svc_func_t svc_func_ptrs[];

#endif /* _TASK_SVC_H_ */
