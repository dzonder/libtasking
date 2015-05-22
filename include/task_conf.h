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

#ifndef _TASK_CONF_H_
#define _TASK_CONF_H_

#ifndef TASK_PREEMPTION_TIMESLICE_US
#define TASK_PREEMPTION_TIMESLICE_US	(10 * 1000) /* 10 ms */
#endif

#ifndef TASK_MAX_TASKS
#define TASK_MAX_TASKS			16U
#endif

#ifndef TASK_DEFAULT_PRIORITY
#define TASK_DEFAULT_PRIORITY		0U
#endif

#ifndef TASK_DEFAULT_STACK_SIZE
#define TASK_DEFAULT_STACK_SIZE		512U
#endif

#endif /* _TASK_CONF_H_ */
