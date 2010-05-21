#!/usr/bin/env python

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
# This is the rio de hola client for FIFE.

import sys, os, re, math, random, shutil

from fife import fife
from fife.extensions.loaders import loadMapFile

class Scene(object):
	def __init__(self, gamecontroller):
		self._gamecontroller = gamecontroller
		
		self._map = None
		self._cameras = {}
		
	def createScene(self, mapfilename):
		if not self._map:
			self._map = loadMapFile(mapfilename, self._gamecontroller.engine)
		else:
			self.destroyScene()

		for cam in self._map.getCameras():
			self._cameras[cam.getId()] = cam
		
		self._cameras[self._gamecontroller.settings.get("RPG", "DefaultCameraName", "camera1")].setZoom(1.0)
		
	def destroyScene(self):
		self._cameras.clear()
		
		if self._map:
			self._gamecontroller.engine.getModel().deleteMap(self._map)
		
		self._map = None
				
	def updateScene(self):
		pass
		
