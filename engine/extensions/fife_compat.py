
"""
FIFE Backwards Combatibility Layer
==================================

This module can be imported if you want to
run code that wasn't adapted to API changes in FIFE.

2008.1
------

 - Animation.addFrame now expects a fife.ResourcePtr instead of an fife.Image


"""

import fife

# 2008.1 compatibility functions

def decorate_addFrame(f):
	""" Revision 2616 - Animation.addFrame doesn't accept Image* anymore """
	def addFrame(self,image_ptr, delay):
		resource_ptr = fife.ResourcePtr(image_ptr)
		return f(self,resource_ptr,delay)
	return addFrame

fife.Animation.addFrame = decorate_addFrame(fife.Animation.addFrame)