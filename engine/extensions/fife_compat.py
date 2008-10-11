
"""
FIFE Backwards Combatibility Layer
==================================

This module can be imported if you want to
run code that wasn't adapted to API changes in FIFE.

2008.1
------

 - Animation.addFrame now expects a fife.ResourcePtr instead of an fife.Image
 - Pool.getIndex is just an alias for Pool.addResourceFromFile.

"""

import fife

# Utility functions

def deprecated(revision,message):
	print "fife_compat: Deprecation warning - See revision %d " % revision
	print " - ",message

def this_is_deprecated(func,revision=0,message=None):
	if message is None:
		message = repr(func) + " is deprecated."
	def wrapped_func(*args,**kwargs):
		deprecated(revision,message)
		return func(*args,**kwargs)
	return wrapped_func

# 2008.1 compatibility functions

def decorate_addFrame(f):
	
	def addFrame(self,image_ptr, delay):
		if not isinstance(image_ptr,fife.ResourcePtr):
			image_ptr = fife.ResourcePtr(image_ptr)
			deprecated(2616,"Animation.addFrame doesn't accept Image* anymore ")
		return f(self,image_ptr,delay)
	return addFrame

fife.Animation.addFrame = decorate_addFrame(fife.Animation.addFrame)
fife.Pool.getIndex = this_is_deprecated(
	fife.Pool.addResourceFromFile,
	revision = 2617,
	message  = "Use addResourceFromFile instead of getIndex"
)
