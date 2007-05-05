import os
def initEnvironment(env):
	env = env.Copy(tools = ['mingw'])
	return env

#Note, if you aren't running from the batch file, you should set an environment variable _ to hold the path to the libraries

def addExtras(context):
	path = os.environ['_']
	context.env.Append(CPPPATH = [path + '\\include', path + '\\include\\openal\\Include', path + '\\include\\sdl_image', path + '\\include\\zlib', path + '\\include\\\lua51\\include', path + '\\include\\guichan', path + '\\include\\boost', path + '\\include\\libvorbis', path, path + '\\include\\sdl_ttf\\include', path + '\\include\\sdl\\include'])
	context.env.Append(LIBPATH = [path + '\\static_libs'])
	
	context.env.Append(LIBS = ['fife', 'mingw32', 'zlib', 'SDL_image', 'lua51', 'guichan', 'guichan_sdl', 'vorbis', 'ogg', 'vorbisfile', 'SDLmain', 'SDL', 'OpenAL32', 'SDL_ttf', 'boost_filesystem', 'boost_regex'])

	if context.env['opengl']:
		context.env.Append(CPPPATH = [path + '\\include\\\opengl\\include'])
		context.env.Append(LIBS = ['guichan_opengl', 'opengl32', 'glu32'])
