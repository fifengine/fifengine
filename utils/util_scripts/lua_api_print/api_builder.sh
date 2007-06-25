#!/bin/bash

ENGINE_DIR=../../../
FIFE_ENGINE=./fife_engine
FIFE_SCRIPT=content/scripts/demos/dump_state_functions.lua

AWK_BUILD_HELPER=build_api.awk

# phase 1

_pwd=$PWD
cd $ENGINE_DIR
$FIFE_ENGINE $FIFE_SCRIPT >/tmp/fife_api_dump 2>/dev/null
cd $_pwd

function get_api_dump() {
sed -e '1,/DUMP_START_HERE/d' -e '/DUMP_END_HERE/,$d' /tmp/fife_api_dump \
  | awk -f $AWK_BUILD_HELPER
}

get_api_dump | grep -e ' ' >/tmp/fife_tables
get_api_dump | grep -v ' ' >/tmp/fife_globals

# phase 2

lua print_docs.lua /tmp/fife_globals /tmp/fife_tables
