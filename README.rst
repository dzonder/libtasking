===================================
**libtasking** multitasking library
===================================

:Author: Michał Olech <michal.olech@gmail.com>
:License: MIT


The *libtasking* library adds support for multitasking - scheduling and
synchronization of tasks. Currently Cortex-M4 architecture is supported.


Prerequisites
-------------

In order for the library to work, the following functions should be set as
exception handlers respectively:

1. SysTick_Handler() - SysTick
2. PendSV_Handler() - PendSV
3. SVC_Handler() - SVCall

Also all device-specific interrupts should have their priority set to greater
or equal 2.

Additionally functions specified in ``include/arch.h`` file should be
implemented.


Important files and directories
-------------------------------

``include/task.h``
  Contains the main library interface (task management).

``include/task_conf.h``
  Contains available library configuration flags with their default values.

``include/mutex.h``
  Contains mutex interface.

``include/semaphore.h``
  Contains semaphore interface.

``sched/``
  Contains available scheduler implementations.


Compilation and examples
------------------------

Please refer to ``libtasking-testsuite`` for an example configuration of the
library, compilation flags and examples.
