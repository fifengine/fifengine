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

import os, sys
import re
import subprocess
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
	return tryConfigCommand(context, 'pkg-config --libs --cflags \'%s\'' %name)

def checkConf(context, name):
	binary = '%s-config' % name
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
	
def include_by_dir(dirfilters, files):
	result = [ f for f in files for p in dirfilters for s in f.split(_sep) if s == p ]
	return result
	
def gen_swig_interface(templatefile, intfiles, outdir, outfile, importfiles = None):
	template = Template(open(templatefile).read())
	inclusions = sorted([os.path.join('%include engine', str(f)) for f in intfiles])
	inclusions = '\n'.join(inclusions)
	interfacefile = os.path.join(outdir, outfile)
	if importfiles:
		imports = sorted([os.path.join('%import engine', str(f)) for f in importfiles])
		imports = '\n'.join(imports)
		open(interfacefile, 'w').write(template.substitute(inclusions=inclusions, imports=imports))
	else:
		open(interfacefile, 'w').write(template.substitute(inclusions=inclusions, imports=''))

def get_fife_version(srcpath):
	MAJOR_VERSION_PATTERN = re.compile(r"#define\s+FIFE_MAJOR_VERSION\s+(.*)")
	MINOR_VERSION_PATTERN = re.compile(r"#define\s+FIFE_MINOR_VERSION\s+(.*)")
	PATCH_VERSION_PATTERN = re.compile(r"#define\s+FIFE_PATCH_VERSION\s+(.*)")
	PRERELEASE_TYPE_PATTERN = re.compile(r"#define\s+FIFE_PRERELEASE_TYPE\s+(.*)")
	PRERELEASE_VERSION_PATTERN = re.compile(r"#define\s+FIFE_PRERELEASE_VERSION\s+(.*)")
	patterns = [MAJOR_VERSION_PATTERN,
				MINOR_VERSION_PATTERN,
				PATCH_VERSION_PATTERN,
				PRERELEASE_TYPE_PATTERN,
				PRERELEASE_VERSION_PATTERN]
	
	source = open(os.path.join(srcpath, 'version.h'), 'r').read()
	versionInfo = []
	for pattern in patterns:
		match = pattern.search(source)
		if match:
			versionInfo.append(match.group(1).strip())
			
	return '.'.join(versionInfo)

def get_fife_git_hash(path):
	fifehash = ""
	githash_output = get_program_output('git', ['rev-parse','--short=8','HEAD'])
	if githash_output is not None:
		fifehash = githash_output.strip()
		return fifehash

	#when git is not available check the .githash file
	hashfile = os.path.join(path, '.githash')  
	if os.path.isfile(hashfile):
		f = open(hashfile, "r")
		hashf = f.read()
		return hashf.strip()
	
	return fifehash

#checks the users PATH environment variable for a executable program and 
#returns the full path
def which(program):
	def is_exe(fpath):
		return os.path.exists(fpath) and os.access(fpath, os.X_OK)

	fpath, fname = os.path.split(program)
	if fpath:
		if is_exe(program):
			return program
	else:
		for path in os.environ["PATH"].split(os.pathsep):
			exe_file = os.path.join(path, program)
			if is_exe(exe_file):
				return exe_file

	return None

#Return the data written to stdout if the process succeeds or None if it crashes
def get_program_output(program, args):
	program_path = which(program)
	if not program_path:
		return None

	process = subprocess.Popen([program_path] + args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	process.wait()

	if process.returncode != 0:
		return None
	return process.stdout.read()
