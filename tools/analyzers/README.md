FIFE Analyzers
==============

dep_analyzer.py
---------------

This analyzes our C++ source code and looks for any unexpected dependencies that
may have been introduced.  All dependencies are defined in 
`_allowed_dependencies.py`.  It will warn about all dependencies it finds that
are not defined in that file.  It also outputs some postscript and png files to
visually show dependencies.

### Executing

This requires graphviz to be installed and in your path.  Run from from FIFE's 
root directory `python tools/analyzers/dep_analyzer.py`.

All output goes to `<fife>/doc/dependencies`.

