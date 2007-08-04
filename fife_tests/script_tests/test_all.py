#!/usr/bin/python

from __init__ import *

def run_and_get_results():
	import os
	
	os.chdir(genpath('fife_tests/script_tests'))
	pythonfilenames = [p for p in os.listdir(os.getcwd()) if len(p) > 3 and p[-3:] == '.py']
	testmodulenames = [p[:-3] for p in pythonfilenames if p not in ('__init__.py', 'test_all.py')]
	testmodules = [__import__(p) for p in testmodulenames]
	suites = []
	for m in testmodules:
		try:
			for c in m.__dict__['TEST_CLASSES']:
				suites.append(unittest.TestLoader().loadTestsFromTestCase(c))
		except (AttributeError, KeyError):
			pass
	os.chdir(genpath('../..'))
	mastersuite = unittest.TestSuite(suites)
	runner = unittest.TextTestRunner(verbosity=2)
	return runner.run(mastersuite)

if __name__ == '__main__':
	run_and_get_results()