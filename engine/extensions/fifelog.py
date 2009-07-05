import fife

class LogManager(object):
	'''
	Log manager provides convenient apis to access engine logging functionality.
	You can set log targets individually (prompt, file). You can also adjust
	things like visible modules through log manager.
	'''

	def __init__(self, engine, promptlog=True, filelog=False):
		'''
		Constructs new log manager
		@param engine: Engine to hook into
		@param promptlog: If true, logs to prompt
		@param filelog: If true, logs to file (fife.log)
		'''
		self.engine = engine
		self.lm = engine.getLogManager()
		self.lm.setLogToPrompt(promptlog)
		self.lm.setLogToFile(filelog)
		self.mod2name = {}
		for k, v in fife.__dict__.items():
			if k.startswith('LM_') and k not in ('LM_CORE', 'LM_MODULE_MAX'):
				self.mod2name[v] = self.lm.getModuleName(v)
		self.name2mod = dict([(v.lower(), k) for k, v in self.mod2name.items()])

	def addVisibleModules(self, *names):
		'''
		Adds modules that are visible in logs. By default, all modules
		are disabled. Does not remove previously visible modules
		@param names: module names to set visible
		@see modules.h file for available modules in the engine
		'''
		names = [n.lower() for n in names]
		if 'all' in names:
			for k in self.mod2name.keys():
				self.lm.addVisibleModule(k)
		else:
			for m in names:
				try:
					self.lm.addVisibleModule(self.name2mod[m])
				except KeyError:
					print 'Tried to enable non-existing log module "%s"' % m

	def removeVisibleModules(self, *names):
		'''
		Removes modules that are visible in logs. By default, all modules
		are disabled.
		@param names: module names to set invisible
		@see addVisibleModules
		'''
		names = [n.lower() for n in names]
		if 'all' in names:
			for k in self.mod2name.keys():
				self.lm.removeVisibleModule(k)
		else:
			for m in names:
				self.lm.removeVisibleModule(self.name2mod[m])

	def getVisibleModules(self):
		'''
		Gets currently visible modules
		@see addVisibleModules
		'''
		mods = []
		for k in self.mod2name.keys():
			if self.lm.isVisible(k):
				mods.append(self.mod2name[k])

	def setVisibleModules(self, *names):
		'''
		Sets visible modules. Clears previously set modules.
		@param names: module names to set visible
		@see addVisibleModules
		'''
		self.lm.clearVisibleModules()
		self.addVisibleModules(*names)
