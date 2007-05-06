import os, sys
from utils.util_scripts.path import path as upath

opts = Options('options.py', ARGUMENTS)
opts.Add(BoolOption('audio',  'Enable audio (openal/libvorbis) support', 1)) 
opts.Add(BoolOption('debug',  'Build with debuginfos and without optimisations', 1))
opts.Add(BoolOption('testcases',  'Build testcases in unit_tests', 0))
opts.Add(BoolOption('opengl', 'Compile OpenGL support', 1))
opts.Add(EnumOption('script', 'Enable which script-language backend', 'lua', allowed_values=('none', 'lua')))
opts.Add(BoolOption('xmlmap', 'Build demo xml-maploader', 1))
opts.Add(BoolOption('tinyxml', 'Build tinyxml (required for xmlmap)', 1))
opts.Add(BoolOption('lite',   'Build the lite version of the library (used for editor, overrides other settings)', 0))
opts.Add(BoolOption('profile', 'Build with profiling information', 0))
opts.Add(EnumOption('guichan', 'Choose guichan version (default 0.6)', '6', allowed_values=('4','5','6')))
opts.Add(BoolOption('msvcproj',  'Create MSVC project file', 0))
opts.Add(BoolOption('utils',  'Build utilities', 0))

env = Environment(options = opts, ENV = {'PATH' : os.environ['PATH']})

Help(opts.GenerateHelpText(env))

# helper functions
def tryConfigCommand(context, cmd):
	ret = context.TryAction(cmd)[0]
	context.Result(ret)
	if ret:
		context.env.ParseConfig(cmd)
	return ret

def importConfig(config):
	module = __import__(config)
	parts = config.split('.')
	for part in parts[1:]:
		module = getattr(module, part)
	return module

def getPlatformConfig():
	pathparts = ('build', '%s-config' % sys.platform)
	filename = os.path.join(*pathparts)
	sconsfilename = '.'.join(pathparts)
	if os.path.exists(filename + '.py'):
		return importConfig(sconsfilename)
	else:
		print 'no custom platform-config found (searched: %s.py)' % filename
		filename += '-dist'
		sconsfilename += '-dist'
		if os.path.exists(filename + '.py'):
			return importConfig(sconsfilename)
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

if env['msvcproj']:
	Export('env')
	SConscript(['engine/SConscript'])
else:
	platformConfig = getPlatformConfig()
	env = platformConfig.initEnvironment(env)
	conf = Configure(env, 
                     custom_tests = {'checkConf': checkConf, 'checkPKG': checkPKG, 'checkSimpleLib': checkSimpleLib},
					 conf_dir = '#/build/.sconf_temp',
					 log_file = '#/build/config.log')
	
	platformConfig.addExtras(conf)
	env = conf.Finish()

	if sys.platform == "win32":
		env.Append(CPPFLAGS = ['-Wall'])
	else:
		env.Append(CPPFLAGS = ['-Wall', '-Wold-style-cast'])
	
	if env['debug'] == 1:
		env.Append(CPPFLAGS = ['-ggdb', '-O0'])
	else:
		if os.getenv('CXXFLAGS'):
			env.Append(CPPFLAGS = Split(os.environ['CXXFLAGS']))
		else:
			env.Append(CPPFLAGS = ['-O2'])
	
	if env['profile']:
		env.Append(CPPFLAGS = ['-pg'])
		env.Append(LINKFLAGS = ['-pg'])
	
	if env['xmlmap']:
		env.Append(CPPDEFINES = ['HAVE_XMLMAP'])
	
	if env['opengl']:
		env.Append(CPPDEFINES = ['HAVE_OPENGL'])
	
	if env['script'] == 'lua':
		env.Append(CPPDEFINES = ['SCRIPTENGINE_LUA'])
	
	if env['lite']:
		env.Append(CPPDEFINES = ['LITE'])	
	
	env.Append(CPPDEFINES = ['GUICHAN_VERSION='+env['guichan']])
	
	if env['audio']:
		env.Append(CPPDEFINES = ['HAVE_OPENAL'])	
		
	Export('env')
	
	SConscript('engine/SConscript')
	
	if not env['lite']:
		env.Append(LIBS = ['fife'])
		env.Append(LIBPATH = ['#/engine'])
	
		enginefiles = ['engine/main.cpp']
		if sys.platform == 'darwin':
			env.Object('SDLMain.m')
			enginefiles.append('SDLMain.o')
		env.Program('fife_engine', enginefiles)
		
		if env['testcases']:
			SConscript('tests/unit_tests/SConscript')

		if env['utils']:
			SConscript([str(p) for p in upath('utils').walkfiles('SConscript')])


	

# vim: set filetype=python: 

