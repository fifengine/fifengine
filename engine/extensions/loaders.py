# coding: utf-8
# Loader interface for FIFE's native xml format.

import fife

from serializers.xmlmap import XMLMapLoader

def loadMapFile(path, engine, content = ''):
	map_loader = XMLMapLoader(engine)
	return map_loader.loadResource(fife.ResourceLocation(path))

from serializers.xmlobject import XMLObjectLoader

def loadImportFile(path, engine):
	object_loader = XMLObjectLoader(engine.getImagePool(), engine.getAnimationPool(), engine.getModel(), engine.getVFS())
	return object_loader.loadResource(fife.ResourceLocation(path))

def loadImportDir(path, engine):
	for file in filter(lambda f: f.split('.')[-1] == 'xml', engine.getVFS().listFiles(path)):
		try:
			loadImportFile('/'.join([path, file]), engine)
		except:
			# Probably this happened because we tried to load a non-xml file. Exception types should be
			# made more available so we don't have these null handlers.
			continue

def loadImportDirRec(path, engine):
	loadImportDir(path, engine)

	for dir in filter(lambda d: not d.startswith('.'), engine.getVFS().listDirectories(path)):
		loadImportDirRec('/'.join([path, dir]), engine)
