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

import os,sys
import platform
from distutils.sysconfig import get_python_inc

def initEnvironment(env):


	include_dirs = ['/usr/local/include',
		        '/usr/local/include',
			'/usr/X11/include',
			'/usr/local/include/SDL',
			'/usr/local/include/fifechan',
			'/usr/local/include/boost',
			'/System/Library/Frameworks/OpenAL.framework/Headers',
			'/usr/local/include/vorbis/',
			get_python_inc()]

	lib_dirs = ['/System/Library', '/usr/local/lib', '/usr/X11/lib']

	env.Append(CPPPATH = include_dirs)
	env.Append(LIBPATH = lib_dirs)

	env.Tool('swig')

	return env

def addExtras(env, opengl):
	env.Append(SHLINKFLAGS='-framework OpenAL')
	env.Append(SHLINKFLAGS='-framework Cocoa')
	env.Append(SHLINKFLAGS='-framework CoreFoundation')
	env.Append(SHLINKFLAGS='-framework Python')

	env.Prepend(CXXFLAGS = '-DUSE_COCOA')
        version = platform.mac_ver()[0]
        major, minor, patch = [int(v) for v in version.split('.')]
        if major >= 10 and minor >= 9:
            env.Prepend(CXXFLAGS = '-DOSX_109')

	if opengl:
		env.Append(SHLINKFLAGS='-framework OpenGL')

	# define for using tinyxml with stl support enabled
	env.AppendUnique(CPPDEFINES = ['TIXML_USE_STL'])

	return env

def getRequiredHeaders(opengl):
	return ['SDL/SDL_image.h',
			'SDL/SDL_ttf.h']

def getRequiredLibs(reqLibs):
	libs = [('objc', ''),
		(('libpng', 'png'), ''),
		('SDL', ''),
		('SDL_image', ''),
		('SDL_ttf', ''),
		('vorbisfile', 'vorbisfile.h'),
		('z', 'zlib.h'),
		('boost_filesystem', ''),
		('boost_system', ''),
		('boost_regex', '')]

	opengl = reqLibs['opengl']
	fifechan = reqLibs['fifechan']
	librocket = reqLibs['librocket']
	cegui = reqLibs['cegui']
	cegui_0 = reqLibs['cegui-0']

	if fifechan:
		libs.append(('fifechan', ''))
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
	fifechan_lib_env = standard_env.Clone(LIBS = ['fifechan'])

	return fifechan_lib_env

def getOptionalLibs(opengl):
	libs = [('tinyxml', 'tinyxml.h')]

	return libs

