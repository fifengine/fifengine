#!/usr/bin/python
from __init__ import *

class MyEventListener(fife.ICommandListener):
	def __init__(self):
		fife.ICommandListener.__init__(self)
	
	def onCommand(self, command):
		print "received command with code %d" % command.getCode()		

class TestEventChannel(unittest.TestCase):
	
	def setUp(self):
		self.engine = fife.Engine(True)
		self.eventmanager = self.engine.getEventManager()
		
	def tearDown(self):
		del self.engine
		
	def testEvents(self):
		l = MyEventListener()
		self.eventmanager.addCommandListener(l)
		cmd = fife.Command()
		cmd.setCode(0)
		print "Sending commands..."
		for i in xrange(20):
			self.eventmanager.dispatchCommand(cmd)
			cmd.setCode(i)
		self.eventmanager.removeCommandListener(l)		

TEST_CLASSES = [TestEventChannel]

if __name__ == '__main__':
	unittest.main()
