import re
import os.path

import SCons.Defaults
import SCons.Tool
import SCons.Util


class ToolQtWarning(SCons.Warnings.Warning):
	pass
class GeneratedMocFileNotIncluded(ToolQtWarning):
	pass
class QtdirNotFound(ToolQtWarning):
	pass
SCons.Warnings.enableWarningClass(ToolQtWarning)

qrcinclude_re = re.compile(r'<file>([^<]*)</file>', re.M)


header_extensions = [".h", ".hxx", ".hpp", ".hh"]
if SCons.Util.case_sensitive_suffixes('.h', '.H'):
	header_extensions.append('.H')
#cplusplus = __import__('c++', globals(), locals(), ['Scons.Tools'])
#cxx_suffixes = cplusplus.CXXSuffixes
cxx_suffixes = [".c", ".cxx", ".cpp", ".cc"]

def _checkMocIncluded(target, source, env):
	moc = target[0]
	cpp = source[0]
	# looks like cpp.includes is cleared before the build stage :-(
	# not really sure about the path transformations (moc.cwd? cpp.cwd?) :-/
	path = SCons.Defaults.CScan.path_function(env, moc.cwd)
	includes = SCons.Defaults.CScan(cpp, env, path)
	if not moc in includes:
		SCons.Warnings.warn(
			GeneratedMocFileNotIncluded,
			"Generated moc file '%s' is not included by '%s'" %
			(str(moc), str(cpp)))

def _find_file(filename, paths, node_factory):
	retval = None
	for dir in paths:
		node = node_factory(filename, dir)
		if node.rexists():
			return node
	return None

class _Automoc:
	"""
	Callable class, which works as an emitter for Programs, SharedLibraries and
	StaticLibraries.
	"""

	def __init__(self, objBuilderName):
		self.objBuilderName = objBuilderName
		
	def __call__(self, target, source, env):
		"""
		Smart autoscan function. Gets the list of objects for the Program
		or Lib. Adds objects and builders for the special qt files.
		"""
		try:
			if int(env.subst('$QT_AUTOSCAN')) == 0:
				return target, source
		except ValueError:
			pass
		try:
			debug = int(env.subst('$QT_DEBUG'))
		except ValueError:
			debug = 0
		
		# some shortcuts used in the scanner
		FS = SCons.Node.FS.default_fs
		splitext = SCons.Util.splitext
		objBuilder = getattr(env, self.objBuilderName)

		# some regular expressions:
		# Q_OBJECT detection
		q_object_search = re.compile(r'[^A-Za-z0-9]Q_OBJECT[^A-Za-z0-9]') 
		# cxx and c comment 'eater'
		#comment = re.compile(r'(//.*)|(/\*(([^*])|(\*[^/]))*\*/)')
		# CW: something must be wrong with the regexp. See also bug #998222
		#     CURRENTLY THERE IS NO TEST CASE FOR THAT
		
		# The following is kind of hacky to get builders working properly (FIXME)
		objBuilderEnv = objBuilder.env
		objBuilder.env = env
		mocBuilderEnv = env.Moc4.env
		env.Moc4.env = env
		
		# make a deep copy for the result; MocH objects will be appended
		out_sources = source[:]

		for obj in source:
			if not obj.has_builder():
				# binary obj file provided
				if debug:
					print "scons: qt: '%s' seems to be a binary. Discarded." % str(obj)
				continue
			cpp = obj.sources[0]
			if not splitext(str(cpp))[1] in cxx_suffixes:
				if debug:
					print "scons: qt: '%s' is no cxx file. Discarded." % str(cpp) 
				# c or fortran source
				continue
			#cpp_contents = comment.sub('', cpp.get_contents())
			cpp_contents = cpp.get_contents()
			h=None
			for h_ext in header_extensions:
				# try to find the header file in the corresponding source
				# directory
				hname = splitext(cpp.name)[0] + h_ext
				h = _find_file(hname,
							  (cpp.get_dir(),),
							  FS.File)
				if h:
					if debug:
						print "scons: qt: Scanning '%s' (header of '%s')" % (str(h), str(cpp))
					#h_contents = comment.sub('', h.get_contents())
					h_contents = h.get_contents()
					break
			if not h and debug:
				print "scons: qt: no header for '%s'." % (str(cpp))
			if h and q_object_search.search(h_contents):
				# h file with the Q_OBJECT macro found -> add moc_cpp
				moc_cpp = env.Moc4(h)
				moc_o = objBuilder(moc_cpp)
				out_sources.append(moc_o)
				#moc_cpp.target_scanner = SCons.Defaults.CScan
				if debug:
					print "scons: qt: found Q_OBJECT macro in '%s', moc'ing to '%s'" % (str(h), str(moc_cpp))
			if cpp and q_object_search.search(cpp_contents):
				# cpp file with Q_OBJECT macro found -> add moc
				# (to be included in cpp)
				moc = env.Moc4(cpp)
				env.Ignore(moc, moc)
				if debug:
					print "scons: qt: found Q_OBJECT macro in '%s', moc'ing to '%s'" % (str(cpp), str(moc))
				#moc.source_scanner = SCons.Defaults.CScan
		# restore the original env attributes (FIXME)
		objBuilder.env = objBuilderEnv
		env.Moc4.env = mocBuilderEnv

		return (target, out_sources)

AutomocShared = _Automoc('SharedObject')
AutomocStatic = _Automoc('StaticObject')

def _detect(env):
	"""Not really safe, but fast method to detect the QT library"""

	QTDIR = env.get('QTDIR',None)
	if QTDIR!=None : return QTDIR

	QTDIR = os.environ.get('QTDIR',None)
	if QTDIR!=None : return QTDIR

	moc = env.WhereIs('moc-qt4') or env.WhereIs('moc')
	if moc:
		SCons.Warnings.warn(
			QtdirNotFound,
			"QTDIR variable is not defined, using moc executable as a hint (QTDIR=%s)" % QTDIR)
		return os.path.dirname(os.path.dirname(moc))

	SCons.Warnings.warn(
		QtdirNotFound,
		"Could not detect qt, using empty QTDIR")
	return None

def generate(env):
	"""Add Builders and construction variables for qt to an Environment."""

	print "Loading qt4 tool..."

	def locateQt4Command(env, command, qtdir) :
		fullpath = env.Detect([command+'-qt4', command])
		if not (fullpath is None) : return fullpath
		fullpath1 = os.path.join(qtdir,'bin',command +'-qt4')
		if os.access(fullpath1, os.X_OK) or \
			os.access(fullpath1+".exe", os.X_OK):
			return fullpath1
		fullpath2 = os.path.join(qtdir,'bin',command)
		if os.access(fullpath2, os.X_OK) or \
			os.access(fullpath2+".exe", os.X_OK):
			return fullpath2
		raise "Qt4 command '" + command + "' not found. Tried: " + fullpath1 + " and "+ fullpath2
		

	CLVar = SCons.Util.CLVar
	Action = SCons.Action.Action
	Builder = SCons.Builder.Builder
	splitext = SCons.Util.splitext

	# the basics
	env['QTDIR']  = _detect(env)
	env['QT4_MOC'] = locateQt4Command(env,'moc', env['QTDIR'])
	env['QT4_UIC'] = locateQt4Command(env,'uic', env['QTDIR'])
	env['QT4_RCC'] = locateQt4Command(env,'rcc', env['QTDIR'])
	env['QT4_LUPDATE'] = locateQt4Command(env,'lupdate', env['QTDIR'])
	env['QT4_LRELEASE'] = locateQt4Command(env,'lrelease', env['QTDIR'])

	# Should the qt tool try to figure out, which sources are to be moc'ed ?
	env['QT4_AUTOSCAN'] = 1

	# Some QT specific flags. I don't expect someone wants to
	# manipulate those ...
	env['QT4_UICDECLFLAGS'] = CLVar('')
	env['QT4_MOCFROMHFLAGS'] = CLVar('')
	env['QT4_MOCFROMCXXFLAGS'] = CLVar('-i')
	env['QT4_QRCFLAGS'] = ''

	# suffixes/prefixes for the headers / sources to generate
	env['QT4_MOCHPREFIX'] = 'moc_'
	env['QT4_MOCHSUFFIX'] = '$CXXFILESUFFIX'
	env['QT4_MOCCXXPREFIX'] = 'moc_'
	env['QT4_MOCCXXSUFFIX'] = '.moc'
	env['QT4_UISUFFIX'] = '.ui'
	env['QT4_UICDECLPREFIX'] = 'ui_'
	env['QT4_UICDECLSUFFIX'] = '.h'
	env['QT4_QRCSUFFIX'] = '.qrc',
	env['QT4_QRCCXXSUFFIX'] = '$CXXFILESUFFIX'
	env['QT4_QRCCXXPREFIX'] = 'qrc_'

	env['QT4_LIB'] = '' # KLUDGE to avoid linking qt3 library

	# Translation builder
	tsbuilder = Builder(
		action ='$QT4_LUPDATE $SOURCES -ts $TARGETS',
		multi=1
		)
	env.Append( BUILDERS = { 'Ts': tsbuilder } )
	qmbuilder = Builder(
		action =[
			'$QT4_LRELEASE $SOURCE',
			],
		src_suffix = '.ts',
		suffix = '.qm',
		single_source = True
		)
	env.Append( BUILDERS = { 'Qm': qmbuilder } )

	# Resource builder
	def scanResources(node, env, path, arg):
		contents = node.get_contents()
		includes = qrcinclude_re.findall(contents)
		return includes
	qrcscanner = env.Scanner(name = 'qrcfile',
		function = scanResources,
		argument = None,
		skeys = ['.qrc'])
	qrcbuilder = Builder(
		action ='$QT4_RCC $QT4_QRCFLAGS $SOURCE -o $TARGET',
		source_scanner = qrcscanner,
		src_suffix = '$QT4_QRCSUFFIX',
		suffix = '$QT4_QRCCXXSUFFIX',
		prefix = '$QT4_QRCCXXPREFIX',
		single_source = True
		)
	env.Append( BUILDERS = { 'Qrc': qrcbuilder } )

	# Interface builder
	env['QT4_UIC4COM'] = [
		CLVar('$QT4_UIC $QT4_UICDECLFLAGS -o ${TARGETS[0]} $SOURCE'),
		]
	uic4builder = Builder(
		action='$QT4_UIC4COM',
		src_suffix='$QT4_UISUFFIX',
		suffix='$QT4_UICDECLSUFFIX',
		prefix='$QT4_UICDECLPREFIX',
		single_source = True
		)
	env.Append( BUILDERS = { 'Uic4': uic4builder } )

	# Metaobject builder
	env['QT4_MOCFROMHCOM'] = (
		'$QT4_MOC $QT4_MOCFROMHFLAGS -o ${TARGETS[0]} $SOURCE')
	env['QT4_MOCFROMCXXCOM'] = [
		CLVar('$QT4_MOC $QT4_MOCFROMCXXFLAGS -o ${TARGETS[0]} $SOURCE'),
		Action(_checkMocIncluded,None)]
	mocBld = Builder(action={}, prefix={}, suffix={})
	for h in header_extensions:
		mocBld.add_action(h, '$QT4_MOCFROMHCOM')
		mocBld.prefix[h] = '$QT4_MOCHPREFIX'
		mocBld.suffix[h] = '$QT4_MOCHSUFFIX'
	for cxx in cxx_suffixes:
		mocBld.add_action(cxx, '$QT4_MOCFROMCXXCOM')
		mocBld.prefix[cxx] = '$QT4_MOCCXXPREFIX'
		mocBld.suffix[cxx] = '$QT4_MOCCXXSUFFIX'
	env.Append( BUILDERS = { 'Moc4': mocBld } )

	# er... no idea what that was for
	static_obj, shared_obj = SCons.Tool.createObjBuilders(env)
	static_obj.src_builder.append('Uic4')
	shared_obj.src_builder.append('Uic4')

	# We use the emitters of Program / StaticLibrary / SharedLibrary
	# to scan for moc'able files
	# We can't refer to the builders directly, we have to fetch them
	# as Environment attributes because that sets them up to be called
	# correctly later by our emitter.
	env.AppendUnique(PROGEMITTER =[AutomocStatic],
					 SHLIBEMITTER=[AutomocShared],
					 LIBEMITTER  =[AutomocStatic],
					 # Of course, we need to link against the qt libraries
					 CPPPATH=[os.path.join('$QTDIR', 'include')],
					 LIBPATH=[os.path.join('$QTDIR', 'lib')],
					 LIBS=['$QT4_LIB'])
	
	import new
	method = new.instancemethod(enable_modules, env, SCons.Environment)
	env.EnableQt4Modules=method

def enable_modules(self, modules, debug=False) :
	import sys

	validModules = [
		'QtCore',
		'QtGui',
		'QtOpenGL',
		'Qt3Support',
		# The next modules have not been tested yet so, please
		# maybe they require additional work on non Linux platforms
		'QtSql',
		'QtNetwork',
		'QtSvg',
		'QtTest',
		'QtXml',
		'QtUiTools',
		]
	pclessModules = [
		'QtUiTools',
		'QtUiTools_debug',
	]
	invalidModules=[]
	for module in modules:
		if module not in validModules :
			invalidModules.append(module)
	if invalidModules :
		raise "Modules %s are not Qt4 modules. Valid Qt4 modules are: %s"% \
			(str(invalidModules),str(validModules))

	# TODO: Check whether we should add QT_CORE_LIB, QT_XML_LIB, QT_NETWORK_LIB...
	if 'QtGui' in modules:
		self.AppendUnique(CPPFLAGS='-DQT_GUI_LIB')

	if sys.platform == "linux2" :
		if debug : modules = [module+"_debug" for module in modules]
		for module in modules :
			if module in pclessModules :
			#	self.AppendUnique(LIBS=[module])
				self.AppendUnique(LIBPATH=[os.path.join(self["QTDIR"],"lib",module)])
				self.AppendUnique(CPPPATH=[os.path.join(self["QTDIR"],"include","qt4",module)])
				modules.remove(module)
		self.ParseConfig('pkg-config %s --libs --cflags'%
		(
			
			' '.join(modules)))
		return
	if sys.platform == "win32" :
		if debug : debugSuffix = 'd'
		else : debugSuffix = ''
		self.AppendUnique(LIBS=[lib+'4'+debugSuffix for lib in modules])
		if 'QtOpenGL' in modules:
			self.AppendUnique(LIBS=['opengl32'])
		self.AppendUnique(CPPPATH=[ '$QTDIR/include/'+module
			for module in modules])
		self.AppendUnique(LIBPATH=['$QTDIR/lib'])


def exists(env):
	return _detect(env)

