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

print "Testing audio"
audiomanager = engine.getAudioManager()
audiomanager.setAmbientSound('content/audio/music/maybe.ogg')
for i in xrange(8):
	audiomanager.setVolume(i % 2)
	time.sleep(0.2)

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
	
guimanager = engine.getGuiManager()
console = guimanager.getConsole()
consoleexec = MyExecuter()
console.setConsoleExecuter(consoleexec)

while True:
	engine.pump()
	if consoleexec.quitRequested:
		break
