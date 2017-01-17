#!/usr/bin/env python
# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2017 by the FIFE team
#  http://www.fifengine.net
#  This file is part of FIFE.
#
#  FIFE is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the
#  Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
# ####################################################################

import os, re, sys, optparse, unittest

def genpath(somepath):
	return os.path.sep.join(somepath.split('/'))

def print_header(text):
	print '\n'
	print 80 * '='
	print text
	print 80 * '-'

def resolve_test_progs(sconscript_filename):
	""" Get the names of all test programs by evaluating the SConscript file """
	reprg = re.compile(r"""^env.Program\(["'](.*?)['"]""")
	progs = []
	for line in open(sconscript_filename):
		m = reprg.match(line.strip())
		if m:
			progs.append(m.group(1))
	return progs

def resolve_test_modules(directory):
	pythonfilenames = [p for p in os.listdir(directory) if len(p) > 3 and p[-3:] == '.py']
	modname = directory.replace(os.path.sep, '.') + '.'
	modules = []
	skipped_filenames = ('test_all.py',)
	for p in pythonfilenames:
		skip = False
		for s in skipped_filenames:
			if p.find(s) != -1:
				skip = True
		if p[0] == '_':
			skip = True
		if not skip:
			modules.append(modname + p[:-3])
	return modules
	
def run_core_tests(progs):
	prevdir = os.getcwd()
	
	errors, failures = [], []
	for prog in progs:
		print '\n===== Running %s =====' % prog
		if os.system(os.sep.join(('build','tests','debug', prog))):
			errors.append(prog)
	os.chdir(prevdir)
	return errors, failures

def get_dynamic_imports(modules):
	imported = []
	for module in modules:
		m = __import__(module)
		for part in module.split('.')[1:]:
			m = getattr(m, part)
		imported.append(m)
	return imported
		
def run_test_modules(modules):
	imported = get_dynamic_imports(modules)
	suites = []
	for m in imported:
		try:
			for c in m.__dict__['TEST_CLASSES']:
				suites.append(unittest.TestLoader().loadTestsFromTestCase(c))
		except (AttributeError, KeyError):
			pass
	mastersuite = unittest.TestSuite(suites)
	runner = unittest.TextTestRunner(verbosity=2)
	result = runner.run(mastersuite)
	return [e[1] for e in result.errors], [f[1] for f in result.failures]
		
def run_all(tests):
	def print_errors(txt, errs):
		if errs:
			print txt + ':'
			for msg in errs:
				print '  ' + msg
		
	core_errors, core_failures = run_core_tests(tests['core'])
	swig_errors, swig_failures = run_test_modules(tests['swig'])
	ext_errors, ext_failures = run_test_modules(tests['ext'])
	
	print 80 * '='
	errorsfound = False
	
	if core_errors or core_failures:
		print_errors('Errors in core tests', core_errors)
		print_errors('Failures in core tests', core_failures)
		errorsfound = True
	else:
		print 'No Core errors found'
	
	if swig_errors or swig_failures:
		print_errors('Errors in SWIG tests', swig_errors)
		print_errors('Failures in SWIG tests', swig_failures)
		errorsfound = True
	else:
		print 'No SWIG errors found'
	
	if swig_errors or swig_failures:
		print_errors('Errors in extensions tests', ext_errors)
		print_errors('Failures in extensions tests', ext_failures)
		errorsfound = True
	else:
		print 'No Extensions errors found'

	print 80 * '='	
	if errorsfound:
		print 'ERROR. One or more tests failed!'
	else:
		print 'OK. All tests ran succesfully!'
	print ''
	
def quit(dummy):
	sys.exit(0)

def run(automatic, selected_cases):
	index = 0
	tests = {}
	
	core_tests = resolve_test_progs(genpath('tests/core_tests/SConscript'))
	for t in core_tests:
		tests[index] = ('Core tests', t, [t], run_core_tests)
		index += 1
	tests[index] = ('Core tests', 'all', core_tests, run_core_tests)	
	index += 1
	
	swig_tests = resolve_test_modules(genpath('tests/swig_tests'))
	for t in swig_tests:
		tests[index] = ('SWIG tests', t, [t], run_test_modules)
		index += 1
	tests[index] = ('SWIG tests', 'all', swig_tests, run_test_modules)	
	index += 1
		
	extension_tests = resolve_test_modules(genpath('tests/extension_tests'))
	for t in extension_tests:
		tests[index] = ('Extension tests', t, [t], run_test_modules)
		index += 1
	tests[index] = ('Extension tests', 'all', extension_tests, run_test_modules)	
	index += 1

	alltests = {'core': core_tests, 'swig': swig_tests, 'ext': extension_tests}
	tests[index] = ('Other', 'Run all tests', alltests, run_all)
	tests[index+1] = ('Other', 'Cancel and quit', None, quit)

	if (not automatic) and (not selected_cases):
		selection = None
		while True:
			print 'Select test module to run:'
			prevheader = ''
			for ind in sorted(tests.keys()):
				header, name, params, fn = tests[ind]
				if header != prevheader:
					print header
					prevheader = header
				print '  %d) %s' % (ind, name)
			selection = raw_input('-> : ')
			
			try:
				selection = int(selection)
				if (selection < 0) or (selection > max(tests.keys())):
					raise ValueError
				break
			except ValueError:
				print 'Please enter number between 0-%d\n' % max(tests.keys())
				continue
		header, name, params, fn = tests[selection]
		fn(params)
	elif (selected_cases):
		for case in selected_cases:
			try:
				caseid = int(case)
				if (caseid < 0) or (caseid > max(tests.keys())):
					raise ValueError
				header, name, params, fn = tests[caseid]
				fn(params)
			except ValueError:
				print 'No test case with value %s found' % case
	else:
		run_all(alltests)

def main():
	usage = 'usage: %prog [options] [args]\n' + \
		'This is a test runner.\n' + \
		'It enables you to test functionalities of fifengine core, extensions and the generated swig interface.\n' + \
		'You can give a list of test ids as arguments to the script.\n' + \
		'This is useful when running a test over and over again with little changes.\n' + \
		'Available test ids can be seen from interactive menu (run script without any parameters).\n' + \
		'You can also use "-a" to run all tests from the CLI.'
	parser = optparse.OptionParser(usage)
	parser.add_option("-a", "--automatic",
			action="store_true", dest="automatic", default=False,
			help="In case selected, runs all the tests automatically")
	options, args = parser.parse_args()
	run(options.automatic, args)
	
	
	
	
if __name__ == '__main__':
	main()
