# -*- coding: utf-8 -*-
import fife

"""
Convenient timers
=================

Usage::
  import timer
  timer.init( my_fife_engine.getTimeManager() )
  def spam():
     print "SPAM SPAM ",
     return "More spam?" # a string is a true value, so it's repeated.
  repeater = timer.repeatCall(500,spam)
  def stop_spam():
     repeater.stop()
     print "BACON EGGS AND SPAM"
  timer.delayCall(50000,stop_spam)

"""


_manager = None
_alltimers = {}

def init(timemanager):
	"""
	Initialize timers.

	@param timemanager: A L{fife.TimeManager} as retuned by L{fife.Engine.getTimeManager}.
	"""
	global _manager
	_manager = timemanager

class Timer(fife.TimeEvent):
	def __init__(self,delay=0,callback=None):
		super(Timer,self).__init__(0)
		self.manager = _manager
		self.is_registered = False
		self.callback = callback
		self.setPeriod(delay)

	def start(self):
		if self.is_registered:
			return
		self.is_registered = True
		global _alltimers
		_alltimers[self]=1
		self.manager.registerEvent(self)

	def stop(self):
		if not self.is_registered:
			return
		self.is_registered = False
		global _alltimers
		del _alltimers[self]
		self.manager.unregisterEvent(self)

	def updateEvent(self,delta):
		if callable(self.callback):
			self.callback()

def delayCall(delay,callback):
	"""
	Delay a function call by a number of milliseconds.

	@param delay Delay in milliseconds.
	@param callback The function to call.

	@return The timer.
	"""
	timer = Timer(delay)
	def real_callback():
		timer.stop()
		callback()
	timer.callback = real_callback
	timer.start()
	return timer

from traceback import print_exc
def repeatCall(period,callback):
	"""
	Repeat a function call.

	@param period Period between calls in milliseconds.
	@param callback The function to call.

	@return The timer.

	The call is repeated until the callback returns a False
	value (i.e. None) or the timer is stopped.
	"""
	timer = Timer(period)
	def real_callback():
		try:
			if not callback():
				timer.stop()
		except Exception:
			print_exc()
			timer.stop()

	timer.callback = real_callback
	timer.start()
	return timer

__all__ = [init,Timer,delayCall,repeatCall]

