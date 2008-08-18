def initEnvironment(env):
	return env

def addExtras(context):
	context.env.Append(LIBS = 'm')
	context.env.Replace(LIBPATH = ['/opt/lib', '%s/ext/install/lib' % context.env['SCONS_ROOT_PATH']])
	context.env.Append(CPPPATH = ['/opt/include',
	                              '/usr/include/vorbis',
	                              '/usr/include/python2.5',
	                              '%s/ext/install/include' % context.env['SCONS_ROOT_PATH']])
	context.env.Append(CPPPATH = [])	
	
	context.checkSimpleLib(['vorbisfile'], 'vorbisfile.h')
	context.checkSimpleLib(['openal_soft'], 'AL/al.h')
	context.checkSimpleLib(['python', 'python2.5'], ['python2.5/Python.h'])
	context.checkSimpleLib(['SDL'], 'SDL.h')
	context.checkSimpleLib(['SDL_ttf'], 'SDL_ttf.h')
	context.checkSimpleLib(['SDL_image'], 'SDL_image.h')
	context.checkSimpleLib(['guichan'])
	context.checkSimpleLib(['guichan_sdl'])
	context.checkSimpleLib(['boost_filesystem', 'boost_filesystem-gcc', 'boost_filesystem-gcc41'])
	context.checkSimpleLib(['boost_regex', 'boost_regex-gcc', 'boost_regex-gcc41'])
	context.checkSimpleLib(['png'], 'png.h');
	
	if context.env['opengl']:
		# linking explicitly against libstdc++ to work around Segfault_in_cxa_allocate_exception issue: http://wiki.fifengine.de/Segfault_in_cxa_allocate_exception
		context.env.Append(LIBS = ['stdc++', 'GL', 'GLU'])
		context.env.Append(LIBPATH = ['/usr/X11R6/lib'])
		context.checkSimpleLib(['guichan_opengl'])
