# Microsoft Developer Studio Generated NMAKE File, Based on shmqstat.dsp
!IF "$(CFG)" == ""
CFG=shmqstat - Win32 Debug
!MESSAGE No configuration specified. Defaulting to shmqstat - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "shmqstat - Win32 Release" && "$(CFG)" != "shmqstat - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "shmqstat.mak" CFG="shmqstat - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "shmqstat - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "shmqstat - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "shmqstat - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\shmqstat.exe" ".\shmqstat"


CLEAN :
	-@erase "$(INTDIR)\shmqstat.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\shmqstat.exe"
	-@erase "shmqstat"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "../../../include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "__THINK_UNIX2WINDOWS__" /Fp"$(INTDIR)\shmqstat.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\shmqstat.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=think.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\shmqstat.pdb" /machine:I386 /out:"$(OUTDIR)\shmqstat.exe" /libpath:"../../../lib" 
LINK32_OBJS= \
	"$(INTDIR)\shmqstat.obj"

"$(OUTDIR)\shmqstat.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetName=shmqstat
InputPath=.\Release\shmqstat.exe
SOURCE="$(InputPath)"

".\shmqstat" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy /Y Release\shmqstat.exe ..\..\..\bin\shmqstat.exe
<< 
	

!ELSEIF  "$(CFG)" == "shmqstat - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\shmqstat.exe" ".\shmqstat"


CLEAN :
	-@erase "$(INTDIR)\shmqstat.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\shmqstat.exe"
	-@erase "$(OUTDIR)\shmqstat.ilk"
	-@erase "$(OUTDIR)\shmqstat.pdb"
	-@erase "shmqstat"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "../../../include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "__THINK_UNIX2WINDOWS__" /Fp"$(INTDIR)\shmqstat.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\shmqstat.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=think.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\shmqstat.pdb" /debug /machine:I386 /out:"$(OUTDIR)\shmqstat.exe" /pdbtype:sept /libpath:"../../../lib" 
LINK32_OBJS= \
	"$(INTDIR)\shmqstat.obj"

"$(OUTDIR)\shmqstat.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetName=shmqstat
InputPath=.\Debug\shmqstat.exe
SOURCE="$(InputPath)"

".\shmqstat" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy /Y Debug\shmqstat.exe ..\..\..\bin\shmqstat.exe
<< 
	

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("shmqstat.dep")
!INCLUDE "shmqstat.dep"
!ELSE 
!MESSAGE Warning: cannot find "shmqstat.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "shmqstat - Win32 Release" || "$(CFG)" == "shmqstat - Win32 Debug"
SOURCE=..\shmqstat.c

"$(INTDIR)\shmqstat.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

