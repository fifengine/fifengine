# coding: utf-8
# Loader interface for FIFE's native xml format.

import fife

from serializers.xmlmap import XMLMapLoader

def loadMapFile(path, engine, content = ''):
	map_loader = XMLMapLoader(engine)
	return map_loader.loadResource(fife.ResourceLocation(path))

from serializers.xmlobject import XMLObjectLoader

def loadIncludeFile(path, engine):
	object_loader = XMLObjectLoader(engine.getImagePool(), engine.getAnimationPool(), engine.getModel(), engine.getVFS())
	return object_loader.loadResource(fife.ResourceLocation(path))

