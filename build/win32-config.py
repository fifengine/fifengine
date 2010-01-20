import os
import SCons.Util

def initEnvironment(env):
	#a hack to stop passing the -nologo flag to mingw
	env['CCFLAGS'] = SCons.Util.CLVar('') 
	
	path = os.getcwd()
	
	swigdir = os.path.join(path, 'build', 'win32', 'applications', 'swigwin-1.3.40')
	mingwbindir = os.path.join(path, 'build', 'win32', 'applications', 'mingw', 'bin')
	
	env.AppendENVPath('PATH', swigdir)
	env.AppendENVPath('PATH', mingwbindir)
	
	includepath = os.path.join(path, 'build', 'win32', 'includes')
	staticlibpath = os.path.join(path, 'build', 'win32', 'static_libs', 'mingw')

	env.Append(CPPPATH = [includepath + '\\libogg', includepath + '\\openal', includepath + '\\sdl_image', includepath + '\\zlib', includepath + '\\libguichan', includepath + '\\boost_1_38_0', includepath + '\\libvorbis', includepath + '\\libpng', includepath + '\\sdl_ttf', includepath + '\\sdl', includepath + '\\python26', includepath + '\\unittest++'])
	env.Append(LIBPATH = [staticlibpath, staticlibpath + '\\python26'])
	
	env.Tool('swig')
	env.Tool('mingw')
	
	return env
	
	
def addExtras(env, opengl):
	env.Append(LIBS = ['libguichan_sdl', 'libguichan', 'mingw32', 'zlib', 'vorbis', 'ogg', 'vorbisfile', 'libpng', 'SDL_image', 'SDLmain', 'SDL.dll', 'OpenAL32', 'SDL_ttf', 'boost_filesystem', 'boost_regex', 'boost_system', 'python26'])

	if opengl:
		env.Prepend(LIBS = ['libguichan_opengl'])
		env.Append(LIBS = ['opengl32', 'glu32'])

	return env


def getRequiredHeaders(opengl):
	return None

def getRequiredLibs(opengl):
	return None

def getOptionalLibs(opengl):
	return None
