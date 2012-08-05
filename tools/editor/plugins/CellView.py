#!/usr/bin/env python
# -*- coding: utf-8 -*-
# ###################################################
# Copyright (C) 2008-2011 The Zero-Projekt team
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

""" a plugin for FIFEdit to select a particular Instance in a cell
	as well as manipulation of some data (e.g. stackpos)
	
	c++ cell blocker policy constants

		# not editable (fife assigns them automaticly)
		CTYPE_NO_BLOCKER
		CTYPE_STATIC_BLOCKER
		CTYPE_DYNAMIC_BLOCKER

		# can be switched by user
		CTYPE_CELL_NO_BLOCKER
		CTYPE_CELL_BLOCKER
		
    def createTransition(self, *args):
        createTransition(self, Layer layer, ModelCoordinate mc)
        return _fife.Cell_createTransition(self, *args)

    def deleteTransition(self, *args):
        deleteTransition(self, Layer layer)
        return _fife.Cell_deleteTransition(self, *args)		
	
"""

from fife import fife
from fife.extensions import pychan
import fife.extensions.pychan.widgets as widgets
from fife.extensions.pychan.tools import callbackWithArguments as cbwa

import scripts
import scripts.plugin as plugin
from scripts.events import *
from scripts.gui.action import Action

_MIN_STACKPOS = 0
_MAX_STACKPOS = 255
_WRAPPER_COLOR = (120, 120, 120)
_WRAPPER_COLOR_SELECTED = (150,150,150)
_WRAPPER_COLOR_HOVER = (100,100,100)
_SUBWRAPPER_SIZE = (200,50)
_HBOX_SIZE = (200,15)
_HBOX_BTN_SIZE = (15,15)
_LABEL1_SIZE = (200,15)
_LABEL2_SIZE = (85,15)
_LABEL3_SIZE = (85,15)

_OUTLINE_COLOR = (255,255,255)
_OUTLINE_SIZE = 1

# Transition wizard constants
# widgets
_T_START_COL = (0,113,0,255)
_T_END_COL = (228,72,70,255)
_T_BTN_SIZE = (20,20)
# cell renderer


# id lookup-table for section hiding		
_SECTION_TO_WRAPPER = {
	'cell_costs_section' : 'cell_costs_wrapper',
	'cell_speed_section' : 'cell_speed_wrapper',
	'transition_wizard_section' : 'transition_wizard_wrapper',
	'instance_on_cell_section' : 'instance_on_cell_wrapper',
	'cell_blocking_policy_section' : 'cell_blocking_policy_wrapper',
	'current_mode' : 'expert_mode_wrapper'
}
# create reverse lookup
_WRAPPER_TO_SECTION = {}
for key, value in _SECTION_TO_WRAPPER.iteritems():
	_WRAPPER_TO_SECTION[value] = key	

# this plugin supports different modes, to separate options which
# can't be edited in certain circumstances (e.g. cell data only
# can be edited if there is a cellcache on the current layer), and
# to hide options which need a certain level of understanding of the
# fife internals (e.g. cell blocking types)
_MODES = {
	# this is the default mode - it gives the plugin the functionality
	# to view instances in a selected cell and e.g. separate them
	# by selecting a particular one and move it out of the cell
	# by using the editors move tool
	'cell_view' : {
		'show' : ['instance_on_cell_section',],
		'allow' : [],
		'disable' : ['expert_mode_section'],		
	},	
	# only available if a cellcache is available
	'cell_edit' : {
		'show' : ['cell_costs_section', 'transition_wizard_section', 'cell_speed_section', 'current_mode'],
		'allow' : [],
		'disable' : ['cell_blocking_policy_section'],			
	},
	# has to be explicitly selected by the user (basically a show all)
	'cell_edit_expert' : {
		'show' : [],
		'allow' : [],
		'disable' : [],
	},		
}
_MODES['cell_edit_expert']['show'] = _SECTION_TO_WRAPPER.keys()
_MODES['cell_edit_expert']['allow'] = _SECTION_TO_WRAPPER.keys()
_MODES['cell_view']['disable'] = list(set(_SECTION_TO_WRAPPER.keys()) - set(_MODES['cell_view']['allow']) - set(_MODES['cell_view']['show']))
_MODES['cell_edit']['allow'] = _MODES['cell_view']['show']

# used for resets etc... (should be the mode which shows the most collapsed sections)
_DEFAULT_MODE = 'cell_view'

# cell blocking policy (see module doc for more infos)
# DON'T touch this - read module doc why
_CELL_TYPE_TO_WDGT_NAME = {
	#fife.CTYPE_NO_BLOCKER : 'blocking_policy_no_blocking',
	#fife.CTYPE_STATIC_BLOCKER : 'blocking_policy_static_blocking',
	#fife.CTYPE_DYNAMIC_BLOCKER : 'blocking_policy_dynamic_blocking',
	fife.CTYPE_CELL_NO_BLOCKER : 'blocking_policy_no_blocking',
	fife.CTYPE_CELL_BLOCKER : 'blocking_policy_blocking',
}	
# create reverse lookup table
_WDGT_NAME_TO_CELL_TYPE = {}
for key, value in _CELL_TYPE_TO_WDGT_NAME.iteritems():
	_WDGT_NAME_TO_CELL_TYPE[value] = key

class CellView(plugin.Plugin):
	""" A simple plugin to control StackPos attribute of 
		fife.Instance -> 2dGfxVisual
	"""
	def __init__(self):
		"""		
		"""
		self._camera = None
		self._renderer = None
		self._controller = None
		self.__cell = None
		self.mode = _DEFAULT_MODE
		
		self._enabled = False
		self._action_show = None		
		
		self.container = None
		self.wrapper = None
		self._help_dialog = None
		
		self.attrSetCallback = pychan.tools.attrSetCallback
		
		self.active = None
		self.previous = None
		
		# FIXME: fields should store ModelCoordinate later instead of str
		self.transition = {
			'start' : '',
			'end' : '',
		}
		
	def _reset(self):
		""" reset dynamic data """
		self._instances = []
		self.active = None
		self.previous = None
		
		self.__cell = None
		self._location = None
		
		if self._help_dialog:
			self._help_dialog.hide()
		
		if self.wrapper:
			self.wrapper.removeAllChildren()
			
		if self.container:
			self.set_mode()

			wdgt = self.container.findChild(name="cell_coords")
			wdgt.text = unicode("- : -", 'utf-8')			
		
		# cleanup controller changes made by this plugin
		if self._controller:
			self._controller._single_instance = False
			self._controller._instance = None	
			
	def _show_help(self):
		""" shows the help dialog """
		if self._help_dialog is not None:
			self._help_dialog.show()
			return
		
		self._help_dialog = pychan.loadXML("gui/help.xml")
		self._help_dialog.title = u"Help (CellView)"
		self._help_dialog.mapEvents({
			"closeButton" : self._help_dialog.hide,
		})
		
		# gimme some more space
		_SIZE = (320,400)
		scrollarea = self._help_dialog.findChildren(__class__=pychan.widgets.ScrollArea)[0]
		scrollarea.size = _SIZE
		scrollarea.min_size = _SIZE
		scrollarea.max_size = _SIZE
		
		f = open('lang/help_cellview.txt', 'r')
		self._help_dialog.findChild(name="helpText").text = unicode(f.read(), 'utf-8')
		f.close()
		
		self._help_dialog.show()	
	
	def enable(self):
		""" plugin method overwrite """
		if self._enabled: return
			
		self._editor = scripts.editor.getEditor()
		self._engine = self._editor.getEngine()
		
		self._action_show = Action(unicode(self.getName(),"utf-8"), checkable=True)
		scripts.gui.action.activated.connect(self.toggle_gui, sender=self._action_show)
		self._editor._tools_menu.addAction(self._action_show)
		
		events.onCellSelected.connect(self.input)
		events.preMapClosed.connect(self.hide)
		events.postMapShown.connect(self.map_shown)
		events.mousePressed.connect(self.mouse_pressed)
		
		self._reset()		
		self.create()
		
		self.container.x = 0
		self.container.y = 0
		self.show()

	def disable(self):
		""" plugin method """
		if not self._enabled: return
			
		self._reset()
		
		self.wrapper = None
		self.container.setDocked(False)
		self.container.hide()	
		
		events.onInstancesSelected.disconnect(self.input)
		events.preMapClosed.disconnect(self.hide)
		events.preMapShown.disconnect(self.map_shown)
		events.mousePressed.disconnect(self.mouse_pressed)
		
		self._editor._tools_menu.removeAction(self._action_show)

	def isEnabled(self):
		""" plugin method """
		return self._enabled;

	def getName(self):
		""" plugin method """
		return "Cell view"		
	
	def create(self):
		""" create gui sceleton """
		if self.container: return
		self.container = pychan.loadXML('gui/cellview.xml')
		self.wrapper = self.container.findChild(name="cell_instance_wrapper")
		
		btn = self.container.findChild(name="show_help")
		btn.capture(self._show_help, 'mousePressed')
		
		# create callbacks for section hiding
		for wdgt_name in _SECTION_TO_WRAPPER.keys():
			wdgt = self.container.findChild(name=wdgt_name)
			wdgt.capture(self.cb_toggle_section, "mousePressed")
		
		# create callbacks for cell blocking policy manipulation
		for name in _WDGT_NAME_TO_CELL_TYPE:
			value = _WDGT_NAME_TO_CELL_TYPE[name]
			wdgt = self.container.findChild(name=name)
			if not wdgt: continue
			wdgt.capture(cbwa(self.set_cell_blocking_policy, value), "mousePressed")
			
		# create callback for setting the expert mode
		wdgt = self.container.findChild(name='expert_mode')
		wdgt.capture(self.updateMode, "mousePressed")
		
		# create callback for setting the speed multiplier
		wdgt = self.container.findChild(name='apply_speed_multiplier')
		wdgt.capture(self.set_cell_speed_multiplier, "mousePressed")
		
		# create callback for setting the default cost_multiplier
		wdgt = self.container.findChild(name='apply_cost_multiplier')
		wdgt.capture(self.set_cell_default_cost_multiplier, "mousePressed")
		
		# create callbacks for mini editor
		wdgt = self.container.findChild(name="mini_editor_cost_save")
		wdgt.capture(self.mini_editor_create_cost, "mousePressed")
		# hide mini editor for cost entries
		self.minieditor = self.container.findChild(name='cost_mini_editor')
		self.minieditor.hide()
		# create callback to control the mini editor
		wdgt = self.container.findChild(name='add_cost_entry')
		wdgt.capture(self.minieditor.show, 'mousePressed')
		wdgt = self.container.findChild(name="edit_current_cost_entry")
		wdgt.capture(self.mini_editor_edit_cost, 'mousePressed')		
		wdgt = self.container.findChild(name="remove_current_cost_entry")
		wdgt.capture(self.mini_editor_remove_cost, 'mousePressed')
		# create fancy callbacks for mouse actions on the listbox
		listbox = self.container.findChild(name="cost_entries")
		listbox.capture(self.mini_editor_edit_cost, 'mousePressed')
		
		# transition
		wdgt = self.container.findChild(name="transition_start")
		wdgt.capture(cbwa(self.transition_set, start=1), 'mousePressed')
		wdgt = self.container.findChild(name="transition_end")
		wdgt.capture(cbwa(self.transition_set, end=1), 'mousePressed')
		wdgt.hide()
		wdgt = self.container.findChild(name="transition_create")
		wdgt.capture(cbwa(self.transition_set, create=1), 'mousePressed')
		wdgt.hide()
		wdgt = self.container.findChild(name="transition_immediate")
		wdgt.hide()
		
		
		self.container.hide()
		
	def transition_set(self, start=0, end=0, create=0):
		""" create transition data and the transition itself if we have
			all data
			
		@todo:
				- split this function up
				- click on ToggleButtons should focus cam on the corresponding cell
				- split set/unset code like that:
				  - 1st click on button -> collect data
				  - RMB on button -> delete data
				  - 2nd click on button -> focus the cell with cam
				- we should store all transition attempts in a datastructure
				  instead of erasing it with each update
				- update() should be locked if we are in the "click transition target"-mode
		"""
		if self.__cell is None: return

		tbtn = self.container.findChild(name="transition_end")
		btn = self.container.findChild(name="transition_create")
		chkbox = self.container.findChild(name="transition_immediate")

		if start:
			if self.transition['start']:
				tbtn.hide()
				btn.hide()
				chkbox.hide()
				self.transition['start'] = ''
			else:
				self.transition['start'] = 'put start coords here'
				tbtn.show()
		if end:
			if self.transition['end']:
				self.transition['end'] = ''
				btn.hide()
				chkbox.hide()
			else:
				self.transition['end'] = 'put end coords here'
				btn.show()
				chkbox.show()
		# create or destroy previous transition
		if create:
			if self.transition['start'] and self.transition['end']:
				wdgt = self.container.findChild(name="transition_immediate")
				immediate = False
				if wdgt.marked: immediate = True

	def set_mode(self, mode=_DEFAULT_MODE):
		""" set the mode for the plugin
		
		@type	mode:	str
		@param	mode:	key for the _MODES dict
		"""
		if mode in _MODES:
			self.mode = mode
		
		if self.mode is None: return
		
		# update 'current mode' widget
		lbl = self.container.findChild(name='current_mode')
		lbl.text = u"Current mode: " + self.mode
		
		# show / hide all sections according to the new mode
		# show
		for section in _MODES[self.mode]['show']:
			self.show_section(section=section)
		# hide
		for section in _MODES[self.mode]['disable']:
			self.hide_section(section=section)
			
		self.container.adaptLayout(True)
		
	def mini_editor_remove_cost(self):
		""" delete the current selected cost entry
		
		@todo:
				- do we need one of those annoying pop-ups here to
				  ask "do you really want to delete this"?
		"""
		if self.__cell is None: return
		msg = ''

		listbox = self.container.findChild(name="cost_entries")
		if len(listbox.items) == 0: return
		index = listbox.selected
		
		cost_id = listbox.items[index]
		cache = self.__cell.getLayer().getCellCache()
		print cost_id
		print cache.getCellCosts(self.__cell)

		try:
			cache.removeCellFromCost(str(cost_id), self.__cell)
			print cache.getCellCosts(self.__cell)
		except:
			msg = self.getName() + "Plugin: Error! Couldn't remove cost from cell"
		else:	
			msg = self.getName() + "Plugin: Success! Cost was removed from cell"
			self.update()

		if msg == '':
			self._editor.getStatusBar().setText(unicode(msg, 'utf-8'))
				
	def mini_editor_edit_cost(self, event=None, widget=None):
		""" edit the given current selected cost entry by reading
			the values into the mini editor & show it
			
			We also check if RMB was pressed on the listbox and return otherwise
			
		@type	widget:	pychan.Widget
		@param	widget:	widget which triggered an event
		@type	event:	pychan.Event
		@type	event:	event caused by an widget			
		"""
		if self.__cell is None: return
		if widget is not None:
			if widget.name == 'cost_entries' and event.getButton() != fife.MouseEvent.RIGHT:
				return

		wdgt_id = self.container.findChild(name="mini_editor_cost_id")
		wdgt_val = self.container.findChild(name="mini_editor_cost_value")
	
		listbox = self.container.findChild(name="cost_entries")
		index = listbox.selected
		if len(listbox.items) == 0: return
		
		cost_id = listbox.items[index]
		cost_val = self.__cell.getLayer().getCellCache().getCost(cost_id)
		
		wdgt_id.text = unicode(cost_id, 'utf-8')
		wdgt_val.text = unicode(str(cost_val), 'utf-8')

		self.minieditor.show()		
		
	def mini_editor_create_cost(self, event=None, widget=None):
		""" read out data from the mini editor and apply it if possible
		
		@type	widget:	pychan.Widget
		@param	widget:	widget which triggered an event
		@type	event:	pychan.Event
		@type	event:	event caused by an widget
		"""
		if event is None and widget is None: return
		if widget is None: return
		if self.__cell is None: return
		
		msg = ''
		wdgt_id = self.container.findChild(name="mini_editor_cost_id")
		wdgt_val = self.container.findChild(name="mini_editor_cost_value")
		
		cost_id = wdgt_id.text
		cost_value = wdgt_val.text
		
		try:
			cost_value = float(cost_value)
		except:
			msg = self.getName() + "Plugin: new cost value has to be a float"
			self._editor.getStatusBar().setText(unicode(msg, 'utf-8'))
		else:
			cache = self.__cell.getLayer().getCellCache()
			cache.registerCost(str(cost_id), cost_value)
			cache.addCellToCost(str(cost_id), self.__cell)
			msg = self.getName() + "Plugin: Success! New cost id and value saved to cell"			
			self.update()	
			
		if msg:
			self._editor.getStatusBar().setText(unicode(msg, 'utf-8'))						
	
	def set_cell_default_cost_multiplier(self):
		""" set the default cost multiplier of the current selected cell """
		if self.__cell is None: 
			msg = self.getName() + "Plugin: no cell selected, aborting update of default cost multiplier"
			self._editor.getStatusBar().setText(unicode(msg, 'utf-8'))		
			return		
		if not hasattr(self.__cell, 'setCostMultiplier'):
			msg = self.getName() + "Plugin: wrong fife version, cell lacks method 'setCostMultiplier'"
			self._editor.getStatusBar().setText(unicode(msg, 'utf-8'))		
			return

		msg = ''

		wdgt = self.container.findChild(name="default_cost_multiplier")
		default_cost_multiplier = wdgt.text
		
		try:
			default_cost_multiplier = float(default_cost_multiplier)
		except: 
			msg = self.getName() + "Plugin: wrong value type for default cost multiplier, must be float"
		else:
			try:
				self.__cell.setCostMultiplier(default_cost_multiplier)
			except:
				msg = self.getName() + "Plugin: something went wrong - couldn't set default cost multiplier for current cell"
			else: msg = self.getName() + "Plugin: Success! Set new default cost multiplier for current cell to %s" % default_cost_multiplier
			
		if msg:
			self._editor.getStatusBar().setText(unicode(msg, 'utf-8'))			
	
	def set_cell_speed_multiplier(self):
		""" set the current user input for the cell's speed multiplier """
		if self.__cell is None: 
			msg = self.getName() + "Plugin: no cell selected, aborting update of speed multiplier"
			self._editor.getStatusBar().setText(unicode(msg, 'utf-8'))		
			return		
		if not hasattr(self.__cell, 'setSpeedMultiplier'):
			msg = self.getName() + "Plugin: wrong fife version, cell lacks method 'setSpeedMultiplier'"
			self._editor.getStatusBar().setText(unicode(msg, 'utf-8'))		
			return

		msg = ''

		wdgt = self.container.findChild(name="speed_multiplier")
		speed_multiplier = wdgt.text
		
		try:
			speed_multiplier = float(speed_multiplier)
		except: 
			msg = self.getName() + "Plugin: wrong value type for speed multiplier, must be float"
		else:	
			try:
				self.__cell.setSpeedMultiplier(speed_multiplier)
			except:
				msg = self.getName() + "Plugin: something went wrong - couldn't set speed multiplier for current cell"
			else: msg = self.getName() + "Plugin: Success! Set new speed multiplier for current cell to %s" % speed_multiplier
			
		if msg:
			self._editor.getStatusBar().setText(unicode(msg, 'utf-8'))			
		
	def set_cell_blocking_policy(self, value, widget=None):
		""" set the current cell to the next available blocking policy 
		
		@type	value:	int
		@param	value:	integer code for cell blocking type
		@type	widget:	pychan.Widget
		@param	widget:	widget which calls this method
		"""
		if not self.__cell: 
			msg = self.getName() + "Plugin: no cell selected, aborting update of blocking policy"
			self._editor.getStatusBar().setText(unicode(msg, 'utf-8'))		
			return
		if not hasattr(self.__cell, 'setCellType'):
			msg = self.getName() + "Plugin: wrong fife version, cell lacks method 'setCellType'"
			self._editor.getStatusBar().setText(unicode(msg, 'utf-8'))		
			return
			
		self.__cell.setCellType(value)
		msg = self.getName() + "Plugin: set cell blocking policy to %s" % value
		self._editor.getStatusBar().setText(unicode(msg, 'utf-8'))		
	
	def cb_toggle_section(self, event, widget):
		""" widget callback - toggles a section wrapper connected to the given widget
		
		@type	widget:	pychan.Widget
		@param	widget:	widget which triggered an event
		@type	event:	pychan.Event
		@type	event:	event caused by an widget
		"""
		if widget.name not in _SECTION_TO_WRAPPER: return
		section_wrapper_name = _SECTION_TO_WRAPPER[widget.name]
		self.toggle_section(section_wrapper_name)
		
	def toggle_section(self, section_wrapper_name=''):
		""" flip between show/hide of a section wrapper
		
		@type	section_wrapper_name:	str
		@param	section_wrapper_name:	widget name of a section wrapper		
		"""
		section_wrapper = self.container.findChild(name=section_wrapper_name)
		
		if section_wrapper.isVisible():
			self.hide_section(widget=section_wrapper)
		else:
			self.show_section(widget=section_wrapper)
		
		self.container.adaptLayout(True)
		
	def hide_section(self, widget=None, section=None):
		""" hides a section - hiding is always possible (not restricted
		    by the current mode of this plugin
		    
		    FIXME:
				- decide if we need section here (e.g. to set the users
				  last choice of the layout of the mode - might be nice
				  if FIFedit allows to save plugin data and restore
				  the users last choices in a new FIFedit session)
		
		@type	widget:	pychan.Widget
		@param	widget:	section wrapper
		@param	section:	str
		@param	section:	section type which should be hidden
		"""
		if widget is None and section is not None:
			section_wrapper_name = _SECTION_TO_WRAPPER[section]
			widget = self.container.findChild(name=section_wrapper_name)
#		if section is None and widget is not None:
#			if widget.name in _WRAPPER_TO_SECTION:
#				section = _WRAPPER_TO_SECTION[widget.name]
#			else:
#				return
		if widget is None and section is None: return

		widget.hide()
		
	def show_section(self, widget=None, section=None):
		"""
		@type	widget:	pychan.Widget
		@param	widget:	section wrapper
		@param	section:	str
		@param	section:	section type which should be hidden
		"""
		if widget is None and section is not None:
			section_wrapper_name = _SECTION_TO_WRAPPER[section]
			widget = self.container.findChild(name=section_wrapper_name)
		if section is None and widget is not None:
			if widget.name in _WRAPPER_TO_SECTION:
				section = _WRAPPER_TO_SECTION[widget.name]
			else:
				return
		if widget is None and section is None: return

		if section in _MODES[self.mode]['disable']:
			# give feedback to the user 
			msg = self.getName() + "Plugin: section cannot be shown due to mode restrictions"
			self._editor.getStatusBar().setText(unicode(msg, 'utf-8'))
		else:
			widget.show()		
	
	def toggle_gui(self):
		""" toggle visibility of main container """
		if self.container.isVisible():
			self.hide()
		else:
			self.show()
		
	def hide(self):
		""" hides the main container """
		self.container.hide()
		self._action_show.setChecked(False)
		
	def show(self):
		""" shows the main container """
		self.container.show()
		self._action_show.setChecked(True)

	def map_shown(self):
		""" callback for editor signal: postMapShown """
		mapview = self._editor.getActiveMapView()
		self._controller = mapview.getController()
		self.set_mode()		
		self.show()		
			
	def mouse_pressed(self, event):
		""" callback for editor signal: mousePressed """
		if event.getButton() == fife.MouseEvent.RIGHT:
			self._reset()

	def increase_cell_stackpos(self, event, widget):
		""" callback for widget; increase cell stack  pos
		
		@type	widget:	pychan.Widget
		@param	widget:	widget which triggered an event
		@type	event:	pychan.Event
		@type	event:	event caused by an widget	
		"""
		index = widget.name
		print "Implement increase of cell_stackpos"

	def decrease_cell_stackpos(self, event, widget):
		""" callback for widget; decrease cell stack  pos
		
		@type	widget:	pychan.Widget
		@param	widget:	widget which triggered an event
		@type	event:	pychan.Event
		@type	event:	event caused by an widget	
		"""
		index = widget.name
		print "Implement decrease of cell_stackpos"
		
	def increase_stackpos(self, event, widget):
		""" callback for btn1; increases stackpos
		
		@type	widget:	pychan.Widget
		@param	widget:	widget which triggered an event
		@type	event:	pychan.Event
		@type	event:	event caused by an widget	
		"""
		index = widget.name
		instance = self.get_instance(index)
		if not instance: return

		sp = instance.get2dGfxVisual().getStackPosition()
		if sp < _MAX_STACKPOS:
			sp += 1
			instance.get2dGfxVisual().setStackPosition(sp)
			self.update()

	def decrease_stackpos(self, event, widget):
		""" callback for btn1; decreases stackpos
		
		@type	widget:	pychan.Widget
		@param	widget:	widget which triggered an event
		@type	event:	pychan.Event
		@type	event:	event caused by an widget	
		"""
		index = widget.name
		instance = self.get_instance(index)
		if not instance: return

		sp = instance.get2dGfxVisual().getStackPosition()
		if sp > _MIN_STACKPOS:
			sp -= 1
			instance.get2dGfxVisual().setStackPosition(sp)
			self.update()
		
	def get_instance(self, index=None):
		""" turn list index into instance
		
		@type	index:	int
		@param	index:	list index to access instance
		"""
		instance = None
		try:
			instance = self._instances[index]
		except IndexError:
			print "index %s not in self._instances" % index
		return instance
		
	def select(self, event, widget):
		""" callback for selected subwrapper 
		
		@todo:
				- FIFedit needs a flag to either
				  only fetch single instances or all instances
				  on a cell, otherwise this plugin can´t
				  work as it should (selecting a particular
				  instance and edit it)
		
		@type	widget:	pychan.Widget
		@param	widget:	widget which triggered an event
		@type	event:	pychan.Event
		@type	event:	event caused by an widget
		"""
		index = int(widget.name)
		instance = self.get_instance(index)

		if self.active:
			wdgt = self.wrapper.findChild(name=self.active)
			if wdgt:
				wdgt.base_color = _WRAPPER_COLOR
				self.previous = self.active
		self.active = index
		widget.base_color = _WRAPPER_COLOR_SELECTED	
		
		# interesting part - reset editor selection and dump our instance into it
		self._controller.deselectSelection()
		self._controller._selection.append(instance.getLocation())
		self._controller._single_instance = True
		self._controller._instance = instance
		fife.CellSelectionRenderer.getInstance(self._controller._camera).selectLocation(instance.getLocation())	
	
	def hover_enter_subwrapper(self, event, widget):
		""" callback for on mouse enter events on subwrapper """
		widget.base_color = _WRAPPER_COLOR_HOVER

		instance = self.get_instance(widget.name)
		if instance:
			self._renderer.removeAllOutlines()
			args = (
				instance,
				_OUTLINE_COLOR[0],
				_OUTLINE_COLOR[1],
				_OUTLINE_COLOR[2],
				_OUTLINE_SIZE
			)
			self._renderer.addOutlined(*args)
		
	def hover_exit_subrwapper(self, event, widget):
		""" callback for on mouse exit events on subwrapper """
		if widget.name != self.active:
			widget.base_color = _WRAPPER_COLOR
		else:
			widget.base_color = _WRAPPER_COLOR_SELECTED
		self._renderer.removeAllOutlines()			
	
	def update(self):
		""" redraws dynamic widgets; interprets self._instances """
		if not self._location and not self.__cell: return

		if self.__cell:
			coords = self.__cell.getLayerCoordinates()
		else:
			coords = self._location.getLayerCoordinates()
			
		x, y = round(coords.x, 2), round(coords.y, 2)
		wdgt = self.container.findChild(name="cell_coords")
		wdgt.text = unicode(str(x) + " : " + str(y), 'utf-8')
		
		if self.__cell:
			cache = self.__cell.getLayer().getCellCache()
			if hasattr(self.__cell, 'getCellType'):
				cell_type = self.__cell.getCellType()
				if cell_type in _CELL_TYPE_TO_WDGT_NAME:
					wdgt_name = _CELL_TYPE_TO_WDGT_NAME[cell_type]
					wdgt = self.container.findChild(name=wdgt_name)
					wdgt.toggled = 1
			if hasattr(self.__cell, 'getSpeedMultiplier'):
				speed_multiplier = self.__cell.getSpeedMultiplier()
				wdgt = self.container.findChild(name="speed_multiplier")
				wdgt.text = unicode(str(speed_multiplier), 'utf-8')
			if hasattr(self.__cell, 'getCostMultiplier'):
				default_cost_multiplier = self.__cell.getCostMultiplier()
				wdgt = self.container.findChild(name="default_cost_multiplier")
				wdgt.text = unicode(str(default_cost_multiplier), 'utf-8')

			if hasattr(cache, 'getCosts'):
				self.minieditor.hide()
				costs = cache.getCellCosts(self.__cell)
				listbox = self.container.findChild(name="cost_entries")
				listbox.items = costs
				if costs:
					listbox.selected = 0
					
			wdgt = self.container.findChild(name="transition_end")
			wdgt.hide()
			wdgt = self.container.findChild(name="transition_create")
			wdgt.hide()
			wdgt = self.container.findChild(name="transition_immediate")
			wdgt.hide()					

		# rebuild the gui list of all instances on the current cell
		self.wrapper.removeAllChildren()
		for index, instance in enumerate(self._instances):
			fife_object_id = instance.getObject().getId()
			
			subwrapper = pychan.widgets.VBox(padding=2)
			subwrapper.name= index
			subwrapper.base_color = _WRAPPER_COLOR
			subwrapper.size = subwrapper.min_size = subwrapper.max_size = _SUBWRAPPER_SIZE

			subwrapper.capture(self.hover_enter_subwrapper, "mouseEntered")
			subwrapper.capture(self.hover_exit_subrwapper, "mouseExited")
			subwrapper.capture(self.select, "mousePressed")

			# object id
			label1 = pychan.widgets.Label()
			label1.text = unicode(fife_object_id, 'utf-8')
			label1.size = label1.min_size = label1.max_size = _LABEL1_SIZE

			subwrapper.addChild(label1)

			data = {
				'index' : index,
				'stack_pos' : instance.get2dGfxVisual().getStackPosition(),
				'stack_pos_name' : 'Stack pos',
				'inc_callback' : self.increase_stackpos,
				'dec_callback' : self.decrease_stackpos,
			}
			
			stackpos_edit_wdgt = StackposEditWidget(data)
			subwrapper.addChild(stackpos_edit_wdgt.widget)

			data['stack_pos'] = instance.getCellStackPosition()
			data['stack_pos_name'] = 'Cell stack pos'
			data['inc_callback'] = self.increase_cell_stackpos
			data['dec_callback'] = self.decrease_cell_stackpos
			
			stackpos_edit_wdgt = StackposEditWidget(data)
			subwrapper.addChild(stackpos_edit_wdgt.widget)

			subwrapper.adaptLayout()
			
			self.wrapper.addChild(subwrapper)
			self.wrapper.adaptLayout()

		self.container.adaptLayout(True)

	def input(self, locations):
		""" receiver for onCellSelected 
			we filter given instances, to not let the
			user dump instances from different cells here
		"""
		self._reset()
		if not locations: return

		self._camera = self._editor.getActiveMapView().getCamera()
		self._renderer = fife.InstanceRenderer.getInstance(self._camera)

		self._instances = []
		self._location = locations[0]
		layer = self._location.getLayer()
		fcoords = self._location.getLayerCoordinates()
		
		# decide wether we allow cell manipulation or not
		# no cell cache, no cell manipulation
		cellcache = layer.getCellCache()
		if cellcache is None:
			self.__cell = None
			self.set_mode('cell_view')
			# get instances from layer
			self._instances = layer.getInstancesAt(self._location)
		else:
			self.__cell = cellcache.getCell(fcoords)
			if self.__cell is None:
				self.__cell = None
				self.set_mode('cell_view')
				# get instances from layer
				self._instances = layer.getInstancesAt(self._location)
			else:
				wdgt = self.container.findChild(name="expert_mode")
				if wdgt.marked:
					self.set_mode('cell_edit_expert')
				else:
					self.set_mode('cell_edit')
				# get instances from cell
				self._instances = self.__cell.getInstances()

		self.update()

	def updateMode(self):
		if self.__cell is None:
			self.set_mode('cell_view')
		else:
			wdgt = self.container.findChild(name="expert_mode")
			if not wdgt.marked:
				self.set_mode('cell_edit_expert')
			else:
				self.set_mode('cell_edit')

		self.update()
		
class StackposEditWidget(object):
	""" just to avoid code duplication """
	def __init__(self, data):
		self.widget = self.create(data)
		
	def create(self, data):
		""" """
		index = data['index']
		stack_pos = data['stack_pos']
		name = data['stack_pos_name']
		inc_callback = data['inc_callback']
		dec_callback = data['dec_callback']

		hbox = pychan.widgets.HBox(padding=0)
		hbox.base_color = (0,0,0,0)
		hbox.size = hbox.min_size = hbox.max_size = _HBOX_SIZE
		
		# buttons for increase / decrease visual stack pos
		btn1 = pychan.widgets.Button()
		btn1.size = btn1.min_size = btn1.max_size = _HBOX_BTN_SIZE
		btn1.name = index
		btn1.text = unicode('+','utf-8')
		btn1.capture(inc_callback, 'mousePressed')
		btn2 = pychan.widgets.Button()
		btn2.size = btn2.min_size = btn2.max_size = _HBOX_BTN_SIZE
		btn2.text = unicode('-','utf-8')
		btn2.name = index
		btn2.capture(dec_callback, 'mousePressed')			
		
		# visual stackpos description
		label2 = pychan.widgets.Label()
		text = unicode(name, 'utf-8')
		label2.text = text
		label2.size = label2.min_size = label2.max_size = _LABEL2_SIZE
		# visual stackpos value
		label3 = pychan.widgets.Label()
		text = unicode(str(stack_pos), 'utf-8')
		label3.text = text
		label3.size = label3.min_size = label3.max_size = _LABEL3_SIZE

		hbox.addChild(label2)
		hbox.addChild(label3)
		hbox.addChild(btn1)
		hbox.addChild(btn2)
		hbox.adaptLayout()
		
		return hbox
