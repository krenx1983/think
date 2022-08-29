# Microsoft Developer Studio Generated NMAKE File, Based on dbfcreate.dsp
!IF "$(CFG)" == ""
CFG=dbfcreate - Win32 Debug
!MESSAGE No configuration specified. Defaulting to dbfcreate - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "dbfcreate - Win32 Release" && "$(CFG)" != "dbfcreate - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dbfcreate.mak" CFG="dbfcreate - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dbfcreate - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "dbfcreate - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "dbfcreate - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\dbfcreate.exe" ".\dbfcreate"


CLEAN :
	-@erase "$(INTDIR)\dbfcreate.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\dbfcreate.exe"
	-@erase "dbfcreate"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "../../../include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "__THINK_UNIX2WINDOWS__" /Fp"$(INTDIR)\dbfcreate.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dbfcreate.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=think_dbf.lib think.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\dbfcreate.pdb" /machine:I386 /out:"$(OUTDIR)\dbfcreate.exe" /libpath:"../../../lib" 
LINK32_OBJS= \
	"$(INTDIR)\dbfcreate.obj"

"$(OUTDIR)\dbfcreate.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetName=dbfcreate
InputPath=.\Release\dbfcreate.exe
SOURCE="$(InputPath)"

".\dbfcreate" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy /Y Release\dbfcreate.exe ..\..\..\bin\dbfcreate.exe
<< 
	

!ELSEIF  "$(CFG)" == "dbfcreate - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\dbfcreate.exe" ".\dbfcreate"


CLEAN :
	-@erase "$(INTDIR)\dbfcreate.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\dbfcreate.exe"
	-@erase "$(OUTDIR)\dbfcreate.ilk"
	-@erase "$(OUTDIR)\dbfcreate.pdb"
	-@erase "dbfcreate"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "../../../include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "__THINK_UNIX2WINDOWS__" /Fp"$(INTDIR)\dbfcreate.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dbfcreate.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=think_dbf.lib think.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\dbfcreate.pdb" /debug /machine:I386 /out:"$(OUTDIR)\dbfcreate.exe" /pdbtype:sept /libpath:"../../../lib" 
LINK32_OBJS= \
	"$(INTDIR)\dbfcreate.obj"

"$(OUTDIR)\dbfcreate.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetName=dbfcreate
InputPath=.\Debug\dbfcreate.exe
SOURCE="$(InputPath)"

".\dbfcreate" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy /Y Debug\dbfcreate.exe ..\..\..\bin\dbfcreate.exe
<< 
	

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("dbfcreate.dep")
!INCLUDE "dbfcreate.dep"
!ELSE 
!MESSAGE Warning: cannot find "dbfcreate.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "dbfcreate - Win32 Release" || "$(CFG)" == "dbfcreate - Win32 Debug"
SOURCE=..\dbfcreate.c

"$(INTDIR)\dbfcreate.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

