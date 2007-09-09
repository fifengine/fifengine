def initEnvironment(env):
	env = env.Copy(tools = ['default', 'qt4'], toolpath = '.')
	env.EnableQt4Modules(["QtGui","QtCore","QtNetwork","QtOpenGL","QtXml","QtSvg","QtSql","QtTest"])

	return env

def addExtras(context):
	context.env.Append(LIBS = 'm')
	context.env.Append(LIBPATH = ["/opt/lib", "../../core/src/engine"])
	context.env.Append(CPPPATH = ["/opt/include", "../../core/src/engine"])

	context.checkSimpleLib(['SDL'], 'SDL.h')
	context.checkSimpleLib(['SDL_ttf'], 'SDL_ttf.h')
	context.checkSimpleLib(['SDL_image'], 'SDL_image.h')
	context.checkSimpleLib(['guichan'])
	context.checkSimpleLib(['guichan_sdl'])
	context.checkSimpleLib(['boost_filesystem', 'boost_filesystem-gcc'])
	context.checkSimpleLib(['boost_regex', 'boost_regex-gcc'])
	context.checkSimpleLib(['libQtGui'])
	context.checkSimpleLib(['libQtCore'])

	if context.env['opengl']:
		context.env.Append(LIBS = ['GL', 'GLU'])
		context.checkSimpleLib(['guichan_opengl'])

	context.env.Append(LIBS = 'fife')
