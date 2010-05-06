# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2010 by the FIFE team
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

from fife import fife

from scripts.common.helpers import Timer
from fife.extensions.pychan.tools import callbackWithArguments as cbwa

class SoundClip(object):
	"""
	SoundClip

	This class stores an instance of a FIFE SoundEmitter class along 
	with some information about a sound clip (like gain and if its
	looping).  All instances of SoundClip should be created by SoundManager.
	"""
	def __init__(self, soundmanager, clipid, soundname, emitter):
		"""
		@param soundmanager A reference to the SoundManager
		@param clipid The FIFE sound clip ID from the sound clip pool
		@param soundname The filename of the sound
		@param emitter A reference to the FIFE SoundEmitter associated with this clip
		
		"""
		self._soundmanager = soundmanager
		self._name = soundname
			
		#The FIFE SoundEmitter associated with this SoundClip.
		#Note that we do NOT own the emitter.
		self._fifeemitter = emitter
		self._fifeemitter.thisown = 0
		self._fifeclipid = clipid
				
		#0 = mute, 255 = normal volume
		self._gain = 255.0
		self._looping = False
		
		#if you set the callback it will be executed after the sound
		#has finished playing.
		self._callback = None
		
		#length of the sound
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
	"""
	SoundManger

	This class manages and plays all the sounds of the game.  
	It creates SoundClips and ensures that there is only one 
	FIFE SoundEmitter per unique sound.
	"""
	def __init__(self, engine):
		"""
		@param engine A reference to the FIFE engine
		"""
		self._engine = engine
		
		self._fifesoundmanager = self._engine.getSoundManager()
		self._fifesoundmanager.init()
		
		#A dict of fife emitters
		self._loadedclips = {}
		
	def loadSoundClip(self, filename):
		"""
		Returns a valid SoundClip instance.
		
		@param filename the relative path and filename of the sound file
		
		@return Returns a new SoundClip instance.
		"""
		if not self._loadedclips.has_key(filename):
			clipid = self._engine.getSoundClipPool().addResourceFromFile(filename)
			fifeemitter = self._fifesoundmanager.createEmitter()
			fifeemitter.thisown = 0
			fifeemitter.setSoundClip(clipid)
			
			self._loadedclips[filename] = fifeemitter
			clip = SoundClip(self, clipid, filename, fifeemitter)
			clip.duration = fifeemitter.getDuration()
		else:
			fifeemitter = self._loadedclips[filename]
			clip = SoundClip(self, fifeemitter.getID(), filename, fifeemitter)
			clip.duration = fifeemitter.getDuration()
		
		return clip
		
	def playClip(self, clip):
		"""
		Plays a sound clip.  
		
		This function does not use the FIFE
		emitters "looping" property to loop a sound.  Instead
		it registers a new timer and uses the duration of the 
		clip as the timer length.
		
		If the SoundClip is invalid (no fifeemitter) then
		it attempts to load it before playing it.
		
		Note that this will stop any clips that use the same
		FIFE emitter.  You cannot play the same sound more than
		once at a time.
		
		@param clip The SoundClip to be played
		"""
		
		if clip.fifeemitter:
			if clip.callback:
				if clip.timer:
					clip.timer.stop()
					timer = None
					
				if clip.looping:
					repeat = 0
					def real_callback(c, e, g):
						c()
						e.stop()
						e.setGain(g)
						e.play()

					clip.callback = cbwa(real_callback, clip.callback, clip.fifeemitter, clip.gain)

				else:
					repeat = 1
					
				clip.timer = Timer(self._engine.getTimeManager(), clip.duration, clip.callback, repeat)
				clip.timer.start()
				
			clip.fifeemitter.setGain(clip.gain)
			clip.fifeemitter.play()	
		else:
			clip = self.loadSoundClip(clip.name)
			self.playClip(clip)
				
	def stopClip(self, clip):
		"""
		Stops playing the sound clip.   Note that this will
		affect all clips that use the same FIFE emitter.
		
		@parm clip The SoundClip to stop.
		"""
		if clip.fifeemitter:
			clip.fifeemitter.stop()
			
		if clip.timer:
			clip.timer.stop()
			clip.timer = None

	def stopAllSounds(self):
		for emitter in self._loadedclips.values():	
			emitter.stop()
			
	def destroy(self):
		"""
		Releases all FIFE emitters.  This does not free the resources
		from the FIFE sound clip pool.
		"""
		self.stopAllSounds()
	
		for emitter in self._loadedclips.values():
			self._fifesoundmanager.releaseEmitter(emitter.getID())
			emitter = None
		
		self._loadedclips.clear()
