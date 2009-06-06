import os
import editor

class PluginManager:
	def __init__(self, *args, **kwargs):
		self._settings = editor.getEditor().getSettings()
		
		self._pluginDir = "plugins"
		self._plugins = []
		
		files = []
		for f in os.listdir(self._pluginDir):
			path = os.path.join(self._pluginDir, f)
			if os.path.isfile(path) and os.path.splitext(f)[1] == ".py" and f != "__init__.py":
				files.append(os.path.splitext(f)[0])
				
		for f in files:
			importPlugin = self._settings.get("Plugins", f, False)
			if importPlugin:
				try:
					print "Importing plugin:", f
					exec "import plugins."+f
					plugin = eval("plugins."+f+"."+f+"()")
					if isinstance(plugin, Plugin) is False:
						print f+" is not an instance of Plugin!"
					else:
						plugin.enable()
						self._plugins.append(plugin)
				except BaseException, error:
					print "Error: ", error
					print "Invalid plugin:", f
			else:
				print "Not importing plugin:", f
				
			self._settings.set("Plugins", f, importPlugin)
			
		self._settings.saveSettings()

		
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
