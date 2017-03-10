#!/usr/bin/env python
# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2017 by the FIFE team
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
	log.setVisibleModules('all')
	s = e.getSettings()
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
