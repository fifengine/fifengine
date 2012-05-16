@echo off

:: run scons
pushd ..\..\..\..\
python setup.py install > engine_install_log.txt 2>&1
popd
