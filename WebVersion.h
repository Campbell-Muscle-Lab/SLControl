// WebVersion.h
// Created: 4/21/2003  (rk)
// Last modified: 4/21/2003  (rk)
// CWebVersion header file
// Based on posting by Paul DiLascia in April 2003 MSDN magazine
//
// MSDN Magazine -- April 2003
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual Studio .NET on Windows XP. Tab size=3.
//
#pragma once

//////////////////
// This class encapsulates over-the-web version checking. It expects a
// text version file that contains 4 number separated by commas, the
// same format for FILEVERSION and PRODUCTVERSION in VS_VERSION_INFO.
// ReadVersion reads these numbers into dwVersionMS and dwVersionLS.
//
class CWebVersion {
protected:
	enum { BUFSIZE = 64 };
	LPCTSTR m_lpServer;						 // server name
	DWORD	  m_dwError;						 // most recent error code
	TCHAR   m_errInfo[256];					 // extended error info
	char	  m_version[BUFSIZ];				 // version number as text
	void    SaveErrorInfo();				 // helper to save error info

public:
	DWORD dwVersionMS;		// version number: most-sig 32 bits
	DWORD dwVersionLS;		// version number: least-sig 32 bits
	int iMajor;             // Major Version number
	int iMinor;             // Minor Version number
	int iRevLevel;          // Revision Level

	// Note: Assume that complete version is of the form:
	//   iMajor.iMinor.iRevLevel.iRevMinor

	CWebVersion(LPCTSTR server) : m_lpServer(server) { }
	~CWebVersion()	{ }

	static  BOOL Online();
	BOOL	  ReadVersion(LPCTSTR lpFileName);
	LPCSTR  GetVersionText()		 { return m_version; }
	DWORD   GetError()				 { return m_dwError; }
	LPCTSTR GetExtendedErrorInfo() { return m_errInfo; }
};

