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

from fife import fife

from scripts.common.helpers import Timer
from fife.extensions.pychan.tools import callbackWithArguments as cbwa

class SoundClip(object):
	def __init__(self, soundmanager, clipid, soundname, emitter):
		self._soundmanager = soundmanager
		self._name = soundname
		self._fifeclipid = clipid
		self._fifeemitter = emitter
		self._fifeemitter.thisown = 0
		self._gain = 255.0
		self._looping = False
		self._callback = None
		self._duration = 0
		self._timer = None
		
	def _getClipID(self):
		return self._fifeclipid

	def _getGain(self):
		return self._gain
		
	def _setGain(self, gain):
		self._gain = float(gain)
		
	def _getLooping(self):
		return self._looping
		
	def _setLooping(self, looping):
		self._looping = looping
		
	def _getFifeEmitter(self):
		return self._fifeemitter
		
	def _setFifeEmitter(self, emitter):
		self._fifeemitter = emitter
		if self._fifeemitter:
			self._fifeemitter.thisown = 0

	def _getName(self):
		return self._name

	def _getCallback(self):
		return self._callback

	def _setCallback(self, cb):
		self._callback = cb	
		
	def _getDuration(self):
		return self._duration
	
	def _setDuration(self, millliseconds):
		self._duration = millliseconds

	def _getTimer(self):
		return self._timer
		
	def _setTimer(self, timer):
		self._timer = timer

	timer = property(_getTimer, _setTimer)
	clipid = property(_getClipID)
	gain = property(_getGain, _setGain)
	looping = property(_getLooping, _setLooping)
	fifeemitter = property(_getFifeEmitter, _setFifeEmitter)
	name = property(_getName)
	callback = property(_getCallback, _setCallback)
	duration = property(_getDuration, _setDuration)

class SoundManager(object):
	def __init__(self, engine):
		self._engine = engine
		
		self._fifesoundmanager = self._engine.getSoundManager()
		self._fifesoundmanager.init()
		
		self._emitters = []
		self._loadedclips = {}
		
	def loadSoundClip(self, filename):
		if not self._loadedclips.has_key(filename):
			clipid = self._engine.getSoundClipPool().addResourceFromFile(filename)
			fifeemitter = self._fifesoundmanager.createEmitter()
			fifeemitter.thisown = 0
			fifeemitter.setSoundClip(clipid)
			
			time = fifeemitter.getDuration()
				
			self._loadedclips[filename] = SoundClip(self, clipid, filename, fifeemitter)
			self._loadedclips[filename].duration = time
			self._emitters.append(fifeemitter)
		
		return self._loadedclips[filename]
		
	def playClip(self, clip):
		if clip.fifeemitter:
			if clip.callback:
				if clip.timer:
					clip.timer.stop()
					timer = None
					
				if clip.looping:
					repeat = 0
					print "setting up callback1"

					def real_callback(c, e):
						c()
						print "in here"
						e.stop()
						e.play()

					clip.callback = cbwa(real_callback, clip.callback, clip.fifeemitter)

				else:
					repeat = 1

					
				clip.timer = Timer(self._engine.getTimeManager(), clip.duration, clip.callback, repeat)
				clip.timer.start()
				
			clip.fifeemitter.setGain(clip.gain)
			#clip.fifeemitter.setLooping(clip.looping)
			clip.fifeemitter.play()	
		else:
			clip = self.loadSoundClip(clip.name)
			self.playClip(clip)
				
	def stopClip(self, clip):
		if clip.fifeemitter:
			clip.fifeemitter.stop()
			
		if clip.timer:
			clip.timer.stop()
			clip.timer = None

	def stopAllSounds(self):
		for clip in self._loadedclips.values():	
			self.stopClip(clip)
			
	def destroy(self):
		self.stopAllSounds()
	
		for emitter in self._emitters[:]:
			self._fifesoundmanager.releaseEmitter(emitter.getID())
			self._emitters.remove(emitter)
		
		for clip in self._loadedclips.values():
			clip.fifeemitter = None
		
			
		self._emitters = list()
		self._loadedclips.clear()
