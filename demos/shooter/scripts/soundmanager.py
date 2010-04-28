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


class SoundEmitter(object):
	def __init__(self, fifeemitter, clipid, soundname ):
		self._name = soundname
		self._fifeemitter = fifeemitter
		self._fifeclipid = clipid
		self._gain = 255.0
	
	def play(self, looping=False):
		self._fifeemitter.stop()
		self._fifeemitter.setGain(float(self._gain)/255.0)
		self._fifeemitter.setLooping(looping)
		self._fifeemitter.play()
		
	def stop(self):
		self._fifeemitter.stop()
		
	def _getGain(self):
		return self._gain
		
	def _setGain(self, gain):
		self._gain = float(gain)
		
	def _getClipID(self):
		return self._fifeclipid
	
	def _getCallback(self):
		return self._callback
		
	def _setCallback(self, cb):
		self._callback = cb
		
	def _getFifeEmitter(self):
		return self._fifeemitter
		
	def _setFifeEmitter(self, emitter):
		self._fifeemitter = emitter
	
	clipid = property(_getClipID)
	gain = property(_setGain, _getGain)
	callback = property(_getCallback, _setCallback)
	fifeemitter = property(_getFifeEmitter, _setFifeEmitter)
	

class SoundManager(object):
	def __init__(self, engine):
		self._engine = engine
		
		self._soundmanager = self._engine.getSoundManager()
		self._soundmanager.init()
		
		self._emitters = list()
		self._loadedclips = {}

	def createSoundEmitter(self, filename):
		if not filename in self._loadedclips:
			clipid = self._engine.getSoundClipPool().addResourceFromFile(filename)
			fifeemitter = self._soundmanager.createEmitter()
			fifeemitter.setSoundClip(clipid)
			self._loadedclips[filename] = clipid
			emitter = SoundEmitter(fifeemitter, clipid, filename)
		else:
			fifeemitter = self._soundmanager.createEmitter()
			fifeemitter.setSoundClip(self._loadedclips[filename])
			emitter = SoundEmitter(fifeemitter, self._loadedclips[filename], filename)
		
		self._emitters.append(emitter)
		return emitter

	def stopAllSounds(self):
		for emitter in self._emitters:
			emitter.stop()
			
	def destroy(self):
		for emitter in self._emitters:
			emitter.stop()
			self._soundmanager.releaseEmitter(emitter.fifeemitter.getID())
			emitter.fifeemitter = None			

		self._loadedclips.clear()