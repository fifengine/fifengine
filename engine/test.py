import fife, time

print "Creating engine"
engine = fife.Engine()

print "Creating settings manager"
settings = fife.SettingsManager()

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

timemanager = fife.TimeManager()
e = MyTimeEvent(100)
timemanager.registerEvent(e)

for i in xrange(10):
	time.sleep(0.1)
	timemanager.update()

print "finishing"
timemanager.unregisterEvent(e)
del e

eventmanager = fife.EventManager()
class MyEventListener(fife.ICommandListener):
	def __init__(self):
		fife.ICommandListener.__init__(self)
	
	def onCommand(self, command):
		print "received command with code %d" % command.getCode()		

l = MyEventListener()
eventmanager.addCommandListener(l)
cmd = fife.Command()
cmd.setCode(0)
print "Sending command..."
for i in xrange(20):
	eventmanager.dispatchCommand(cmd)
	cmd.setCode(i)
eventmanager.removeCommandListener(l)
del l


vfs = fife.VFSUtility()
print vfs.listFiles('.')
print vfs.listDirectories('.')
print vfs.readLines('test.py')
print vfs.readBytes('test.py')


audiomanager = fife.AudioManager()
audiomanager.setAmbientSound('../content/audio/music/maybe.ogg')
for i in xrange(30):
	audiomanager.setVolume(i % 8)
	time.sleep(0.2)
del audiomanager
