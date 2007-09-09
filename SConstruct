import os
import sys

opts = Options('options.py')

opts.Add(BoolOption('debug', 'Build with debuginfos and without optimisations', 1))
opts.Add(BoolOption('opengl', 'Compile OpenGL support', 1))
opts.Add(BoolOption('useenv', 'Import environment variables', 0))
opts.Add(BoolOption('xmlmap', 'Build demo xml-maploader (and required libs)', 0))
opts.Add(BoolOption('swig', 'Use swig to create bindings (swig not required)', 0))
opts.Add(BoolOption('makeswig', 'Generate binding code using swig', 0))
opts.Add(BoolOption('audio', 'Enable audio (openal/libvorbis) support', 1)) 
opts.Add(EnumOption('script', 'Enable which script-language backend', 'lua', allowed_values=('none', 'lua')))

env = Environment(options = opts)
Help(opts.GenerateHelpText(env))

if env['useenv'] == 1:
	env.Replace(ENV = os.environ)

# helper functions
def tryConfigCommand(context, cmd):
	ret = context.TryAction(cmd)[0]
	context.Result(ret)
	if ret:
		context.env.ParseConfig(cmd)
	return ret

def importConfig(config):
	config = __import__(config)
	return config

def getPlatformConfig():
	filename = '%s-config' % sys.platform
	if os.path.exists(filename + '.py'):
		return importConfig(filename)
	else:
		print 'no custom platform-config found (searched: %s.py)' % filename
		filename += '-dist'
		if os.path.exists(filename + '.py'):
			return importConfig(filename)
		print 'no platform-config found (searched: %s.py)' % filename
		Exit(1)

# custom checks
def checkPKG(context, name):
	context.Message('Checking for %s (using pkg-config)... ' % name)
	return tryConfigCommand(context, 'pkg-config --libs --cflags \'%s\'' % name)

def checkConf(context, name):
	binary = '%s-config' % name.lower()
	context.Message('Checking for %s (using %s)... ' % (name, binary))
	configcall = '%s --libs --cflags' %binary
	return tryConfigCommand(context, configcall)

def checkSimpleLib(context, liblist, header = '', lang = 'c', required = 1):
	for lib in liblist:
		ret = checkPKG(context, lib)
		if ret:
			return ret

		ret = checkConf(context, lib)
		if ret:
			return ret

		if len(header):
			ret = conf.CheckLibWithHeader(lib, header, lang)
		else:
			ret = conf.CheckLib(lib)

		if ret:
			if not lib in conf.env['LIBS']:
				conf.env.Append(LIBS = [lib])
			return ret

	if required:
		print 'required lib %s not found :(' % lib
		Exit(1)

	return False


# if not env.GetOption('clean'): 
platformConfig = getPlatformConfig()
env = platformConfig.initEnvironment(env)
conf = Configure(env, custom_tests = { 'checkConf' : checkConf, 'checkPKG' : checkPKG, 'checkSimpleLib' : checkSimpleLib })

platformConfig.addExtras(conf)
env = conf.Finish()
env.Append(CPPFLAGS = ['-Wall', '-Wold-style-cast'])
if env['debug'] == 1:
	env.Append(CPPFLAGS = ['-ggdb', '-O0'])
else:
	env.Append(CPPFLAGS = ['-O2'])

if env['opengl'] == 1:
	env.Append(CPPDEFINES = ['HAVE_OPENGL'])

if env['xmlmap'] == 1:
	env.Append(CPPDEFINES = ['HAVE_XMLMAP'])

if env['audio'] == 0:
	env.Append(CPPDEFINES = ['NO_AUDIO'])

if env['script'] == 'lua':
	env.Append(CPPDEFINES = ['SCRIPTENGINE_LUA'])

if env['swig'] == 1:
	env.Append(CPPDEFINES = ['SWIG'])

env_sdl = env.Copy();
Export('env')
Export('env_sdl')

SConscript(['src/SConscript'])

# vim: set filetype=python: 
