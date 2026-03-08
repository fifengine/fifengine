#!/usr/bin/env python
# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

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
      version='0.4.2',
      description='Flexible Isometric Free Engine',
      url='www.fifengine.net',
      packages = ['fife', 'fife.extensions', 'fife.extensions.pychan', 'fife.extensions.cegui', 'fife.extensions.pychan.widgets', 'fife.extensions.pychan.dialog', 'fife.extensions.pychan.widgets.ext', 'fife.extensions.serializers' ],
      package_dir = { '': os.path.join('engine','python') },
      package_data = pkg_data,
      data_files = [(os.path.join('lib','site-packages', 'fife'),['AUTHORS','CHANGELOG.md', 'LICENSE.md' ,'README.md'])],
      license = 'GNU Lesser General Public License, version 2.1',
      cmdclass = { 'set_metadata': SetMetadataCommand }
      )
