# Needed pkg-config additions:
# SDL_ttf & SDL_image
# guichan, guichan_opengl & guichan_sdl
# boost_filesystem & boost_regex
# vorbisfile
# openal

import sys

def initEnvironment(env):
	return env

def addExtras(context):
	context.env.Append(CPPPATH = ['/usr/local/include/vorbis'])
	context.checkSimpleLib(['vorbisfile'], 'vorbisfile.h')
	context.checkSimpleLib(['openal'], 'AL/al.h')

	context.checkSimpleLib(['sdl11', 'sdl12', 'sdl'], 'SDL.h')
	context.checkSimpleLib(['SDL_ttf'], 'SDL_ttf.h')
	context.checkSimpleLib(['SDL_image'], 'SDL_image.h')
	context.checkSimpleLib(['guichan'])
	context.checkSimpleLib(['guichan_sdl'])
	context.checkSimpleLib(['boost_filesystem'])
	context.checkSimpleLib(['boost_regex'])
	context.checkSimpleLib(['xcursor'])
	context.checkSimpleLib(['png'], 'png.h')

	if context.env['opengl']:
		context.env.Append(LIBS = ['GL', 'GLU'])
		context.env.Append(CPPPATH = ['/usr/local/include'])
		context.env.Append(LIBPATH = ['/usr/local/lib'])
		context.checkSimpleLib(['guichan_opengl'])
        # For swig/python:
        context.env.Append(CPPPATH = ['/usr/local/include/python%s'%sys.version[:3]])
