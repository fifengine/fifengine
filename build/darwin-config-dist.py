import os, sys

def initEnvironment(env):
	env.Append(ENV={'PATH' : os.environ['PATH']})
	env.Prepend(CXXFLAGS = '-DUSE_COCOA')
	return env

def addExtras(context):
	context.env.Append(LIBPATH = ['/System/Library','/opt/local/lib'])
	context.env.Append(SHLINKFLAGS='-F/opt/local/Library/Frameworks')
	context.env.Append(SHLINKFLAGS='-framework OpenAL')
	context.env.Append(SHLINKFLAGS='-framework Cocoa')
	context.env.Append(SHLINKFLAGS='-framework CoreFoundation')
	context.env.Append(SHLINKFLAGS='-framework OpenGL')
	context.env.Append(SHLINKFLAGS='-framework SDL')
	context.env.Append(SHLINKFLAGS='-framework SDL_image')
	context.env.Append(SHLINKFLAGS='-framework SDL_ttf')
	context.env.Append(SHLINKFLAGS='-framework Python')
	context.env.Append(SHLINKFLAGS='-lpng')
	context.env['SHLIBSUFFIX']='.so'

	include_dirs = ['/opt/local/include',
			'/usr/local/include',
			'/usr/X11/include',
			'/Library/Frameworks/SDL.framework/Headers',
			'/opt/local/Library/Frameworks/SDL.framework/Headers',
			'/Library/Frameworks/SDL_image.framework/Headers',
			'/opt/local/Library/Frameworks/SDL_image.framework/Headers',
			'/Library/Frameworks/SDL_ttf.framework/Headers',
			'/opt/local/Library/Frameworks/SDL_ttf.framework/Headers',
			'/opt/local/include/boost',
			'/System/Library/Frameworks/OpenAL.framework/Headers',
			'/opt/local/include/vorbis/',
			'/usr/include/python%s'%sys.version[:3],
			'/opt/local/include/guichan',
			'ext/install/include',
			'../ext/install/include',
			'../ext/install/include/guichan']

	context.env.Append(CPPPATH = include_dirs)

	lib_dirs = ['/opt/local/lib','/usr/local/lib','/usr/X11/lib','ext/install/lib','../ext/install/lib']
	context.env.Append(LIBPATH = lib_dirs)

	context.env.Append(CPPFLAGS='-D_THREAD_SAFE -F/opt/local/Library/Frameworks')

	context.CheckHeader('SDL/SDL_image.h')
	context.CheckHeader('SDL/SDL_ttf.h')
	context.checkSimpleLib(['objc'])
	context.checkSimpleLib(['vorbisfile'], 'vorbisfile.h')
	context.checkSimpleLib(['guichan'])
	context.checkSimpleLib(['guichan_sdl'])
	context.checkSimpleLib(['z'], 'zlib.h')
	context.checkSimpleLib(['boost_filesystem-mt'])
	context.checkSimpleLib(['boost_system-mt'])
	context.checkSimpleLib(['boost_regex-mt'])

	if context.env['opengl']:
		context.checkSimpleLib(['guichan_opengl'], ['guichan/opengl.hpp'], 'cxx')
		context.env.Append(SHLINKFLAGS='-framework OpenGL')
