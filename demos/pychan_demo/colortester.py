# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2010 by the FIFE team
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

""" pychan demo app for testing rgba colors on widgets """

from pychan_demo import PyChanExample
from fife.extensions import pychan

class ColorExample(PyChanExample):
	""" a small app (^^) to show how guichan uses colors on various widgets 
	
	"""
	def __init__(self):
		super(ColorExample,self).__init__('gui/colortester.xml')
		
	def start(self):
		""" 
		load XML file and setup callbacks
		"""
		self.widget = pychan.loadXML(self.xmlFile)
		self.widget.mapEvents({
			'base_rslider': self.update_basecolor,
			'base_gslider': self.update_basecolor,
			'base_bslider': self.update_basecolor,
			'base_aslider': self.update_basecolor,

			'background_rslider': self.update_background_color,
			'background_gslider': self.update_background_color,
			'background_bslider': self.update_background_color,
			'background_aslider': self.update_background_color,
			
			'foreground_rslider': self.update_foreground_color,
			'foreground_gslider': self.update_foreground_color,
			'foreground_bslider': self.update_foreground_color,
			'foreground_aslider': self.update_foreground_color,
			
			'selection_rslider': self.update_selection_color,
			'selection_gslider': self.update_selection_color,
			'selection_bslider': self.update_selection_color,
			'selection_aslider': self.update_selection_color,						

			'closeButton':self.stop,
		})
		# alpha value needs to be set, otherwise you don't see colors ;-)
		self.widget.findChild(name="base_aslider").setValue(float(255))
		self.widget.findChild(name="background_aslider").setValue(float(255))
		self.widget.findChild(name="foreground_aslider").setValue(float(255))
		self.widget.findChild(name="selection_aslider").setValue(float(255))
		
		# init stuff
		self.update_basecolor()
		self.update_background_color()
		self.update_foreground_color()
		self.update_selection_color()
		self.widget.show()

	def update_basecolor(self):
		"""
		Update rgba base colors of all examples and show the values
		"""
		r = int(self.widget.findChild(name="base_rslider").getValue())
		g = int(self.widget.findChild(name="base_gslider").getValue())
		b = int(self.widget.findChild(name="base_bslider").getValue())
		a = int(self.widget.findChild(name="base_aslider").getValue())

		# update slider labels
		self.widget.findChild(name="base_rvalue").text = unicode(str(r), "utf-8")
		self.widget.findChild(name="base_gvalue").text = unicode(str(g), "utf-8")
		self.widget.findChild(name="base_bvalue").text = unicode(str(b), "utf-8")
		self.widget.findChild(name="base_avalue").text = unicode(str(a), "utf-8")
		
		rgba = (r, g, b, a)
		
		self.widget.findChild(name="example1").base_color = rgba
		self.widget.findChild(name="example2").base_color = rgba
		self.widget.findChild(name="example3").base_color = rgba
		self.widget.findChild(name="example4").base_color = rgba
		self.widget.findChild(name="example5").base_color = rgba
		self.widget.findChild(name="example6").base_color = rgba
		self.widget.findChild(name="example7").base_color = rgba
		self.widget.findChild(name="example8").base_color = rgba
		self.widget.findChild(name="example9").base_color = rgba
		
	def update_background_color(self):
		"""
		Update rgba background colors of all examples and show the values
		"""
		r = int(self.widget.findChild(name="background_rslider").getValue())
		g = int(self.widget.findChild(name="background_gslider").getValue())
		b = int(self.widget.findChild(name="background_bslider").getValue())
		a = int(self.widget.findChild(name="background_aslider").getValue())

		# update slider labels
		self.widget.findChild(name="background_rvalue").text = unicode(str(r), "utf-8")
		self.widget.findChild(name="background_gvalue").text = unicode(str(g), "utf-8")
		self.widget.findChild(name="background_bvalue").text = unicode(str(b), "utf-8")
		self.widget.findChild(name="background_avalue").text = unicode(str(a), "utf-8")
		
		rgba = (r, g, b, a)
		
		self.widget.findChild(name="example1").background_color = rgba
		self.widget.findChild(name="example2").background_color = rgba
		self.widget.findChild(name="example3").background_color = rgba
		self.widget.findChild(name="example4").background_color = rgba
		self.widget.findChild(name="example5").background_color = rgba
		self.widget.findChild(name="example6").background_color = rgba
		self.widget.findChild(name="example7").background_color = rgba
		self.widget.findChild(name="example8").background_color = rgba
		self.widget.findChild(name="example9").background_color = rgba
		
	def update_selection_color(self):
		"""
		Update rgba selection colors of all examples and show the values
		"""
		r = int(self.widget.findChild(name="selection_rslider").getValue())
		g = int(self.widget.findChild(name="selection_gslider").getValue())
		b = int(self.widget.findChild(name="selection_bslider").getValue())
		a = int(self.widget.findChild(name="selection_aslider").getValue())

		# update slider labels
		self.widget.findChild(name="selection_rvalue").text = unicode(str(r), "utf-8")
		self.widget.findChild(name="selection_gvalue").text = unicode(str(g), "utf-8")
		self.widget.findChild(name="selection_bvalue").text = unicode(str(b), "utf-8")
		self.widget.findChild(name="selection_avalue").text = unicode(str(a), "utf-8")
		
		rgba = (r, g, b, a)
		
		self.widget.findChild(name="example1").selection_color = rgba
		self.widget.findChild(name="example2").selection_color = rgba
		self.widget.findChild(name="example3").selection_color = rgba
		self.widget.findChild(name="example4").selection_color = rgba
		self.widget.findChild(name="example5").selection_color = rgba
		self.widget.findChild(name="example6").selection_color = rgba
		self.widget.findChild(name="example7").selection_color = rgba
		self.widget.findChild(name="example8").selection_color = rgba
		self.widget.findChild(name="example9").selection_color = rgba
		
	def update_foreground_color(self):
		"""
		Update rgba foreground colors of all examples and show the values
		"""
		r = int(self.widget.findChild(name="foreground_rslider").getValue())
		g = int(self.widget.findChild(name="foreground_gslider").getValue())
		b = int(self.widget.findChild(name="foreground_bslider").getValue())
		a = int(self.widget.findChild(name="foreground_aslider").getValue())

		# update slider labels
		self.widget.findChild(name="foreground_rvalue").text = unicode(str(r), "utf-8")
		self.widget.findChild(name="foreground_gvalue").text = unicode(str(g), "utf-8")
		self.widget.findChild(name="foreground_bvalue").text = unicode(str(b), "utf-8")
		self.widget.findChild(name="foreground_avalue").text = unicode(str(a), "utf-8")
		
		rgba = (r, g, b, a)
		
		self.widget.findChild(name="example1").foreground_color = rgba
		self.widget.findChild(name="example2").foreground_color = rgba
		self.widget.findChild(name="example3").foreground_color = rgba
		self.widget.findChild(name="example4").foreground_color = rgba
		self.widget.findChild(name="example5").foreground_color = rgba
		self.widget.findChild(name="example6").foreground_color = rgba
		self.widget.findChild(name="example7").foreground_color = rgba
		self.widget.findChild(name="example8").foreground_color = rgba
		self.widget.findChild(name="example9").foreground_color = rgba
