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
from scripts.ships.shipbase import *
from scripts.ships.player import Player
from scripts.ships.enemies import *
from scripts.powerups import *
from scripts.common.helpers import Rect

class SceneNode(object):
	def __init__(self, spaceobjects = None):
		if not spaceobjects:
			self._spaceobjects = list()
		else:
			self._spaceobjects = spaceobjects
		
	def _getObjects(self):
		return self._spaceobjects
		
	def _setObjects(self, spaceobjects):
		self._spaceobjects = spaceobjects
		
	spaceobjects = property(_getObjects, _setObjects)

class Scene(object):
	def __init__(self, world, engine, objectLayer, soundmanager):
		self._engine = engine
		self._world = world
		self._model = engine.getModel()
		self._layer = objectLayer
		self._soundmanager = soundmanager
		self._nodes = list()
		
		self._player = None
		self._objectstodelete = list()
		
		self._maxnodes = 128
		self._xscale = 0
		
		self._time = 0
		self._timedelta = 0
		self._lasttime = 0
		
		self._xscale = self._layer.getCellGrid().getXScale()
		self._yscale = self._layer.getCellGrid().getYScale()
		
		self._paused = False
		self._timemod = 0
		
		self._gameover = False
		
		self._music = None

	def destroyScene(self):
		nodestodelete = list()
		objtodelete = list()
	
		for node in self._nodes:
			nodestodelete.append(node)
			for obj in node.spaceobjects:
				objtodelete.append(node)
			
			for obj in objtodelete:
				if obj in node.spaceobjects:
					if obj.instance:
						self._layer.deleteInstance(obj.instance)
						obj.instance = None
					node.spaceobjects.remove(obj)
				
			objtodelete = list()
					
		for node in nodestodelete:
			if node in self._nodes:
				self._nodes.remove(node)
			
	def initScene(self, mapobj):
		#initialize our scene array to some arbitrary size
		for i in range(0,self._maxnodes):
			self._nodes.append(SceneNode())

		self._player = Player(self, 'player')
		self._player.init()
		self._player.start()

		enemies = list()
		powerups = list()
		
		temp = self._layer.getInstances('dodge1')
		enemies.extend(temp)

		temp = self._layer.getInstances('dodge2')
		enemies.extend(temp)

		temp = self._layer.getInstances("diag_top_right")
		enemies.extend(temp)

		temp = self._layer.getInstances("diag_bottom_right")
		enemies.extend(temp)

		temp = self._layer.getInstances("streaker")
		enemies.extend(temp)

		temp = self._layer.getInstances("boss")
		enemies.extend(temp)
		
		temp = self._layer.getInstances("cannonspread5")
		powerups.extend(temp)

		temp = self._layer.getInstances("extralife")
		powerups.extend(temp)

		for instance in enemies:
			objectName = instance.getId()
			
			if objectName == "dodge1":
				enemy = Saucer1(self, 'enemy', instance, False)
			elif objectName == "dodge2":
				enemy = Saucer2(self, 'enemy', instance, False)
			elif objectName == "diag_top_right":
				enemy = DiagSaucer(self, 'enemy', 0, instance, False)
			elif objectName == "diag_bottom_right":
				enemy = DiagSaucer(self, 'enemy', 1, instance, False)
			elif objectName == "streaker":
				enemy = Streaker(self, 'enemy', instance, False)
			elif objectName == "boss":
				enemy = Boss(self, 'enemy', instance, False)
			else:
				enemy = Ship(self, 'enemy', instance, False)
				
			enemy.start()

			loc = instance.getLocation().getExactLayerCoordinates()
			nodeindex = int(loc.x * self._xscale)
			enemy.scenenodeid = nodeindex
			self._nodes[nodeindex].spaceobjects.append(enemy)
			
		for instance in powerups:
			objectName = instance.getId()
			
			print objectName
			
			if objectName == "cannonspread5":
				powerup = CannonSpread5PU(self, 'cannonspread5', instance, False)
			elif objectName == "extralife":
				powerup = ExtraLifePU(self, 'extralife', instance, False)
			else:
				powerup = PowerUp(self, 'powerup', instance, False)
				
			powerup.start()
			
			loc = instance.getLocation().getExactLayerCoordinates()
			nodeindex = int(loc.x * self._xscale)
			powerup.scenenodeid = nodeindex
			self._nodes[nodeindex].spaceobjects.append(powerup)			
			
		#and finally add the player to the scene
		self.addObjectToScene(self._player)
		
		self._music = self._soundmanager.loadSoundClip("music/waynesmind2.ogg")
		self._music.callback = self.musicHasFinished
		self._music.looping = True
		self._soundmanager.playClip(self._music)
		
		self.startCamera()

	def musicHasFinished(self):
		print self._music.name + " has finished playing.  Starting it again...\n"
		
	def pause(self, time):
		self._pausedtime = time
		self._paused = True
		
	def unpause(self, time):
		self._timemod += time - self._pausedtime
		self._paused = False
		
	def playerHit(self, damage):
		self._player.applyHit(damage)
		
	def gameOver(self):
		self._gameover = True
		self._soundmanager.stopClip(self._music)		
		self._world.gameOver()
		
	def endLevel(self):
		self._soundmanager.stopClip(self._music)
		self._world.endLevel()
		
	def queueObjectForRemoval(self, obj):
		self._objectstodelete.append(obj)

	def getObjectsInNode(self, nodeindex):
		return self._nodes[nodeindex].instances

	def getObjectsInRange(self, rangeL, rangeR):
		objects = list()
		
		for i in range(rangeL, rangeR):
			objects.extend(self._nodes[i].spaceobjects)
			
		return objects
		
	def addObjectToScene(self, obj):
		#TODO: search to ensure the object isn't already part of the scene
		loc = obj.instance.getLocation().getExactLayerCoordinates()
		nodeindex = int(loc.x * self._xscale)

		if nodeindex >= 0:
			obj.scenenodeid = nodeindex
			if not obj in self._nodes[nodeindex].spaceobjects:
				self._nodes[nodeindex].spaceobjects.append(obj)
		else:
			self.queueObjectForRemoval(obj)
	
	def moveObjectInScene(self, obj):
		loc = obj.instance.getLocation().getExactLayerCoordinates()
		nodeindex = int(loc.x * self._xscale)
		
		if nodeindex >= 0:
			if nodeindex != obj.scenenodeid:
				if obj in self._nodes[obj.scenenodeid].spaceobjects:
					self._nodes[obj.scenenodeid].spaceobjects.remove(obj)

				self._nodes[nodeindex].spaceobjects.append(obj)
			
				obj.scenenodeid = nodeindex
		else:
			self.queueObjectForRemoval(obj)

	def removeObjectFromScene(self, obj):
		for node in self._nodes:
			if obj in node.spaceobjects:
				if obj.instance:
					self._layer.deleteInstance(obj.instance)
					obj.instance = None
				node.spaceobjects.remove(obj)
				break
	
	def attachCamera(self, cam):
		self._camera = cam
		self._camera.setLocation(self._player.location)
	
	def stopCamera(self):
		self._cameraspeed = 0
		
	def startCamera(self):
		self._cameraspeed = 0.001
		
	def collectGarbage(self):
		for obj in self._objectstodelete:
			self.removeObjectFromScene(obj)
		
		self._objectstodelete = list()
	
	def update(self, time, keystate):
		timedelta = (time - self._timemod) - self._time
		self._timedelta = timedelta
		self._time = time - self._timemod
		
		self._keystate = keystate
		
		
		#some garbage cleanup
		self.collectGarbage()
		
		#update camera location
		loc = self._camera.getLocation()
		exactloc = self._camera.getLocation().getExactLayerCoordinates()
		#slowly move to the right
		exactloc.x += timedelta * self._cameraspeed
		loc.setExactLayerCoordinates(exactloc)
		self._camera.setLocation(loc)
		
		topleft = self._camera.toMapCoordinates(fife.ScreenPoint(0,0))
		bottomright = self._camera.toMapCoordinates(fife.ScreenPoint(1024,768))
		
		#which scene nodes to use to update objects
		leftnode = int(topleft.x)
		rightnode = int(bottomright.x) + 1
		
		#get a list of objects on the screen
		if leftnode < 0:
			leftnode = 0
		if rightnode > self._maxnodes:
			rightnode = self._maxnodes
		screenlist = self.getObjectsInRange(leftnode, rightnode)

		#update objects on the screen
		for obj in screenlist:
			if obj.type == SHTR_LASTBOSS:
				if bottomright.x > ((obj.location.getExactLayerCoordinates().x * self._xscale) + 0.5):
					self.stopCamera()

			if not (obj.type == SHTR_PLAYER and self._gameover):
				obj.update()
			
			if obj.changedposition:
				self.moveObjectInScene(obj)

			if obj.type != SHTR_PLAYER and obj.type != SHTR_PROJECTILE:
				if obj.running and obj.boundingbox.intersects(self._player.boundingbox):
					if obj.type == SHTR_ENEMYSHIP:
						#player touched an enemy.  Destroy player and 
						#re-initialize scene
						if not self._player.invulnerable:
							#collision damage of 1
							self.playerHit(1)
							obj.applyHit(1)
					elif obj.type == SHTR_POWERUP:
						obj.applyPowerUp(self._player)

			elif obj.type == SHTR_PROJECTILE:
			
				#projectile went off the right edge of the screen.  Remove it from the scene.
				if obj.scenenodeid >= rightnode:
					self.queueObjectForRemoval(obj)
					continue
					
				#could probably just get the nodes in the projectiles scenenode.
				#use a range to be sure.
				pcollide = self.getObjectsInRange(obj.scenenodeid - 1, obj.scenenodeid + 1)
				
				for o in pcollide:
					#cant get hit by your own bullet
					if obj.owner != o and o.type != SHTR_PROJECTILE and o.type != SHTR_POWERUP:
						if o.running and obj.boundingbox.intersects(o.boundingbox):
							if o != self._player and obj.owner.type == SHTR_PLAYER:
								o.applyHit(obj.damage)
								#check if enemy ship was destroyed
								if not o.running:
									self._player.applyScore(o.scorevalue)
								obj.destroy()
							elif o == self._player:
								#player got hit by a projectile
								if not self._player.invulnerable:
									self.playerHit(obj.damage)
									obj.destroy()
			
				#queue list of projectiles to remove (ttl expired or has been destroyed)
				if not obj.running:
					self.queueObjectForRemoval(obj)

#			self._world.renderBoundingBox(obj)			


				
	def _getPlayer(self):
		return self._player
		
	def _getKeyState(self):
		return self._keystate
		
	def _getCamera(self):
		return self._camera
		
	def _getObjectLayer(self):
		return self._layer
		
	def _getModel(self):
		return self._model
		
	def _getTime(self):
		return self._time
	
	def _getTimeDelta(self):
		return self._timedelta
		
	def _getPaused(self):
		return self._paused

	def _getSoundManager(self):
		return self._soundmanager
	
	player = property(_getPlayer)
	keystate = property(_getKeyState)
	camera = property(_getCamera)
	objectlayer = property(_getObjectLayer)
	model = property(_getModel)
	time = property(_getTime)
	timedelta = property(_getTimeDelta)
	paused = property(_getPaused)
	soundmanager = property(_getSoundManager)
