# -*- coding: utf-8 -*-
# ####################################################################
#  Copyright (C) 2005-2012 by the FIFE team
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

""" a simple plugin to highlight selected instances 

	no gui, thus no docking. always active
"""

from fife import fife

import scripts.plugin as plugin
import scripts.editor

from scripts.events import onInstancesSelected
from scripts.events import preMapClosed
from scripts.events import postMapShown
from scripts.events import onLayerDelete
from scripts.events import mouseClicked
from scripts.events import onQuit

# color definition, default: 30,144,255 (nice blue :D)
_HIGHLIGHT_COLOR = (30,144,255)
_OUTLINE_SIZE = 1

# highlight type: colored or outlined
_HIGHLIGHT_TYPE = 'colored'

class InstanceHighlighter(plugin.Plugin):
	""" The B{InstanceHighlighter} highlights the selected instances
		either with an outline or a color overlay by using the
		B{fife.InstanceRenderer}
		
	@note:	this plugin doesn't have a gui
	
	@todo:	this plugin can offer options later	(e.g. for defining the
			highlight color or the switching the highlight types)
			ATM the editor has no options menu where plugins can register
			own options
	"""
	_VALID_TYPES = ('colored', 'outline')
	def __init__(self):
		super(InstanceHighlighter, self).__init__()
		self._enabled = False
		self._camera = None
		self._renderer = None
		self._editor = scripts.editor.getEditor()
		
	def enable(self):
		""" enables the plugin """
		if self._enabled: return
		self._enabled = True

		onInstancesSelected.connect(self.highlight)
		preMapClosed.connect(self.reset)
		postMapShown.connect(self.reset)
		onLayerDelete.connect(self.reset)
		mouseClicked.connect(self.mouse_clicked)
		onQuit.connect(self.reset)

	def disable(self):
		""" disables the plugin """
		if not self._enabled: return
		self._enabled = False
		
		onInstancesSelected.disconnect(self.highlight)
		preMapClosed.disconnect(self.reset)
		postMapShown.disconnect(self.reset)
		onLayerDelete.disconnect(self.reset)
		mouseClicked.disconnect(self.mouse_clicked)
		onQuit.disconnect(self.reset)

		self.reset()

		self._camera = None
		self._renderer = None

	def isEnabled(self):
		""" returns plugin status """
		return self._enabled

	def getName(self):
		""" returns the plugin name as unicode string """
		return u"Instance highlighter"

	def highlight(self, instances):
		""" highlights the given instances
		
		@type	instances:	list
		@param	instances:	selected instances
		"""
		self.update_camera()
		if self._renderer is None: return
		
		if _HIGHLIGHT_TYPE not in InstanceHighlighter._VALID_TYPES:
			return
		
		r = self._renderer
		
		col_args = [int(i) for i in _HIGHLIGHT_COLOR]
		
		for instance in instances:
			args = [instance, ]
			if _HIGHLIGHT_TYPE == 'colored':
				args += col_args
				r.addColored(*args)
			elif _HIGHLIGHT_TYPE == 'outline':
				args += col_args + [_OUTLINE_SIZE, ]
				r.addOutlined(*args)
			else:
				continue
		
	def update_camera(self):
		""" updates camera and fetches the renderer """
		mapview = self._editor.getActiveMapView()
		if mapview is None: return
		self._camera = mapview.getCamera()
		if self._camera is None: return
		self._renderer = fife.InstanceRenderer.getInstance(self._camera)					
		if self._renderer is None: return

		self.reset()
		
	def reset(self):
		""" resets the renderer """
		if self._renderer is None: return

		if _HIGHLIGHT_TYPE == 'outline':
			self._renderer.removeAllOutlines()
		elif _HIGHLIGHT_TYPE == 'colored':
			fun = self._renderer.removeAllColored()
		
	def mouse_clicked(self, event):
		""" callback for mouse events, we
			only process RMB events here though	
		"""
		if event is None: return
		if event.getButton() != fife.MouseEvent.RIGHT:
			return
		self.reset()
						
	def getAuthor(self):
		return "FIFE"
		
	def getDescription(self):
		return ""

	def getLicense(self):
		return ""
		
	def getVersion(self):
		return "0.1"
	

