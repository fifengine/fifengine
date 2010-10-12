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
from scripts.common.baseobject import *
from scripts.ships.shipbase import *
from scripts.weapons import *


class PlayerActionListener(ShipActionListener):
	def __init__(self, ship):
		super(PlayerActionListener, self).__init__(ship)

	def onInstanceActionFinished(self, instance, action):
		super(PlayerActionListener, self).onInstanceActionFinished(instance, action)
		
		if action.getId() == 'explode':
			self._ship.respawn()

		
		
class Player(Ship):
	def __init__(self, scene, playerName):
		super(Player, self).__init__(scene, playerName)

		self._type = SHTR_PLAYER

		self._score = 0
		self._maxvelocity = 1.5
		self._acceleration = 1.0
		
		self.width = 0.22
		self.height = 0.12		
		
		self._actionlistener = PlayerActionListener(self)

		self._lives = 3		
		self.init()

		
	def init(self):
		self._hitpoints = 2
		self._dead = False
		self._invulnerable = False

		#give player the default weapon (the cannon)
		self.weapon = Cannon(self._scene, self, 200)
	
	def _getScore(self):
		return self._score
		
	def respawn(self):
		if self._lives >= 0:
			self.init()
			self.setInvulnerable(1000)

			campos = self._scene.camera.getLocation().getExactLayerCoordinates()
			location = self.location
			playerpos = location.getExactLayerCoordinates()
			
			playerpos.x = campos.x - 6.5
			playerpos.y = campos.y
			location.setExactLayerCoordinates(playerpos)
			self.location = location
		
		else:
			self._instance.get2dGfxVisual().setVisible(False)
			self._scene.gameOver()
		
	def setInvulnerable(self, milliseconds):
		"""
		50 is defined in the players "flash" animation file
		2 is the number of frames in the animation		
		
		@todo: read these values somehow from the animation
		
		"""
		number = int((milliseconds / 50) / 2)
		
		if number <= 0:
			return
		
		self._invulnerable = True
		self.flash(number)
		
	def applyScore(self, sc):
		self._score += sc
		
	def applyHit(self, hp):
		if not self._invulnerable and not self._dead:
			super(Player, self).applyHit(hp)
			if not self._dead:
				self.flash(1)
		
	def destroy(self):
		if not self._invulnerable and not self._dead:
			self._instance.act('explode', self._instance.getFacingLocation())
			self._explodclip.play()
			self._dead = True
			self._invulnerable = True
			self._lives -= 1		

	def fire(self, direction):
		return self._weapon.fire(direction)

	def update(self):
	
		NSkey = False
		EWkey = False
		
		#player is no longer invulnerable
		if not self._flashing and self._invulnerable and not self._dead:
			self._invulnerable = False

		oldpos = self.location
		
		if not self._dead:
			if self._scene.keystate['UP']:
				self.applyThrust(fife.DoublePoint(0,-1*self._acceleration))
				NSkey = True
			if self._scene.keystate['DOWN']:
				self.applyThrust(fife.DoublePoint(0,self._acceleration))
				NSkey = True
			if self._scene.keystate['LEFT']:
				self.applyThrust(fife.DoublePoint(-1*self._acceleration,0))
				EWkey = True
			if self._scene.keystate['RIGHT']:
				self.applyThrust(fife.DoublePoint(self._acceleration,0))
				EWkey = True
		
			if NSkey and not EWkey:
				if self._velocity.x != 0:
					vel = self._acceleration * cmp(self._velocity.x, 0) * -1
					self.applyThrust(fife.DoublePoint(vel, 0))
			elif EWkey and not NSkey:
				if self._velocity.y != 0:
					vel = self._acceleration * cmp(self._velocity.y, 0) * -1
					self.applyThrust(fife.DoublePoint(0, vel))
			elif not NSkey and not EWkey:
				self.applyBrake(self._acceleration)
		
			#fire the currently selected gun
			if self._scene.keystate['SPACE']:
				self.fire(fife.DoublePoint(1,0))
			
		if self._dead and self._velocity.length() > 0:
			self.applyBrake(self._acceleration)
		
		super(Player, self).update()
		
		#set up the players camera bounds
		#@todo: grab screen resolution from somewhere
		topleft = self._scene.camera.toMapCoordinates(fife.ScreenPoint(0,0))
		bottomright = self._scene.camera.toMapCoordinates(fife.ScreenPoint(1024,768))

		camrect = Rect(topleft.x, topleft.y, bottomright.x - topleft.x, bottomright.y - topleft.y)
	
		#add the padding to the edge
		camrect.x += self._boundingBox.w
		camrect.y += self._boundingBox.h
		camrect.w -= 2*self._boundingBox.w
		camrect.h -= 2*self._boundingBox.h
		
		pos = oldpos.getExactLayerCoordinates()
		
		if not self._boundingBox.intersects(camrect):
			if (self._boundingBox.x + self._boundingBox.w) < camrect.x:
				if self._velocity.x < 0:
					self._velocity.x = 0
				pos.x += (camrect.x - (self._boundingBox.x + self._boundingBox.w))/self._xscale + 0.03
				
				if not ((self._boundingBox.y + self._boundingBox.h) < camrect.y) and not (self._boundingBox.y > (camrect.y + camrect.h)):
					pos.y += self._velocity.y * (self._scene.timedelta/1000.0)/self._yscale
					
				oldpos.setExactLayerCoordinates(pos)
			if self._boundingBox.x > ( camrect.x + camrect.w ):
				self._velocity.x = 0

			if (self._boundingBox.y + self._boundingBox.h) < camrect.y:
				if self._velocity.y < 0:
					self._velocity.y = 0
				pos.x += self._velocity.x * (self._scene.timedelta/1000.0)/self._xscale
				oldpos.setExactLayerCoordinates(pos)			
			if self._boundingBox.y > (camrect.y + camrect.h):
				if self._velocity.y > 0:
					self._velocity.y = 0
				pos.x += self._velocity.x * (self._scene.timedelta/1000.0)/self._xscale
				oldpos.setExactLayerCoordinates(pos)

			self.location = oldpos

	def _getLives(self):
		return self._lives
		
	def _setLives(self, lives):
		self._lives = lives
		
	def _getInvulnerable(self):
		return self._invulnerable

	def _setInvulnerable(self, inv):
		self._invulnerable = inv

	score = property(_getScore)
	lives = property(_getLives, _setLives)
	invulnerable = property(_getInvulnerable, _setInvulnerable)
