# Microsoft Developer Studio Generated NMAKE File, Based on think.dsp
!IF "$(CFG)" == ""
CFG=think - Win32 Debug
!MESSAGE No configuration specified. Defaulting to think - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "think - Win32 Release" && "$(CFG)" != "think - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "think.mak" CFG="think - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "think - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "think - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "think - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\think.lib" ".\think"


CLEAN :
	-@erase "$(INTDIR)\think_cfg.obj"
	-@erase "$(INTDIR)\think_conf.obj"
	-@erase "$(INTDIR)\think_db.obj"
	-@erase "$(INTDIR)\think_error.obj"
	-@erase "$(INTDIR)\think_file.obj"
	-@erase "$(INTDIR)\think_gl.obj"
	-@erase "$(INTDIR)\think_log.obj"
	-@erase "$(INTDIR)\think_mutex.obj"
	-@erase "$(INTDIR)\think_net.obj"
	-@erase "$(INTDIR)\think_netcenter.obj"
	-@erase "$(INTDIR)\think_os.obj"
	-@erase "$(INTDIR)\think_sem.obj"
	-@erase "$(INTDIR)\think_shm.obj"
	-@erase "$(INTDIR)\think_shmq.obj"
	-@erase "$(INTDIR)\think_thread.obj"
	-@erase "$(INTDIR)\think_threadmutex.obj"
	-@erase "$(INTDIR)\think_timer.obj"
	-@erase "$(INTDIR)\think_timercenter.obj"
	-@erase "$(INTDIR)\think_utility.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\think.lib"
	-@erase "think"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "__THINK_UNIX2WINDOWS__" /Fp"$(INTDIR)\think.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\think.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\think.lib" 
LIB32_OBJS= \
	"$(INTDIR)\think_cfg.obj" \
	"$(INTDIR)\think_conf.obj" \
	"$(INTDIR)\think_db.obj" \
	"$(INTDIR)\think_error.obj" \
	"$(INTDIR)\think_file.obj" \
	"$(INTDIR)\think_gl.obj" \
	"$(INTDIR)\think_log.obj" \
	"$(INTDIR)\think_mutex.obj" \
	"$(INTDIR)\think_net.obj" \
	"$(INTDIR)\think_netcenter.obj" \
	"$(INTDIR)\think_os.obj" \
	"$(INTDIR)\think_sem.obj" \
	"$(INTDIR)\think_shm.obj" \
	"$(INTDIR)\think_shmq.obj" \
	"$(INTDIR)\think_thread.obj" \
	"$(INTDIR)\think_threadmutex.obj" \
	"$(INTDIR)\think_timer.obj" \
	"$(INTDIR)\think_timercenter.obj" \
	"$(INTDIR)\think_utility.obj"

"$(OUTDIR)\think.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

TargetName=think
InputPath=.\Release\think.lib
SOURCE="$(InputPath)"

".\think" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy /Y Release\think.lib ..\..\..\lib\think.lib 
	copy /Y ..\think.h ..\..\..\include\think.h 
<< 
	

!ELSEIF  "$(CFG)" == "think - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\think.lib" ".\think"


CLEAN :
	-@erase "$(INTDIR)\think_cfg.obj"
	-@erase "$(INTDIR)\think_conf.obj"
	-@erase "$(INTDIR)\think_db.obj"
	-@erase "$(INTDIR)\think_error.obj"
	-@erase "$(INTDIR)\think_file.obj"
	-@erase "$(INTDIR)\think_gl.obj"
	-@erase "$(INTDIR)\think_log.obj"
	-@erase "$(INTDIR)\think_mutex.obj"
	-@erase "$(INTDIR)\think_net.obj"
	-@erase "$(INTDIR)\think_netcenter.obj"
	-@erase "$(INTDIR)\think_os.obj"
	-@erase "$(INTDIR)\think_sem.obj"
	-@erase "$(INTDIR)\think_shm.obj"
	-@erase "$(INTDIR)\think_shmq.obj"
	-@erase "$(INTDIR)\think_thread.obj"
	-@erase "$(INTDIR)\think_threadmutex.obj"
	-@erase "$(INTDIR)\think_timer.obj"
	-@erase "$(INTDIR)\think_timercenter.obj"
	-@erase "$(INTDIR)\think_utility.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\think.lib"
	-@erase "think"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "__THINK_UNIX2WINDOWS__" /Fp"$(INTDIR)\think.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\think.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\think.lib" 
LIB32_OBJS= \
	"$(INTDIR)\think_cfg.obj" \
	"$(INTDIR)\think_conf.obj" \
	"$(INTDIR)\think_db.obj" \
	"$(INTDIR)\think_error.obj" \
	"$(INTDIR)\think_file.obj" \
	"$(INTDIR)\think_gl.obj" \
	"$(INTDIR)\think_log.obj" \
	"$(INTDIR)\think_mutex.obj" \
	"$(INTDIR)\think_net.obj" \
	"$(INTDIR)\think_netcenter.obj" \
	"$(INTDIR)\think_os.obj" \
	"$(INTDIR)\think_sem.obj" \
	"$(INTDIR)\think_shm.obj" \
	"$(INTDIR)\think_shmq.obj" \
	"$(INTDIR)\think_thread.obj" \
	"$(INTDIR)\think_threadmutex.obj" \
	"$(INTDIR)\think_timer.obj" \
	"$(INTDIR)\think_timercenter.obj" \
	"$(INTDIR)\think_utility.obj"

"$(OUTDIR)\think.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

TargetName=think
InputPath=.\Debug\think.lib
SOURCE="$(InputPath)"

".\think" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy /Y Debug\think.lib ..\..\..\lib\think.lib 
	copy /Y ..\think.h ..\..\..\include\think.h 
<< 
	

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("think.dep")
!INCLUDE "think.dep"
!ELSE 
!MESSAGE Warning: cannot find "think.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "think - Win32 Release" || "$(CFG)" == "think - Win32 Debug"
SOURCE=..\think_cfg.c

"$(INTDIR)\think_cfg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\think_conf.c

"$(INTDIR)\think_conf.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\think_db.c

"$(INTDIR)\think_db.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\think_error.c

"$(INTDIR)\think_error.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\think_file.c

"$(INTDIR)\think_file.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\think_gl.c

"$(INTDIR)\think_gl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\think_log.c

"$(INTDIR)\think_log.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\think_mutex.c

"$(INTDIR)\think_mutex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\think_net.c

"$(INTDIR)\think_net.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\think_netcenter.c

"$(INTDIR)\think_netcenter.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\think_os.c

"$(INTDIR)\think_os.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\think_sem.c

"$(INTDIR)\think_sem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\think_shm.c

"$(INTDIR)\think_shm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\think_shmq.c

"$(INTDIR)\think_shmq.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\think_thread.c

"$(INTDIR)\think_thread.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\think_threadmutex.c

"$(INTDIR)\think_threadmutex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\think_timer.c

"$(INTDIR)\think_timer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\think_timercenter.c

"$(INTDIR)\think_timercenter.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\think_utility.c

"$(INTDIR)\think_utility.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

