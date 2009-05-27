#!/usr/bin/env python
# coding: utf-8
# ###################################################
# Copyright (C) 2008 The Zero-Projekt team
# http://zero-projekt.net
# info@zero-projekt.net
# This file is part of Zero "Was vom Morgen blieb"
#
# The Zero-Projekt codebase is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the
# Free Software Foundation, Inc.,
# 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
# ###################################################

""" a tool for FIFEdit to edit object and instance attributes """

import fife
import pychan
import pychan.widgets as widgets
from pychan.tools import callbackWithArguments as cbwa

import scripts
import scripts.plugin as plugin
from scripts.events import *
from scripts.gui.action import Action

import settings as Settings
import math

class ObjectEdit(plugin.Plugin):
	""" The B{ObjectEdit} module is a plugin for FIFedit and allows to edit
	attributes of an selected instance - like offset, instance id or rotation
	(namespaces and object id editing is excluded)
	
	current features:
		- click instance and get all known data
		- edit offsets, rotation, instance id
		- outline highlighting of the selected object
			
	missing features:
		- blocking flag (flag doesn't work yet from FIFE side)
		- static flag (flag doesn't work yet from FIFE side)		
		- object saving
		- a lot of bug fixing concerning the rotation
		- use sliders to allow offset changes
		- the module should be able to use the editors global undo history

	FIXME:
		- this module owns a pointer to the mapedit module - this shouldn't be
		  necessary; a better plugin system of fifedit should only hand over the needed
		  data (selected instance)
		- we also need to edit run.py of the editor core to make this plugin work (shouldn't be necessary, too)
	"""
	def __init__(self):
		# this is _very bad_ - but I need to change the current rotation code by providing
		# project specific rotation angles. FIFE later should provide a list of the loaded
		# object rotations (they are provided by the xml files, so we just need to use them...)
		#self._mapedit._objectedit_rotations = None
	
		self.active = False
		self._camera = None
		self._layer = None
		
		self._enabled = False
		
		self.offset_slider = {}
		self.offset_slider['x'] = False
		self.offset_slider['y'] = False
		
		self.imagepool = None
		self.animationpool = None
		
		self.guidata = {}
		self.objectdata = {}

	def _reset(self):
		"""
			resets all dynamic vars, but leaves out static ones (e.g. camera, layer)

		"""
		self._instances = None
		self._image = None
		self._image_default_x_offset = None
		self._image_default_y_offset = None
		self._animation = False
		self._rotation = None
		self._avail_rotations = []
		self._namespace = None	
		self._blocking = 0
		self._static = 0
		self._object_id = None	
		self._instance_id = None
		self._fixed_rotation = None
		
		if self._camera is not None:
			self.renderer.removeAllOutlines()		
		

	def enable(self):
		if self._enabled is True:
			return
			
		self._editor = scripts.editor.getEditor()
		self.engine = self._editor.getEngine()
		
		self.imagepool = self.engine.getImagePool()
		self.animationpool = self.engine.getAnimationPool()
		
		self._showAction = Action(u"Object editor", checkable=True)
		scripts.gui.action.activated.connect(self.toggle_gui, sender=self._showAction)
		
		self._editor._toolsMenu.addAction(self._showAction)
		
		events.onInstancesSelected.connect(self.input)
		
		self._reset()		
		self.create_gui()

	def disable(self):
		if self._enabled is False:
			return
			
		self._reset()
		self.container.hide()
		self.removeAllChildren()
		
		events.onInstancesSelected.disconnect(self.input)
		
		self._editor._toolsMenu.removeAction(self._showAction)

	def isEnabled(self):
		return self._enabled;

	def getName(self):
		return "Object editor"

	def create_gui(self):
		"""
			- creates the gui skeleton by loading the xml file
			- finds some important childs and saves their widget in the object
		"""
		self.container = pychan.loadXML('gui/objectedit.xml')
		self.container.mapEvents({
			'x_offset_up' 	: cbwa(self.change_offset_x, 1),
			'x_offset_dn' 	: cbwa(self.change_offset_x, -1),
			
			'y_offset_up' 	: cbwa(self.change_offset_y, 1),
			'y_offset_dn' 	: cbwa(self.change_offset_y, -1),
			
			'x_offset_slider' : cbwa(self.get_slider_value, "x"),
			'y_offset_slider' : cbwa(self.get_slider_value, "y"),
			
			'use_data'		: self.use_user_data,
			
		})

		self._gui_anim_panel_wrapper = self.container.findChild(name="animation_panel_wrapper")
		self._gui_anim_panel = self._gui_anim_panel_wrapper.findChild(name="animation_panel")
		
		self._gui_anim_panel_wrapper.removeChild(self._gui_anim_panel)

		self._gui_rotation_dropdown = self.container.findChild(name="select_rotations")
		
		self._gui_xoffset_textfield = self.container.findChild(name="x_offset")
		self._gui_yoffset_textfield = self.container.findChild(name="y_offset")
		
		self._gui_instance_id_textfield = self.container.findChild(name="instance_id")
		
		print "Steplength x slider", self.container.findChild(name="x_offset_slider").getStepLength()
		print "Steplength y slider", self.container.findChild(name="y_offset_slider").getStepLength()
		self.container.findChild(name="x_offset_slider").setStepLength(0.01)
		self.container.findChild(name="y_offset_slider").setStepLength(0.01)
		print "New steplength x slider", self.container.findChild(name="x_offset_slider").getStepLength()
		print "New steplength y slider", self.container.findChild(name="y_offset_slider").getStepLength()
		
	def get_slider_value(self, orientation):
		""" get current slider value for offset manipulation """
		
		slider_name = orientation + "_offset_slider"
		widget = self.container.findChild(name=slider_name)
		value = widget.getValue()

		print "%s slider value: %s" % (orientation, str(value))
		
		if value < 0: 
			self.offset_slider[orientation] = False
			return
		
		callback = getattr(self, "change_offset_" + orientation)

		if self.offset_slider[orientation] == widget.getScaleStart():
			self.set_default_offset(orientation)
			self.offset_slider[orientation] = False
			return	
		elif self.offset_slider[orientation] >= widget.getScaleEnd():
			pass
		elif self.offset_slider[orientation] < value:
			callback(1)
		elif self.offset_slider[orientation] > value :
			callback(-1)

		self.offset_slider[orientation] = value

	def set_default_offset(self, axis):
		""" set default image offset for given axis """
		if axis == 'x':
			self._image.setXShift(self._image_default_x_offset)
		elif axis == 'y':
			self._image.setYShift(self._image_default_y_offset)

	def _get_gui_size(self):
		"""
			gets the current size of the gui window and calculates new position
			(atm top right corner)
		"""
		size = self.container._getSize()
		self.position = ((Settings.ScreenWidth - 10 - size[0]), 10)
		
	def update_gui(self):
		"""
			updates the gui widgets with current instance data
			
			FIXME: 
				- drop animation support or turn it into something useful
		"""
		#if self._animation is False:
			#try:
				#self._gui_anim_panel_wrapper.removeChild(self._gui_anim_panel)
			#except:
				#pass
		#elif self._animation is True:
			#try:
				#self._gui_anim_panel_wrapper.resizeToContent()				
				#self._gui_anim_panel_wrapper.addChild(self._gui_anim_panel)
				#self._gui_anim_panel_wrapper.resizeToContent()
			#except:
				#pass

		self.container.distributeInitialData({
			'select_rotations' 	: self._avail_rotations,
			'instance_id'		: unicode( self._instances[0].getId() ),
			'object_id'			: unicode( self._object_id ),
			'x_offset'			: unicode( self._image.getXShift() ),
			'y_offset'			: unicode( self._image.getYShift() ),
			'instance_rotation' : unicode( self._instances[0].getRotation() ),
			'object_namespace'	: unicode( self._namespace ),
			'object_blocking'	: unicode( self._blocking ),
			'object_static'		: unicode( self._static ),
		})
		try:
			print self._avail_rotations
			print self._fixed_rotation
			index = self._avail_rotations.index( str(self._fixed_rotation) )
			self._gui_rotation_dropdown._setSelected(index)
		except:
#			pass
			print "Angle (", self._fixed_rotation, ") not supported by this instance"
		
	def toggle_gui(self):
		"""
			show / hide the gui
		"""
		if self.active is True:
			self.active = False
			if self.container.isVisible():
				self.container.hide()
			self._showAction.setChecked(False)
		else:
			self.active = True
			self._showAction.setChecked(True)
	
	def highlight_selected_instance(self):
		"""
			just highlights selected instance
		"""
		self.renderer.removeAllOutlines() 
		self.renderer.addOutlined(self._instances[0], 205, 205, 205, 1)
			
	def change_offset_x(self, value=1):
		"""
			- callback for changing x offset
			- changes x offset of current instance (image)
			- updates gui
			
			@type	value:	int
			@param	value:	the modifier for the x offset
		"""		
		if self._image is not None:
			self._image.setXShift(self._image.getXShift() + value)
			self.update_gui()

	def change_offset_y(self, value=1):
		"""
			- callback for changing y offset
			- changes y offset of current instance (image)
			- updates gui
			
			@type	value:	int
			@param	value:	the modifier for the y offset
		"""
		if self._image is not None:
			self._image.setYShift(self._image.getYShift() + value)
			self.update_gui()

	def use_user_data(self):
		"""
			- takes the users values and applies them directly to the current ._instance
			- writes current data record
			- writes previous data record
			- updates gui
		
			FIXME:
			- parse user data in case user think strings are considered to be integer offset values...
		"""
		xoffset = self._gui_xoffset_textfield._getText()
		yoffset = self._gui_yoffset_textfield._getText()
		
		instance_id = self._gui_instance_id_textfield._getText()
		if instance_id is not None and instance_id is not "None":
			existing_instances = self._editor.getActiveMapView().getController()._layer.getInstances(instance_id)
			if existing_instances == ():
				self._instances[0].setId(instance_id)
				print "Set new instance id: ", instance_id		
			else:
				for i in existing_instances:
					print i
		
		# workaround - dropdown list only has 2 entries, but sends 3 -> pychan bug?
		if len(self._avail_rotations) < self._gui_rotation_dropdown._getSelected():
			index = len(self._avail_rotations)
		else:
			index = self._gui_rotation_dropdown._getSelected()
		
		# strange, but this helps to rotate the image correctly to the value the user selected
		angle = int( self._avail_rotations[index] )
		angle = int(angle - abs( self._camera.getTilt() ) )
		if angle == 360:
			angle = 0
		
		self._instances[0].setRotation(angle)
		self.get_instance_data(None, None, angle)
		
		try:
			self._image.setXShift( int(xoffset) )
		except:
			pass
#		print "x offset must me of type int!"
		try:
			self._image.setYShift( int(yoffset) )
		except:
			pass
#		print "y offset must be of type int!"

		self.update_gui()
		
	def get_instance_data(self, timestamp=None, frame=None, angle=-1, instance=None):
		"""
			- grabs all available data from both object and instance
			- checks if we already hold a record (namespace + object id)
			
			FIXME:
				1.) we need to fix the instance rotation / rotation issue
				2.) use correct instance rotations to store data for _each_ available rotation
				3.) move record code out of this method
		"""
		visual = None
		self._avail_rotations = []
			
		if instance is None:
			instance = self._instances[0]
			
		object = instance.getObject()
		self._namespace = object.getNamespace()
		self._object_id = object.getId()

		self._instance_id = instance.getId()
	
		if self._instance_id == '':
			self._instance_id = 'None'

		if angle == -1:
			angle = int(instance.getRotation())
		else:
			angle = int(angle)	
			
		self._rotation = angle
		
		if object.isBlocking():
			self._blocking = 1
			
		if object.isStatic():
			self._static = 1
		
		try:
			visual = object.get2dGfxVisual()
		except:
			print 'Fetching visual of object - failed. :/'
			raise			

#		print "Camera Tilt: ", self._camera.getTilt()
#		print "Camera Rotation: ", self._camera.getRotation()

		self._fixed_rotation = int(instance.getRotation() + abs( self._camera.getTilt() ) )		
		self._fixed_rotation = visual.getClosestMatchingAngle(self._fixed_rotation)	

		index = visual.getStaticImageIndexByAngle(self._fixed_rotation)

		if index == -1:
			# object is an animation
			self._animation = True
			# no static image available, try default action
			action = object.getDefaultAction()
			if action:
				animation_id = action.get2dGfxVisual().getAnimationIndexByAngle(self._fixed_rotation)
				animation = self.animationpool.getAnimation(animation_id)
#				if timestamp is None and frame is not None:
#					self._image = animation.getFrame(frame)	
#				elif timestamp is not None and frame is None:
#					self._image = animation.getFrameByTimestamp(timestamp)
#				else:
				self._image = animation.getFrameByTimestamp(0)
				index = self._image.getPoolId()
		elif index != -1:
			# object is a static image
			self._animation = False
			self._image = self.imagepool.getImage(index)

		if not self._animation:
			rotation_tuple = visual.getStaticImageAngles()
			for angle in rotation_tuple:
				self._avail_rotations.append( str(angle) )
				
		self._image_default_x_offset = self._image.getXShift()
		self._image_default_y_offset = self._image.getYShift()


# FIXME: see l. 40
		self._editor.getActiveMapView().getController()._objectedit_rotations = self._avail_rotations
# end FIXME
		
	def input(self, instances):
		"""
			if called _and_ the user wishes to edit offsets,
			gets instance data and show gui
			
			(see run.py, pump() )
		"""
		if instances != self._instances:
			if self.active is True:
				self._reset()
				self._instances = instances
				
				if self._camera is None:
					self._camera = self._editor.getActiveMapView().getCamera()
					self.renderer = fife.InstanceRenderer.getInstance(self._camera)				
					
				self._layer = self._editor.getActiveMapView().getController()._layer
			
				if self._instances != ():
					self.highlight_selected_instance()
					self.get_instance_data()
					self.update_gui()
					self.container.adaptLayout()
					self.container.show()
					self._get_gui_size()
					self.container._setPosition(self.position)
				else:
					self._reset()
					self.container.hide()
