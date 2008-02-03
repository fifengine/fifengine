import sys, os, re, math, random

def _jp(path):
	return os.path.sep.join(path.split('/'))

_paths = ('../../engine/swigwrappers/python', '../../engine/extensions')
for p in _paths:
	if p not in sys.path:
		sys.path.append(_jp(p))

import fife, fifelog
from scripts import world, eventlistenerbase
from basicapplication import ApplicationBase
import pychan
import pychan.widgets as widgets
import settings as TDS


class ApplicationListener(eventlistenerbase.EventListenerBase):
	def __init__(self, engine, world):
		eventlistenerbase.EventListenerBase.__init__(self,engine,regKeys=True,regCmd=True, regMouse=True, regConsole=True, regWidget=True)
		#super(ApplicationListener, self).__init__()
		self.engine = engine
		self.world = world
		
		self.quit = False
		self.aboutWindow = None
		
		self.rootpanel = pychan.loadXML('content/gui/rootpanel.xml')
		self.rootpanel.mapEvents({ 
			'quitButton' : self.onQuitButtonPress,
			'aboutButton' : self.onAboutButtonPress,
		})
		self.rootpanel.show()

	def keyPressed(self, evt):
		keyval = evt.getKey().getValue()
		keystr = evt.getKey().getAsString().lower()
		consumed = False
		if keyval == fife.IKey.ESCAPE:
			self.quit = True
			consumed = True
		elif keyval == fife.IKey.F10:
			self.engine.getGuiManager().getConsole().toggleShowHide()
			consumed = True
		elif keystr == 'p':
			self.engine.getRenderBackend().captureScreen('screenshot.bmp')
			consumed = True
		if not consumed:
			self.world.keyPressed(evt)
	
	def onCommand(self, command):
		self.quit = (command.getCommandType() == fife.CMD_QUIT_GAME)
		if not self.quit:
			self.world.onCommand(command)

	def onConsoleCommand(self, command):
		result = "no result"
		if command.lower() in ('quit', 'exit'):
			self.quit = True
			return 'quitting'
		
		if command.lower() in ( 'help', 'help()' ):
			self.engine.getGuiManager().getConsole().println( open( 'content/infotext.txt', 'r' ).read() )
			return "-- End of help --"
		
		return self.world.onConsoleCommand(command)

	def mouseReleased(self, evt):
		self.world.mouseReleased(evt)

	def onQuitButtonPress(self):
		cmd = fife.Command()
		cmd.setSource(None)
		cmd.setCommandType(fife.CMD_QUIT_GAME)
		self.engine.getEventManager().dispatchCommand(cmd);
		
	def onAboutButtonPress(self):
		if not self.aboutWindow:
			self.aboutWindow = pychan.loadXML('content/gui/help.xml')
			self.aboutWindow.mapEvents({ 'closeButton' : self.aboutWindow.hide })
			self.aboutWindow.distributeData({ 'helpText' : open("content/infotext.txt").read() })
		self.aboutWindow.show()


class IslandDemo(ApplicationBase):
	def __init__(self):
		super(IslandDemo,self).__init__()
		pychan.init(self.engine, debug=True)
		self.world = world.World(self.engine)
		self.listener = ApplicationListener(self.engine, self.world)
		self.world.load(TDS.MapFile)
		
		self.soundmanager = self.engine.getSoundManager()
		self.soundmanager.init()

		# play track as background music
		emitter = self.soundmanager.createEmitter()
		emitter.load('content/audio/music/caribic.ogg')
		emitter.setLooping(True)
		emitter.play()
		
	def _pump(self):
		if self.listener.quit:
			self.breakRequested = True
			self.world.save('content/maps/savefile.xml')

def main():
	app = IslandDemo()
	app.run()


if __name__ == '__main__':
	main()