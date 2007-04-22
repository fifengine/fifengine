def initEnvironment(env):
	env = env.Copy(tools = ['mingw'])
	return env

def addExtras(context):
	context.env.Append(CPPPATH = ['C:\\FIFE_compile\\OpenAL\\Include', 'C:\\FIFE_compile\\SDL_image', 'C:\\FIFE_compile\\zlib', 'C:\\FIFE_compile\\lua\\include', 'C:\\FIFE_compile\\guichan', 'C:\\FIFE_compile\\boost', 'C:\\FIFE_compile\\libvorbis', 'C:\\FIFE_compile', 'C:\\FIFE_compile\\sdl_ttf\\include', 'C:\\FIFE_compile\\sdl\\include'])
	context.env.Append(LIBPATH = ['C:\\FIFE_compile\\lib_diverse'])
	
	context.env.Append(LIBS = ['fife', 'guichanfife', 'mingw32', 'zlib', 'SDL_image', 'lua50', 'guichan', 'guichan_sdl', 'vorbis', 'ogg', 'vorbisfile', 'SDLmain', 'SDL', 'OpenAL32', 'SDL_ttf'])

	if context.env['opengl']:
		context.env.Append(CPPPATH = ['C:\\FIFE_compile\\opengl\\include'])
		context.env.Append(LIBS = ['guichan_opengl', 'opengl32', 'glu32'])