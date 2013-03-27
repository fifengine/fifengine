#!/usr/bin/env python
# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2009 by the FIFE team
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
import sys, os, re

fife_path = os.path.join('..','..','engine','python')
if os.path.isdir(fife_path) and fife_path not in sys.path:
	sys.path.insert(0,fife_path)

from fife import fife
import rocket

print "Using the FIFE python module found here: ", os.path.dirname(fife.__file__)

from fife.extensions.librocket.rocketbasicapplication import RocketApplicationBase
from fife.extensions.librocket.rocketbasicapplication import RocketEventListener
from fife.extensions.fife_timer import init;

class RocketScriptMediator(object):
	def __init__(self):
		self.docs_to_close = []
		
	def initialize(self, application):
		self.application = application
		
	def closeDocument(self, doc):
		self.docs_to_close.append(doc)
		
		
	def closeDocuments(self):
		for doc in self.docs_to_close:
			doc.Hide()
			self.application.rocketcontext.UnloadDocument
			
		self.docs_to_close = []
		

rocketscriptmediator = RocketScriptMediator()
		
class RocketDemoEventListener(RocketEventListener):
	def __init__(self, app):
		super(RocketDemoEventListener, self).__init__(app)
		self.debuggeractive = True
		
class RocketDemo(RocketApplicationBase):
	
	def __init__(self):
		super(RocketDemo, self).__init__()
		
		rocketscriptmediator.initialize(self)
		
		self.guimanager.showDebugger()
		self._loadFonts()
		self._loadDocuments()
		
	
	def _loadFonts(self):
		font_dir = 'fonts/'
		
		fonts = [
					'FreeSans.ttf'
				]
				
		for font in fonts:
			self.guimanager.loadFont(font_dir + font)
	
	def _loadDocuments(self):
		doc_dir = 'gui/RML/'
		
		docs =  [
					'buttons.rml',
				]
				
		for doc in docs:
			self._documents = [self.rocketcontext.LoadDocument(doc_dir + doc) for doc in docs]
	
	def _pump(self):
		"""
		Overloaded this function to check for quit message.  Quit if message
		is received.
		"""
		rocketscriptmediator.closeDocuments()
		
		if self._listener.quitrequested:
			self.quit()
			
	def run(self):
		#use the id of a document to locate it. This is defined in the body tag of the document using the id attribute.
		self.rocketcontext.documents['buttons_demo'].Show()
		
		super(RocketDemo, self).run()

	def quit(self):
		for doc in self._documents:
			self.rocketcontext.UnloadDocument(doc)
		self._documents = []
		
		#unload documents that may have been loaded using scripts in RML.
		self.rocketcontext.UnloadAllDocuments()
		
		super(RocketDemo, self).quit()
			
	def createListener(self):
		self._listener = RocketDemoEventListener(self)
		return self._listener
			
if __name__ == '__main__':
	app = RocketDemo()
	app.run()
