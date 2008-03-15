#!/usr/bin/python
from util_scripts.path import path
import os, sys

DIRNAME = 'clients/rio_de_hola/objects/items/common/crates/empty_lid/open'
DELAY = 130
NAMESPACE = 'http://www.fifengine.de/xml/rio_de_hola'
ACTIONFRAME = -1
X_OFFSET = 0
Y_OFFSET = 0

_sep = os.path.sep
p = path(DIRNAME)
files = p.walkfiles('*.png')

dir2anim = {}
for f in files:
	curdir = _sep.join(str(f).split(_sep)[:-1])
	if not dir2anim.has_key(curdir):
		dir2anim[curdir] = []
	dir2anim[curdir].append(f)

ANIM_HEADER = '<animation delay="%d" namespace="%s" id="%s" x_offset="%d" y_offset="%d">'
ANIM_ACTION_HEADER = '<animation delay="%d" namespace="%s" id="%s" x_offset="%d" y_offset="%d" action="%d">'
ANIM_LINE = '<frame source="%s" />'
ANIM_FOOTER = '</animation>\n'

for d in sorted(dir2anim.keys()):
	anim = []
	animation_id = ':'.join(d.split(_sep)[-3:])
	if ACTIONFRAME < 0:
		anim.append(ANIM_HEADER % (DELAY, NAMESPACE, animation_id, X_OFFSET, Y_OFFSET))
	else:
		anim.append(ANIM_ACTION_HEADER % (DELAY, NAMESPACE, animation_id, X_OFFSET, Y_OFFSET, ACTIONFRAME))
	
	for f in sorted(dir2anim[d]):
		filename = f.split(_sep)[-1]
		anim.append('\t' + ANIM_LINE % filename)
	anim.append(ANIM_FOOTER)
	
	outfile = _sep.join([d, 'animation.xml'])
	contents = '\n'.join(anim)
	print outfile
	print contents
	print ''
	open(outfile, 'w').write(contents)

print "all done"