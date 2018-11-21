// SLControl.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SLControl.h"
#include "SLControlDlg.h"

////////////////////////////
// Ken's code starts here //
////////////////////////////

#include "global_definitions.h"

extern CString COMMAND_LINE_DATA_FILE_STRING;
extern int NO_OF_COMMAND_LINE_DATA_FILES;

////////////////////////////
//  Ken's code ends here  //
////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSLControlApp

BEGIN_MESSAGE_MAP(CSLControlApp, CWinApp)
	//{{AFX_MSG_MAP(CSLControlApp)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSLControlApp construction

CSLControlApp::CSLControlApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSLControlApp object

CSLControlApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSLControlApp initialization

BOOL CSLControlApp::InitInstance()
{
/*
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
*/
	

	CSLControlDlg dlg;
	m_pMainWnd = &dlg;

	////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	// Checks to see whether SLControl has been called with a data file specified on the
	// command line, copies the file name to COMMAND_LINE_DATA_FILE_STRING and sets
	// NO_OF_COMMAND_LINE_DATA_FILES appropriately

	// Variables

	// Code 

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Check to see whether a command line parameter was specified, if not, end function

	// Copy file name to COMMAND_LINE_DATA_FILE_STRING and set NO_OF_COMMAND_LINE_DATA_FILES

	COMMAND_LINE_DATA_FILE_STRING=cmdInfo.m_strFileName;

	NO_OF_COMMAND_LINE_DATA_FILES=0;

	TRACE("Test");

	if (strlen(COMMAND_LINE_DATA_FILE_STRING)>1)
	{
		NO_OF_COMMAND_LINE_DATA_FILES=1;
	}

	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////

	// Start window

	dlg.DoModal();

/*
	
	
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
*/
	return FALSE;
	
}

