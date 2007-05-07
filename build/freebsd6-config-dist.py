# Needed pkg-config additions:
# lualib50 & lua
# SDL_ttf
# SDL_image
# guichan, guichan_opengl & guichan_sdl
# boost_filesystem & boost_regex

def initEnvironment(env):
	return env

def addExtras(context):
	context.env.Append(CPPPATH = ['/usr/local/include/vorbis'])
	context.checkSimpleLib(['vorbisfile'], 'vorbisfile.h')
	context.checkSimpleLib(['openal'], 'AL/al.h')

	if context.env['script'] == 'lua':
		context.env.Append(LIBS = ['lua'])
		context.checkSimpleLib(['lua5.1'])
		# context.checkSimpleLib(['lualib50', 'lua'], 'lualib.h')

	context.checkSimpleLib(['sdl11'], 'SDL.h')
	context.checkSimpleLib(['SDL_ttf'], 'SDL_ttf.h')
	context.checkSimpleLib(['SDL_image'], 'SDL_image.h')
	context.checkSimpleLib(['guichan'])
	context.checkSimpleLib(['guichan_sdl'])
	context.checkSimpleLib(['boost_filesystem'])
	context.checkSimpleLib(['boost_regex'])

	if context.env['opengl']:
		context.env.Append(LIBS = ['GL', 'GLU'])
		context.env.Append(CPPPATH = ['/usr/X11R6/include'])
		context.env.Append(LIBPATH = ['/usr/X11R6/lib'])
		context.checkSimpleLib(['guichan_opengl'])
