#!/usr/bin/env python
# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2009 by the FIFE team
#  http://www.fifengine.de
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