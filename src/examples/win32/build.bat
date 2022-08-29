del /F /S /Q Release
nmake /f threadtest.mak CFG="threadtest - Win32 Release"
del /F /S /Q Release
nmake /f gltest.mak CFG="gltest - Win32 Release"

pause
