import os
def initEnvironment(env):
	env = env.Copy(tools = ['mingw', 'qt4'], toolpath = '.')
	env.EnableQt4Modules(["QtGui","QtCore","QtNetwork","QtOpenGL","QtXml","QtSvg","QtSql","QtTest"])

	return env

def addExtras(context):
	path = os.environ['_']
	qtlibpath = os.environ['QTLIBPATH']
	qtdir = os.environ['QTDIR']
	#Library include path setup
	context.env.Append(CPPPATH = [path + '\\SDL_image', path + '\\zlib', path + '\\guichan', path + '\\boost', path, path + '\\sdl_ttf\\include', path + '\\sdl\\include'])
	#FIFE source path setup
	context.env.Append(CPPPATH = ['..\\..\\core\\src\\engine'])
	context.env.Append(CPPPATH = [qtdir + '\\include\\Qt'])
	context.env.Append(LIBPATH = [path + '\\lib_diverse', qtlibpath, '..\\..\\core\\src\\engine'])
	
	context.env.Append(LIBS = ['fife', 'mingw32', 'zlib', 'SDL_image', 'guichan', 'guichan_sdl', 'SDLmain', 'SDL', 'SDL_ttf', 'boost_filesystem', 'boost_regex'])
	context.env.Append(LIBS = ['libqtgui4', 'libqtmain', 'libqtcore4'])
