import os, sys
from utils.util_scripts.path import path as upath

opts = Options('options.py', ARGUMENTS)
opts.Add(BoolOption('debug',  'Build with debuginfos and without optimisations', 1))
opts.Add(BoolOption('tests',  'Build testcases in unit_tests', 0))
opts.Add(BoolOption('noengine',  'Prevents building of engine, use e.g. for util/test tweaking', 0))
opts.Add(BoolOption('opengl', 'Compile OpenGL support', 1))
opts.Add(EnumOption('script', 'Selects generated scripting language bindings', 'python', allowed_values=('python', 'lua')))
opts.Add(BoolOption('profile', 'Build with profiling information', 0))
opts.Add(BoolOption('projectfiles_only',  "Creates IDE project files only. If defined, won't build code. " +
                    "Note that normal builds generate these files also automatically.", 0))
opts.Add(BoolOption('utils',  'Build utilities', 0))
opts.Add(BoolOption('ext',  'Build external dependencies', 0))
opts.Add(BoolOption('docs',  "Generates static analysis documentation into doc-folder. If defined, won't build code", 0))
opts.Add(BoolOption('zip', 'Enable ZIP archive support', 1))
opts.Add(BoolOption('log', 'Enables logging for the engine', 1))

opts.Add(BoolOption('rend_camzone', 'Enables camera zone renderer', 0))
opts.Add(BoolOption('rend_grid', 'Enables grid renderer', 0))

# Platform-specific prefix directories
if sys.platform == 'linux2':
	opts.Add(PathOption('PREFIX', 'Directory to install under', '/usr'))

#env = Environment(options = opts, ENV = {'PATH' : os.environ['PATH']})
env = Environment(options = opts, ENV = os.environ)
env.Replace(SCONS_ROOT_PATH=str(upath('.').abspath()))
rootp = env['SCONS_ROOT_PATH']

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

def checkSimpleLib(context, liblist, header = '', lang = 'c++', required = 1):
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
			ret = conf.CheckLib(lib,language=lang)

		if ret:
#			print "ret: " + ret
			if not lib in conf.env['LIBS']:
				conf.env.Append(LIBS = [lib])
			return ret

	if required:
		print 'required lib %s not found :(' % lib
		Exit(1)

	return False

if env['projectfiles_only']:
	Export('env')
	SConscript(['engine/SConscript'])
elif env['docs']:
	_jp = os.path.join
	# should prolly be done using scons builders...
	try:
		print "removing old documentation directories"
		upath('doc/doxygen/html').rmtree()
	except OSError:
		pass
	print "generating new doxygen documentation"
	os.system('doxygen ' + _jp('doc', 'doxygen', 'doxyfile'))
	print "doxygen documentation created succesfully"

elif env['ext']:
	Export('env')
	SConscript('ext/SConscript')
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
		env.Append(CPPFLAGS = ['-Wall']) # removed old style cast warnings for now (swig creates these)
	
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
	
	definemap = {
		'opengl': 'HAVE_OPENGL',
		'zip': 'HAVE_ZIP',
		'log': 'LOG_ENABLED',
		'rend_camzone': 'RENDER_CAMZONES',
		'rend_grid': 'RENDER_GRID',
	}
	for k, v in definemap.items():
		if env[k]:
			env.Append(CPPDEFINES = [v])
	
	Export('env')
	
	if not env['noengine']:
		SConscript('engine/SConscript')
	
	env.Append(LIBPATH = ['#/engine'])

	if env['tests']:
		SConscript('tests/core_tests/SConscript')

	if env['utils']:
		SConscript([str(p) for p in upath('utils').walkfiles('SConscript')])

# vim: set filetype=python: 
