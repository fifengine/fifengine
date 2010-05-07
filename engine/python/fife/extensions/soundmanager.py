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

import fife.extensions.fife_timer as fife_timer
from fife.extensions.pychan.tools import callbackWithArguments as cbwa

"""
Sound Manager
==================================

This is a simple implementation of a sound manager that was originaly
intended for the shooter demo.  It was functional enough that we decided
to include it in the FIFE extensions.  This is by no means a fully featured
implementation for several reasons.  It doesnt limit how many sounds can 
play at once or allow the positioning of sounds.  It does however provide 
a good starting point for a more advanced version of a sound manager.

"""

class SoundEmitter(object):
	"""
	SoundEmitter

	This class wraps an instance of a L{fife.SoundEmitter} class along 
	with some information about a sound clip (like gain and if its
	looping).  All instances of SoundEmitter should be created by SoundManager.
	
	@todo At some point this class will store positional information
	and also be responsible for updating the L{fife.SoundEmitter} position.
	"""
	def __init__(self, soundmanager, clipid, soundname, emitter):
		"""
		@param soundmanager A reference to the SoundManager
		@param clipid The FIFE sound clip ID from the sound clip pool
		@param soundname The filename of the sound
		@param emitter A reference to the L{fife.SoundEmitter} associated with this clip
		
		"""
		self._soundmanager = soundmanager
		self._name = soundname
			
		#The FIFE SoundEmitter associated with this SoundEmitter.
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
		
	def play(self):
		self._soundmanager.playClip(self)
		
	def stop(self):
		self._soundmanager.stopClip(self)
		
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
	It creates SoundEmitters and ensures that there is only one 
	L{fife.SoundEmitter} per unique sound.
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
		
	def createSoundEmitter(self, filename, forceUnique=False):
		"""
		Returns a valid SoundEmitter instance.
		
		@param filename The relative path and filename of the sound file
		@parm forceUnique This forces a new L{fife.SoundEmitter} to be created.  
		This is useful if you want more than one instance of the same sound 
		to be played at the same time.
		
		@return Returns a new SoundEmitter instance.
		"""
		if not self._loadedclips.has_key(filename):
			clipid = self._engine.getSoundClipPool().addResourceFromFile(filename)
			fifeemitter = self._fifesoundmanager.createEmitter()
			fifeemitter.thisown = 0
			fifeemitter.setSoundClip(clipid)
			
			self._loadedclips[filename] = [fifeemitter]
			clip = SoundEmitter(self, clipid, filename, fifeemitter)
			clip.duration = fifeemitter.getDuration()
		else:
			if forceUnique:
				clipid = self._engine.getSoundClipPool().addResourceFromFile(filename)
				fifeemitter = self._fifesoundmanager.createEmitter()
				fifeemitter.thisown = 0
				fifeemitter.setSoundClip(clipid)
				self._loadedclips[filename].append(fifeemitter)	
			else:
				fifeemitter = self._loadedclips[filename][0]
				
			clip = SoundEmitter(self, fifeemitter.getID(), filename, fifeemitter)
			clip.duration = fifeemitter.getDuration()
		
		return clip
		
	def playClip(self, clip):
		"""
		Plays a sound clip.  
		
		This function does not use the L{fife.SoundEmitter}
		"looping" property to loop a sound.  Instead it registers 
		a new timer and uses the duration of the clip as the timer length.
		
		If the SoundEmitter is invalid (no fifeemitter) then it attempts 
		to load it before playing it.
		
		@note This will stop any clips that use the same L{fife.SoundEmitter}.
		You cannot play the same sound more than once at a time unless you create
		the SoundEmitter with the forceUnique paramater set to True.
		
		@param clip The SoundEmitter to be played
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
						e.setGain(float(g)/255.0)
						e.play()

					clip.callback = cbwa(real_callback, clip.callback, clip.fifeemitter, clip.gain)

				else:
					repeat = 1
					
				clip.timer = fife_timer.Timer(clip.duration, clip.callback, repeat)
				clip.timer.start()
			else:
				if clip.looping:
					def real_callback(e, g):
						e.stop()
						e.setGain(float(g)/255.0)
						e.play()

					clip.callback = cbwa(real_callback, clip.fifeemitter, clip.gain)
					clip.timer = fife_timer.Timer(clip.duration, clip.callback, 0)
					clip.timer.start()
					
				
			clip.fifeemitter.setGain(float(clip.gain)/255.0)
			clip.fifeemitter.play()	
		else:
			clip = self.createSoundEmitter(clip.name)
			self.playClip(clip)
				
	def stopClip(self, clip):
		"""
		Stops playing the sound clip.   Note that this will stop all clips that 
		use the same FIFE emitter.
		
		@parm clip The SoundEmitter to stop.
		"""
		if clip.fifeemitter:
			clip.fifeemitter.stop()
			
		if clip.timer:
			clip.timer.stop()
			clip.timer = None

	def stopAllSounds(self):
		for emitterlist in self._loadedclips.values():
			for emitter in emitterlist:
				emitter.stop()
			
	def destroy(self):
		"""
		Releases all instances of L{fife.SoundEmitter}.  This does not free 
		the resources from the FIFE sound clip pool.
		"""
		self.stopAllSounds()
	
		for emitterlist in self._loadedclips.values():
			for emitter in emitterlist:
				self._fifesoundmanager.releaseEmitter(emitter.getID())
				emitter = None
		
		self._loadedclips.clear()

__all__ = ['SoundEmitter','SoundManager']
