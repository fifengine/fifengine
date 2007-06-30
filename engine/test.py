import fife, time

print "Creating engine"
engine = fife.Engine()

print "Creating settings manager"
settings = fife.SettingsManager()

print "Testing log"
log = fife.Log.initialize(fife.Log.LEVEL_MAX, True, True)
fife.Log("Testing log...")


class TT(fife.TimerListener):
	def __init__(self):
		fife.TimerListener.__init__(self)
		self.counter = 0
	
	def onTimer(self):
		print "testing timer event... %d" % self.counter
		self.counter += 1

m = fife.TimeManager()
timer = fife.Timer()
t = TT()
print issubclass(TT, fife.TimerListener)

timer.setListener(t)
timer.setInterval(300)
timer.start()

for i in xrange(20):
	time.sleep(0.1)
	m.update()

print "finishing"
del timer