del .\..\..\*.dll
del .\..\..\clients\editor\*.dll
del .\..\..\clients\pychan_demo\*.dll
del .\..\..\clients\rio_de_hola\*.dll

del .\..\..\engine\swigwrappers\python\*.dll
copy .\binaries\mingw\*.dll .\..\..\engine\swigwrappers\python\*.*
