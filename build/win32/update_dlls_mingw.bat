del .\..\..\*.dll
del .\..\..\clients\editor\*.dll
del .\..\..\clients\pychan_demo\*.dll
del .\..\..\clients\rio_de_hola\*.dll

copy .\binaries\mingw\zlib.dll .\binaries\mingw\zlib1.dll
copy .\binaries\mingw\*.dll .\..\..\*.*
copy .\binaries\mingw\*.dll .\..\..\clients\editor\*.*
copy .\binaries\mingw\*.dll .\..\..\clients\pychan_demo\*.*
copy .\binaries\mingw\*.dll .\..\..\clients\rio_de_hola\*.*
