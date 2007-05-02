import os
def initEnvironment(env):
	env = env.Copy(tools = ['mingw'])
	return env

#Note, if you aren't running from the batch file, you should set an environment variable _ to hold the path to the libraries

def addExtras(context):
	path = os.environ['_']
	context.env.Append(CPPPATH = [path + '\\SDL_image', path + '\\sdl\\include', path + '\\boost', path + '\\zlib', path + '\\png'])
	context.env.Append(LIBPATH = [path + '\\lib_diverse'])
	
	context.env.Append(LIBS = ['fife', 'mingw32', 'zlib', 'SDL_image', 'SDLmain', 'SDL', 'png', 'boost_filesystem', 'boost_regex'])
