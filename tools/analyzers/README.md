```
__/\\\\\\\\\\\\\\\___/\\\\\\\\\\\___/\\\\\\\\\\\\\\\___/\\\\\\\\\\\\\\\_        
 _\/\\\///////////___\/////\\\///___\/\\\///////////___\/\\\///////////__       
  _\/\\\__________________\/\\\______\/\\\______________\/\\\_____________      
   _\/\\\\\\\\\\\__________\/\\\______\/\\\\\\\\\\\______\/\\\\\\\\\\\_____     
    _\/\\\///////___________\/\\\______\/\\\///////_______\/\\\///////______    
     _\/\\\__________________\/\\\______\/\\\______________\/\\\_____________   
      _\/\\\__________________\/\\\______\/\\\______________\/\\\_____________  
       _\/\\\_______________/\\\\\\\\\\\__\/\\\______________\/\\\\\\\\\\\\\\\_ 
        _\///_______________\///////////___\///_______________\///////////////__
```
FIFE Analyzers
==============

We don't have a lot of these yet but this is where all our analyzers go.


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

