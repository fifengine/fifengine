import os

def initEnvironment(env):
      env.Append(ENV={'PATH' : os.environ['PATH']})
      return env

def addExtras(context):
      context.env.Append(LINKFLAGS='-framework OpenAL')
      context.env.Append(LINKFLAGS='-framework Cocoa')
      context.env.Append(LINKFLAGS='-framework CoreFoundation')
      
      include_dirs = ['/opt/local/include', '/usr/local/include', '/sw/include']
      dirs = [d for d in include_dirs]
      for d in dirs:
            include_dirs += [d+'/vorbis']
      context.env.Append(CPPPATH = include_dirs)
      
      lib_dirs = ['/opt/local/lib', '/usr/local/lib', '/sw/lib']
      context.env.Append(LIBPATH = lib_dirs)
      
      #OpenAL is a framework on OS X so it's harder for scons to find it
      #context.checkSimpleLib(['openal'], 'AL/al.h')
      
      context.checkSimpleLib(['vorbisfile'], 'vorbisfile.h')
      context.checkSimpleLib(['lua.5'], 'lua.h')
      context.checkSimpleLib(['lualib.5'], 'lualib.h')
      context.checkSimpleLib(['SDL'], 'SDL.h')
      #context.checkSimpleLib(['SDL_image'], 'SDL_image.h')
      #context.checkSimpleLib(['SDL_ttf'], 'SDL_ttf.h')
      #the above don't seem to work...
      context.CheckHeader('SDL_image.h')
      context.CheckHeader('SDL_ttf.h')
      context.env.Append(LIBS = ['SDL_image', 'SDL_ttf'])
      context.checkSimpleLib(['guichan'])
      context.checkSimpleLib(['guichan_sdl'])
      context.checkSimpleLib(['z'], 'zlib.h')
      
      if context.env['opengl']:
            context.env.Append(LINKFLAGS = '-framework OpenGL')
            context.checkSimpleLib(['guichan_opengl'])
