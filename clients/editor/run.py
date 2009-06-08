#!/usr/bin/env python
# coding: utf-8
# This is the editor client for FIFE.

import sys, os, re

def _jp(path):
	return os.path.sep.join(path.split('/'))

_paths = ('.', '../../engine/swigwrappers/python', '../../engine/extensions')
for p in _paths:
	if p not in sys.path:
		sys.path.append(_jp(p))

import fife
import basicapplication
import scripts
from scripts.editor import Editor

if __name__ == '__main__':
	print sys.argv
	params = None
	if len(sys.argv) > 1:
		params = sys.argv[1]
	app = Editor(params)
	app.run()
