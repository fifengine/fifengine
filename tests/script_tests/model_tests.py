# copy this to root directory so it can find FIFE
import os,sys

_jp = os.path.sep.join
sys.path.append(os.path.abspath(_jp(['.', 'engine', 'swigwrappers', 'python'])))
sys.path.append(os.path.abspath(_jp(['.', 'engine', 'extensions'])))

import fife
import time

print "Creating a Model"
model = fife.Model()
metamodel = model.getMetaModel()

print "Adding 2 maps to the Model"
map1 = model.addMap()
map2 = model.addMap()

map1.set_string("Name", "Map1")
map2.set_string("Name", "Map2")

print "Maps are named:"
print map1.get_string("Name")
print map2.get_string("Name")
