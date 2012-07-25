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
import sys, os, re

fife_path = os.path.join('..','..','engine','python')
if os.path.isdir(fife_path) and fife_path not in sys.path:
	sys.path.insert(0,fife_path)

from fife import fife
from fife import librocket
print "Using the FIFE python module found here: ", os.path.dirname(fife.__file__)

from fife.extensions.librocket.rocketbasicapplication import RocketApplicationBase

class RocketDemo(RocketApplicationBase):
	
	def __init__(self):
		super(RocketDemo, self).__init__()
	
		self._loadFonts()
		self._loadDocuments()
		self._showDocuments()
	
	def _loadFonts(self):
		font_dir = 'fonts/'
		
		fonts = [
					'FreeSans.ttf'
				]
				
		for font in fonts:
			self.guimanager.loadFont(font_dir + font)
	
	def _loadDocuments(self):
		doc_dir = 'gui/'
		
		docs =  [
					'simple.rml'
				]
				
		self._documents = [self.guimanager.loadDocument(doc_dir + doc) for doc in docs if doc]
		
	def _showDocuments(self):
		for doc in self._documents:
			doc.show()
	
	def _pump(self):
		"""
		Overloaded this function to check for quit message.  Quit if message
		is received.
		"""
		pass

if __name__ == '__main__':
	app = RocketDemo()
	app.run()