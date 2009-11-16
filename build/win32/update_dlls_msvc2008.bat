del .\..\..\*.dll
del .\..\..\clients\editor\*.dll
del .\..\..\clients\rio_de_hola\*.dll
del .\..\..\clients\pychan_demo\*.dll

del .\..\..\engine\swigwrappers\python\*.dll
copy .\binaries\msvc2008\*.dll .\..\..\engine\swigwrappers\python\*.*
