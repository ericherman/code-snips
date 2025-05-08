#!/usr/bin/env python3

import sys
import random
import time
import multiprocessing


# Tests can intercept system call more easily if the code uses a level of
# indirection. The default case is simply a pass-through to the system call,
# but tests can replace these if needed.
class System_Context:

	def print(self, *args, **kwargs):
		print(*args, **kwargs)

	def randint(self, a, b):
		return random.randint(a, b)

	def sleep(self, secs):
		time.sleep(secs)

	def errfile(self):
		return sys.stderr


def do_task(context, task_id, threshold):
	sleep_val = context.randint(0, threshold)
	context.print(f"Task '{task_id}' may run for {sleep_val} seconds")
	# silly, but this aborts if equal to the highest number
	if sleep_val >= threshold:
		context.print(
			f"Task '{task_id}' is aborting",
			f" ({sleep_val} >= {threshold})",
			file=context.errfile())
		context.shared_dict[f"task-{task_id}"] = 1
		return
	context.sleep(sleep_val)
	context.shared_dict[f"task-{task_id}"] = 0


def usage(context, name):
	context.print(f"Usage: {name} NUMBER_OF_TASKS [THRESHOLD]")


def main(sys_argv=sys.argv, context=System_Context()):
	if len(sys_argv) < 2 or not sys_argv[1].isdigit() or int(
		sys_argv[1]) < 1:
		usage(context, sys_argv[0])
		return 1
	if len(sys_argv) == 3 and not sys_argv[2].isdigit():
		usage(context, sys_argv[0])
		return 1
	if len(sys_argv) > 3:
		usage(context, sys_argv[0])
		return 1

	num_tasks = int(sys_argv[1])
	if (len(sys_argv) >= 3):
		threshold = int(sys_argv[2])
	else:
		threshold = 3
	context.manager = multiprocessing.Manager()
	context.shared_dict = context.manager.dict()
	context.lock = multiprocessing.Lock()
	processes = []

	for task_id in range(1, 1 + num_tasks):
		p = multiprocessing.Process(
			target=do_task, args=(context, task_id, threshold))
		p.start()
		processes.append((task_id, p))

	for task_id, p in processes:
		p.join()
		# context.print(f"Task {task_id} ended")

	exit_code = 0
	for task_id in range(1, 1 + num_tasks):
		task_code = context.shared_dict.get(f"task-{task_id}", -1)
		context.print(f"Task {task_id} exited with: {task_code}")
		if task_code != 0:
			exit_code = 1

	return exit_code


if __name__ == "__main__":
	sys.exit(main())
