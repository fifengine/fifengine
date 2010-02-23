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

	return env

def addExtras(env, opengl):
	if opengl:
		env.Append(LIBS = ['stdc++', 'GL', 'GLU'])
		env.Append(LIBPATH = os.path.join('/', 'usr', 'X11R6', 'lib'))

	return env

def getRequiredHeaders(opengl):
	return None

def getRequiredLibs(opengl):
	libs = [
			('vorbisfile', 'vorbisfile.h'),
			('openal', 'AL/al.h'),
			('SDL', 'SDL.h'),
			('SDL_ttf', 'SDL_ttf.h'),
			('SDL_image', 'SDL_image.h'),
			('guichan', 'guichan.hpp'),
			('guichan_sdl', ''),
			(('boost_filesystem', 'boost_filesystem-gcc', 'boost_filesystem-gcc41', 'boost_filesystem-mt'), 'boost/filesystem.hpp'),
			(('boost_regex', 'boost_regex-gcc', 'boost_regex-gcc41', 'boost_regex-mt'), 'boost/regex.hpp'),
			('png', 'png.h'),
			('Xcursor', 'X11/Xcursor/Xcursor.h')]

	if (opengl):
		libs.append(('guichan_opengl', ''))

	return libs

def getOptionalLibs(opengl):
	return None

# vim: ft=python:

