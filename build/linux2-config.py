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

import sys
import os
import platform

pythonversion = 'python' + str(os.sys.version_info[0]) + '.' + str(os.sys.version_info[1])

def initEnvironment(env):

	rootpath = env.Dir('#.').srcnode().abspath
	extincludepath = os.path.join(rootpath, 'ext', 'install', 'include')
	extlibpath = os.path.join(rootpath, 'ext', 'install', 'lib')
	
	env.Append(CPPPATH = [os.path.join('/', 'opt', 'include'),
							os.path.join('/', 'usr', 'include', 'vorbis'),
							os.path.join('/', 'usr', 'include', 'SDL'),
							os.path.join('/', 'usr', 'include', pythonversion),
							extincludepath])

	env.Append(LIBPATH = [os.path.join('/', 'opt', 'lib'),
						  extlibpath,
						  env.subst('$LIBDIR')])
	
	env.AppendENVPath('LD_RUN_PATH', os.path.join('..', '..', '..', extlibpath))
	
	env.AppendUnique(CXXFLAGS=["-DPNG_SKIP_SETJMP_CHECK"])

	env.Tool('swig')

	return env

def addExtras(env, opengl):
	env.Append(LIBS = pythonversion)
	
	if opengl:
		env.Append(LIBS = ['stdc++', 'GL',])
		env.Append(LIBPATH = os.path.join('/', 'usr', 'X11R6', 'lib'))
		
	# define for using tinyxml with stl support enabled
	env.AppendUnique(CPPDEFINES = ['TIXML_USE_STL'])
	
	return env

def getRequiredHeaders(opengl):
	return None

def getRequiredLibs(reqLibs):
	# libs is a list of tuples that have the form:
	#	(libname, headers)
	#	libname - may be a single library or a tuple of libraries
	#	headers - may be a single header or a list of headers
	#
	#	This list is somewhat order dependent
	#		fifechan_sdl - depends on at least the SDL libs and guichan prior in the list
	#		fifechan_opengl - depends on at least guichan prior in the list
	libs = [('vorbisfile', 'vorbisfile.h'),
			(pythonversion, pythonversion + '/Python.h'),
			('openal', 'AL/al.h'),
			('SDL', 'SDL.h'),
			('SDL_ttf', 'SDL_ttf.h'),
			('SDL_image', 'SDL_image.h'),
			('boost_system', ''),
			(('boost_filesystem', 'boost_filesystem-gcc', 'boost_filesystem-gcc41', 'boost_filesystem-mt', 'libboost_filesystem-mt'), 'boost/filesystem.hpp'),
			(('boost_regex', 'boost_regex-gcc', 'boost_regex-gcc41', 'boost_regex-mt', 'libboost_regex-mt'), 'boost/regex.hpp'),
			(('libpng', 'png'), 'png.h'),
			('Xcursor', '')]

	opengl = reqLibs['opengl']
	fifechan = reqLibs['fifechan']
	librocket = reqLibs['librocket']
	cegui = reqLibs['cegui']
	cegui_0 = reqLibs['cegui-0']

	if fifechan:
		libs.append(('fifechan', 'fifechan.hpp'))
		libs.append(('fifechan_sdl', ''))
		if opengl:
			libs.append(('fifechan_opengl', ''))
	
	if librocket:
		libs.append(('RocketCore', 'Rocket/Core.h'))
		libs.append(('RocketControls', 'Rocket/Controls.h'))
		librocket_debug = reqLibs['librocket-debug']
		if librocket_debug:
			libs.append(('RocketDebugger', 'Rocket/Debugger.h'))
	
	if cegui:
		libs.append(('CEGUIBase', ''))
		libs.append(('CEGUIOpenGLRenderer', 'CEGUI/RendererModules/OpenGL/CEGUIOpenGLRenderer.h'))

	if cegui_0:
		libs.append((('CEGUI-0', 'CEGUIBase-0'), ''))
		libs.append((('CEGUI-0-OPENGL', 'CEGUIOpenGLRenderer-0'), ''))
	
	return libs
	
def createFifechanEnv(standard_env):
	fifechan_lib_env = standard_env.Clone(LIBS = ['python2.7', 'fifechan'])
		
	return fifechan_lib_env

def getOptionalLibs(opengl):
	libs = [('tinyxml', 'tinyxml.h')]
	
	return libs
	
# vim: set filetype=python:
			   
