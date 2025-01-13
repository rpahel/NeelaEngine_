set srcfile=src/%1.cpp
set headerfile=include/%1.h

echo: #include "%1.h" >> %srcfile%
echo: >> %srcfile%
::echo namespace AC >> %srcfile%
::echo ^{ >> %srcfile%
echo: >> %srcfile%
::echo ^} >> %srcfile%

echo #pragma once > %headerfile%
::echo #include ^<Neela^/Export.h^> >> %headerfile%
echo: >> %headerfile%
::echo namespace AC >> %headerfile%
::echo ^{ >> %headerfile%
echo: >> %headerfile%
::echo ^} >> %headerfile%