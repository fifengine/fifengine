#!/bin/sh
# replaces the ugly make target with a nearly as ugly scons builder
# (and this helper)

cat <<EOF > $2
#include <iostream>
void usage(const char* name) {

  std::cout << "USAGE: " << name;
EOF
awk -f reformat.awk $1 >> $2
echo '  }' >> $2

