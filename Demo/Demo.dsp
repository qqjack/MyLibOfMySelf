# Microsoft Developer Studio Project File - Name="Demo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Demo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Demo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Demo.mak" CFG="Demo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Demo - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Demo - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Demo - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "Demo - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /force /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Demo - Win32 Release"
# Name "Demo - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\MyLibSrc\Mail\Attachment.cpp
# End Source File
# Begin Source File

SOURCE=..\MyLibSrc\Charset.cpp
# End Source File
# Begin Source File

SOURCE=..\MyLibSrc\common.cpp
# End Source File
# Begin Source File

SOURCE=..\MyLibSrc\Mail\DataRecord.cpp
# End Source File
# Begin Source File

SOURCE=.\Demo.cpp
# End Source File
# Begin Source File

SOURCE=..\MyLibSrc\Encrypt.cpp
# End Source File
# Begin Source File

SOURCE=..\MyLibSrc\Http.cpp
# End Source File
# Begin Source File

SOURCE=..\MyLibSrc\Mail\MailRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\MyLibSrc\MyArrayList.cpp
# End Source File
# Begin Source File

SOURCE=..\MyLibSrc\MyAsyncHttp.cpp
# End Source File
# Begin Source File

SOURCE=..\MyLibSrc\MyBuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\MyLibSrc\MyDebug.cpp
# End Source File
# Begin Source File

SOURCE=..\MyLibSrc\MyFile.cpp
# End Source File
# Begin Source File

SOURCE=..\MyLibSrc\MyHashMap.cpp
# End Source File
# Begin Source File

SOURCE=..\MyLibSrc\MyIOStream.cpp
# End Source File
# Begin Source File

SOURCE=..\MyLibSrc\MyJsonObj.cpp
# End Source File
# Begin Source File

SOURCE=..\MyLibSrc\MyMsg.cpp
# End Source File
# Begin Source File

SOURCE=..\MyLibSrc\MyPOP3.cpp
# End Source File
# Begin Source File

SOURCE=..\MyLibSrc\MyRegex.cpp
# End Source File
# Begin Source File

SOURCE=..\MyLibSrc\MyRWLock.cpp
# End Source File
# Begin Source File

SOURCE=..\MyLibSrc\MySMTP.cpp
# End Source File
# Begin Source File

SOURCE=..\MyLibSrc\MyString.cpp
# End Source File
# Begin Source File

SOURCE=..\MyLibSrc\MyTaskThread.cpp
# End Source File
# Begin Source File

SOURCE=..\MyLibSrc\MyTempFile.cpp
# End Source File
# Begin Source File

SOURCE=..\MyLibSrc\MyThread.cpp
# End Source File
# Begin Source File

SOURCE=..\MyLibSrc\MyThreadPool.cpp
# End Source File
# Begin Source File

SOURCE=..\MyLibSrc\MyTime.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\MyLibSrc\MyIOStream.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
