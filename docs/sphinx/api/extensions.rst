Python Extensions
================

The FIFE includes pure Python extensions for higher-level functionality.

.. note::

   Some extensions depend on the SWIG-generated fifechan module and require
   the C++ engine to be built for full documentation.

Extensions Overview
-------------------

.. automodule:: fife.extensions
   :members:
   :undoc-members:
   :show-inheritance:

Utilities
---------

These extensions are Python-only and don't require the compiled C++ engine:

.. automodule:: fife.extensions.fifelog
   :members:
   :undoc-members:
   :show-inheritance:

.. automodule:: fife.extensions.fife_timer
   :members:
   :undoc-members:
   :show-inheritance:

.. automodule:: fife.extensions.fife_utils
   :members:
   :undoc-members:
   :show-inheritance:

.. automodule:: fife.extensions.pythonize
   :members:
   :undoc-members:
   :show-inheritance:

Serializers
-----------

.. automodule:: fife.extensions.serializers
   :members:
   :undoc-members:
   :show-inheritance:

.. automodule:: fife.extensions.serializers.xmlobject
   :members:
   :undoc-members:
   :show-inheritance:

.. automodule:: fife.extensions.serializers.simplexml
   :members:
   :undoc-members:
   :show-inheritance:

Savers
------

.. automodule:: fife.extensions.savers
   :members:
   :undoc-members:
   :show-inheritance:

Loaders
-------

.. automodule:: fife.extensions.loaders
   :members:
   :undoc-members:
   :show-inheritance:

Extensions (Require C++ Engine)
-------------------------------

The following extensions require the compiled C++ engine for documentation:

* ``fife.extensions.basicapplication`` - Basic application framework
* ``fife.extensions.pychan`` - PyChan GUI toolkit
* ``fife.extensions.pychan.widgets`` - PyChan widgets
* ``fife.extensions.fife_settings`` - Settings management

See the C++ API documentation for the underlying fifechan module.
