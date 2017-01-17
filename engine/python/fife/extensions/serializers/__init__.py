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

import fife, sys, os
from traceback import print_exc

__all__ = ('ET', 'SerializerError', 'InvalidFormat', 'WrongFileType', 'NameClash', 'NotFound', 'warn', 'root_subfile', 'reverse_root_subfile')

try:
	import xml.etree.cElementTree as ET
except ImportError:
	import xml.etree.ElementTree as ET

class SerializerError(Exception):
	pass

class InvalidFormat(SerializerError):
	pass

class WrongFileType(SerializerError):
	pass

class NameClash(SerializerError):
	pass

class NotFound(SerializerError):
	pass

def warn(self, msg):
	print 'Warning (%s): %s' % (self.filename, msg)

def root_subfile(masterfile, subfile):
	"""
	Returns new path for given subfile (path), which is rooted against masterfile
	E.g. if masterfile is ./../foo/bar.xml and subfile is ./../foo2/subfoo.xml,
	returned path is ../foo2/subfoo.xml
	NOTE: masterfile is expected to be *file*, not directory. subfile can be either
	"""
	s = '/'

	masterfile = norm_path(os.path.abspath(masterfile))
	subfile = norm_path(os.path.abspath(subfile))

	master_fragments = masterfile.split(s)
	sub_fragments = subfile.split(s)

	master_leftovers = []
	sub_leftovers = []

	for i in xrange(len(master_fragments)):
		try:
			if master_fragments[i] == sub_fragments[i]:
				master_leftovers = master_fragments[i+1:]
				sub_leftovers = sub_fragments[i+1:]
		except IndexError:
			break

	pathstr = ''
	for f in master_leftovers[:-1]:
		pathstr += '..' + s
	pathstr += s.join(sub_leftovers)
	return pathstr

def reverse_root_subfile(masterfile, subfile):
	"""
	does inverse operation to root_subfile. E.g. 
	E.g. if masterfile is ./../foo/bar.xml and subfile is ../foo2/subfoo.xml,
	returned path ./../foo2/subfoo.xml
	Usually this function is used to convert saved paths into engine relative paths
	NOTE: masterfile is expected to be *file*, not directory. subfile can be either
	"""
	s = '/'

	masterfile = norm_path(os.path.abspath(masterfile)).split(s)[:-1]
	subfile = norm_path(os.path.abspath( s.join(masterfile) + s + subfile ))
	masterfile = norm_path(os.getcwd()) + s + 'foo.bar' # cheat a little to satisfy root_subfile
	return root_subfile(masterfile, subfile)

def norm_path(path):
	"""
	Makes the path use '/' delimited separators. FIFE always uses these delimiters,
	but some os-related routines will default to os.path.sep.
	"""
	if os.path.sep == '/':
		return path

	return '/'.join(path.split(os.path.sep))	

def loadImportFile(loader, path, engine, debug=False):
	""" uses XMLObjectLoader to load import files from path
	
	@type	path:	string
	@param	path:	path to import file
	@type	debug:	bool
	@param	debug:	flag to activate / deactivate print statements
	"""
	loader.loadResource(fife.ResourceLocation(path))
	if debug: print 'imported object file ' + path

def loadImportDir(loader, path, engine, debug=False):
	""" helper function to call loadImportFile on a directory
	
	@type	path:	string
	@param	path:	path to import directory
	@type	debug:	bool
	@param	debug:	flag to activate / deactivate print statements
	"""
	for _file in filter(lambda f: f.split('.')[-1] == 'xml', engine.getVFS().listFiles(path)):
		loadImportFile(loader, '/'.join([path, _file]), engine, debug)

def loadImportDirRec(loader, path, engine, debug=False):
	""" helper function to call loadImportFile recursive on a directory

	@type	path:	string
	@param	path:	path to import directory
	@type	debug:	bool
	@param	debug:	flag to activate / deactivate print statements	
	"""
	loadImportDir(loader, path, engine, debug)

	for _dir in filter(lambda d: not d.startswith('.'), engine.getVFS().listDirectories(path)):
		loadImportDirRec(loader, '/'.join([path, _dir]), engine, debug)
		
def root_subfile(masterfile, subfile):
	"""
	Returns new path for given subfile (path), which is rooted against masterfile
	E.g. if masterfile is ./../foo/bar.xml and subfile is ./../foo2/subfoo.xml,
	returned path is ../foo2/subfoo.xml
	NOTE: masterfile is expected to be *file*, not directory. subfile can be either
	"""
	s = '/'

	masterfile = norm_path(os.path.abspath(masterfile))
	subfile = norm_path(os.path.abspath(subfile))

	master_fragments = masterfile.split(s)
	sub_fragments = subfile.split(s)

	master_leftovers = []
	sub_leftovers = []

	for i in xrange(len(master_fragments)):
		try:
			if master_fragments[i] == sub_fragments[i]:
				master_leftovers = master_fragments[i+1:]
				sub_leftovers = sub_fragments[i+1:]
		except IndexError:
			break

	pathstr = ''
	for f in master_leftovers[:-1]:
		pathstr += '..' + s
	pathstr += s.join(sub_leftovers)
	return pathstr
