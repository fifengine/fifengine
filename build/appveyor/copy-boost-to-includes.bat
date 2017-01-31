@echo off

echo "=== Copy the pre-installed Boost to the includes folder"

:: create folders
mkdir C:\projects\fifengine-dependencies\includes\lib\boost
mkdir C:\projects\fifengine-dependencies\includes\include\boost

:: copy boost includes
xcopy /Y /S "C:\Libraries\boost_1_63_0\boost" "C:\projects\fifengine-dependencies\includes\include\boost" >nul 2>&1

:: copy binaries (dlls) and libs
copy "C:\Libraries\boost_1_63_0\lib32-msvc-14.0\boost_filesystem-vc140-mt-1_63.dll" "C:\projects\fifengine-dependencies\includes\bin\boost_filesystem-vc140-mt-1_63.dll"
copy "C:\Libraries\boost_1_63_0\lib32-msvc-14.0\boost_system-vc140-mt-1_63.dll" "C:\projects\fifengine-dependencies\includes\bin\boost_system-vc140-mt-1_63.dll"
copy "C:\Libraries\boost_1_63_0\lib32-msvc-14.0\boost_regex-vc140-mt-1_63.dll" "C:\projects\fifengine-dependencies\includes\bin\boost_regex-vc140-mt-1_63.dll"
copy "C:\Libraries\boost_1_63_0\lib32-msvc-14.0\libboost_filesystem-vc140-mt-1_63.lib" "C:\projects\fifengine-dependencies\includes\lib\boost\libboost_filesystem-vc140-mt-1_63.lib"
copy "C:\Libraries\boost_1_63_0\lib32-msvc-14.0\libboost_system-vc140-mt-1_63.lib" "C:\projects\fifengine-dependencies\includes\lib\boost\libboost_system-vc140-mt-1_63.lib"
copy "C:\Libraries\boost_1_63_0\lib32-msvc-14.0\libboost_regex-vc140-mt-1_63.lib" "C:\projects\fifengine-dependencies\includes\lib\boost\libboost_regex-vc140-mt-1_63.lib"

:: remove the "share" folder
rmdir /S /Q C:\projects\fifengine-dependencies\includes\share

echo "Done."
