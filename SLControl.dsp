# Microsoft Developer Studio Project File - Name="SLControl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SLControl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SLControl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SLControl.mak" CFG="SLControl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SLControl - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SLControl - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SLControl - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "DAPIO32" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 dapio32.lib /nologo /subsystem:windows /machine:I386 /libpath:"c:\Program Files\Microstar Laboratories\DapDev\DLL\LIB\MC"
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ELSEIF  "$(CFG)" == "SLControl - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "c:\Program Files\Microstar Laboratories\DapDev\DLL\Lib\MC" /D "DAPIO32" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 dapio32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"c:\Program Files\Microstar Laboratories\DapDev\DLL\LIB\MC"

!ENDIF 

# Begin Target

# Name "SLControl - Win32 Release"
# Name "SLControl - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Abort_multiple_trials.cpp
# End Source File
# Begin Source File

SOURCE=.\Analysis_display.cpp
# End Source File
# Begin Source File

SOURCE=.\Batch_analysis.cpp
# End Source File
# Begin Source File

SOURCE=.\Cal_display.cpp
# End Source File
# Begin Source File

SOURCE=.\Calibration.cpp
# End Source File
# Begin Source File

SOURCE=.\Chirp.cpp
# End Source File
# Begin Source File

SOURCE=.\Cyclical.cpp
# End Source File
# Begin Source File

SOURCE=.\Data_record.cpp
# End Source File
# Begin Source File

SOURCE=.\Experiment_display.cpp
# End Source File
# Begin Source File

SOURCE=.\File_header.cpp
# End Source File
# Begin Source File

SOURCE=.\Fit_control.cpp
# End Source File
# Begin Source File

SOURCE=.\Fit_parameters.cpp
# End Source File
# Begin Source File

SOURCE=.\global_functions.cpp
# End Source File
# Begin Source File

SOURCE=.\NR.cpp
# End Source File
# Begin Source File

SOURCE=.\Plot.cpp
# End Source File
# Begin Source File

SOURCE=.\Scan_channels.cpp
# End Source File
# Begin Source File

SOURCE=.\SL_SREC.cpp
# End Source File
# Begin Source File

SOURCE=.\SLControl.cpp
# End Source File
# Begin Source File

SOURCE=.\SLControl.rc
# End Source File
# Begin Source File

SOURCE=.\SLControlDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Square_wave.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Superposition.cpp
# End Source File
# Begin Source File

SOURCE=.\Superposition_control.cpp
# End Source File
# Begin Source File

SOURCE=.\Tension_control_dialog.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Abort_multiple_trials.h
# End Source File
# Begin Source File

SOURCE=.\Analysis_display.h
# End Source File
# Begin Source File

SOURCE=.\Batch_analysis.h
# End Source File
# Begin Source File

SOURCE=.\Cal_display.h
# End Source File
# Begin Source File

SOURCE=.\Calibration.h
# End Source File
# Begin Source File

SOURCE=.\Chirp.h
# End Source File
# Begin Source File

SOURCE=.\Cyclical.h
# End Source File
# Begin Source File

SOURCE=.\data_record.h
# End Source File
# Begin Source File

SOURCE=.\Experiment_display.h
# End Source File
# Begin Source File

SOURCE=.\File_header.h
# End Source File
# Begin Source File

SOURCE=.\Fit_control.h
# End Source File
# Begin Source File

SOURCE=.\Fit_parameters.h
# End Source File
# Begin Source File

SOURCE=.\global_definitions.h
# End Source File
# Begin Source File

SOURCE=.\global_functions.h
# End Source File
# Begin Source File

SOURCE=.\NR.h
# End Source File
# Begin Source File

SOURCE=.\Plot.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Scan_channels.h
# End Source File
# Begin Source File

SOURCE=.\SL_SREC.h
# End Source File
# Begin Source File

SOURCE=.\SLControl.h
# End Source File
# Begin Source File

SOURCE=.\SLControlDlg.h
# End Source File
# Begin Source File

SOURCE=.\Square_wave.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Superposition.h
# End Source File
# Begin Source File

SOURCE=.\Superposition_control.h
# End Source File
# Begin Source File

SOURCE=.\Tension_control_dialog.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\methods_256colors.bmp
# End Source File
# Begin Source File

SOURCE=.\res\SLControl.ico
# End Source File
# Begin Source File

SOURCE=.\res\SLControl.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
