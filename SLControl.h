// SLControl.h : main header file for the SLCONTROL application
//

#if !defined(AFX_SLCONTROL_H__A1090D7A_AEA9_4DEC_BF6C_7FB2D52604BB__INCLUDED_)
#define AFX_SLCONTROL_H__A1090D7A_AEA9_4DEC_BF6C_7FB2D52604BB__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSLControlApp:
// See SLControl.cpp for the implementation of this class
//

class CSLControlApp : public CWinApp
{
public:
	CSLControlApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSLControlApp)
	public:
	
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSLControlApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SLCONTROL_H__A1090D7A_AEA9_4DEC_BF6C_7FB2D52604BB__INCLUDED_)
