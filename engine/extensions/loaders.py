# coding: utf-8
# Loader interface for FIFE's native xml format.

import fife

from serializers.xmlmap import XMLMapLoader
from serializers import WrongFileType, NameClash

from serializers.xmlobject import XMLObjectLoader

fileExtensions = ('xml',)

def loadMapFile(path, engine, callback=None):
	""" load map file and get (an optional) callback if major stuff is done:
	- map creation
	- parsed imports
	- parsed layers 
	- parsed cameras
	the callback will send both a string and a float (which shows
	the overall process), callback(string, float)
		
	@return	map	: map object
	"""
	map_loader = XMLMapLoader(engine, callback)
	map = map_loader.loadResource(fife.ResourceLocation(path))
	#print "--- Loading map took: ", map_loader.time_to_load, " seconds."
	return map


def loadImportFile(path, engine):
	object_loader = XMLObjectLoader(engine.getImagePool(), engine.getAnimationPool(), engine.getModel(), engine.getVFS())
	res = None
	try:
		res = object_loader.loadResource(fife.ResourceLocation(path))
		print 'imported object file ' + path
	except WrongFileType:
		pass
#		print 'ignored non-object file ' + path
	except NameClash:
		pass
#		print 'ignored already loaded file ' + path
	return res

def loadImportDir(path, engine):
	for file in filter(lambda f: f.split('.')[-1] == 'xml', engine.getVFS().listFiles(path)):
		loadImportFile('/'.join([path, file]), engine)

def loadImportDirRec(path, engine):
	loadImportDir(path, engine)

	for dir in filter(lambda d: not d.startswith('.'), engine.getVFS().listDirectories(path)):
		loadImportDirRec('/'.join([path, dir]), engine)
