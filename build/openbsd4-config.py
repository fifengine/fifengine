# Install the following packages: libvorbis python py-yaml swig png openal sdl
# sdl-image sdl-ttf guichan boost
#

import sys
import os

def initEnvironment(env):
	# Vorbisfile
	env.Append(CPPPATH = [
		os.path.join('/', 'usr', 'local', 'include'),
		os.path.join('/', 'usr', 'local', 'include', 'vorbis')
		])
	env.Append(LIBPATH = [os.path.join('/', 'usr', 'local', 'lib')])
	env.Append(LIBS = ['ogg', 'vorbis'])

	# PNG
	env.Append(CPPPATH = [os.path.join('/', 'usr', 'local', 'include', 'libpng')])

	# Python library
	pythonversion = 'python' + str(os.sys.version_info[0]) + '.' + str(os.sys.version_info[1])
	env.Append(CPPPATH = os.path.join('/', 'usr', 'local', 'include', pythonversion))
	env.Append(LIBS = [pythonversion, 'util'])

	# OpenBSD specific pthreads option
	env.Append(LINKFLAGS = ['-pthread'])

	env.Tool('swig')

	return env

def addExtras(env, opengl):
	if opengl:
		env.Append(LIBS = ['stdc++', 'GL'])
		env.Append(LIBPATH = os.path.join('/', 'usr', 'X11R6', 'lib'))

	# define for using tinyxml with stl support enabled
	env.AppendUnique(CPPDEFINES = ['TIXML_USE_STL'])
	
	return env

def getRequiredHeaders(opengl):
	return None

def getRequiredLibs(reqLibs):
	libs = [
			('vorbisfile', 'vorbisfile.h'),
			('openal', 'AL/al.h'),
			('SDL', 'SDL.h'),
			('SDL_ttf', 'SDL_ttf.h'),
			('SDL_image', 'SDL_image.h'),
			('boost_system', ''),
			(('boost_filesystem', 'boost_filesystem-gcc', 'boost_filesystem-gcc41', 'boost_filesystem-mt'), 'boost/filesystem.hpp'),
			(('boost_regex', 'boost_regex-gcc', 'boost_regex-gcc41', 'boost_regex-mt'), 'boost/regex.hpp'),
			(('libpng', 'png'), 'png.h'),
			('Xcursor', 'X11/Xcursor/Xcursor.h')]

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

# vim: ft=python:

