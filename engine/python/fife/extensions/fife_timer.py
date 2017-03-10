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

"""
Convenient timers
=================

Usage::
  import fife.extensions.fife_timer
  
  fife_timer.init( my_fife_engine.getTimeManager() )
  
  def spam():
     print "SPAM SPAM ",
  
  repeater = fife_timer.repeatCall(500,spam)
  
  def stop_spam():
     repeater.stop()
     print "BACON EGGS AND SPAM"
  
  delayed = fife_timer.delayCall(50000,stop_spam)

"""

from fife import fife

#global time manager
_manager = None

def init(timemanager):
	"""
	Initialize timers.

	@param timemanager: A L{fife.TimeManager} as retuned by L{fife.Engine.getTimeManager}.
	"""
	global _manager
	_manager = timemanager

class Timer(fife.TimeEvent):
	"""
	Timer
	
	This class wraps the fife.TimeEvent class to make it easily usable from Python
	It allows for a TimeEvent to be executed once or multiple times.
	
	Remember FIFE::TimeManager does NOT delete the timer so make sure you keep a reference 
	to this timer to ensure python doesnt delete the timer prematurely.
	"""
	def __init__(self,delay=0,callback=None,repeat=0):
		"""
		@param delay: The delay in milliseconds to execute the callback
		@param callback: The function to execute when the time delay has passed
		@param repeat: The number of times to execute the callback.  1=once, 0=forever 
		"""
		super(Timer,self).__init__(delay)
		self._active = False
		self._callback = callback
		self._manager = _manager
		self.setPeriod(delay)
		self._repeat = repeat
		self._executed = 0
		
	def start(self):
		"""
		Call this to start the timer.
		
		This registers the timer with the time manager.  The time manger then
		calls the timers updateEvent() function when the delay time has passed.
		"""
		if self._active:
			return
		self._active = True
		
		self._executed = 0
		
		self.setLastUpdateTime(self._manager.getTime())
		self._manager.registerEvent(self)
		
	def stop(self):
		"""
		Stops the timer
		
		This unregisters the timer from the time manager.  
		"""
		if not self._active:
			return
			
		self._active = False
		self._manager.unregisterEvent(self)
		
	def updateEvent(self,delta):
		"""
		This is called by FIFE::TimeManager when the delay has passed.
		
		Should not be called directly.
		"""
		
		if self._repeat != 0:
			self._executed += 1
			if self._executed >= self._repeat:
				self.stop()

		if callable(self._callback):
			self._callback()

	def _setDelay(self, delay):
		"""
		Sets how many milliseconds to wait before executing the callback.
		
		The timer must not be active to change this value
		
		@param delay: Number of milliseconds to wait before executing the callback.
		@type delay: C{integer}
		"""
		
		if not self._active:
			self.setPeriod(delay)
	
	def _getDelay(self):
		"""
		Returns the number of milliseconds to wait before executing the callback.
		
		@return: Number of milliseconds.
		@rtype: C{integer}
		"""
		return self.getPeriod()
	
	def _setCallback(self, callback):
		self._callback = callback
		
	def _getCallback(self):
		return self._callback
	
	def _setRepeat(self, repeat):
		"""
		Sets how many times the timer should be repeated.
		
		The timer must not be active to change it's repeat value.
		
		@param repeat: Number of times to repeat the timer.  0=forever, 1=once.
		@type repeat: C{integer}
		"""
		
		if not self._active:
			self._repeat = repeat
		
	def _getRepeat(self, repeat):
		"""
		Returns the number of times the timer will be executed.
		
		@return: Number of times the timer will be executed.
		@rtype: C{integer}
		"""
		return self._repeat
	
	def _getActive(self):
		"""
		Returns True if the timer is active and False if it is not.
		
		@return: True if timer is active, False if it is not.
		@rtype: C{boolean}
		"""
		return self._active
	
	def _getNumExecuted(self):
		"""
		Returns the number of times the timer has been executed
		
		@return: Number of times the timer has been executed
		@rtype: C{integer}
		"""
		return self._executed
	
	delay = property(_getDelay, _setDelay)
	callback = property(_getCallback, _setCallback)
	repeat = property(_getRepeat, _setRepeat)
	active = property(_getActive)
	numexecuted = property(_getNumExecuted)
	

def delayCall(delay,callback):
	"""
	Delay a function call by a number of milliseconds.
	
	Remember to keep a reference to the timer this function returns.  If you 
	do not python will delete the timer prematurely which may case a segfault.

	@param delay: Delay in milliseconds.
	@param callback: The function to call.

	@return: The timer.
	@rtype: L{Timer}
	"""
	timer = Timer(delay, callback, 1)
	timer.start()
	return timer


def repeatCall(period,callback):
	"""
	Repeat a function call. The call is repeated until the timer is stopped.

	Remember to keep a reference to the timer this function returns.  If you 
	do not python will delete the timer prematurely which may case a segfault.

	@param period: Period between calls in milliseconds.
	@param callback: The function to call.

	@return: The timer.
	@rtype: L{Timer}
	"""
	timer = Timer(period, callback, 0)
	timer.start()
	return timer

__all__ = ['init','Timer','delayCall','repeatCall']

