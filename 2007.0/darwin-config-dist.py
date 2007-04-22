import os

def initEnvironment(env):
    env.Append(ENV={'PATH' : os.environ['PATH']})
    return env

def addExtras(context):
    context.env.Append(LINKFLAGS='-framework OpenAL')
    context.env.Append(LINKFLAGS='-framework Cocoa')
    context.env.Append(LINKFLAGS='-framework CoreFoundation')

    include_dirs = ['/opt/local/include', '/usr/local/include', '/sw/include', '/usr/X11R6/include']
    dirs = [d for d in include_dirs]
    for d in dirs:
        include_dirs += [d+'/vorbis']
    context.env.Append(CPPPATH = include_dirs)

    lib_dirs = ['/opt/local/lib', '/usr/local/lib', '/sw/lib', '/usr/X11R6/lib']
    context.env.Append(LIBPATH = lib_dirs)

    #OpenAL is a framework on OS X so it's harder for scons to find it
    #context.checkSimpleLib(['openal'], 'AL/al.h')

    context.checkSimpleLib(['vorbisfile'], 'vorbisfile.h')
    context.checkSimpleLib(['lua'], 'lua.h', 'c', 0)
    context.checkSimpleLib(['lualib.5'], 'lualib.h', 'c', 0)

#    context.checkSimpleLib(['SDL'], 'SDL.h')
    context.env.Append(LINKFLAGS='-framework OpenGL')
    context.env.Append(LIBS=['SDLmain', 'SDL'])
    context.env.Append(LIBPATH='/opt/local/lib')
    context.env.Append(CPPPATH='/opt/local/include/SDL')
    context.env.Append(CPPFLAGS='-D_THREAD_SAFE')

    #context.checkSimpleLib(['SDL_image'], 'SDL_image.h')
    #context.checkSimpleLib(['SDL_ttf'], 'SDL_ttf.h')
    #the above don't seem to work...
    context.CheckHeader('SDL_image.h')
    context.CheckHeader('SDL_ttf.h')
    context.env.Append(LIBS = ['SDL_image', 'SDL_ttf'])
    context.checkSimpleLib(['guichan'])
    context.checkSimpleLib(['guichan_sdl'])
    context.checkSimpleLib(['z'], 'zlib.h')
    context.checkSimpleLib(['boost_filesystem'], ['boost/filesystem/convenience.hpp'], 'cxx')
    context.checkSimpleLib(['boost_regex'], ['boost/regex.hpp'], 'cxx')

    if context.env['opengl']:
        context.checkSimpleLib(['guichan_opengl'], ['guichan/opengl.hpp'], 'cxx')
#        context.env.Append(LINKFLAGS = '-framework OpenGL')
