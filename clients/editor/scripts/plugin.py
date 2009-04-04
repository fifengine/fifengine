import os

class PluginManager:
	def __init__(self, *args, **kwargs):
		self._pluginDir = "plugins"
		self._plugins = []
		
		files = []
		for f in os.listdir(self._pluginDir):
			path = os.path.join(self._pluginDir, f)
			if os.path.isfile(path) and os.path.splitext(f)[1] == ".py" and f != "__init__.py":
				files.append(os.path.splitext(f)[0])
				
		for f in files:
			print "Importing plugin:", f
			exec "import plugins."+f
			plugin = eval("plugins."+f+"."+f+"()")
			if isinstance(plugin, Plugin) is False:
				print f+" is not an instance of Plugin!"
			else:
				plugin.enable()
				self._plugins.append(plugin)

		
class Plugin:
	def enable(self):
		raise NotImplementedError, "Plugin has not implemented the enable() function!"

	def disable(self):
		raise NotImplementedError, "Plugin has not implemented the disable() function!"

	def isEnabled(self):
		raise NotImplementedError, "Plugin has not implemented the isEnabled() function!"

	def getName(self):
		raise NotImplementedError, "Plugin has not implemented the getName() function!"
		
	#--- These are not so important ---#
	def getAuthor(self):
		return "Unknown"
		
	def getDescription(self):
		return ""

	def getLicense(self):
		return ""
		
	def getVersion(self):
		return "0.1"
