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

import os,sys
from distutils.sysconfig import get_python_inc

def initEnvironment(env):

		
	include_dirs = ['/opt/local/include',
		        '/usr/local/include',
			'/usr/X11/include',
			'/opt/local/include/SDL',
			'/opt/local/include/fifechan',
			'/opt/local/include/boost',
			'/System/Library/Frameworks/OpenAL.framework/Headers',
			'/opt/local/include/vorbis/',
			get_python_inc()]

	lib_dirs = ['/System/Library', '/opt/local/lib', '/usr/X11/lib']

	env.Append(CPPPATH = include_dirs)
	env.Append(LIBPATH = lib_dirs)

	return env
	
def addExtras(env, opengl):
	env.Append(SHLINKFLAGS='-F/opt/local/Library/Frameworks')
	env.Append(SHLINKFLAGS='-framework OpenAL')
	env.Append(SHLINKFLAGS='-framework Cocoa')
	env.Append(SHLINKFLAGS='-framework CoreFoundation')
	env.Append(SHLINKFLAGS='-framework Python')
	
	env.Prepend(CXXFLAGS = '-DUSE_COCOA')
	
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
		('png', ''),
		('SDL', ''),
		('SDL_image', ''),
		('SDL_ttf', ''),
		('vorbisfile', 'vorbisfile.h'),
		('z', 'zlib.h'),
		('boost_filesystem-mt', ''),
		('boost_system-mt', ''),
		('boost_regex-mt', '')]
	
	opengl = reqLibs['opengl']
	fifechan = reqLibs['fifechan']
	librocket = reqLibs['librocket']
	cegui = reqLibs['cegui']
	
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
			
	return libs

def getOptionalLibs(opengl):
	libs = [('tinyxml', 'tinyxml.h')]
	
	return libs

