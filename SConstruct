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
import utils.scons.scons_utils as utils
from distutils.sysconfig import get_python_lib

_sep = os.path.sep

#**************************************************************************
#add any command line options here
#**************************************************************************

vars = Variables()
vars.Add(PathVariable('DESTDIR', 
			'Destination directory (prepended to prefix)',
			None,
			PathVariable.PathAccept ))

#propagate the invoking users complete external environment
env = Environment(variables = vars, 
			ENV = os.environ,
			DESTDIR = '${DESTDIR}')

env.EnsureSConsVersion(1,2)

AddOption('--enable-debug',
		dest='enable-debug',
		action="store_true",
		help='Builds the debug version of the binaries',
		default=False)
		
AddOption('--disable-opengl',
		dest='disable-opengl',
		action="store_true",
		help='Disable OpenGL support',
		default=False)		

AddOption('--disable-zip',
		dest='disable-zip',
		action="store_true",
		help='Disable ZIP archive support',
		default=False)	

AddOption('--disable-log',
		dest='disable-log',
		action="store_true",
		help='Disable engine logging support',
		default=False)	

AddOption('--enable-rend-camzone',
		dest='enable-rend-camzone',
		action="store_true",
		help='Enables camera zone renderer',
		default=False)
		
AddOption('--enable-rend-grid',
		dest='enable-rend-grid',
		action="store_true",
		help='Enables camera grid renderer',
		default=False)

AddOption('--enable-profile',
		dest='enable-profile',
		action="store_true",
		help='Build with profiling information.  This automatically enables debug.',
		default=False)
		
AddOption('--prefix',
		dest='prefix',
		nargs=1, type='string',
		action='store',
		metavar='DIR',
		help='installation prefix')
		
AddOption('--python-prefix',
		dest='python-prefix',
		nargs=1, type='string',
		action='store',
		metavar='DIR',
		help='Python module installation prefix')
		
#**************************************************************************
#save command line options here
#**************************************************************************
if GetOption('enable-debug'):
	debug = 1
	env['FIFE_DEBUG'] = True
else:
	debug = 0
	env['FIFE_DEBUG'] = False
	
if GetOption('disable-opengl'):
	opengl = 0
else:
	opengl = 1

if GetOption('disable-zip'):
	zip = 0
else:
	zip = 1

if GetOption('disable-log'):
	log = 0
else:
	log = 1

if GetOption('enable-rend-camzone'):
	rend_camzone = 1
else:
	rend_camzone = 0

if GetOption('enable-rend-grid'):
	rend_grid = 1
else:
	rend_grid = 0

if GetOption('enable-profile'):
	debug = 1
	profile = 1
else:
	profile = 0

#**************************************************************************
#set the installation directories.
#**************************************************************************
prefix = GetOption('prefix')
if prefix is None:
	if sys.platform == 'win32':
		prefix = '\\fife'
	elif sys.platform == 'darwin':
		prefix = '/opt/local'
	else:
		prefix = '/usr/local'

pythonprefix = GetOption('python-prefix')
if pythonprefix is None:
	pythonprefix = get_python_lib()

if env.has_key('DESTDIR'):
	prefix = env['DESTDIR'] + prefix
	pythonprefix = env['DESTDIR'] + pythonprefix

#**************************************************************************
#get platform specific information
#**************************************************************************
platformConfig = utils.getPlatformConfig()
env = platformConfig.initEnvironment(env)
env = platformConfig.addExtras(env, opengl)

#**************************************************************************
#define custom library/header check functions
#**************************************************************************
conf = Configure(env,
		custom_tests = {'checkPKG': utils.checkPKG,
				'checkConf': utils.checkConf },
				conf_dir = os.path.join('#','build','.sconf_temp'),
				log_file = os.path.join('#','build','config.log'),
				clean=True)
	
def checkForLib(lib, header='', language='c++'):
	ret = conf.checkPKG(lib)
	if not ret:
		ret = conf.checkConf(lib)
		if not ret:
			if len(header):
				ret = conf.CheckLibWithHeader(libs=lib, header=header, language=language)
			else:
				ret = conf.CheckLib(library=lib, language=language)
					
	return ret
	
def checkForLibs(liblist, required=1, language='c++'):
	ret = 0
	for lib, header in liblist:
		if (isinstance(lib, tuple)):
			for item in lib:
				ret = checkForLib(item, header, language)
					
				if ret:
					env.AppendUnique(LIBS = [item])
					break
		else:
			ret = checkForLib(lib, header, language)
			if ret:
				env.AppendUnique(LIBS=[lib])
	
		if required and not ret:
			if (isinstance(lib, tuple)):
				for item in lib:
					print 'Required lib %s not found!' %item
			else:
				print 'Required lib %s not found!' %lib
			Exit(1)
		
#**************************************************************************
#check the existence of required libraries and headers
#**************************************************************************
required_libs = platformConfig.getRequiredLibs(opengl)
optional_libs = platformConfig.getOptionalLibs(opengl)

required_headers = platformConfig.getRequiredHeaders(opengl)

# do not run the check for dependencies if you are running
# a clean or building the external dependencies
if not GetOption('clean') and 'ext' not in COMMAND_LINE_TARGETS:
	if required_libs:
		checkForLibs(required_libs, required = 1)
	if optional_libs:
		checkForLibs(optional_libs, required = 0)
	if required_headers:
		for h in required_headers:
			conf.CheckHeader(h)

#**************************************************************************
#set variables based on command line and environment options
#**************************************************************************

if os.environ.has_key('SWIG'):
	env['SWIG'] = os.environ['SWIG']

haveusercxxflags = False

if os.environ.has_key('CXXFLAGS'):
	usercxxflags = Split(os.environ['CXXFLAGS'])
	haveusercxxflags = True
elif ARGUMENTS.get('CXXFLAGS'):
	usercxxflags = Split(ARGUMENTS.get('CXXFLAGS'))
	haveusercxxflags = True

if debug:
	if haveusercxxflags:
		env.AppendUnique(CXXFLAGS=usercxxflags)
	else:	
		env.AppendUnique(CXXFLAGS=['-O0', '-Wall', '-Wno-unused'])
		
	env.AppendUnique(CXXFLAGS=['-g', '-D_DEBUG'])
	env.AppendUnique(LINKFLAGS=['-Wl'])
	engine_var_dir = os.path.join('build','engine','debug')
	tests_var_dir = os.path.join('build','tests','debug')
	print "Building DEBUG binaries..."
else:
	if haveusercxxflags:
		env.AppendUnique(CXXFLAGS=usercxxflags)
	else:	
		env.AppendUnique(CXXFLAGS=['-O2', '-Wall', '-Wno-unused'])
	
	env.AppendUnique(LINKFLAGS=['-Wl'])
	engine_var_dir = os.path.join('build','engine','release')
	tests_var_dir = os.path.join('build','tests','release')
	print "Building RELEASE binaries..."

if opengl: 
	env.Append(CPPDEFINES = ['HAVE_OPENGL'])
if zip:
	env.Append(CPPDEFINES = ['HAVE_ZIP'])
if log:
	env.Append(CPPDEFINES = ['LOG_ENABLED'])
if rend_camzone:
	env.Append(CPPDEFINES = ['RENDER_CAMZONES'])
if rend_grid:
	env.Append(CPPDEFINES = ['RENDER_GRID'])
if profile:
	env.Append(CXXFLAGS = ['-pg'])
	env.Append(LINKFLAGS = ['-pg'])
	
#**************************************************************************
#global compiler flags used for ALL targets
#**************************************************************************
env.Append(CPPPATH='#engine/core')

#**************************************************************************
#variables to pass to the SConscript
#TODO: clean this up a bit.  Should probably make sure unittest++ exists.
#**************************************************************************
opts = {'SRC' : os.path.join(os.getcwd(), 'engine',),
		'DLLPATH' : os.path.join(os.getcwd(), 'build', 'win32', 'binaries', 'mingw'),
		'DEBUG' : debug,
		'PREFIX' : prefix,
		'TESTLIBS' : ['fife', 'UnitTest++'],
		'PYTHON_PREFIX' : pythonprefix,
		'WRAP_COPY_DEST' : os.path.join('#engine', 'swigwrappers', 'python'),
		'PYLIB_COPY_DEST' : os.path.join('#engine', 'python', 'fife')}

if debug:
	opts['LIBPATH'] = os.path.join(os.getcwd(), 'build', 'engine', 'debug')
else:
	opts['LIBPATH'] = os.path.join(os.getcwd(), 'build', 'engine', 'release')

#**************************************************************************
#target for static and shared libraries
#**************************************************************************
Export('env')

#build the engine
env.SConscript('engine/SConscript', variant_dir=engine_var_dir, duplicate=0, exports='opts')

#build the engine tests
env.SConscript('tests/core_tests/SConscript', variant_dir=tests_var_dir, duplicate=0, exports='opts')

#build the external dependencies
env.SConscript('ext/SConscript')

#**************************************************************************
#documentation target
#**************************************************************************
def generate_docs(target = None, source = None, env = None):
	os.system('doxygen $SOURCES')

doc_builder = Builder(action = generate_docs)
env.Append(BUILDERS = {'BuildDocs': doc_builder})
Alias('docs', env.BuildDocs('docs', os.path.join('doc', 'doxygen', 'doxyfile')))

#**************************************************************************
#Set the default target
#**************************************************************************
#clear the default target
Default()
#make fife-python the default target
Default('fife-python')

# vim: set filetype=python:

