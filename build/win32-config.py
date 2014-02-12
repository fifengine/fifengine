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

import os
import SCons.Util

def initEnvironment(env):
	#a hack to stop passing the -nologo flag to mingw
	env['CCFLAGS'] = SCons.Util.CLVar('') 
	
	path = os.getcwd()
	
	swigdir = os.path.join(path, 'build', 'win32', 'applications', 'swigwin-2.0.8')
	mingwbindir = os.path.join(path, 'build', 'win32', 'applications', 'mingw', 'bin')
	
	env.PrependENVPath('PATH', swigdir)
	env.PrependENVPath('PATH', mingwbindir)
	
	includepath = os.path.join(path, 'build', 'win32', 'includes')
	staticlibpath = os.path.join(path, 'build', 'win32', 'static_libs', 'mingw')

	env.Append(CPPPATH = [includepath + '\\libogg', includepath + '\\openal', includepath + '\\sdl_image', includepath + '\\zlib', includepath + '\\boost_1_47_0', includepath + '\\libvorbis', includepath + '\\libpng', includepath + '\\sdl_ttf', includepath + '\\sdl', includepath + '\\python27', includepath + '\\unittest++\\src'])
	if env['ENABLE_FIFECHAN']:
		env.Append(CPPPATH = [includepath + '\\libfifechan'])
	
	if env['ENABLE_LIBROCKET']:
		env.Append(CPPPATH = [includepath + '\\Rocket'])
		
	if env['ENABLE_CEGUI']:
		env.Append(CPPPATH = [includepath + '\\CEGUI'])
	
	if env['ENABLE_CEGUI_0']:
		env.Append(CPPPATH = [includepath + '\\CEGUI-0'])
		env.Append(CPPPATH = [includepath + '\\CEGUI-0\cegui-0'])

	env.Append(LIBPATH = [staticlibpath, staticlibpath + '\\python27'])
	
	env.Tool('swig')
	env.Tool('mingw')
	
	return env
	
def addExtras(env, reqLibs):
	env.Append(LIBS = ['mingw32', 'zlib', 'vorbis', 'ogg', 'vorbisfile', 'libpng', 'SDL_image', 'SDLmain', 'SDL.dll', 'OpenAL32', 'SDL_ttf', 'boost_filesystem', 'boost_regex', 'boost_system'])

	opengl = reqLibs['opengl']
	fifechan = reqLibs['fifechan']
	librocket = reqLibs['librocket']
	cegui = reqLibs['cegui']
	cegui_0 = reqLibs['cegui-0']
	if env['FIFE_DEBUG']:
		env.Append(LIBS = ['python27_d'])
	else:
		env.Append(LIBS = ['python27'])

	if fifechan:
		env.Prepend(LIBS = ['libfifechan_sdl', 'libfifechan']) 
		if opengl:
			env.Prepend(LIBS = ['libfifechan_opengl'])

	if opengl:
		env.Append(LIBS = ['opengl32'])

	if librocket:
		rocket_libs = ['libRocketCore', 'libRocketControls']
		if env['FIFE_DEBUG']:
			rocket_libs.append('libRocketDebugger')
		
		env.Prepend(LIBS = rocket_libs)
		
	if cegui:
		cegui_libs = ['libCEGUIBase', 'libCEGUIOpenGLRenderer' ]
		env.Prepend(LIBS = cegui_libs)
	if cegui_0:
		cegui_0_libs = ['libCEGUIBase-0', 'libCEGUIOpenGLRenderer-0' ]    
		env.Prepend(LIBS = cegui_0_libs)
		
	# define for using tinyxml with stl support enabled
	env.AppendUnique(CPPDEFINES = ['TIXML_USE_STL'])
	
	return env
	
def createFifechanEnv(standard_env):
	fifechan_lib_env = standard_env.Clone(LIBS = ['libfifechan'])
	if standard_env['FIFE_DEBUG']:
		fifechan_lib_env.Append(LIBS = ['python27_d'])
	else:
		fifechan_lib_env.Append(LIBS = ['python27'])
		
	return fifechan_lib_env


def getRequiredHeaders(opengl):
	return None

def getRequiredLibs(opengl):
	return None

def getOptionalLibs(opengl):
	libs = [('tinyxml', 'tinyxml.h')]
	
	return libs
