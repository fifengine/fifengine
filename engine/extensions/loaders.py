# coding: utf-8
# Loader interface for FIFE's native xml format.

import fife

from serializers.xmlmap import XMLMapLoader
from serializers import WrongFileType, NameClash

fileExtensions = ('xml',)

def loadMapFile(path, engine, content = ''):
	map_loader = XMLMapLoader(engine)
	return map_loader.loadResource(fife.ResourceLocation(path))

from serializers.xmlobject import XMLObjectLoader

def loadImportFile(path, engine):
	object_loader = XMLObjectLoader(engine.getImagePool(), engine.getAnimationPool(), engine.getModel(), engine.getVFS())
	res = None
	try:
		res = object_loader.loadResource(fife.ResourceLocation(path))
		print 'imported object file ' + path
	except WrongFileType:
		print 'ignored non-object file ' + path
	except NameClash:
		print 'ignored already loaded file ' + path
	return res

def loadImportDir(path, engine):
	for file in filter(lambda f: f.split('.')[-1] == 'xml', engine.getVFS().listFiles(path)):
		loadImportFile('/'.join([path, file]), engine)

def loadImportDirRec(path, engine):
	loadImportDir(path, engine)

	for dir in filter(lambda d: not d.startswith('.'), engine.getVFS().listDirectories(path)):
		loadImportDirRec('/'.join([path, dir]), engine)
