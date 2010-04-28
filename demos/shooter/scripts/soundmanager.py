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


class SoundManager(object):
	def __init__(self, engine):
		self._engine = engine
		
		self._soundmanager = self._engine.getSoundManager()
		self._soundmanager.init()
		
		self._emitters = {}

	def playSound(self, soundname, volume=255):
		if soundname in self._emitters:
			self._emitters[soundname].stop()
			self._emitters[soundname].play()
		else:
			#create an emitter for this sound
			emitter = self._soundmanager.createEmitter()
			clipid = self._engine.getSoundClipPool().addResourceFromFile(soundname)
			emitter.setSoundClip(clipid)
			emitter.setLooping(False)
			
			self._emitters[soundname] = emitter
			self._emitters[soundname].thisown = 0
			
			self._emitters[soundname].play()
			
	def stopSound(self, soundname):
		if soundname in self._emitters:
			self._emitters[soundname].stop()
			
	def destroy(self):
		todelete = {}
		for emitter in self._emitters:
			self._emitters[emitter].stop()
			clipid = self._emitters[emitter].getID()
			self._soundmanager.releaseEmitter(clipid)

		self._emitters.clear()