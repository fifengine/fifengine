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

from fife import fife, fifechan

from fife.extensions import pychan
from fife.extensions.pychan import internal

import scripts.editor
import scripts.plugin as plugin

from scripts.events import *
from scripts.gui.action import Action
from scripts.gui.panel import Panel

from scripts.events import postMapShown
from scripts.events import onObjectSelected
from scripts.events import onObjectsImported

# settings for gui
_POSITION = (30,200)
_SIZE = (200,500)
_MAX_PREVIEW_SIZE = (120,120)
_MODE_TEXT = 0
_MODE_IMAGE = 1
_MODES = [_MODE_TEXT, _MODE_IMAGE]

_COLOR_STEP = fifechan.Color(10,10,10)
_MAX_PREVIEW_WRAPPER_SIZE = (140,140)

_DEFAULT_BASE_COLOR = internal.DEFAULT_STYLE['default']['base_color']
_DEFAULT_SELECTION_COLOR = internal.DEFAULT_STYLE['default']['selection_color']

# plugin default settings
_PLUGIN_SETTINGS = {
	'module' : "ObjectSelectorSettings",
	'items' : {
		'dockarea' : 'right',
		'docked' : True,
		
	},
}

class ListItem(object):
	""" small wrapper class for ListBox widgets 
		to transport objects including their corresponding
		namespace	
	"""
	def __init__(self, object_id, namespace):
		"""
		@type	object_id:	str
		@param	object_id:	a object id
		@type	namespace:	str
		@param	namespace:	namespace of the object
		"""
		self.object_id = object_id
		self.namespace = namespace
	def __str__( self ):
		""" give pychan a string to display """
		return self.object_id
		
class ObjectIcon(pychan.widgets.Container):
	""" small wrapper class for image mode, grouped in a VBox widget """
	def __init__(self, object_id, namespace, image):
		super(ObjectIcon, self).__init__(size=_MAX_PREVIEW_WRAPPER_SIZE, min_size=_MAX_PREVIEW_WRAPPER_SIZE, max_size=_MAX_PREVIEW_WRAPPER_SIZE, padding=1)
		self.base_color = self.background_color
		self.index = None
		self.callback = None

		self.capture(self._mouse_entered, "mouseEntered")
		self.capture(self._mouse_exited, "mouseExited")
		
		self.object_id = object_id
		self.namespace = namespace
		
		self.icon = pychan.widgets.Icon()
		self.label = pychan.widgets.Label()
		
		self.addChild(self.icon)
		self.addChild(self.label)
		
		self.set_text(self.object_id)
		self.set_image(image)
		
		self.label.position = (5, _MAX_PREVIEW_SIZE[1])

		self.adaptLayout(True)
		
	def _mouse_entered(self):
		""" callback for mouse entered events """
		self.base_color += _COLOR_STEP
		""" callback for mouse exited events """
	def _mouse_exited(self):
		self.base_color -= _COLOR_STEP
	def _mouse_pressed(self, event):
		""" callback for mouse pressed events
		
			select the deselect the current ObjectIcon and select self,
			afterwards trigger the callback
		"""
		self.parent.items[self.parent.selected]._deselect()
		self.parent.selected = self.index
		self._select()
		self.callback()
		
	def set_image(self, image):
		""" sets the object image """
		w, h = image.getWidth(), image.getHeight()
		w, h = resize(w, h)

		self.icon.image = image
		self.icon.size = self.icon.max_size = (w, h)
		x, y = _MAX_PREVIEW_SIZE
		self.icon.position = (int(x / 2 - w / 2), int(y / 2 - h / 2))		
		
	def set_text(self, text):
		""" sets the object id as text """
		self.label.text = unicode(text, 'utf-8')
		
	def _select(self):
		""" highlights the ObjectIcon """
		self.base_color = _DEFAULT_SELECTION_COLOR
	def _deselect(self):
		""" disables highlight of the ObjectIcon """
		self.base_color = self.background_color
	
def resize(w, h):
	""" little helper to preserve aspect ratio

	@type	w:	int
	@param	w:	width
	@type	h:	int
	@param	h:	height
	"""
	x, y = _MAX_PREVIEW_SIZE
	
	if w < x and h < y: return w, h
	
	i_r = image_ratio = float(w) / float(h)
	n_r = new_ratio = float(x) / float(y)
	
	if n_r < i_r:
		n_h = h * (float(y) / float(h))
		n_w = w * (float(y) / float(w))
	else:
		n_h = h * (float(x) / float(h))
		n_w = w * (float(x) / float(w))	

	return int(n_w), int(n_h)

class ObjectSelector(plugin.Plugin):
	""" The B{ObjectSelector} plugin offers a gui that let you browse the loaded
		namespaces and their associated objects.
		
		It can be used in text- and image-mode. Text mode provides a preview image
		of the current selected object.
		
	@todo:
		- we need some sort of caching to prevent pychan from resizing all the object
		  images over and over again
		- image aspect ratio is still broken - I guess my resize() function is junk
		  (actually, I bet it is)
		- current selection of the user is overwritten on new updates	
	"""
	ORIENTATION = {
		"Horizontal"	: 0,
		"Vertical"		: 1,
	}	
	def __init__(self):
		super(ObjectSelector, self).__init__()
		# editor instance
		self._editor = scripts.editor.getEditor()
		self._engine = self._editor.getEngine()		

		# plugin variables
		self._enabled = False
		self.last_dockarea = None
		self._action_show = None
		self._mode = _MODE_IMAGE

		self._object = None
		# @todo: we should store all the selection data here
		self._current_selection = {
			'object' : None,
			'object_id' : None,
			'object_namespace' : None,
			'namespace' : None,
		}		
		
		# gui vars
		self.container = None
		self.default_x, self.default_y = _POSITION		
		
		w,h = self._engine.getSettings().getScreenWidth(), self._engine.getSettings().getScreenHeight()

		self._panel_size = int(w * 0.25) / 2
		
		self.max_x = int(w * 0.85)
		self.max_y = int(h * 0.7)	
		
		if self.max_y < _MAX_PREVIEW_WRAPPER_SIZE[1]:
			self.max_y = _MAX_PREVIEW_WRAPPER_SIZE[1] + 20 # compensate for scrollbar
		
		self._orientation = ObjectSelector.ORIENTATION['Vertical']
		self._default_orientation = ObjectSelector.ORIENTATION['Vertical']

		self.wrappers = {
			ObjectSelector.ORIENTATION['Vertical'] : {
				'wrapper' : None,
				'image_mode_wrapper' : None,
				'text_mode_wrapper' : None,
			},
			ObjectSelector.ORIENTATION['Horizontal'] : {
				'wrapper' : None,
				'image_mode_wrapper' : None,
				'text_mode_wrapper' : None,			
			},
		}
		
		# setting variables
		self.default_settings = _PLUGIN_SETTINGS
		self.eds = self._editor._settings
		self.update_settings()
	
	def enable(self):
		""" enables the plugin and connects to the editor """
		if self._enabled: return
		self._enabled = True

		# Fifedit plugin data
		self._action_show = Action(self.getName(), checkable=True)
		scripts.gui.action.activated.connect(self.toggle, sender=self._action_show)
		self._editor._tools_menu.addAction(self._action_show)
		
		postMapShown.connect(self.update)
		onObjectSelected.connect(self.set_object)
		onObjectsImported.connect(self.update_namespaces)
		
		self.create()
		self.toggle()		

		if self.settings['docked']:
			self._editor.dockWidgetTo(self.container, self.settings['dockarea'])		

	def disable(self):
		""" disables the plugin and connects to the editor """
		if self._enabled: return
		self._enabled = False
		
		postMapShown.disconnect(self.update_namespaces)
		onObjectSelected.disconnect(self.set_object)
		onObjectsImported.disconnect(self.update_namespaces)		

	def set_orientation(self, orientation=None, key=None):
		if key is not None:
			if key not in ObjectSelector.ORIENTATION: return
			orientation = ObjectSelector.ORIENTATION[key]
		
		if orientation is None: return
		
		if orientation == ObjectSelector.ORIENTATION['Vertical']:
			self._orientation = ObjectSelector.ORIENTATION['Vertical']
			self._max_size = (self._panel_size, self.max_y)
		else:
			self._orientation = ObjectSelector.ORIENTATION['Horizontal']
			self._max_size = (self.max_x, self._panel_size)
		self._orientation = orientation
		
		self.update_orientation()
	def get_orientation(self):
		return self._orientation
	orientation = property(get_orientation, set_orientation)
	
	def update_orientation(self):
		""" change widget orientation """
		self.container.removeAllChildren()
		
		wrapper = self.wrappers[self.orientation]['wrapper']
		
		self.apply_mode()
		
		self.container.addChild(wrapper)
		self.container.max_size = self.container.min_size = self._max_size
		self.container.adaptLayout(True)
		self.map_callbacks()
		
		self.update_namespaces()
		
	def set_mode(self, mode):
		""" set new mode
		
		@type	mode:	int
		@param	mode:	int id of the mode, either 0 (text mode) or 1 (image mode)
		"""
		if mode not in _MODES: return
		self._mode = mode
		self.apply_mode()
		
	def toggle_mode(self):
		""" callback for button to toggle mode """
		self.set_mode(int(not self._mode))
		self.map_callbacks()
		self.update_namespaces()
		
	def apply_mode(self):
		""" set the gui into the current active mode 
		
		@note:
			- text mode: list item is used
			- image mode: V/HBox with Icon childs is used
		
		"""
		wrapper = self.wrappers[self.orientation]['wrapper']
		vert_img_wrapper = self.wrappers[1]['image_mode_wrapper']
		vert_text_wrapper = self.wrappers[1]['text_mode_wrapper']
		hor_img_wrapper = self.wrappers[0]['image_mode_wrapper']
		hor_text_wrapper = self.wrappers[0]['text_mode_wrapper']
		
		if self._mode == _MODE_TEXT:
			if self.orientation == ObjectSelector.ORIENTATION['Horizontal']:
				vert_img_wrapper.hide()
				vert_text_wrapper.hide()
				hor_img_wrapper.hide()
				hor_text_wrapper.show()
			elif self.orientation == ObjectSelector.ORIENTATION['Vertical']:
				vert_img_wrapper.hide()
				vert_text_wrapper.show()
				hor_img_wrapper.hide()
				hor_text_wrapper.hide()				
		elif self._mode == _MODE_IMAGE:
			if self.orientation == ObjectSelector.ORIENTATION['Horizontal']:
				vert_img_wrapper.hide()
				vert_text_wrapper.hide()
				hor_img_wrapper.show()
				hor_text_wrapper.hide()				
			elif self.orientation == ObjectSelector.ORIENTATION['Vertical']:
				vert_img_wrapper.show()
				vert_text_wrapper.hide()
				hor_img_wrapper.hide()
				hor_text_wrapper.hide()				
		
	def mouse_clicked(self, event):
		if self.container.isDocked(): return
		if event.getButton() == fife.MouseEvent.RIGHT:
			
			self.set_orientation(orientation=int(not self.get_orientation()))
			event.consume()	
		
	def create(self):
		""" creates the gui sceleton """
		if self.container is not None: return	

		self.container = pychan.loadXML('gui/objectselector.xml')
		self.container.position_technique = 'explicit'
		self.container.position = _POSITION
		self.container.name = self.getName()
		self.container.set_orientation = self.set_orientation
		self.container.capture(self.mouse_clicked, "mouseClicked")

		vert_wrapper = self.container.findChild(name='vertical_wrapper')
		hor_wrapper = self.container.findChild(name='horizontal_wrapper')

		self.wrappers[1]['wrapper'] = vert_wrapper
		self.wrappers[1]['image_mode_wrapper'] = vert_wrapper.findChild(name="image_mode_wrapper")
		self.wrappers[1]['text_mode_wrapper'] = vert_wrapper.findChild(name="text_mode_wrapper")
		self.wrappers[1]['image_mode_wrapper'].hide()
		self.wrappers[1]['text_mode_wrapper'].hide()

		self.wrappers[0]['wrapper'] = hor_wrapper
		self.wrappers[0]['image_mode_wrapper'] = hor_wrapper.findChild(name="image_mode_wrapper")
		self.wrappers[0]['text_mode_wrapper'] = hor_wrapper.findChild(name="text_mode_wrapper")
		self.wrappers[0]['image_mode_wrapper'].hide()
		self.wrappers[0]['text_mode_wrapper'].hide()
		
		self.container.removeChild(vert_wrapper)
		self.container.removeChild(hor_wrapper)
		
		self.set_orientation(orientation=self._orientation)
		
		self.container.afterUndock = self.on_undock
		self.container.afterDock = self.on_dock

	def map_callbacks(self):
		""" captures events for the main widgets """
		btn = self.container.findChild(name="toggle_mode")
		btn.capture(self.toggle_mode)

		self.namespace_list = self.container.findChild(name="namespace_list")
		self.namespace_list.capture(self.namespace_id_selected, "action")
		self.namespace_list.capture(self.namespace_id_selected, "mouseWheelMovedUp")
		self.namespace_list.capture(self.namespace_id_selected, "mouseWheelMovedDown")		

		self.search_field = self.container.findChild(name="search_field")
		self.search_field.capture(self._search)
		self.search_field.capture(self._search, "keyPressed")
		self.container.findChild(name="search_button").capture(self._search)		
		
		if self._mode == _MODE_TEXT:
			subwrapper = self.wrappers[self.orientation]['text_mode_wrapper']
			self.preview_icon = subwrapper.findChild(name="preview_icon")
			self.object_list = subwrapper.findChild(name="object_list")
			self.object_list.capture(self.object_id_selected, "action")
			self.object_list.capture(self.object_id_selected, "mouseWheelMovedUp")
			self.object_list.capture(self.object_id_selected, "mouseWheelMovedDown")
		elif self._mode == _MODE_IMAGE:
			subwrapper = self.wrappers[self.orientation]['image_mode_wrapper']
			if self.orientation == ObjectSelector.ORIENTATION['Horizontal']:
				self.object_list = subwrapper.findChild(__class__=pychan.widgets.HBox, name="object_list")
			elif self.orientation == ObjectSelector.ORIENTATION['Vertical']:
				self.object_list = subwrapper.findChild(__class__=pychan.widgets.VBox, name="object_list")
			
	def set_preview(self):
		""" set the preview icon 
		
		@note:
			- only text mode shows a preview icon
		"""
		if self._mode == _MODE_IMAGE: return
		if self._object is None: return
		
		image = self._get_image(object=self._object)
		if image is None: return
		
		w, h = image.getWidth(), image.getHeight()
		w, h = resize(w, h)
		
		self.preview_icon.image = image
		self.preview_icon.size = self.preview_icon.max_size = (w, h)
		
		parent = self.preview_icon.parent
		
		x, y = parent.size
		
		self.preview_icon.position = (int(x / 2 - w / 2), int(y / 2 - h / 2))
		
	def update_namespaces(self, namespaces=None):
		""" fetchs the available namespaces """
		if namespaces is None:
			namespaces = self._engine.getModel().getNamespaces()
			a = set(self.namespace_list.items)
			b = set(namespaces)

			if self.namespace_list.items == list(namespaces):
				return
			self.namespace_list.items = a.union(b)
		else:
			self.namespace_list.items = namespaces
			
		self.namespace_list.items.sort()

		if namespaces:
			self.namespace_list.selected = 0
			ns = namespaces[0]
			self.update_objects(namespace=ns)

		self.container.adaptLayout(True)

	def update_objects(self, namespace=None, objects=None):
		""" fetch the available objects for the selected namespace 

		@type	objects:	list
		@param	objects:	list of fife.Object instances
		@type	namespace:	str
		@param	namespace:	namespace of the object		
		"""
		if namespace is None and objects is None: return
		
		if objects is None and namespace is not None:	
			objects = self._engine.getModel().getObjects(namespace)
		if objects is None: return
	
		if self._mode == _MODE_TEXT:
			object_list = [ListItem(object.getId(), object.getNamespace()) for object in objects]
			self.object_list.items = object_list
		elif self._mode == _MODE_IMAGE:
			self.object_list.removeAllChildren()
			object_list = [ObjectIcon(object.getId(), object.getNamespace(), self._get_image(object=object)) for object in objects]
			for index, object_icon in enumerate(object_list):
				object_icon.capture(object_icon._mouse_pressed, "mousePressed")
				object_icon.index = index
				object_icon.callback = self.object_id_selected
				self.object_list.addChild(object_icon)
			self.object_list.selected = 0
			self.object_list.items = object_list

			self.container.adaptLayout(True)
		
		if object_list:
			self.object_list.selected = 0
			self._current_object_id = object_list[0].object_id	
			self.object_id_selected()
			
	def namespace_id_selected(self):
		""" callback for the namespace list """
		index = self.namespace_list.selected
		if index >= 0:
			ns = self.namespace_list.items[index]
			self.update_objects(namespace=ns)
			
	def object_id_selected(self):
		""" callback for the object list """
		index = self.object_list.selected
		if index >= 0:
			object_id = self.object_list.items[index].object_id
			namespace = self.object_list.items[index].namespace
			object = self._engine.getModel().getObject(object_id, namespace)
			
			self.object_selected(object)
			onObjectSelected.send(sender=self, object=object)

	def object_selected(self, object):
		""" plugin event on selecting a new object
		
		@type	object:	fife.Object
		@param	object:	selected object instance
		"""
		self.set_object(object=object)
		self.set_preview()
		
	def set_object(self, sender=None, object=None):
		""" set new active object for this plugin and notify the editor


		@note:	the editor will create an instance of this object on the map
				once the player uses the insert command

		@type	object:	fife.Object
		@param	object:	selected object instance
		"""
		if sender == self: return
		self._object = object
		
	def on_dock(self):
		""" callback for dock event of B{Panel}	widget """
		side = self.container.dockarea.side
		if not side: return

		module = self.default_settings['module']
		self.eds.set(module, 'dockarea', side)
		self.eds.set(module, 'docked', True)
	
	def on_undock(self):
		""" callback for undock event of B{Panel} widget """
		self.set_mode(self._mode)
		self.set_orientation(orientation=self._default_orientation)
		self.container.hide()
		self.toggle()

		module = self.default_settings['module']
		self.eds.set(module, 'dockarea', '')
		self.eds.set(module, 'docked', False)			
		
	def toggle(self):
		""" shows / hides the gui """
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

	def isEnabled(self):
		""" returns plugin status """
		return self._enabled

	def getName(self):
		""" returns the plugin name as unicode string """
		return u"Object selector"

	def getAuthor(self):
		return "FIFE"
		
	def getDescription(self):
		return ""

	def getLicense(self):
		return ""
		
	def getVersion(self):
		return "0.1"
		
	def _get_image(self, object_data=None, object=None):
		""" returns an image for the given object data
		
		@type	object:	fife.Object
		@param	object:	object instance
		@type	object_data:	ListItem
		@param	object_data: wrapper class containing obj id and namespace
		@rtype	result:	fife.GuiImage
		@return	result:	gui image of the visual of the given object
		@return: fife.GuiImage
		"""
		if object_data is None and object is None: return
		
		if object_data is not None:
			object_id = object_data.object_id
			namespace = object_data.namespace
			object = self._engine.getModel().getObject(object_id, namespace)
		if object is None: return
		
		visual = object.get2dGfxVisual()
		result = None
		imageptr = None

		# try to find a usable image
		index = visual.getStaticImageIndexByAngle(0)

		# if no static image available, try default action
		if index == -1:
			action = object.getDefaultAction()
			if action:
				animation = action.get2dGfxVisual().getAnimationByAngle(0)
				imageptr = animation.getFrameByTimestamp(0)
		else:
			# static image found use the index to get the image from the image manager
			imageptr = self._engine.getImageManager().get(index)

		if imageptr is not None:
			result = fife.GuiImage(imageptr)

		return result
		
	def _search(self):
		""" callback for search field """
		self.search_namespace(self.search_field.text)

	def search_namespace(self, text):
		""" search for the matching namespaces while the user is typing 
		
		@type	text:	string
		@param	text:	user input
		"""
		results = []	
			
		# Format search terms
		terms = [term.lower() for term in text.split()]
		
		# Search
		if len(terms) > 0:
			namespaces = self._engine.getModel().getNamespaces()
			for namesp in namespaces:
				objects = self._engine.getModel().getObjects(namesp)
				for obj in objects:
					doAppend = True
					for term in terms:
						if obj.getId().lower().find(term) < 0:
							doAppend = False
							break
					if doAppend:
						results.append(obj)
		else:
			results = None
			
		self.update_objects(objects=results)

	def update(self):
		""" callback for postMapShown event - update plugin """
		self.set_mode(self._mode)
		self.update_namespaces()
