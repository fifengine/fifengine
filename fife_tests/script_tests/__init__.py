import os, sys, unittest

def genpath(somepath):
	return os.path.sep.join(somepath.split('/'))

# Switch to engine root directory
_origpath = os.path.abspath(sys.argv[0])
_rootpath = _origpath.split(genpath('/fife_tests/'))[0]
print _rootpath
os.chdir(_rootpath)

_paths = ('engine/swigwrappers/python', 'engine/extensions')
for p in _paths:
	sys.path.append(os.path.sep.join(p.split('/')))

import fife
__all__ = []
__all__.append('unittest')
__all__.append('fife')
__all__.append('genpath')
