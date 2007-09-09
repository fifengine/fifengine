def initEnvironment(env):
	return env

def addExtras(context):
	if context.env['audio'] == 1:
		context.env.Append(CPPPATH = ['/usr/include/vorbis'])
		context.checkSimpleLib(['vorbisfile'], 'vorbisfile.h')
		context.checkSimpleLib(['openal'], 'AL/al.h')
	if context.env['script'] == 'lua':
		context.checkSimpleLib(['lualib50', 'lua'], 'lualib.h')
	context.checkSimpleLib(['SDL'], 'SDL.h')
	context.checkSimpleLib(['SDL_ttf'], 'SDL_ttf.h')
	context.checkSimpleLib(['SDL_image'], 'SDL_image.h')
	context.checkSimpleLib(['guichan'])
	context.checkSimpleLib(['guichan_sdl'])

	if context.env['opengl']:
		context.env.Append(LIBS = ['GL', 'GLU'])
		context.checkSimpleLib(['guichan_opengl'])
	else:
		context.checkSimpleLib(['guichan_sdl'])
