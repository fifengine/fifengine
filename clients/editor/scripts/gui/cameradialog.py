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

import fife
import pychan

class CameraDialog(object):
	"""
	B{CameraDialog} provides a gui dialog for camera creation. The callback is called when camera creation is complete. A
	partial specification of the camera parameters may optionally be given.
	"""
	def __init__(self, engine, callback=None, onCancel=None, map=None, layer=None):
		self.engine = engine
		self.callback = callback
		self.onCancel = onCancel
		self._widget = pychan.loadXML('gui/cameradialog.xml')

		if map:
			self._widget.distributeData({
				'mapBox'	: unicode(map.getId()),
			})

		if layer:
			self._widget.distributeData({
				'layerBox'	: unicode(layer.getId()),
			})

		self._widget.mapEvents({
			'okButton'     : self._finished,
			'cancelButton' : self._cancelled
		})

		self._widget.show()
		
	def _cancelled(self):
		if self.onCancel:
			self.onCancel()
		self._widget.hide()
		

	def _finished(self):
		id = self._widget.collectData('idBox')
		if id == '':
			print 'Please enter a camera id.'
			return

		try:
			map = self.engine.getModel().getMap(str(self._widget.collectData('mapBox')))
		except fife.Exception:
			print 'Cannot find the specified map id.'
			return

		try:
			layer = map.getLayer(str(self._widget.collectData('layerBox')))
		except fife.Exception:
			print 'Cannot find the specified layer id.'	
			return

		try:
			vals = self._widget.collectData('viewBox').split(',')
			if len(vals) != 4:
				raise ValueError	

			viewport = fife.Rect(*[int(c) for c in vals])
		except ValueError:
			print 'Please enter 4 comma (,) delimited values for viewport x,y,width,height.'
			return

		try:
			refh = int(self._widget.collectData('refhBox'))
			refw = int(self._widget.collectData('refwBox'))
		except ValueError:
			print 'Please enter positive integer values for reference width and height.'
			return

		try:
			rot = int(self._widget.collectData('rotBox'))
			tilt = int(self._widget.collectData('tiltBox'))
		except ValueError:
			print 'Please enter positive integer values for rotation and tilt.'
			return

		cam = self.engine.getView().addCamera(str(id), layer, viewport, fife.ExactModelCoordinate(0,0,0))
		cam.setCellImageDimensions(refw, refh)
		cam.setRotation(rot)
		cam.setTilt(tilt)
	
		self._widget.hide()

		if self.callback:
			self.callback()