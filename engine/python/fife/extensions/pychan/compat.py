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

in_fife = None
guichan = None

def _import_guichan():
	global in_fife

	err_fife = ""
	try:
		from fife import fife
		in_fife = True
		return fife
	except ImportError, e:
		err_fife = str(e)
	
	try:
		import guichan
		in_fife = False
		return guichan
	except ImportError, e:
		import traceback
		traceback.print_exc()
		raise ImportError("Couldn't import neither fife nor guichan: fife:'%s' guichan:'%s'" % (err_fife,str(e)))
guichan = _import_guichan()



def _munge_engine_hook(engine):
	engine.translate_mouse_event = getattr(engine,'translate_mouse_event',lambda x : x )
	engine.translate_key_event   = getattr(engine,'translate_key_event',lambda x : x )

	if not in_fife:
		return engine
	if not isinstance(engine,fife.Engine):
		return engine

	guimanager = engine.getGuiManager()

	def _fife_load_image(filename):
		index = engine.getImagePool().addResourceFromFile(filename)
		return guichan.GuiImage(index,engine.getImagePool())

	class hook:
		pass
	hook = hook()

	hook.add_widget    = guimanager.add
	hook.remove_widget = guimanager.remove
	hook.default_font  = engine.getDefaultFont()
	hook.load_image    = _fife_load_image
	hook.translate_mouse_event = guimanager.translateMouseEvent
	hook.translate_key_event   = guimanager.translateKeyEvent

	hook.screen_width  = engine.getRenderBackend().getScreenWidth()
	hook.screen_height = engine.getRenderBackend().getScreenHeight()

	hook.engine        = engine
	return hook


class _multilistener(guichan.ActionListener,guichan.MouseListener,guichan.KeyListener):
	def __init__(self):
		guichan.ActionListener.__init__(self)
		guichan.MouseListener.__init__(self)
		guichan.KeyListener.__init__(self)


class _point(object):
	def __init__(self,x=0,y=0):
		self.x=0
		self.y=0

if in_fife:
	fife = guichan
	guichan.ActionListener._ActionListener_init__ = lambda x : x
	#guichan.MouseListener.__init__ = lambda x : x
	#guichan.KeyListener.__init__ = lambda x : x
else:
	guichan.Point = _point
	guichan.ScrollArea.SHOW_AUTO = guichan.ScrollArea.ShowAuto
	guichan.ScrollArea.SHOW_NEVER = guichan.ScrollArea.ShowNever
	guichan.ScrollArea.SHOW_ALWAYS = guichan.ScrollArea.ShowAlways

assert isinstance(_multilistener(),guichan.ActionListener)
assert isinstance(_multilistener(),guichan.MouseListener)
assert isinstance(_multilistener(),guichan.KeyListener)
