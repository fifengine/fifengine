#!/usr/bin/python
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
	return [modname + p[:-3] for p in pythonfilenames if p not in ('__init__.py', 'test_all.py')]
	
def run_core_tests(progs):
	prevdir = os.getcwd()
	os.chdir(genpath('fife_tests/core_tests'))
	
	errors, failures = [], []
	for prog in progs:
		print '\n===== Running %s =====' % prog
		if os.system(os.sep.join(('.', prog))):
			errors.append(prog)
	os.chdir(prevdir)
	return errors, failures

def run_test_modules(modules):
	imported = []
	for module in modules:
		m = __import__(module)
		for part in module.split('.')[1:]:
			m = getattr(m, part)
		imported.append(m)
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
	
	print 80 * '='	
	if errorsfound:
		print 'Looks like there is some errors in the code, svn commit is probably not a good idea yet...'
	else:
		print 'All tests run succesfully!'
	print ''
	
def quit(dummy):
	sys.exit(0)

def run(automatic):
	index = 0
	tests = {}
	
	core_tests = resolve_test_progs(genpath('fife_tests/core_tests/SConscript'))
	for t in core_tests:
		tests[index] = ('Core tests', t, [t], run_core_tests)
		index += 1
	tests[index] = ('Core tests', 'all', core_tests, run_core_tests)	
	index += 1
	
	swig_tests = resolve_test_modules(genpath('fife_tests/swig_tests'))
	for t in swig_tests:
		tests[index] = ('SWIG tests', t, [t], run_test_modules)
		index += 1
	tests[index] = ('SWIG tests', 'all', swig_tests, run_test_modules)	
	index += 1
		
	alltests = {'core': core_tests, 'swig': swig_tests}
	tests[index] = ('Other', 'Run all tests', alltests, run_all)
	tests[index+1] = ('Other', 'Cancel and quit', None, quit)

	if not automatic:
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
	else:
		run_all(alltests)

def main():
	usage = 'usage: %prog [options]\n' + \
		'Runs programs that test fife functionality. It is recommended that you run\n' + \
		'these tests always before svn commit\n'
	parser = optparse.OptionParser(usage)
	parser.add_option("-a", "--automatic",
			action="store_true", dest="automatic", default=False,
			help="In case selected, runs all the tests automatically")
	options, args = parser.parse_args()
	run(options.automatic)
	
	
	
	
if __name__ == '__main__':
	main()