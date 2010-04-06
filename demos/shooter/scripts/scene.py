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
from scripts.ships.shipbase import Ship
from scripts.ships.player import Player
from scripts.ships.enemies import *
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
	def __init__(self, engine, objectLayer):
		self._engine = engine
		self._model = engine.getModel()
		self._layer = objectLayer
		self._nodes = list()
		
		self._player = None
		self._projectiles = list()
		
		self._maxnodes = 128
		self._xscale = 0
		
		self._time = 0
		self._timedelta = 0
		self._lasttime = 0
		
		self._xscale = self._layer.getCellGrid().getXScale()
		self._yscale = self._layer.getCellGrid().getYScale()

	def initScene(self, mapobj):
		self._player = Player(self, 'player')
		self._player.width = 0.075
		self._player.height = 0.075
		self._player.start()
				
		enemies = self._layer.getInstances('enemy')
		
		#initialize our scene array to some arbitrary size
		for i in range(0,self._maxnodes):
			self._nodes.append(SceneNode())
		
		for instance in enemies:
			objectName = instance.getObject().getId()
			print objectName
			
			if objectName == "saucer1":
				enemy = Saucer1(self, 'enemy', False)
			elif objectName == "saucer2":
				enemy = Saucer2(self, 'enemy', False)
			else:
				enemy = Ship(self, 'enemy', False)
				
			enemy.instance = instance
			enemy.start()

			loc = instance.getLocation().getExactLayerCoordinates()
			nodeindex = int(loc.x * self._xscale)
			enemy.scenenodeid = nodeindex
			self._nodes[nodeindex].spaceobjects.append(enemy)
			
		#and finally add the player to the scene
		self.addObjectToScene(self._player)

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
		
		self._nodes[nodeindex].spaceobjects.append(obj)
		obj.scenenodeid = nodeindex		

	def addProjectileToScene(self, projectile):
		self._projectiles.append(projectile)
	
	def moveObjectInScene(self, obj):
		loc = obj.instance.getLocation().getExactLayerCoordinates()
		nodeindex = int(loc.x * self._xscale)
		
		if nodeindex != obj.scenenodeid:
			self._nodes[obj.scenenodeid].spaceobjects.remove(obj)
			self._nodes[nodeindex].spaceobjects.append(obj)
			obj.scenenodeid = nodeindex
	
	def removeObjectFromScene(self, obj):
		for node in self._nodes:
			if obj in node.spaceobjects:
				node.spaceobjects.remove(obj)
				return
	
	def attachCamera(self, cam):
		self._camera = cam
		self._camera.setLocation(self._player.location)
		
	def update(self, time, keystate):
		timedelta = time - self._time
		self._timedelta = timedelta
		self._time = time
		
		self._keystate = keystate
		
		#update camera location
		loc = self._camera.getLocation()
		exactloc = self._camera.getLocation().getExactLayerCoordinates()
		#slowly move to the right
		exactloc.x += timedelta * 0.001
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
			obj.update()
			if obj.changedposition:
				self.moveObjectInScene(obj)

			if obj != self._player:
				#TODO: enemy should fire weapon in their update function
				obj.fire(fife.DoublePoint(-1,0))
			
				if obj.boundingbox.intersects(self._player.boundingbox):
					#player touched an enemy.  Destroy player and 
					#re-initialize scene
					self._player.destroy()
		
		
		#update the list of projectiles
		projtodelete = list()
		for p in self._projectiles:
			p.update()
			#check to see if the projectile hit any object on the screen
			for o in screenlist:
				#cant get hit by your own bullet
				if p.owner != o:
					if p.boundingbox.intersects(o.boundingbox):
						self._player.applyScore(100)
						p.destroy()
						o.destroy()
						#TODO:  the destroy functions should spawn an explosion
						#and also destroy the instance and remove itself from the scene
						self.removeObjectFromScene(o)
			
			#build a list of projectiles to remove (ttl expired)
			if not p.running:
				projtodelete.append(p)

		#remove any expired projectiles 
		for p in projtodelete:
			self._projectiles.remove(p)

				
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

	player = property(_getPlayer)
	keystate = property(_getKeyState)
	camera = property(_getCamera)
	objectlayer = property(_getObjectLayer)
	model = property(_getModel)
	time = property(_getTime)
	timedelta = property(_getTimeDelta)
		
