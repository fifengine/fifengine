#!/usr/bin/env python
# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2013 by the FIFE team
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

# For given object description (object.xml) converts it to new one
# Usage: packanim.py path/to/object.xml [output/dir]

from animpacker import AnimPacker
import sys
import os
import traceback

if __name__ == '__main__':
	if len(sys.argv) not in range(2,4):
		usage = 'Usage: {0} path/to/object.xml [output/dir]'.format(os.path.basename(sys.argv[0]))
		sys.exit(usage)	

	file = os.path.normpath(sys.argv[1])
	if len(sys.argv) is 3:
		outdir = sys.argv[2];
	else:
		outdir = None

	try:
		packer = AnimPacker()
		packer.startPacking(file, outdir)
	except:
		traceback.print_exc(sys.exc_info()[1])
		print u"Importing file failed:\n"
		print u"File: "+unicode(file)+u"\n"
		print u"Error: "+unicode(sys.exc_info()[1])