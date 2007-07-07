import os
os.environ['PYTHONPATH'] = '.'
print os.getenv('PYTHONPATH')
import engine.fife as fife
import time

print "Creating engine"
engine = fife.Engine()

print "Testing settings manager"
settings = engine.getSettingsManager()
print "  RenderBackend = " + settings.read_string('RenderBackend', 'none defined')
screenwidth = settings.read_int("ScreenWidth", 800)
screenheight = settings.read_int("ScreenHeight", 600)

print "Testing log"
log = fife.Log.initialize(fife.Log.LEVEL_MAX, True, True)
fife.Log("Testing log...")


print "Testing timers"
class MyTimeEvent(fife.TimeEvent):
	def __init__(self, period):
		fife.TimeEvent.__init__(self, period)
		self.counter = 0
	
	def updateEvent(self, curtime):
		print "testing timer event... %d, %d" % (curtime, self.counter)
		self.counter += 1

timemanager = engine.getTimeManager()
e = MyTimeEvent(100)
timemanager.registerEvent(e)

for i in xrange(10):
	time.sleep(0.1)
	timemanager.update()

print "finishing"
timemanager.unregisterEvent(e)

eventmanager = engine.getEventManager()
class MyEventListener(fife.ICommandListener):
	def __init__(self):
		fife.ICommandListener.__init__(self)
	
	def onCommand(self, command):
		print "received command with code %d" % command.getCode()		

l = MyEventListener()
eventmanager.addCommandListener(l)
cmd = fife.Command()
cmd.setCode(0)
print "Sending commands..."
for i in xrange(20):
	eventmanager.dispatchCommand(cmd)
	cmd.setCode(i)
eventmanager.removeCommandListener(l)

print "Testing VFS"
vfs = fife.VFSUtility()
print vfs.listFiles('.')
print vfs.listDirectories('.')
print vfs.readLines('fife_engine.py')
print vfs.readBytes('fife_engine.py')

print "Testing GUI"
guimanager = engine.getGuiManager()
fonts = [fife.TTFont('content/fonts/FreeMono.ttf', 14), 
         fife.ImageFont('content/fonts/rpgfont.png', ' abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/:();%`\'*#=[]"')]
for f in fonts:
	f.setColor(255,20,20)
container = fife.Container()
guimanager.add(container)
container.setSize(screenwidth, screenheight)
container.setOpaque(False)
label1 = fife.Label('This is a Truetype font')
label1.setPosition(0, 70)
label1.setFont(fonts[0])
container.add(label1)
label2 = fife.Label('This is a Image font')
label2.setPosition(0, 100)
label2.setFont(fonts[1])
container.add(label2)
labels = [label1, label2]



print "Testing audio"
audiomanager = engine.getAudioManager()
audiomanager.setAmbientSound('content/audio/music/maybe.ogg')
for i in xrange(20):
	audiomanager.setVolume(float(i % 10) / 10)
	time.sleep(0.1)

print "Testing map"
ctrl = fife.Control()
#ctrl.addMapLoader(fife.XML())


class MyExecuter(fife.ConsoleExecuter):
	def __init__(self):
		fife.ConsoleExecuter.__init__(self)
		self.quitRequested = False
		
	def onToolsClick(self):
		print "In python, tools clicked"

	def onCommand(self, command):
		print "In python, command %s received" % command
		result = "no result"
		if command.lower() in ('quit', 'exit'):
			self.quitRequested = True
			return "quitting"
		
		try:
			result = str(eval(command))
		except:
			pass
		print "result = " + result
		return result
	
console = guimanager.getConsole()
consoleexec = MyExecuter()
console.setConsoleExecuter(consoleexec)


i = 1
while True:
	engine.pump()
	for l in labels: l.setX(i)
	i = (i + 1) % 100
	if consoleexec.quitRequested:
		break
