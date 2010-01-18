#!/usr/bin/env python

from distutils.core import setup
import os, sys

if sys.platform == 'win32':
	pkg_data = {'fife': ['*.pyd','*.dll'] }
else:
	pkg_data = {'fife': ['*.so'] }


setup(name='fife',
      version='0.3.0',
      description='Flexible Isometric Free Engine',
      url='www.fifengine.de',
      packages = ['fife', 'fife.extensions', 'fife.extensions.pychan', 'fife.extensions.pychan.widgets', 'fife.extensions.pychan.widgets.ext', 'fife.extensions.serializers' ],
      package_dir = { '': os.path.join('engine','python') },
      package_data = pkg_data,
      data_files = [(os.path.join('lib','site-packages', 'fife'),['AUTHORS','CHANGES', 'COPYING' ,'README'])],
      license = 'GNU Lesser General Public License, version 2.1'
      )

