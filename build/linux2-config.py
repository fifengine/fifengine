# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2010 by the FIFE team
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
						  extlibpath])
	
	env.AppendENVPath('LD_RUN_PATH', os.path.join('..', '..', '..', extlibpath))
	
	env.AppendUnique(CXXFLAGS=["-DPNG_SKIP_SETJMP_CHECK"])

	return env

def addExtras(env, opengl):
	env.Append(LIBS = pythonversion)

	if opengl:
		env.Append(LIBS = ['stdc++', 'GL', 'GLU'])
		env.Append(LIBPATH = os.path.join('/', 'usr', 'X11R6', 'lib'))
		
	return env

def getRequiredHeaders(opengl):
	return None

def getRequiredLibs(opengl):
	# libs is a list of tuples that have the form:
	#	(libname, headers)
	#	libname - may be a single library or a tuple of libraries
	#	headers - may be a single header or a list of headers
	#
	#	This list is somewhat order dependent
	#		guichan_sdl - depends on at least the SDL libs and guichan prior in the list
	#		guichan_opengl - depends on at least guichan prior in the list
	libs = [('vorbisfile', 'vorbisfile.h'),
			('openal', 'AL/al.h'),
			('SDL', 'SDL.h'),
			('SDL_ttf', 'SDL_ttf.h'),
			('SDL_image', 'SDL_image.h'),
			('guichan', 'guichan.hpp'),
			('guichan_sdl', ''),
			(('boost_filesystem', 'boost_filesystem-gcc', 'boost_filesystem-gcc41', 'boost_filesystem-mt', 'libboost_filesystem-mt'), 'boost/filesystem.hpp'),
			(('boost_regex', 'boost_regex-gcc', 'boost_regex-gcc41', 'boost_regex-mt', 'libboost_regex-mt'), 'boost/regex.hpp'),
			('png', 'png.h'),
			('Xcursor', '')]

	# fedora hack
	if 'fedora' in platform.platform():	
		libs.insert(0,('boost_system', ''))

	if (opengl):
		libs.append(('guichan_opengl', ''))
		
	return libs

def getOptionalLibs(opengl):
	return None
	
# vim: set filetype=python:
			   
