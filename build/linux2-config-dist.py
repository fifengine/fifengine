def initEnvironment(env):
	return env

def addExtras(context):
	context.env.Append(LIBS = 'm')
	context.env.Replace(LIBPATH = ['/opt/lib', '%s/ext/install/lib' % context.env['SCONS_ROOT_PATH']])
	context.env.Append(CPPPATH = ['/opt/include', 
	                              '/usr/include/vorbis', 
	                              '/usr/include/python2.4',
	                              '/usr/include/python2.5',
	                              '%s/ext/install/include' % context.env['SCONS_ROOT_PATH']])
	context.env.Append(CPPPATH = [])	
	
	context.checkSimpleLib(['vorbisfile'], 'vorbisfile.h')
	context.checkSimpleLib(['openal'], 'AL/al.h')
	context.checkSimpleLib(['python2.4', 'python2.5'], ['python2.4/Python.h', 'python2.5/Python.h'])
	if context.env['script'] == 'lua':
		context.checkSimpleLib(['lua5.1', 'lua-5.1', 'lua'], 'lua.h')
	context.checkSimpleLib(['SDL'], 'SDL.h')
	context.checkSimpleLib(['SDL_ttf'], 'SDL_ttf.h')
	context.checkSimpleLib(['SDL_image'], 'SDL_image.h')
	context.checkSimpleLib(['guichan'])
	context.checkSimpleLib(['guichan_sdl'])
	context.checkSimpleLib(['boost_filesystem', 'boost_filesystem-gcc'])
	context.checkSimpleLib(['boost_regex', 'boost_regex-gcc'])
	
	if context.env['opengl']:
		context.env.Append(LIBS = ['GL', 'GLU'])
		context.env.Append(LIBPATH = ['/usr/X11R6/lib'])
		context.checkSimpleLib(['guichan_opengl'])
	
	if context.env['zip']:
		context.checkSimpleLib(['minizip'])
