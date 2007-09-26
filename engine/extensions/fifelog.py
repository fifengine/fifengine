import fife

class LogManager(object):
	def __init__(self, engine, promptlog=True, filelog=False):
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
		names = [n.lower() for n in names]
		if 'all' in names:
			for k in self.mod2name.keys():
				self.lm.addVisibleModule(k)
		else:
			for m in names:
				self.lm.addVisibleModule(self.name2mod[m])
		
	def removeVisibleModules(self, *names):
		names = [n.lower() for n in names]
		if 'all' in names:
			for k in self.mod2name.keys():
				self.lm.removeVisibleModule(k)
		else:
			for m in names:
				self.lm.removeVisibleModule(self.name2mod[m])
		
	def getVisibleModules(self):
		mods = []
		for k in self.mod2name.keys():
			if self.lm.isVisible(k):
				mods.append(self.mod2name[k])

	def setVisibleModules(self, *names):
		self.lm.clearVisibleModules()
		self.addVisibleModules(*names)

