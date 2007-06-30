import fife, time

print "Creating engine"
engine = fife.Engine()

print "Creating settings manager"
settings = fife.SettingsManager()

print "Testing log"
log = fife.Log.initialize(fife.Log.LEVEL_MAX, True, True)
fife.Log("Testing log...")


class MyEvent(fife.TimeEvent):
	def __init__(self, period):
		fife.TimeEvent.__init__(self, period)
		self.counter = 0
	
	def updateEvent(self, curtime):
		print "testing timer event... %d, %d" % (curtime, self.counter)
		self.counter += 1

m = fife.TimeManager()
e = MyEvent(300)
m.registerEvent(e)

for i in xrange(20):
	time.sleep(0.1)
	m.update()

print "finishing"
m.unregisterEvent(e)
del e