import os, sys, unittest

def genpath(somepath):
	return os.path.sep.join(somepath.split('/'))

_paths = ('engine/swigwrappers/python', 'engine/extensions')
for p in _paths:
	if p not in sys.path:
		sys.path.append(os.path.sep.join(p.split('/')))

import fife
__all__ = []
__all__.append('unittest')
__all__.append('fife')
__all__.append('genpath')
