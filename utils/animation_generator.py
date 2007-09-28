#!/usr/bin/python
from util_scripts.path import path
import os, sys

_sep = os.path.sep

print "NOTE: make sure you are running this script from fife project root!"
dirname = raw_input("Enter directory which contains animation images: ")
p = path(dirname)
files = p.walkfiles('*.png')

dir2anim = {}
for f in files:
	curdir = _sep.join(str(f).split(_sep)[:-1])
	if not dir2anim.has_key(curdir):
		dir2anim[curdir] = []
	dir2anim[curdir].append(f)

print "Planning to write into following directories: "
for d in sorted(dir2anim.keys()):
	print "   " + d
inp = raw_input("Press y if ok: ")
if inp.lower() != 'y':
	print "exiting"
	sys.exit(0)

delay = int(raw_input("Give delay for animations: "))
sid = int(raw_input("Give starting id for animations: "))

ANIM_HEADER = '<animation delay="%d" id="%d">'
ANIM_LINE = '<frame source="%s" />'
ANIM_FOOTER = '</animation>\n'

for d in dir2anim.keys():
	anim = []
	anim.append(ANIM_HEADER % (delay, sid))
	sid += 1
	for f in sorted(dir2anim[d]):
		anim.append('\t' + ANIM_LINE % f) 
	anim.append(ANIM_FOOTER)
	open(_sep.join([d, 'animation.xml']), 'w').write('\n'.join(anim))

print "all done"