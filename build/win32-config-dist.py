import os
def initEnvironment(env):
	env = env.Copy(tools = ['mingw'])
	return env

#Note, if you aren't running from the batch file, you should set an environment variable _ to hold the path to the libraries

def addExtras(context):
	path = os.environ['_']
	context.env.Append(CPPPATH = [path, path + '\\include', path + '\\include\\openal', path + '\\include\\sdl_image', path + '\\include\\zlib', path + '\\include\\guichan', path + '\\include\\boost', path + '\\include\\libvorbis', path + '\\include\\sdl_ttf', path + '\\include\\sdl', path + '\\include\\png', path + '\\include\\python'])
	context.env.Append(LIBPATH = [path + '\\static_libs', path + '\\static_libs\\python'])
	
	context.env.Append(LIBS = ['guichan_sdl', 'guichan', 'mingw32', 'zlib', 'vorbis', 'ogg', 'vorbisfile', 'SDL_image', 'SDLmain', 'SDL.dll', 'OpenAL32', 'SDL_ttf', 'boost_filesystem', 'boost_regex', 'png', 'python25'])

	if context.env['opengl']:
		context.env.Append(CPPPATH = [path + '\\include\\opengl'])
		context.env.Prepend(LIBS = ['guichan_opengl'])
		context.env.Append(LIBS = ['opengl32', 'glu32'])

	if context.env['zip']:
		context.env.Append(CPPPATH = [path + '\\include\\minizip'])
		context.env.Prepend(LIBS = ['minizip'])
