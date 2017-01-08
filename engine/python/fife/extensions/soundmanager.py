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
Sound Manager
==================================

This is a simple implementation of a sound manager that was originaly
intended for the shooter demo.  It was functional enough that we decided
to include it in the FIFE extensions.  This is by no means a fully featured
implementation for several reasons.  It doesnt limit how many sounds can
play at once or allow the positioning of sounds.  It does however provide
a good starting point for a more advanced version of a sound manager.

Usage::
  soundmanager = SoundManager(my_fife_engine)

  emitter = soundmanager.createSoundEmitter("path/filename.ogg")
  emitter.gain = 128
  emitter.play()

"""

from fife import fife

import fife.extensions.fife_timer as fife_timer

def cbwa(callback,*args,**kwargs):
	"""
	This was stolen from pychan.  Defined here to remove the dep
	on pychan.
	"""
	def real_callback():
		callback(*args,**kwargs)
	return real_callback


class SoundEmitter(object):
	"""
	Wraps the L{fife.SoundEmitter} class.

	This class wraps an instance of a L{fife.SoundEmitter} class along
	with some information about a sound clip (like gain and if its
	looping).  All instances of SoundEmitter should be created by SoundManager.

	@todo: At some point this class will store positional information
	and also be responsible for updating the L{fife.SoundEmitter} position.
	"""
	def __init__(self, soundmanager, clip, soundname, emitter):
		"""
		@param soundmanager: A reference to the SoundManager
		@type soundmanager: L{SoundManager}
		@param clip: The FIFE sound clip ID from the sound clip pool
		@type clip: C{int}
		@param soundname: The filename of the sound
		@type soundname: C{string}
		@param emitter: A reference to the L{fife.SoundEmitter} associated with this clip
		@type emitter: L{fife.SoundEmitter}

		"""
		self._soundmanager = soundmanager
		self._name = soundname

		#The FIFE SoundEmitter associated with this SoundEmitter.
		#Note that we do NOT own the emitter.
		self._fifeemitter = emitter
		self._fifeemitter.thisown = 0
		self._fifeclip = clip

		#0 = mute, 255 = normal volume
		self._gain = 255.0
		self._looping = False

		#if you set the callback it will be executed after the sound
		#has finished playing.
		self._callback = None

		#length of the sound
		self._duration = 0

		self._timer = None

		self._position = None
		self._rolloff = 0

	def __del__(self):
		self._soundmanager.unregisterClip(self)

	def play(self):
		self._soundmanager.playClip(self)

	def stop(self):
		self._soundmanager.stopClip(self)

	def _getClip(self):
		return self._fifeclip

	def _getGain(self):
		return self._gain

	def _setGain(self, gain):
		"""
		Sets the volume of the L{SoundEmitter}.

		@param gain: Value should be from 0-255.  0 being mute and 255 being the normal
		volume of the clip.
		@type gain: C{int}
		"""
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

	def _setPosition(self, position):
		self._position = position

	def _getPosition(self):
		return self._position

	def _setRolloff(self, rolloff):
		self._rolloff = rolloff

	def _getRolloff(self):
		return self._rolloff

	rolloff = property(_getRolloff, _setRolloff)
	position = property(_getPosition, _setPosition)
	timer = property(_getTimer, _setTimer)
	clip = property(_getClip)
	gain = property(_getGain, _setGain)
	looping = property(_getLooping, _setLooping)
	fifeemitter = property(_getFifeEmitter, _setFifeEmitter)
	name = property(_getName)
	callback = property(_getCallback, _setCallback)
	duration = property(_getDuration, _setDuration)

class SoundManager(object):
	"""
	A simple sound manager class.

	This class manages and plays all the sounds of the game.
	It creates SoundEmitters and ensures that there is only one
	L{fife.SoundEmitter} per unique sound.
	"""
	def __init__(self, engine):
		"""
		@param engine: A reference to the FIFE engine
		@type engine: L{fife.Engine}
		"""

		self._engine = engine
		
		self._fifesoundclipmanager = self._engine.getSoundClipManager()

		self._fifesoundmanager = self._engine.getSoundManager()
		self._fifesoundmanager.init()
		
		self._fifesoundmanager.setListenerOrientation(0,1,0)

		# basic rolloff used for positional sounds
		self._rolloff = 1

		#A dict of fife emitters
		self._loadedclips = {}
		
		#A list of created clips
		self._soundclips = []
		
		#A tuple representing the listener position (x,y)
		self._listenerposition = None

	def createSoundEmitter(self, filename, forceUnique=False, position=None):
		"""
		Returns a valid SoundEmitter instance.

		@param filename: The relative path and filename of the sound file
		@type filename: C{string}
		@param forceUnique: This forces a new L{fife.SoundEmitter} to be created.
		This is useful if you want more than one instance of the same sound
		to be played at the same time.
		@type forceUnique: C{boolean}
		@param position: The position on the map that the sound emitter
		is to be created at.
		@type position: L{tuple}

		@return: Returns a new L{SoundEmitter} instance.
		@rtype: L{SoundEmitter}
		"""
		if filename not in self._loadedclips:
			soundclipptr = self._fifesoundclipmanager.get(filename)
			fifeemitter = self._fifesoundmanager.createEmitter()
			fifeemitter.thisown = 0
			fifeemitter.setSoundClip(soundclipptr)
			self._loadedclips[filename] = [fifeemitter]
			clip = SoundEmitter(self, soundclipptr, filename, fifeemitter)
			clip.duration = fifeemitter.getDuration()
		else:
			if forceUnique:
				soundclipptr = self._fifesoundclipmanager.get(filename)
				fifeemitter = self._fifesoundmanager.createEmitter()
				fifeemitter.thisown = 0
				fifeemitter.setSoundClip(soundclipptr)
				self._loadedclips[filename].append(fifeemitter)
			else:
				fifeemitter = self._loadedclips[filename][0]

			clip = SoundEmitter(self, fifeemitter.getSoundClip(), filename, fifeemitter)
			clip.duration = fifeemitter.getDuration()

		if position is not None:
			clip.position = position
			clip.rolloff = self.rolloff
		
		self._soundclips.append(clip)
		
		return clip

	def playClip(self, clip):
		"""
		Plays a sound clip.

		This function does not use the L{fife.SoundEmitter}
		"looping" property to loop a sound.  Instead it registers
		a new timer and uses the duration of the clip as the timer length.

		If the SoundEmitter is invalid (no fifeemitter) then it attempts
		to load it before playing it.

		@note: This will stop any clips that use the same L{fife.SoundEmitter}.
		You cannot play the same sound more than once at a time unless you create
		the SoundEmitter with the forceUnique paramater set to True.

		@param clip: The L{SoundEmitter} to be played
		@type clip: L{SoundEmitter}
		"""
		if clip.fifeemitter:
			if clip.callback:
				if clip.timer:
					clip.timer.stop()
					timer = None

				if clip.looping:
					repeat = 0
					def real_callback(clip):
						clip.fifeemitter.stop()
						clip.fifeemitter.setGain(float(clip.gain)/255.0)
						if self.listenerposition and clip.position:
							# Use 1 as z coordinate, no need to specify it
							clip.fifeemitter.setPosition(clip.position[0], clip.position[1], 1)
							clip.fifeemitter.setRolloff(clip.rolloff)
						elif self.listenerposition and not clip.position:
							clip.fifeemitter.setPosition(self._listenerposition[0], self._listenerposition[1], 1)
							clip.fifeemitter.setRolloff(self.rolloff)
			
						clip.fifeemitter.play()

					clip.callback = cbwa(real_callback, clip)

				else:
					repeat = 1

				clip.timer = fife_timer.Timer(clip.duration, clip.callback, repeat)

			else:
				if clip.looping:
					def real_callback(clip):
						clip.fifeemitter.stop()
						clip.fifeemitter.setGain(float(clip.gain)/255.0)
						if self.listenerposition and clip.position:
							# Use 1 as z coordinate, no need to specify it
							clip.fifeemitter.setPosition(clip.position[0], clip.position[1], 1)
							clip.fifeemitter.setRolloff(clip.rolloff)
						elif self.listenerposition and not clip.position:
							clip.fifeemitter.setPosition(self._listenerposition[0], self._listenerposition[1], 1)
							clip.fifeemitter.setRolloff(self.rolloff)
							
						clip.fifeemitter.play()

					clip.callback = cbwa(real_callback, clip)
					clip.timer = fife_timer.Timer(clip.duration, clip.callback, 0)

			clip.fifeemitter.setGain(float(clip.gain)/255.0)

			if self.listenerposition and clip.position:
				# Use 1 as z coordinate, no need to specify it
				clip.fifeemitter.setPosition(clip.position[0], clip.position[1], 1)
				clip.fifeemitter.setRolloff(clip.rolloff)
			elif self.listenerposition and not clip.position:
				clip.fifeemitter.setPosition(self._listenerposition[0], self._listenerposition[1], 1)
				clip.fifeemitter.setRolloff(self.rolloff)

			clip.fifeemitter.play()
			if clip.timer:
				clip.timer.start()

		else:
			clip = self.createSoundEmitter(clip.name)
			self.playClip(clip)

	def unregisterClip(self, clip):
		self.stopClip(clip)
		
		if clip in self._soundclips:
			self._soundclips.remove(clip)

	def stopClip(self, clip):
		"""
		Stops playing the sound clip.   Note that this will stop all clips that
		use the same FIFE emitter.

		@param clip: The SoundEmitter to stop.
		@type clip: L{SoundEmitter}
		"""
		if clip.fifeemitter:
			clip.fifeemitter.stop()

		if clip.timer:
			clip.timer.stop()
			clip.timer = None

	def stopAllSounds(self):
		for clip in self._soundclips:
			self.stopClip(clip)

	def destroy(self):
		"""
		Releases all instances of L{fife.SoundEmitter}.

		@note: This does not free the resources from the FIFE sound clip pool.
		"""
		self.stopAllSounds()

		for emitterlist in self._loadedclips.values():
			for emitter in emitterlist:
				self._fifesoundmanager.releaseEmitter(emitter.getId())
				emitter = None

		self._loadedclips.clear()
	def _getRolloff(self):
		return self._rolloff

	def _setRolloff(self, rolloff):
		self._rolloff = rolloff
		
	def _getListenerPosition(self):
		return self._listenerposition
		
	def _setListenerPosition(self, position):
		self._listenerposition = position
		self._fifesoundmanager.setListenerPosition(self._listenerposition[0], self._listenerposition[1], 10)

	rolloff = property(_getRolloff, _setRolloff)
	listenerposition = property(_getListenerPosition, _setListenerPosition)

__all__ = ['SoundEmitter','SoundManager']
