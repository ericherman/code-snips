#!/usr/bin/env python3

# yapf3 --in-place ./profile-1.py
# vim: set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab :

import pstats
import subprocess
import sys

cprofile = 'python3 -m cProfile -o output.prof'

if len(sys.argv) > 1:
	cmd = cprofile + ' ' + ' '.join(sys.argv[1:])
else:
	cmd = ' '.join([cprofile, './parallel.py 10 6'])

result = subprocess.run(cmd, shell=True)
if result.returncode:
	print(f"result.returncode: {result.returncode}")
	sys.exit(result.returncode)

pstats.Stats('output.prof').sort_stats('cumtime').print_stats(20)
