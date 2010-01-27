#!/usr/bin/env python
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

import sys, os, wx
import wx.grid as gridlib

class AttributedClass(object):
	def __init__(self):
		self.custom_attrs = {}

class Object(AttributedClass):
	def __init__(self):
		self.actions = []
		self.blocking = False
		self.parent = None
		self.filename = ''
	
class Action(AttributedClass):
	def __init__(self):
		self.animations = {}

class Animation(object):
	def __init__(self):
		self.frames = []
		self.actionframe = -1
		self.filename = ''

class Image(object):
	def __init__(self):
		self.delay = 0
		self.x_offset = 0
		self.y_offset = 0
		self.filename = ''

class FifeObjectEditor(wx.Frame):
	def __init__(self, parent, id, title):
		wx.Frame.__init__(self, parent, id, title, wx.DefaultPosition, wx.Size(700, 500))
		self.CreateStatusBar()
		
		self.SetMenuBar(self.createMenus())
		
		splitter = wx.SplitterWindow(self, -1)
		
		self.tree = wx.TreeCtrl(splitter, 1, wx.DefaultPosition, (-1,-1))
		root = self.tree.AddRoot('New Project')
		
		self.notebook = wx.Notebook(splitter, -1)
		self.projectPropertyPanel = self.createProjectPropertyPanel(self.notebook)
		self.objectPropertyPanel = self.createObjectPropertyPanel(self.notebook)
		self.actionPropertyPanel = self.createActionPropertyPanel(self.notebook)
		self.animationPropertyPanel = self.createAnimationPropertyPanel(self.notebook)
		self.imagePropertyPanel = self.createImagePropertyPanel(self.notebook)
		self.customAttrPanel = self.createCustomAttrPanel(self.notebook)
		self.notebook.AddPage(self.projectPropertyPanel, 'Settings')
		self.notebook.AddPage(self.customAttrPanel, 'Custom Attributes')
		
		splitter.SplitVertically(self.tree, self.notebook, 250)
		
		self.Centre()
		
	def createMenus(self):
		menuBar = wx.MenuBar()
		file = wx.Menu()
		edit = wx.Menu()
		help = wx.Menu()
		
		file.Append(110,  '&Open Project', 'Loads project file')
		file.Append(111,  '&Save Project', 'Saves project file')
		file.Append(112,  'Save Project &As...', 'Saves project file with given filename')
		file.AppendSeparator()
		file.Append(110,  '&Open Object', 'Loads object file')
		file.Append(111,  '&Save Object', 'Saves object file')
		file.Append(112,  'Save Object &As...', 'Saves object file with given filename')
		file.AppendSeparator()
		file.Append(113,  'O&pen Animation', 'Loads animation file')
		file.Append(114,  'Sa&ve Animation', 'Saves animation file')
		file.Append(115,  'Save Animation As...', 'Saves animation file with given filename')
		
		edit.Append(130, '&Create New Object', 'Creates a new object')
		edit.Append(131, '&Delete Object', 'Deletes selected object')
		edit.AppendSeparator()
		edit.Append(132, 'C&reate New Action', 'Creates new action')
		edit.Append(133, 'D&elete Action', 'Deletes selected action')
		edit.AppendSeparator()
		edit.Append(134, 'Cr&eate New Animation', 'Creates new animation')
		edit.Append(135, 'De&lete Animation', 'Deletes selected animation')
		edit.AppendSeparator()
		edit.Append(136, 'Add Frames', 'Adds frames into animation')
		edit.Append(137, 'Delete Frames', 'Deletes selected frames from animation')
		
		help.Append(150,  '&About', 'About the FIFE Object Editor')
		
		menuBar.Append(file, "&File")
		menuBar.Append(edit, "&Edit")
		menuBar.Append(help, "&Help")
		return menuBar
	
	def createProjectPropertyPanel(self, notebook):
		panel = wx.Panel(notebook, -1)
		vbox = wx.BoxSizer(wx.VERTICAL)
		
		hbox = wx.BoxSizer(wx.HORIZONTAL)
		hbox.Add(wx.StaticText(panel, -1, 'Project Name', size=(150, -1)))
		panel.projectName = wx.TextCtrl(panel, -1, '')
		hbox.Add(panel.projectName, wx.EXPAND)
		vbox.Add(hbox, 0, wx.EXPAND | wx.ALL, 4)
		
		hbox = wx.BoxSizer(wx.HORIZONTAL)
		hbox.Add(wx.StaticText(panel, -1, 'Author', size=(150, -1)))
		panel.projectAuthor = wx.TextCtrl(panel, -1, '')
		hbox.Add(panel.projectAuthor, wx.EXPAND)
		vbox.Add(hbox, 0, wx.EXPAND | wx.ALL, 4)
		
		panel.SetSizer(vbox)
		return panel
		
	def createObjectPropertyPanel(self, notebook):
		panel = wx.Panel(notebook, -1)
		return panel

	def createActionPropertyPanel(self, notebook):
		panel = wx.Panel(notebook, -1)
		return panel

	def createAnimationPropertyPanel(self, notebook):
		panel = wx.Panel(notebook, -1)
		return panel

	def createImagePropertyPanel(self, notebook):
		panel = wx.Panel(notebook, -1)
		return panel

	def createCustomAttrPanel(self, notebook):
		panel = wx.Panel(notebook, -1)
		vbox = wx.BoxSizer(wx.VERTICAL)
		grid = gridlib.Grid(notebook, -1)
		grid.CreateGrid(100, 2)
		panel.grid = grid
		vbox.Add(panel.grid, 0, wx.EXPAND | wx.ALL, 4)
		panel.SetSizer(vbox)
		return panel


class FifeObjectEditorApp(wx.App):
	def OnInit(self):
		myframe = FifeObjectEditor(None, -1, "FIFE Object Editor")
		myframe.Show(True)
		return True

if __name__ == '__main__':
	app = FifeObjectEditorApp(0)
	app.MainLoop()
