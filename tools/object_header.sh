#!/bin/bash
# Adds an xml preprocessor directive to files named object.xml that indicates to fife that
# these are object files. Call with the root directory to convert under as an argument such
# as:
#      ./object_header my_client/objects

for file in `find $1 -name object.xml`; do
	echo "<?fife type=\"object\"?>" | cat - $file >> tmp.xml
	mv tmp.xml $file
done
