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

class SoundClip(object):
	def __init__(self, soundmanager, clipid, soundname, emitter):
		self._soundmanager = soundmanager
		self._name = soundname
		self._fifeclipid = clipid
		self._fifeemitter = emitter
		self._fifeemitter.thisown = 0
		self._gain = 255.0
		self._looping = False
		
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

	clipid = property(_getClipID)
	gain = property(_getGain, _setGain)
	looping = property(_getLooping, _setLooping)
	fifeemitter = property(_getFifeEmitter, _setFifeEmitter)
	name = property(_getName)

class SoundManager(object):
	def __init__(self, engine):
		self._engine = engine
		
		self._soundmanager = self._engine.getSoundManager()
		self._soundmanager.init()
		
		self._emitters = []
		self._loadedclips = {}
		
	def loadSoundClip(self, filename):
		if not self._loadedclips.has_key(filename):
			clipid = self._engine.getSoundClipPool().addResourceFromFile(filename)
			fifeemitter = self._soundmanager.createEmitter()
			fifeemitter.thisown = 0
			fifeemitter.setSoundClip(clipid)
			self._loadedclips[filename] = SoundClip(self, clipid, filename, fifeemitter)
			self._emitters.append(fifeemitter)
		
		return self._loadedclips[filename]
		
	def playClip(self, clip):
		if clip.fifeemitter:
			clip.fifeemitter.setGain(clip.gain)
			clip.fifeemitter.setLooping(clip.looping)
			clip.fifeemitter.play()	
		else:
			self.loadSoundClip(clip.name)
				
	def stopClip(self, clip):
		if clip.fifeemitter:
			clip.fifeemitter.stop()
		else:
			self.loadSoundClip(clip.name)

	def stopAllSounds(self):
		for emitter in self._emitters:
			emitter.stop()
			
	def destroy(self):
		print "Destroying " + str(len(self._emitters)) + " sound emitters!\n"
		print "There are " + str(len(self._loadedclips)) + " unique sounds!\n"
	
		for emitter in self._emitters[:]:
			emitter.stop()
			self._soundmanager.releaseEmitter(emitter.getID())
			self._emitters.remove(emitter)
		
		for clip in self._loadedclips.values():
			clip.fifeemitter = None
		
			
		self._emitters = list()
		self._loadedclips.clear()
		