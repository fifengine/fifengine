import os
def initEnvironment(env):
	env = env.Copy(tools = ['mingw'])
	return env

#Note, if you aren't running from the batch file, you should set an environment variable _ to hold the path to the libraries

def addExtras(context):
	path = os.environ['_']
	context.env.Append(CPPPATH = [path + '\\OpenAL\\Include', path + '\\SDL_image', path + '\\zlib', path + '\\lua51\\include', path + '\\guichan', path + '\\boost', path + '\\libvorbis', path, path + '\\sdl_ttf\\include', path + '\\sdl\\include'])
	context.env.Append(LIBPATH = [path + '\\lib_diverse'])
	
	context.env.Append(LIBS = ['fife', 'mingw32', 'zlib', 'SDL_image', 'lua51', 'guichan', 'guichan_sdl', 'vorbis', 'ogg', 'vorbisfile', 'SDLmain', 'SDL', 'OpenAL32', 'SDL_ttf', 'boost_filesystem', 'boost_regex'])

	if context.env['opengl']:
		context.env.Append(CPPPATH = [path + '\\opengl\\include'])
		context.env.Append(LIBS = ['guichan_opengl', 'opengl32', 'glu32'])
