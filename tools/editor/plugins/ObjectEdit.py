# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2013 by the FIFE team
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

""" a tool for FIFEdit to edit object and instance attributes """

from fife import fife
from fife.extensions import pychan
from fife.extensions.pychan.tools import callbackWithArguments as cbwa

from fife.extensions.fife_timer import Timer
from fife.extensions.serializers.xmlobject import XMLObjectSaver

import scripts
import scripts.plugin as plugin
from scripts.gui.action import Action

from scripts.events import onObjectSelected
from scripts.events import preMapClosed
from scripts.events import postMapShown
from scripts.events import onInstancesSelected

_DEFAULT_ENTRY = u'N/A'
_POSITION = (120,200)

# plugin default settings
_PLUGIN_SETTINGS = {
	'module' : "ObjectEditSettings",
	'items' : {
		'dockarea' : 'right',
		'docked' : True,
	},
}


class ObjectEdit(plugin.Plugin):
	""" The B{ObjectEdit} module is a plugin for FIFedit and allows to edit
		attributes of a selected object
		
		fife.Objects can be selected via the B{ObjectSelector}. fife.Instances
		are instances of fife.Objects - so clicking on a "map object" is
		actually not selecting an object. Not sure how to handle that - 
		for now this Plugin only reacts on selections in the ObjectSelector
		
		There is an InstanceEdit plugin yet to be written which allows
		the manipulation of instance data.
	"""
	def __init__(self):
		super(ObjectEdit, self).__init__()
		
		self._editor = scripts.editor.getEditor()
		self.engine = self._editor.getEngine()
				
		self.xml_saver = XMLObjectSaver(self.engine)

		self.default_x, self.default_y = _POSITION
		
		self._enabled = False
		self._showAction = None
		self._help_dialog = None

		self.default_settings = _PLUGIN_SETTINGS
		self.eds = self._editor._settings
		self.update_settings()

		self.reset()
		
	def reset(self):
		""" reset major ivars """		
		self.current_angle = None
		self.current_action = None
		self._object = None
		self._instance = None
		
	def enable(self):
		""" plugin method """
		if self._enabled: return
		self._enabled = True

		self._action_show = Action(unicode(self.getName(),"utf-8"), checkable=True)
		scripts.gui.action.activated.connect(self.toggle_gui, sender=self._action_show)
		self._editor._tools_menu.addAction(self._action_show)
		
		onObjectSelected.connect(self.update)
		onInstancesSelected.connect(self.update)
		preMapClosed.connect(self.hide)
		postMapShown.connect(self.update)

		self.create_gui()
		self.update_gui()
		
		if self.settings['docked']:
			self._editor.dockWidgetTo(self.container, self.settings['dockarea'])

	def disable(self):
		""" plugin method """
		if not self._enabled: return
		self._enabled = False
			
		self.container.hide()
		self.reset()
		
		onObjectSelected.disconnect(self.update)
		onInstancesSelected.disconnect(self.update)
		preMapClosed.disconnect(self.hide)
		postMapShown.disconnect(self.update)
		
		self._editor._tools_menu.removeAction(self._action_show)

	def isEnabled(self):
		""" plugin method """
		return self._enabled;

	def getName(self):
		""" plugin method """
		return "Object editor"

	def _show_help(self):
		""" shows the help dialog """
		if self._help_dialog is not None:
			self._help_dialog.show()
			return
		
		self._help_dialog = pychan.loadXML("gui/help.xml")
		self._help_dialog.title = u"Help (Object Editor)"
		self._help_dialog.mapEvents({
			"closeButton" : self._help_dialog.hide,
		})
		
		# gimme some more space
		_SIZE = (320,400)
		scrollarea = self._help_dialog.findChildren(__class__=pychan.widgets.ScrollArea)[0]
		scrollarea.size = _SIZE
		scrollarea.min_size = _SIZE
		scrollarea.max_size = _SIZE
		
		f = open('lang/help_object_edit.txt', 'r')
		self._help_dialog.findChild(name="helpText").text = unicode(f.read())
		f.close()
		
		self._help_dialog.show()

	def create_gui(self):
		"""
			- creates the gui skeleton by loading the xml file
			- finds some important childs and saves their widget in the object
			
		@todo:
			- move all dynamic widgets to dict
		"""
		self.container = pychan.loadXML('gui/objectedit.xml')
		self.container.position_technique = 'explicit'
		self.container.position = _POSITION
		
		self.container.mapEvents({
			'change_data'		: self.save,
			'show_help'	: self._show_help,
		})
		
		self.container.findChild(name="x_offset_up").capture(self.change_offset, "mousePressed")
		self.container.findChild(name="x_offset_dn").capture(self.change_offset, "mousePressed")
		self.x_offset = self.container.findChild(name="x_offset")
		self.x_offset.capture(self.change_offset, "mouseWheelMovedUp")
		self.x_offset.capture(self.change_offset, "mouseWheelMovedDown")

		self.container.findChild(name="y_offset_up").capture(self.change_offset, "mousePressed")
		self.container.findChild(name="y_offset_dn").capture(self.change_offset, "mousePressed")
		self.y_offset = self.container.findChild(name="y_offset")
		self.y_offset.capture(self.change_offset, "mouseWheelMovedUp")
		self.y_offset.capture(self.change_offset, "mouseWheelMovedDown")

		self.container.findChild(name="object_blocking_toggle").capture(self.object_blocking_toggle, "mousePressed")

		self.rotations_listbox = self.container.findChild(name="select_rotations")
		self.rotations_listbox.capture(self.select_rotation,"mouseWheelMovedUp")
		self.rotations_listbox.capture(self.select_rotation,"mouseWheelMovedDown")
		self.rotations_listbox.capture(self.select_rotation,"action")	
		
		self.xoffset_textfield = self.container.findChild(name="x_offset")
		self.yoffset_textfield = self.container.findChild(name="y_offset")
		
		self.actions_wrapper = self.container.findChild(name="actions_wrapper")
		self.rotations_wrapper = self.container.findChild(name="rotations_wrapper")
		
		self.actions_listbox = self.container.findChild(name="select_actions")
		self.actions_listbox.capture(self.select_action,"mouseWheelMovedUp")
		self.actions_listbox.capture(self.select_action,"mouseWheelMovedDown")
		self.actions_listbox.capture(self.select_action,"action")	

	def get_image(self, rotations=None):
		""" try to get the current image of the object 
		
		@rtype	image:	fife.Image
		@return	image:	image of the current angle of the current object
		"""
		if self._object is None: return
		image = None

		if rotations is None:
			rotations = self.get_rotations()

		if rotations:
			if self.current_angle is not None and self.current_angle in rotations:
				angle = self.current_angle
			else:
				angle = rotations[0]
			visual = self._object.get2dGfxVisual()
			index = visual.getStaticImageIndexByAngle(angle)

			if index == -1:
				action = self._object.getDefaultAction()
				if action:
					animation = action.get2dGfxVisual().getAnimationByAngle(angle)
					image = animation.getFrameByTimestamp(0)
			else:
				image = self.engine.getImageManager().get(index)

		return image
		
	def get_rotations(self):
		""" get either the static rotations or those of the action (if the object
			has one)
		"""
		rotations = []
		
		visual = self._object.get2dGfxVisual()
		rotations = visual.getStaticImageAngles()
		
		if not rotations and self._object.getDefaultAction():
			rotations = self._object.getDefaultAction().get2dGfxVisual().getActionImageAngles()
			
		return rotations

	def update_gui(self):
		""" updates the gui widgets with current object data """

		x_offset = _DEFAULT_ENTRY
		y_offset = _DEFAULT_ENTRY
		rotations = []
		actions = []
		namespace = _DEFAULT_ENTRY
		blocking = bool(_DEFAULT_ENTRY)
		static = _DEFAULT_ENTRY
		_id = _DEFAULT_ENTRY
		cost_id = _DEFAULT_ENTRY
		cost = _DEFAULT_ENTRY
		cellstack_pos = _DEFAULT_ENTRY

		if self._object is not None:
			actions = list(self._object.getActionIds())
			rotations = self.get_rotations()
			image = self.get_image(rotations=rotations)
			if image is not None:
				x_offset = unicode(str(image.getXShift()), 'utf-8')
				y_offset = unicode(str(image.getYShift()), 'utf-8')
			namespace = unicode(self._object.getNamespace(), 'utf-8')
			blocking = self._object.isBlocking()
			static = unicode(str(int(self._object.isStatic())), 'utf-8')
			_id = unicode(self._object.getId(), 'utf-8')
			if hasattr(self._object, 'getCostId'):
				cost_id = unicode(self._object.getCostId(), 'utf-8')
			if hasattr(self._object, 'getCost'):
				cost = unicode(str(self._object.getCost()), 'utf-8')
			if hasattr(self._object, 'getCellStackPosition'):
				cellstack_pos = unicode(str(self._object.getCellStackPosition()), 'utf-8')
				
		self.container.distributeInitialData({
			'object_id'			: u"\t" + _id,
			'x_offset'			: x_offset,
			'y_offset'			: y_offset,
			'object_namespace'	: u"\t" + namespace,
			'object_static'		: static,
			'cost_id'	: cost_id,
			'cost_value' : cost,
			'object_cellstack_pos' : cellstack_pos,
		})
		
		wdgt = self.container.findChild(name="object_blocking_toggle")
		wdgt.marked = blocking
		
		self.rotations_listbox.items = rotations
		if rotations:
			if self.current_angle in rotations:
				index = rotations.index(self.current_angle)
			else:
				index = 0
			self.rotations_listbox.selected = index
			self.rotations_wrapper.show()
		else:
			self.rotations_wrapper.hide()

		self.actions_listbox.items = actions
		if actions:
			if self.current_action in actions:
				index = actions.index(self.current_action)
			else:
				index = 0
			self.actions_listbox.selected = index
			self.actions_wrapper.show()
		else:
			self.actions_wrapper.hide()
		
		if not self.container.isDocked():
			self.container.adaptLayout(True)
		
	def toggle_gui(self):
		"""
			show / hide the gui
		"""
		if self.container.isVisible():
			self.last_dockarea = self.container.dockarea
			self.container.hide()
			self._action_show.setChecked(False)			
		else:
			if not self.container.isDocked():
				self.container.show()
				self.container.x = self.default_x
				self.container.y = self.default_y
			else:
				self.container.setDocked(True)
				self.dockWidgetTo(self.container, self.last_dockarea)
			self._action_show.setChecked(True)			
	
	def change_offset(self, event, widget):
		""" widget callback: change the offset of an object 
		
		@type	event:	object
		@param	event:	FIFE mouseevent or keyevent
		@type	widget:	object
		@param	widget:	pychan widget
		"""
		if self._object is None: return
		
		etype = event.getType()
		
		image = self.get_image()
		if image is None: return
		
		x = image.getXShift()
		y = image.getYShift()
		
		if etype == fife.MouseEvent.WHEEL_MOVED_UP or widget.name.endswith("up"):
			modifier = 1
		elif etype == fife.MouseEvent.WHEEL_MOVED_DOWN or widget.name.endswith("dn"):
			modifier = -1

		if widget.name.startswith("x"):
			x += modifier
		elif widget.name.startswith("y"):
			y += modifier

		self.set_offset(x, y, image=image)
		self.update_gui()

	def object_blocking_toggle(self, event, widget):
		""" widget callback: change the blocking of an instance 

		@type	event:	object
		@param	event:	FIFE mouseevent or keyevent
		@type	widget:	object
		@param	widget:	pychan widget
		"""
		if self._object is None: return
		blocking = widget.marked
		self._object.setBlocking(bool(blocking))
		self.update_gui()

	def save(self):
		""" saves the current object to its xml file 
		
		@note:
			- saves only object data and static image data
			- no animation data or atlas data is saved
		"""
		if self._object is None: return
		
		self.xml_saver.save(self._object)
		self._editor.getStatusBar().setText(u"Saving of object data successful: %s" % self._object.getId())
		
	def select_action(self):
		""" let an instance act and set the action of the current object """
		if self._object is None: return
		action = str(self.actions_listbox.selected_item)
		self.current_action = action
		
		# @todo: compat layer - trunk/ doesn't have this method exposed
		# to python yet (cell pathfinding branch has)
		# @todo: remove this check later
		if hasattr(self._object, 'setDefaultAction'):
			self._object.setDefaultAction(action)
		
		if self._instance is not None and action is not None:
			f_loc = self._instance.getFacingLocation()
			self._instance.actOnce(action, f_loc)
	
	def select_rotation(self):
		""" rotate an instance due to selected angle """
		if self._object is None: return
		angle = self.eval_rotation()
		self.current_angle = angle
		
		if self._instance is not None and angle is not None:
			self._instance.setRotation(int(angle))
		
		self.update_gui()
		
	def eval_rotation(self):
		""" prepare rotation from gui and apply it to the current selected instance """
		selected = self.rotations_listbox.selected_item
		if selected is None: return selected
		angle = int(selected)
	
		if angle == 360:
			angle = 0
			
		return angle
		
	def set_offset(self, x=None, y=None, image=None):
		""" set x/y offset of current selected instance """
		if self._object is None: return
		if self._object.isStatic():
			if image is None:
				image = self.get_image()
			if image is None: return

			if x is not None:
				image.setXShift(x)
			if y is not None:
				image.setYShift(y)
		else:
			self.set_animation_offset(x, y)
			
	def set_animation_offset(self, x, y):
		""" apply the offset to all frames of the default action """
		if self.current_action is not None:
			action = self._object.getAction(self.current_action)
		else:
			action = self._object.getDefaultAction()
		if not action: return
		
		if self.current_angle is not None:
			angle = self.current_angle
		else:
			angle = 0
		
		animation = action.get2dGfxVisual().getAnimationByAngle(angle)
		
		if not animation: return

		for index in range(animation.getFrameCount()):
			image = animation.getFrame(index)
			image.setXShift(x)
			image.setYShift(y)
		
	def show(self):
		""" show the plugin gui """
		self.container.show()	
		self.container.adaptLayout(False)						
	
	def hide(self):
		""" hide the plugin gui - and reset it """
		self.container.hide()
		self.reset()		

	def update(self, object=None, instances=[]):
		""" called by the editor onObjectSelected or onInstancesSelected
		
			(we only use the top instance of the selected cell)

		@type	instances:	list
		@param	instances:	a list of instances in the selected cell 
		"""
		self.reset()
		
		if object is None and not instances: 
			self._object = None
			self._instance = None
			self.update_gui()
			return
			
		if instances:
			self._instance = instances[0]
			object = instances[0].getObject()
		
		action = object.getDefaultAction()
		if action:
			self.current_action = action.getId()

		self._object = object
		self.update_gui()
