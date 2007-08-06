#!/usr/bin/python
import os, re, sys

if len(sys.argv) > 1:
    print '\nusage: python test_all.py'
    print '  Runs all test cases defined in SConscript file of unit_tests-folder\n'
    sys.exit(0)

def getTestPrograms():
    """ Get the names of all test programs by evaluating the SConscript file """
    reprg = re.compile(r"""^env.Program\(["'](.*?)['"]""")
    progs = []
    for line in open("SConscript"):
        m = reprg.match(line.strip())
        if m:
            progs.append(m.group(1))
    return progs

def run_and_get_results():
	# Switch to unit_tests directory
	dirpath = os.sep.join(sys.argv[0].split(os.sep)[:-1])
	if dirpath:
		os.chdir(dirpath)

	failed = []
	for prog in getTestPrograms():
		print '\n===== Running %s =====' % prog
		if os.system(os.sep.join(('.', prog))):
			failed.append( prog )

	if failed:
		print '\n===== Failed C++ Tests ====='
		for prog in failed:
			print prog
	else:
		print '\n========================'
		print 'All C++ unit tests run without failure\n'
		
	return failed
		
if __name__ == '__main__':
	run_and_get_results()