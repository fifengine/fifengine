def initEnvironment(env):
	return env

def addExtras(context):
	context.env.Append(LIBS = 'm')
	context.env.Append(LIBPATH = "/opt/lib")
	context.env.Append(CPPPATH = "/opt/include")
		
	context.checkSimpleLib(['fife'])
	context.checkSimpleLib(['SDL'], 'SDL.h')
	context.checkSimpleLib(['SDL_image'], 'SDL_image.h')
	context.checkSimpleLib(['png'], 'png.h')
