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

import sys, os, re

# Additional directories for module search path
_paths = ('.', '../../engine/swigwrappers/python', '../../engine/extensions')

def _joinPath(path):
	""" Insert correct separators """
	return os.sep.join(path.split('/'))

def _setupPaths():
	""" Add _paths to module search path """
	for p in _paths:
		if p not in sys.path:
			sys.path.append(_joinPath(p))

#--- Main function ---#
if __name__ == '__main__':
	_setupPaths()
	
	# Get command line arguments
	params = None
	if len(sys.argv) > 1:
		params = sys.argv[1]
	
	# Start editor
	from scripts.editor import Editor
	app = Editor(params)
	app.run()
