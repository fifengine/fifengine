#!/usr/bin/env python
# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2017 by the FIFE team
#  http://www.fifengine.net
#  This file is part of FIFE.
#
#  FIFE is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the
#  Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
# ####################################################################


from distutils.core import setup, Command
import os, sys


class SetMetadataCommand(Command):
    description = 'set metadata to be used by the following commands'
    user_options = [
        ('version=', None, 'set the package version'),
    ]
    def initialize_options(self):
        self.version = None
    def finalize_options(self):
        return
    def run(self):
        print("setting package version to {}".format(self.version))
        self.distribution.metadata.version = self.version


if sys.platform == 'win32':
	pkg_data = {'fife': ['*.py','*.pyd','*.dll'] }
else:
	pkg_data = {'fife': ['*.so'] }


setup(name='libfife',
      version='0.4.0',
      description='Flexible Isometric Free Engine',
      url='www.fifengine.net',
      packages = ['fife', 'fife.extensions', 'fife.extensions.pychan', 'fife.extensions.librocket', 'fife.extensions.cegui', 'fife.extensions.pychan.widgets', 'fife.extensions.pychan.dialog', 'fife.extensions.pychan.widgets.ext', 'fife.extensions.serializers' ],
      package_dir = { '': os.path.join('engine','python') },
      package_data = pkg_data,
      data_files = [(os.path.join('lib','site-packages', 'fife'),['AUTHORS','CHANGELOG.md', 'LICENSE.md' ,'README.md'])],
      license = 'GNU Lesser General Public License, version 2.1',
      cmdclass = { 'set_metadata': SetMetadataCommand }
      )

