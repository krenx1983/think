# Microsoft Developer Studio Project File - Name="think" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=think - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "think.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "think - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "__THINK_UNIX2WINDOWS__" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build
TargetName=think
InputPath=.\Release\think.lib
SOURCE="$(InputPath)"

"$(TargetName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy /Y Release\think.lib ..\..\..\lib\think.lib 
	copy /Y ..\think.h ..\..\..\include\think.h 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "think - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "__THINK_UNIX2WINDOWS__" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build
TargetName=think
InputPath=.\Debug\think.lib
SOURCE="$(InputPath)"

"$(TargetName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy /Y Debug\think.lib ..\..\..\lib\think.lib 
	copy /Y ..\think.h ..\..\..\include\think.h 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "think - Win32 Release"
# Name "think - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\think_cfg.c
# End Source File
# Begin Source File

SOURCE=..\think_conf.c
# End Source File
# Begin Source File

SOURCE=..\think_db.c
# End Source File
# Begin Source File

SOURCE=..\think_error.c
# End Source File
# Begin Source File

SOURCE=..\think_file.c
# End Source File
# Begin Source File

SOURCE=..\think_gl.c
# End Source File
# Begin Source File

SOURCE=..\think_log.c
# End Source File
# Begin Source File

SOURCE=..\think_mutex.c
# End Source File
# Begin Source File

SOURCE=..\think_net.c
# End Source File
# Begin Source File

SOURCE=..\think_netcenter.c
# End Source File
# Begin Source File

SOURCE=..\think_os.c
# End Source File
# Begin Source File

SOURCE=..\think_sem.c
# End Source File
# Begin Source File

SOURCE=..\think_shm.c
# End Source File
# Begin Source File

SOURCE=..\think_shmq.c
# End Source File
# Begin Source File

SOURCE=..\think_thread.c
# End Source File
# Begin Source File

SOURCE=..\think_threadmutex.c
# End Source File
# Begin Source File

SOURCE=..\think_timer.c
# End Source File
# Begin Source File

SOURCE=..\think_timercenter.c
# End Source File
# Begin Source File

SOURCE=..\think_utility.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\think.h
# End Source File
# Begin Source File

SOURCE=..\think_cfg.h
# End Source File
# Begin Source File

SOURCE=..\think_conf.h
# End Source File
# Begin Source File

SOURCE=..\think_db.h
# End Source File
# Begin Source File

SOURCE=..\think_error.h
# End Source File
# Begin Source File

SOURCE=..\think_file.h
# End Source File
# Begin Source File

SOURCE=..\think_gl.h
# End Source File
# Begin Source File

SOURCE=..\think_log.h
# End Source File
# Begin Source File

SOURCE=..\think_mutex.h
# End Source File
# Begin Source File

SOURCE=..\think_net.h
# End Source File
# Begin Source File

SOURCE=..\think_netcenter.h
# End Source File
# Begin Source File

SOURCE=..\think_os.h
# End Source File
# Begin Source File

SOURCE=..\think_route.h
# End Source File
# Begin Source File

SOURCE=..\think_sem.h
# End Source File
# Begin Source File

SOURCE=..\think_shm.h
# End Source File
# Begin Source File

SOURCE=..\think_shmq.h
# End Source File
# Begin Source File

SOURCE=..\think_thread.h
# End Source File
# Begin Source File

SOURCE=..\think_threadmutex.h
# End Source File
# Begin Source File

SOURCE=..\think_timer.h
# End Source File
# Begin Source File

SOURCE=..\think_timercenter.h
# End Source File
# Begin Source File

SOURCE=..\think_utility.h
# End Source File
# End Group
# End Target
# End Project
