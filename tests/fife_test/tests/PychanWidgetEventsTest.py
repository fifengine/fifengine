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

from fife import fife
from fife.extensions import pychan
from fife.extensions.pychan.tools import callbackWithArguments as cbwa

import scripts.test as test

class PychanWidgetEventsTest(test.Test):
    
    def create(self, engine, application):
        self._application = application
        self._engine = engine
        self._running = False
        
        self._controlsPanel = pychan.loadXML('data/gui/widget_events_controls.xml')
        self._controlsPanel.position = (0, 0)
        self._controlsPanel.mapEvents({
            "controlBtn" : self._toggleWindow,
        })
        
        self._outputBox = self._controlsPanel.findChild(name="outputBox")
        
        self._window = pychan.loadXML('data/gui/widget_events_window.xml')
        self._window.capture(event_name="widgetHidden", callback=cbwa(self._widgetHiddenCallback, self._window.name))
        self._window.capture(event_name="widgetShown", callback=cbwa(self._widgetShownCallback, self._window.name))
        self._window.capture(event_name="widgetMoved", callback=cbwa(self._widgetMovedCallback, self._window.name))
        self._window.mapEvents({
            "testButtonHide/ancestorHidden" : cbwa(self._widgetHiddenCallback, self._window.findChild(name="testButtonHide").name),
            "testButtonShow/ancestorShown" : cbwa(self._widgetShownCallback, self._window.findChild(name="testButtonShow").name),
            "testButtonMove/ancestorMoved" : cbwa(self._widgetMovedCallback, self._window.findChild(name="testButtonMove").name)
        })
    
    def destroy(self):
        #del self._controlsPanel
        del self._window
    
    def run(self):
        self._running = True
        self._controlsPanel.show()
    
    def stop(self):
        self._running = False
        
	#any callbacks referencing _controlsPanel or _window and listening for a widgetHidden event
	#should be removed, because after hiding these widget hierarchies will be freed from memory
	self._window.mapEvents({
	    "testButtonHide/ancestorHidden" : None,
	})
	self._window.capture(event_name="widgetHidden", callback=None)

        self._controlsPanel.hide()
        self._window.hide()
    
    def isRunning(self):
        return self._running
    
    def getName(self):
        return "PychanWidgetEventsTest"
        
    def getAuthor(self):
        return "vdaras"
        
    def getDescription(self):
        return "Use this to test that widget events are working as expected."
        
    def _toggleWindow(self):
        if not self._window.isVisible():
            self._window.show()
            self._controlsPanel.findChild(name="controlBtn").text = unicode("Hide Window", "UTF-8")
        else:
            self._window.hide()
            self._controlsPanel.findChild(name="controlBtn").text = unicode("Show Window", "UTF-8")
            
    def _widgetHiddenCallback(self, widget_name):
        txt = unicode("Widget with name %s was hidden" % widget_name, "UTF-8")
        self._printToOutput(txt)
        
    def _widgetShownCallback(self, widget_name):
        txt = unicode("Widget with name %s was shown" % widget_name, "UTF-8")
        self._printToOutput(txt)
        
    def _widgetMovedCallback(self, widget_name):
        txt = unicode("%s was moved" % widget_name, "UTF-8")
        self._printToOutput(txt)
        
    def _printToOutput(self, txt):
        label = pychan.widgets.Label(max_size=(200,1000),wrap_text=True)
        label.text = txt
        self._outputBox.addChild(label)
        self._controlsPanel.adaptLayout()
