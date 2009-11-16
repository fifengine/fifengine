import os
def initEnvironment(env):
	env = env.Copy(tools = ['mingw'])
	return env

#Note, if you aren't running from the batch file, you should set an environment variable _ to hold the path to the libraries

def addExtras(context):
	path = os.environ['_']
	includepath = path + '\\..\\..\\includes'
	staticlibpath = path + '\\..\\..\\static_libs\\mingw'

	context.env.Append(CPPPATH = [includepath + '\\libogg', includepath + '\\openal', includepath + '\\sdl_image', includepath + '\\zlib', includepath + '\\libguichan', includepath + '\\boost_1_38_0', includepath + '\\libvorbis', includepath + '\\libpng', includepath + '\\sdl_ttf', includepath + '\\sdl', includepath + '\\python26', includepath + '\\unittest++'])
	context.env.Append(LIBPATH = [staticlibpath, staticlibpath + '\\python26'])
	
	context.env.Append(LIBS = ['libguichan_sdl', 'libguichan', 'mingw32', 'zlib', 'vorbis', 'ogg', 'vorbisfile', 'libpng', 'SDL_image', 'SDLmain', 'SDL.dll', 'OpenAL32', 'SDL_ttf', 'boost_filesystem', 'boost_regex', 'boost_system', 'python26'])

	if context.env['opengl']:
		context.env.Prepend(LIBS = ['libguichan_opengl'])
		context.env.Append(LIBS = ['opengl32', 'glu32'])
