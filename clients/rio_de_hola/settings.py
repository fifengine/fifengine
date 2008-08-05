import shutil
import pychan
try:
	import xml.etree.cElementTree as ET
except:
	import xml.etree.ElementTree as ET

class Setting(object):
	def onOptionsPress(self):	
		self.changesRequireRestart = False
		self.isSetToDefault = False
		self.Resolutions = ['640x480', '800x600', '1024x768', '1280x800', '1440x900']
		if not hasattr(self, 'OptionsDlg'):
			self.OptionsDlg = None
		if not self.OptionsDlg:
			self.OptionsDlg = pychan.loadXML('gui/settings.xml')
			self.OptionsDlg.distributeInitialData({
				'screen_resolution' : self.Resolutions,
				'render_backend' : ['OpenGL', 'SDL']
			})
			self.OptionsDlg.distributeData({
				'screen_resolution' : self.Resolutions.index(str(self.readSetting("ScreenWidth")) + 'x' + str(self.readSetting("ScreenHeight"))),
				'render_backend' : 0 if str(self.readSetting("RenderBackend")) == "OpenGL" else 1,
				'enable_fullscreen' : int(self.readSetting("FullScreen")),
				'enable_sound' : int(self.readSetting("PlaySounds"))
			})
			self.OptionsDlg.mapEvents({
				'okButton' : self.saveSettings,
				'cancelButton' : self.OptionsDlg.hide,
				'defaultButton' : self.setDefaults
			})
		self.OptionsDlg.show()

	def setDefaults(self):
		shutil.copyfile('settings-dist.xml', 'settings.xml')
		self.isSetToDefault = True
		self.changesRequireRestart = True

	def readSetting(self, name, type='int', strip=True, text=False):
		if not hasattr(self, 'tree'):
			self.tree = ET.parse('settings.xml')
			self.root_element = self.tree.getroot()
		element = self.root_element.find(name)
		if element is not None:
			element_value = element.text
			if element_value is None:
				if type == 'int':
					return 0
				elif type == 'list':
					list = []
					return list
			else:
				if type == 'int':
					return element_value.strip() if strip else element_value
				elif type == 'list':
					list = []
					list_s = []
					list = str(element_value.strip()).split(";")
					for item in list:
						item = item.strip()
						if text:
							item = item.replace('\\n', '\n')
						list_s.append(item)
					return list_s
				elif type == 'bool':
					return False if element_value.strip() == 'False' else True
		else:
			print 'Setting,', name, 'does not exist!'

	def setSetting(self, name, value):
		element = self.root_element.find(name)
		if element is not None:
			if value is not element.text:
				element.text = str(value)
		else:
			print 'Setting,', name, 'does not exist!'

	def saveSettings(self):
		screen_resolution, render_backend, enable_fullscreen, enable_sound = self.OptionsDlg.collectData('screen_resolution', 'render_backend', 'enable_fullscreen', 'enable_sound')
		render_backend = 'OpenGL' if render_backend is 0 else 'SDL'
		if render_backend != str(self.readSetting("RenderBackend")):
			self.setSetting('RenderBackend', render_backend)
			self.changesRequireRestart = True
		if int(enable_fullscreen) != int(self.readSetting("FullScreen")):
			self.setSetting('FullScreen', int(enable_fullscreen))
			self.changesRequireRestart = True
		if int(enable_sound) != int(self.readSetting("PlaySounds")):
			self.setSetting('PlaySounds', int(enable_sound))
			self.changesRequireRestart = True
		if screen_resolution != self.Resolutions.index(str(self.readSetting("ScreenWidth")) + 'x' + str(self.readSetting("ScreenHeight"))):
			self.setSetting('ScreenWidth', int(self.Resolutions[screen_resolution].partition('x')[0]))
			self.setSetting('ScreenHeight', int(self.Resolutions[screen_resolution].partition('x')[2]))
			self.changesRequireRestart = True

		if not self.isSetToDefault:
			self.tree.write('settings.xml')
		self.OptionsDlg.hide()
		if self.changesRequireRestart:
			RestartDlg = pychan.loadXML('gui/changes_require_restart.xml')
			RestartDlg.mapEvents({ 'closeButton' : RestartDlg.hide })
			RestartDlg.show()
