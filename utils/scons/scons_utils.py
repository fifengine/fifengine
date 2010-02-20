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

import os, sys
from string import Template

_sep = os.path.sep

def is_implfile(fname):
	return fname.endswith('.cpp') or fname.endswith('.cxx') or fname.endswith('.m')

def is_headerfile(fname):
	return fname.endswith('.hpp') or fname.endswith('.h')

def is_swigfile(fname):
	return fname.endswith('.i')
	
#need this function because python 2.5 doesnt support os.path.relpath
def relpath(longPath, basePath):
	if not longPath.startswith(basePath):
		raise RuntimeError("Unexpected arguments")
	if longPath == basePath:
		return "."
	i = len(basePath)
	if not basePath.endswith(os.path.sep):
		i += len(os.path.sep)
	return longPath[i:]

#This generates the a list of absolute paths with filenames
def gen_dest_files(dest_path, source_files):
	dest_files = list()
	for f in source_files:
		dest_files.append(os.path.join(dest_path,f))
	
	return dest_files

def importConfig(config):
	module = __import__(config)
	parts = config.split('.')
	for part in parts[1:]:
		module = getattr(module, part)
	return module

def getPlatformConfig():
	filename = 'build' + _sep + sys.platform + '-config'
	sconsfilename = 'build.' + sys.platform + '-config'
	if os.path.exists(filename + '.py'):
		 return importConfig(sconsfilename)
	else:
		print 'no platform-config found (searched ' + filename + '.py)'
		Exit(1)

def tryConfigCommand(context, cmd):
	ret = context.TryAction(cmd)[0]
	context.Result(ret)
	if ret:
		context.env.ParseConfig(cmd)
	return ret

def checkPKG(context, name):
	context.Message('Checking for %s (using pkg-config)...' %name)
	return tryConfigCommand(context, 'pkg-config --libs --cflags \'%s\'' %name.lower())

def checkConf(context, name):
	binary = '%s-config' % name.lower()
	context.Message('Checking for %s (using %s)... ' % (name, binary))
	configcall = '%s --libs --cflags' %binary
	return tryConfigCommand(context, configcall)

def filter_by_dir(dirfilters, files):
	result = []
	for f in files:
		filtered = False
		for p in dirfilters:
			s_files = f.split(_sep)
			for s in s_files:
				if s == p:
					filtered = True
					break
		if not filtered:
			result.append(f)
	return result
	
def gen_swig_interface(templatefile, source, outdir):
	template = Template(open(templatefile).read())
	inclusions = sorted([os.path.join('%include engine', str(f)) for f in source])
	inclusions = '\n'.join(inclusions)
	interfacefile = os.path.join(outdir, 'fife.i')
	open(interfacefile, 'w').write(template.substitute(inclusions=inclusions))
