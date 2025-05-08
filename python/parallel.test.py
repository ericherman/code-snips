#!/usr/bin/env python3

import sys
import io
import os
import re
import subprocess
import time

parallel = __import__("parallel")


class Test_Context:

	def __init__(self, randomints):
		self.testout = io.StringIO()
		self.randomints = randomints

	def print(self, *args, **kwargs):
		if 'file' not in kwargs:
			kwargs['file'] = self.testout
		print(*args, **kwargs)

	def randint(self, a, b):
		with self.lock:
			count = self.shared_dict.get('rndint_count', 0)
			self.shared_dict['rndint_count'] = count + 1

		assert count < len(self.randomints)
		val = self.randomints[count]
		assert val >= a
		assert val <= b
		return val

	def sleep(self, secs):
		return

	def errfile(self):
		return self.testout


def test_parallel_success():
	print('test_parallel_success ... ', end='')
	fail = 0
	tasks = '10'
	max_val = '5'
	argv = ['parallel.py', tasks, max_val]
	randomints = [0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4]
	ctx = Test_Context(randomints)
	rv = parallel.main(argv, ctx)
	if rv != 0:
		print(f"expected exit code 0, but was {rv}\n")
		fail = 1
	for taskid in range(1, 11):
		expected = f'Task {taskid} exited with: 0'
		out = ctx.testout.getvalue()
		if not expected in out:
			print(f"expected: {expected}\n  not in:\n{out}\n")
			fail = 1
	if fail:
		print('FAIL')
	else:
		print('OK')
	return fail


def test_parallel_fail():
	print('test_parallel_fail ... ', end='')
	fail = 0

	max_val = '10'
	randomints = [1, int(max_val), 2, 3, 0, 1, 2, 3, int(max_val), 0]
	ctx = Test_Context(randomints)
	tasks = '10'
	argv = ['parallel.py', tasks, max_val]
	rv = parallel.main(argv, ctx)
	if rv == 0:
		print(f"expected error code, but was success")
		fail = 1

	out = ctx.testout.getvalue()
	successes = out.count('exited with: 0')
	if successes != 8:
		print(f"expected two success, but was {successes}\n{out}")
		fail = 1
	failures = out.count('exited with: 1')
	if failures != 2:
		print(f"expected two failures, but was {failures}\n{out}")
		fail = 1

	if fail:
		print('FAIL')
	else:
		print('OK')
	return fail


def test_parallel_main():
	fails = test_parallel_success()
	fails += test_parallel_fail()
	if (fails):
		return 1
	return 0


if __name__ == "__main__":
	sys.exit(test_parallel_main())
