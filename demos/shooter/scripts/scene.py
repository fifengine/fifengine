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
		
		self._player = Player(self._model, 'player', self._layer)
		self._player.width = 0.075
		self._player.height = 0.075
		self._player.start()
		
		self._projectiles = list()
		self._lasttime = 0
		
		self._maxnodes = 128

	def initScene(self, mapobj):
		layer = mapobj.getLayer('objects')
		xscale = layer.getCellGrid().getXScale()
				
		enemies = layer.getInstances('enemy')
		
		#initialize our scene array to some arbitrary size
		for i in range(0,self._maxnodes):
			self._nodes.append(SceneNode())
		
		for instance in enemies:
			objectName = instance.getObject().getId()
			print objectName
			
			enemy = Ship(self._model, 'enemy', self._layer, False)
			enemy.instance = instance
			enemy.width = 0.075
			enemy.height = 0.075
			enemy.velocity.x = -0.13
			enemy.start()

			loc = instance.getLocation().getExactLayerCoordinates()
			nodeindex = int(loc.x * xscale)
			self._nodes[nodeindex].spaceobjects.append(enemy)

	def getObjectsInNode(self, nodeindex):
		return self._nodes[nodeindex].instances

	def getObjectsInRange(self, rangeL, rangeR):
		objects = list()
		
		for i in range(rangeL, rangeR):
			objects.extend(self._nodes[i].spaceobjects)
			
		return objects
	
	def removeObjectFromScene(self, obj):
		for node in self._nodes:
			if obj in node.spaceobjects:
				node.spaceobjects.remove(obj)
				return
	
	def attachCamera(self, cam):
		self._camera = cam
		self._camera.setLocation(self._player.location)
		
	def _getPlayer(self):
		return self._player

	def update(self, time, keystate):
		timedelta = time - self._lasttime
		self._lasttime = time
		
		#update camera location
		loc = self._camera.getLocation()
		exactloc = self._camera.getLocation().getExactLayerCoordinates()
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
			obj.update(timedelta)

		#update the player
		self._player.update(timedelta, keystate, self._camera)
		
		#update the list of projectiles
		projtodelete = list()
		for p in self._projectiles:
			p.update(timedelta)
			#check to see if the projectile hit any object on the screen
			for o in screenlist:
				if p.boundingbox.intersects(o.boundingbox):
					self._player.applyScore(100)
					p.destroy()
					o.destroy()
					self.removeObjectFromScene(o)
			
			#build a list of projectiles to remove (ttl expired)
			if not p.running:
				projtodelete.append(p)

		#remove any non running projectiles 
		for p in projtodelete:
			self._projectiles.remove(p)
		
		#fire the currently selected gun
		if keystate['SPACE']:
			prjct = self._player.fire(time)
			if prjct:
				self._projectiles.append(prjct)

	player = property(_getPlayer)
		
