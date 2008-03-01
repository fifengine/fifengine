import os

def initEnvironment(env):
    env.Append(ENV={'PATH' : os.environ['PATH']})
    env.Append(ENV={'PKG_CONFIG_PATH' : os.environ['PKG_CONFIG_PATH']})
    return env

def addExtras(context):
    context.env.Append(LIBPATH = ['/System/Library'])
    context.env.Append(SHLINKFLAGS='-framework OpenAL')
    context.env.Append(SHLINKFLAGS='-framework Cocoa')
    context.env.Append(SHLINKFLAGS='-framework CoreFoundation')
    context.env.Append(SHLINKFLAGS='-framework OpenGL')
    context.env.Append(SHLINKFLAGS='-framework SDL')
    context.env.Append(SHLINKFLAGS='-framework SDL_image')
    context.env.Append(SHLINKFLAGS='-framework SDL_ttf')
    context.env.Append(SHLINKFLAGS='-framework Python')

    include_dirs = ['/opt/local/include', '/usr/local/include', '/sw/include', '/Library/Frameworks/SDL.framework/Headers',
        '/Library/Frameworks/SDL_image.framework/Headers', '/Library/Frameworks/SDL_ttf.framework/Headers', '/usr/local/include/boost-1_33_1',
        '/System/Library/Frameworks/OpenAL.framework/Headers', '/usr/local/include/vorbis', '/opt/local/include/vorbis/', 
	'/opt/local/include/python2.5']
    context.env.Append(CPPPATH = include_dirs)

    lib_dirs = ['/opt/local/lib','/usr/local/lib', '/sw/lib']
    context.env.Append(LIBPATH = lib_dirs)

    context.checkSimpleLib(['vorbisfile'], 'vorbisfile.h')

    context.env.Append(CPPFLAGS='-D_THREAD_SAFE')

    context.CheckHeader('SDL_image.h')
    context.CheckHeader('SDL_ttf.h')
    context.checkSimpleLib(['guichan'])
    context.checkSimpleLib(['guichan_sdl'])
    context.checkSimpleLib(['z'], 'zlib.h')
    context.checkSimpleLib(['boost_filesystem'], ['boost/filesystem/convenience.hpp'], 'cxx')
    context.checkSimpleLib(['boost_regex'], ['boost/regex.hpp'], 'cxx')

    if context.env['opengl']:
        context.checkSimpleLib(['guichan_opengl'], ['guichan/opengl.hpp'], 'cxx')
#        context.env.Append(LINKFLAGS = '-framework OpenGL')
