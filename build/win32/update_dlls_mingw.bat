ren .\binaries\mingw\zlib.dll zlib1.dll

del .\..\..\*.dll
del .\..\..\clients\editor\*.dll
del .\..\..\clients\fo_remake\*.dll
del .\..\..\clients\island_demo\*.dll
del .\..\..\clients\pychan_demo\*.dll

copy .\binaries\mingw\*.dll .\..\..\*.*
copy .\binaries\mingw\*.dll .\..\..\clients\editor\*.*
copy .\binaries\mingw\*.dll .\..\..\clients\fo_remake\*.*
copy .\binaries\mingw\*.dll .\..\..\clients\island_demo\*.*
copy .\binaries\mingw\*.dll .\..\..\clients\pychan_demo\*.*
