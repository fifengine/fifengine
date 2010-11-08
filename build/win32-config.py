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

import os
import SCons.Util

def initEnvironment(env):
	#a hack to stop passing the -nologo flag to mingw
	env['CCFLAGS'] = SCons.Util.CLVar('') 
	
	path = os.getcwd()
	
	swigdir = os.path.join(path, 'build', 'win32', 'applications', 'swigwin-2.0.1')
	mingwbindir = os.path.join(path, 'build', 'win32', 'applications', 'mingw', 'bin')
	
	env.PrependENVPath('PATH', swigdir)
	env.PrependENVPath('PATH', mingwbindir)
	
	includepath = os.path.join(path, 'build', 'win32', 'includes')
	staticlibpath = os.path.join(path, 'build', 'win32', 'static_libs', 'mingw')

	env.Append(CPPPATH = [includepath + '\\libogg', includepath + '\\openal', includepath + '\\sdl_image', includepath + '\\zlib', includepath + '\\libguichan', includepath + '\\boost_1_38_0', includepath + '\\libvorbis', includepath + '\\libpng', includepath + '\\sdl_ttf', includepath + '\\sdl', includepath + '\\python27', includepath + '\\unittest++\\src'])
	env.Append(LIBPATH = [staticlibpath, staticlibpath + '\\python27'])
	
	env.Tool('swig')
	env.Tool('mingw')
	
	return env
	
	
def addExtras(env, opengl):
	env.Append(LIBS = ['libguichan_sdl', 'libguichan', 'mingw32', 'zlib', 'vorbis', 'ogg', 'vorbisfile', 'libpng', 'SDL_image', 'SDLmain', 'SDL.dll', 'OpenAL32', 'SDL_ttf', 'boost_filesystem', 'boost_regex', 'boost_system'])

	if env['FIFE_DEBUG']:
		env.Append(LIBS = ['python27_d'])
	else:
		env.Append(LIBS = ['python27'])

	if opengl:
		env.Prepend(LIBS = ['libguichan_opengl'])
		env.Append(LIBS = ['opengl32', 'glu32'])

	return env


def getRequiredHeaders(opengl):
	return None

def getRequiredLibs(opengl):
	return None

def getOptionalLibs(opengl):
	return None
