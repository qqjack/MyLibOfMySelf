# Microsoft Developer Studio Project File - Name="MyLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=MyLib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MyLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MyLib.mak" CFG="MyLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MyLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "MyLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MyLib - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "MyLib - Win32 Debug"

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
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT CPP /X /YX
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "MyLib - Win32 Release"
# Name "MyLib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\MyLibSrc\common.cpp
# End Source File
# Begin Source File

SOURCE=.\MyLibSrc\Http.cpp
# End Source File
# Begin Source File

SOURCE=.\MyLibSrc\MyArrayList.cpp
# End Source File
# Begin Source File

SOURCE=.\MyLibSrc\MyAsyncHttp.cpp
# End Source File
# Begin Source File

SOURCE=.\MyLibSrc\MyDebug.cpp
# End Source File
# Begin Source File

SOURCE=.\MyLibSrc\MyFile.cpp
# End Source File
# Begin Source File

SOURCE=.\MyLibSrc\MyHashMap.cpp
# End Source File
# Begin Source File

SOURCE=.\MyLibSrc\MyJsonObj.cpp
# End Source File
# Begin Source File

SOURCE=.\MyLibSrc\MyRegex.cpp
# End Source File
# Begin Source File

SOURCE=.\MyLibSrc\MyRWLock.cpp
# End Source File
# Begin Source File

SOURCE=.\MyLibSrc\MyString.cpp
# End Source File
# Begin Source File

SOURCE=.\MyLibSrc\MyTaskThread.cpp
# End Source File
# Begin Source File

SOURCE=.\MyLibSrc\MyTempFile.cpp
# End Source File
# Begin Source File

SOURCE=.\MyLibSrc\MyThread.cpp
# End Source File
# Begin Source File

SOURCE=.\MyLibSrc\MyThreadPool.cpp
# End Source File
# Begin Source File

SOURCE=.\MyLibSrc\MyTime.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\MyLibSrc\common.h
# End Source File
# Begin Source File

SOURCE=.\MyLibSrc\Http.h
# End Source File
# Begin Source File

SOURCE=.\MyLibSrc\MyArrayList.h
# End Source File
# Begin Source File

SOURCE=.\MyLibSrc\MyAsyncHttp.h
# End Source File
# Begin Source File

SOURCE=.\MyBuffer.h
# End Source File
# Begin Source File

SOURCE=.\MyLibSrc\MyDebug.h
# End Source File
# Begin Source File

SOURCE=.\MyLibSrc\MyFile.h
# End Source File
# Begin Source File

SOURCE=.\MyLibSrc\MyHashMap.h
# End Source File
# Begin Source File

SOURCE=.\MyIOStream.h
# End Source File
# Begin Source File

SOURCE=.\MyLibSrc\MyJsonObj.h
# End Source File
# Begin Source File

SOURCE=.\MyMsg.h
# End Source File
# Begin Source File

SOURCE=.\MyLibSrc\MyRegex.h
# End Source File
# Begin Source File

SOURCE=.\MyLibSrc\MyRWLock.h
# End Source File
# Begin Source File

SOURCE=.\MyLibSrc\MyString.h
# End Source File
# Begin Source File

SOURCE=.\MyLibSrc\MyTaskThread.h
# End Source File
# Begin Source File

SOURCE=.\MyLibSrc\MyTempFile.h
# End Source File
# Begin Source File

SOURCE=.\MyLibSrc\MyThread.h
# End Source File
# Begin Source File

SOURCE=.\MyLibSrc\MyThreadPool.h
# End Source File
# Begin Source File

SOURCE=.\MyLibSrc\MyTime.h
# End Source File
# End Group
# End Target
# End Project
