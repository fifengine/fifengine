# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2009 by the FIFE team
#  http://www.fifengine.de
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
  fife_timer.delayCall(50000,stop_spam)

"""

from fife import fife

_manager = None
_alltimers = []

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
	"""
	def __init__(self,delay=0,callback=None,repeat=0):
		"""
		@param delay: The delay in milliseconds to execute the callback
		@param callback: The function to execute
		@param repeat: The number of times to execute the callback.  1=once, 0=forever 
		"""
		super(Timer,self).__init__(delay)
		self._is_registered = False
		self._callback = callback
		self._manager = _manager
		self.setPeriod(delay)
		self._repeat = repeat
		self._executed = 0

	def start(self):
		"""
		Must be called before the timer will start
		"""
		if self._is_registered:
			return
		self._is_registered = True
		self._executed = 0
		self._manager.registerEvent(self)

		global _alltimers
		_alltimers.append(self)

	def stop(self):
		"""
		Stops the timer
		"""
		if not self._is_registered:
			return
		self._is_registered = False
		self._manager.unregisterEvent(self)

		global _alltimers
		_alltimers.remove(self)


	def updateEvent(self,delta):
		"""
		Should not be called directly.  This is called by FIFE.
		"""
		if self._repeat != 0:
			self._executed += 1
			if self._executed >= self._repeat:
				self.stop()

		if callable(self._callback):
			self._callback()


def delayCall(delay,callback):
	"""
	Delay a function call by a number of milliseconds.

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

	@param period: Period between calls in milliseconds.
	@param callback: The function to call.

	@return: The timer.
	@rtype: L{Timer}
	"""
	timer = Timer(period, callback, 0)
	timer.start()
	return timer

__all__ = ['init','Timer','delayCall','repeatCall']

