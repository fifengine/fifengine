# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2017 by the FIFE team
#  http://www.fifengine.net
#  This file is part of FIFE.
#
#  FIFE is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the
#  Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
# ####################################################################

from fife import fife

class LogManager(object):
	"""
	Log manager provides convenient apis to access engine logging functionality.
	You can set log targets individually (prompt, file). You can also adjust
	things like visible modules through log manager.
	"""

	def __init__(self, engine, promptlog=True, filelog=False):
		"""
		Constructs new log manager
		@param engine: Engine to hook into
		@param promptlog: If true, logs to prompt
		@param filelog: If true, logs to file (fife.log)
		"""
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
		"""
		Adds modules that are visible in logs. By default, all modules
		are disabled. Does not remove previously visible modules
		@param names: module names to set visible
		@see: modules.h file for available modules in the engine
		"""
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
		"""
		Removes modules that are visible in logs. By default, all modules
		are disabled.
		@param names: module names to set invisible
		@see: addVisibleModules
		"""
		names = [n.lower() for n in names]
		if 'all' in names:
			for k in self.mod2name.keys():
				self.lm.removeVisibleModule(k)
		else:
			for m in names:
				self.lm.removeVisibleModule(self.name2mod[m])

	def getVisibleModules(self):
		"""
		Gets currently visible modules
		@see: addVisibleModules
		"""
		mods = []
		for k in self.mod2name.keys():
			if self.lm.isVisible(k):
				mods.append(self.mod2name[k])

	def setVisibleModules(self, *names):
		"""
		Sets visible modules. Clears previously set modules.
		@param names: module names to set visible
		@see: addVisibleModules
		"""
		self.lm.clearVisibleModules()
		self.addVisibleModules(*names)
		
	def setLevelFilter(self, fltr):
		"""
		Sets the minimum log level to view.
		
		@param fltr: The filter level
		
		Valid values:
			- L{fife.LogManager.LEVEL_DEBUG}
			- L{fife.LogManager.LEVEL_LOG}
			- L{fife.LogManager.LEVEL_WARN}
   			- L{fife.LogManager.LEVEL_ERROR}
		"""
		self.lm.setLevelFilter(fltr)

	def setLogToPrompt(self, promptlog):
		return self.lm.setLogToPrompt(promptlog)
		
	def getLogToPrompt(self):
		return self.lm.isLogToPrompt()
	
	logToPrompt = property(getLogToPrompt, setLogToPrompt)

	def setLogToFile(self, filelog):
		return self.lm.setLogToFile(filelog)

	def getLogToFile(self):
		return self.lm.isLogToFile()

	logToFile = property(getLogToFile, setLogToFile)
		
	def log_debug(self, message):
		self.lm.log(0, self.name2mod["script"], message)

	def log_log(self, message):
		self.lm.log(1, self.name2mod["script"], message)
		
	def log_warn(self, message):
		self.lm.log(2, self.name2mod["script"], message)

	def log_error(self, message):
		self.lm.log(3, self.name2mod["script"], message)
