// WebVersion.cpp
// Created: 4/21/2003  (rk)
// Last modified: 4/21/2003  (rk)
// Implementation for CWebVersion
// Based on code posted by Paul DiLascia in April 2003 MSDN magazine
//
// MSDN Magazine -- April 2003
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual Studio .NET on Windows XP. Tab size=3.
//
#include "stdafx.h"
#include "WebVersion.h"
#include "InetHandle.h"

//////////////////
// Check if connected to Internet.
//
BOOL CWebVersion::Online()
{
    DWORD dwState = 0; 
    DWORD dwSize = sizeof(DWORD);

	 return InternetQueryOption(NULL,
		 INTERNET_OPTION_CONNECTED_STATE, &dwState, &dwSize)
		 && (dwState & INTERNET_STATE_CONNECTED);
}

BOOL CWebVersion::ReadVersion(LPCTSTR lpFileName)
// Created: ?  (pd)
// Last Modified: 4/21/2003  (rk)
// Read version number as string into buffer
// Also decipher and store in four integers iMajor, iMinor etc.
{
	ASSERT(lpFileName);
	CInternetHandle hInternet;
	CInternetHandle hFtpSession;
	CInternetHandle hFtpFile;

	m_version[0] = 0;
	m_dwError=0;							 // assume success
	m_errInfo[0]=0;							 // ..

	DWORD nRead=0;
	hInternet = InternetOpen(NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (hInternet!=NULL) 
	{
		hFtpSession = InternetConnect(hInternet, m_lpServer,
			INTERNET_DEFAULT_FTP_PORT, NULL, NULL, INTERNET_SERVICE_FTP, 0, NULL);

		if (hFtpSession!=NULL) 
		{
			hFtpFile = FtpOpenFile(hFtpSession, lpFileName,
				GENERIC_READ, FTP_TRANSFER_TYPE_ASCII, NULL);

			if (hFtpFile!=NULL) 
			{
				InternetReadFile(hFtpFile, m_version, BUFSIZE, &nRead);
				if (nRead>0) 
				{
					m_version[nRead] = 0;
					int Mhi,Mlo,mhi;
					sscanf(m_version, "%i,%i,%i", &Mhi, &Mlo, &mhi);
					dwVersionMS = MAKELONG(Mlo,Mhi);
					//dwVersionLS = MAKELONG(mlo,mhi);
					iMajor=Mhi;
					iMinor=Mlo;
					iRevLevel=mhi;
					
					return TRUE;
				}
			}
		}
	}

	// Failed: save error code and extended error info if any.
	m_dwError = GetLastError();
	if (m_dwError==ERROR_INTERNET_EXTENDED_ERROR) 
	{
		DWORD dwErr;
		DWORD len = sizeof(m_errInfo)/sizeof(m_errInfo[0]);
		InternetGetLastResponseInfo(&dwErr, m_errInfo, &len);
	}
	return FALSE;
}
