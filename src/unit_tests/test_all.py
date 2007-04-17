#!/usr/bin/python
import os, re, sys

if len(sys.argv) > 1:
    print '\nusage: python test_all.py'
    print '  Runs all test cases defined in SConscript file of unit_tests-folder\n'
    sys.exit(0)

dirpath = os.sep.join(sys.argv[0].split(os.sep)[:-1])
reprg = re.compile(r"""^env.Program\(["'](.*?)['"]""")
progs = []
for line in open(os.sep.join((dirpath, 'SConscript'))):
    m = reprg.match(line.strip())
    if m:
        progs.append(m.group(1))

failed = []
for prog in progs:
    print '\n===== Running %s =====' % prog
    if os.system(os.sep.join(('.', dirpath, prog))):
        failed.append( prog )

if failed:
    print '\n===== Failed Tests ====='
    for prog in failed:
        print prog
else:
	print '\n========================'
	print 'All unit tests run without failure\n'