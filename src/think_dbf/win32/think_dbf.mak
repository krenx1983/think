# Microsoft Developer Studio Generated NMAKE File, Based on think_dbf.dsp
!IF "$(CFG)" == ""
CFG=think_dbf - Win32 Debug
!MESSAGE No configuration specified. Defaulting to think_dbf - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "think_dbf - Win32 Release" && "$(CFG)" != "think_dbf - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "think_dbf.mak" CFG="think_dbf - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "think_dbf - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "think_dbf - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "think_dbf - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\think_dbf.lib" ".\think_dbf"


CLEAN :
	-@erase "$(INTDIR)\think_dbf.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\think_dbf.lib"
	-@erase "think_dbf"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "../../../include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "__THINK_UNIX2WINDOWS__" /Fp"$(INTDIR)\think_dbf.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\think_dbf.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\think_dbf.lib" 
LIB32_OBJS= \
	"$(INTDIR)\think_dbf.obj"

"$(OUTDIR)\think_dbf.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

TargetName=think_dbf
InputPath=.\Release\think_dbf.lib
SOURCE="$(InputPath)"

".\think_dbf" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy /Y Release\think_dbf.lib ..\..\..\lib\think_dbf.lib 
	copy /Y ..\think_dbf.h ..\..\..\include\think_dbf.h 
<< 
	

!ELSEIF  "$(CFG)" == "think_dbf - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\think_dbf.lib" ".\think_dbf"


CLEAN :
	-@erase "$(INTDIR)\think_dbf.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\think_dbf.lib"
	-@erase "think_dbf"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "__THINK_UNIX2WINDOWS__" /Fp"$(INTDIR)\think_dbf.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\think_dbf.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\think_dbf.lib" 
LIB32_OBJS= \
	"$(INTDIR)\think_dbf.obj"

"$(OUTDIR)\think_dbf.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

TargetName=think_dbf
InputPath=.\Debug\think_dbf.lib
SOURCE="$(InputPath)"

".\think_dbf" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy /Y Debug\think_dbf.lib ..\..\..\lib\think_dbf.lib 
	copy /Y ..\think_dbf.h ..\..\..\include\think_dbf.h 
<< 
	

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("think_dbf.dep")
!INCLUDE "think_dbf.dep"
!ELSE 
!MESSAGE Warning: cannot find "think_dbf.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "think_dbf - Win32 Release" || "$(CFG)" == "think_dbf - Win32 Debug"
SOURCE=..\think_dbf.c

"$(INTDIR)\think_dbf.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

