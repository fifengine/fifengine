import os,sys
from distutils.sysconfig import get_python_inc

def initEnvironment(env):

		
	include_dirs = ['/opt/local/include',
		        '/usr/local/include',
			'/usr/X11/include',
			'/opt/local/include/SDL',
			'/opt/local/include/guichan',
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
	
	return env
	
def getRequiredHeaders(opengl):
	return ['SDL/SDL_image.h',
			'SDL/SDL_ttf.h']

def getRequiredLibs(opengl):
	libs = [('objc', ''),
		('png', ''),
		('SDL', ''),
		('SDL_image', ''),
		('SDL_ttf', ''),
		('vorbisfile', 'vorbisfile.h'),
		('guichan', ''),
		('guichan_sdl', ''),
		('z', 'zlib.h'),
		('boost_filesystem-mt', ''),
		('boost_system-mt', ''),
		('boost_regex-mt', '')]
	
	if opengl:
		libs.append(('guichan_opengl', ''))
		
	return libs

def getOptionalLibs(opengl):
	return None

