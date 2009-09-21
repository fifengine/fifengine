#!/usr/bin/env python
import os, sys, unittest

def genpath(somepath):
	return os.path.sep.join(somepath.split('/'))

_paths = ('engine/swigwrappers/python', 'engine/extensions')
for p in _paths:
	if p not in sys.path:
		sys.path.append(os.path.sep.join(p.split('/')))

import fife, fifelog

def getEngine(minimized=False):
	e = fife.Engine()
	log = fifelog.LogManager(e, promptlog=False, filelog=True)
	#log.setVisibleModules('all')
	s = e.getSettings()
	#s.setRenderBackend('OpenGL')
	s.setDefaultFontPath('tests/data/FreeMono.ttf')
	s.setDefaultFontGlyphs(" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" +
			".,!?-+/:();%`'*#=[]")
	if minimized:
		s.setScreenWidth(1)
		s.setScreenHeight(1)
	s.setDefaultFontSize(12)
	e.init()
	return e

__all__ = []
__all__.append('unittest')
__all__.append('fife')
__all__.append('fifelog')
__all__.append('genpath')
__all__.append('getEngine')
