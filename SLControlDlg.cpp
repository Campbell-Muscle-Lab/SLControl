// SLControlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SLControl.h"
#include "SLControlDlg.h"

////////////////////////////
// Ken's code starts here //
////////////////////////////

#include "dapio32.h"
#include "global_functions.h"
#include <math.h>
#include "Update_check.h"

#include <stdio.h>
#include "WebVersion.h"
#include "PathDialog.h"
#include ".\slcontroldlg.h"

extern float FL_POLARITY;
extern float FL_RESPONSE_CALIBRATION;
extern float FL_COMMAND_CALIBRATION;
extern float FORCE_CALIBRATION;
extern char CALIBRATION_DATE_STRING[100];
extern int FL_OR_SL_CHANNEL_MODE;
extern char SLCONTROL_VERSION_STRING[100];
extern int AUTOMATIC_UPDATE_CHECK;

extern int NO_OF_COMMAND_LINE_DATA_FILES;
extern CString COMMAND_LINE_DATA_FILE_STRING;

#define FTPSERVER "ftp6.physiology.wisc.edu"
#define VERFILE   "ver_slcontrol.txt"


////////////////////////////
//  Ken's code ends here  //
////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	
// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	HCURSOR m_hCursorHand;      // Handle to "Hand" cursor

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEmail();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	virtual BOOL OnInitDialog();
	afx_msg void OnStnClickedUrl();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	// Load the "Hand" cursor (need Win/2000 or later for this)
	m_hCursorHand=LoadCursor(NULL,IDC_HAND);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::OnEmail()
{
	// Executes a shell command to initiate an e-mail

	CString csLink;
	csLink="mailto:campbell@physiology.wisc.edu?subject=Re: SLControl";
    ::ShellExecute( m_hWnd, "open", csLink, "", "", SW_SHOW );
    return;
}

void CAboutDlg::OnStnClickedUrl()
{
    ::ShellExecute( m_hWnd, "open", "http://www.slcontrol.com", "", "", SW_SHOW );
    return;
}


HBRUSH CAboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	// Modified to draw links in blue

    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here

	if(nCtlColor == CTLCOLOR_DLG)
	{
		// Dialog background color
		hbr = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
		return hbr;
	}
	
	if(nCtlColor == CTLCOLOR_STATIC)
	{
		if ((pWnd->GetDlgCtrlID() == IDC_EMAIL)||(pWnd->GetDlgCtrlID() == IDC_URL))
		{
			pDC -> SetBkColor(GetSysColor(COLOR_BTNFACE));
			pDC -> SetTextColor(RGB(0,0,255));
			hbr = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
			return hbr;
		}
	}

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

BOOL CAboutDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if(pWnd->GetDlgCtrlID() == IDC_URL || 
		pWnd->GetDlgCtrlID() == IDC_EMAIL)
	{
        ::SetCursor(m_hCursorHand);
        return TRUE;
    }
    return CDialog::OnSetCursor (pWnd, nHitTest, message);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
	ON_STN_CLICKED(IDC_EMAIL, OnEmail)
	ON_WM_CTLCOLOR()
	ON_WM_SETCURSOR()
	ON_STN_CLICKED(IDC_URL, OnStnClickedUrl)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSLControlDlg dialog

CSLControlDlg::CSLControlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSLControlDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSLControlDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	p_SL_SREC = NULL;
	p_Tension_control_dialog = NULL;
	p_Length_control_dialog = NULL;
	p_Cyclical = NULL;
	p_Chirp = NULL;
	p_Triggers = NULL;

	p_analysis_display = NULL;
	p_superposition_display = NULL;
	p_file_header = NULL;
	p_update_check = NULL;

	p_Scan_channels_dialog = NULL;

	p_batch_analysis = NULL;

	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////
}

void CSLControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSLControlDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSLControlDlg, CDialog)
	//{{AFX_MSG_MAP(CSLControlDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_EXPERIMENTS_SLSREC, OnExperimentsSlsrec)
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
	ON_COMMAND(ID_HELP_CALIBRATIONPARAMETERS, OnHelpCalibrationparameters)
	ON_COMMAND(ID_ANALYSIS_SINGLEDATAFILE, OnAnalysisSingledatafile)
	ON_COMMAND(ID_SYSTEMCHECKS_SQUAREWAVEFL, OnSystemchecksSquarewavefl)
	ON_COMMAND(ID_SYSTEMCHECKS_READCHANNELS, OnSystemchecksReadchannels)
	ON_COMMAND(ID_HELP_DOCUMENTATION, OnHelpDocumentation)
	ON_COMMAND(ID_ANALYSIS_SUPERPOSITION, OnAnalysisSuperposition)
	ON_COMMAND(ID_EXPERIMENTS_TENSIONCONTROL, OnExperimentsTensioncontrol)
	ON_COMMAND(ID_HELP_VERSIONINFORMATION, OnHelpVersioninformation)
	ON_COMMAND(ID_ANALYSIS_BATCHANALYSIS, OnAnalysisBatchanalysis)
	ON_WM_CLOSE()
	ON_COMMAND(ID_EXPERIMENTS_CYCLICAL, OnExperimentsCyclical)
	ON_COMMAND(ID_EXPERIMENTS_CHIRP, OnExperimentsChirp)
	ON_COMMAND(ID_ANALYSIS_READDATAFILEHEADER, OnAnalysisReaddatafileheader)
//	ON_COMMAND(ID_SYSTEMCHECKS_SETMEMSTATE, OnSystemchecksSetmemstate)
//	ON_COMMAND(ID_SYSTEMCHECKS_CHECKMEMORY, OnSystemchecksCheckmemory)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_EXPERIMENTS_TRIGGERS, OnExperimentsTriggers)
	ON_COMMAND(ID_FILE_SAVEEXPERIMENTPROTOCOL, OnFileSaveexperimentprotocol)
	ON_COMMAND(ID_FILE_LOADEXPERIMENTPROTOCOL, OnFileLoadexperimentprotocol)
	ON_COMMAND(ID_HELP_CHECKFORUPDATE, OnHelpCheckforupdate)
	ON_COMMAND(ID_SYSTEMCHECKS_SETMEMSTATE193, OnSystemchecksSetmemstate)
	ON_COMMAND(ID_SYSTEMCHECKS_CHECKMEMORY195, OnSystemchecksCheckmemory)
	ON_COMMAND(ID_EXPERIMENTS_LENGTH, OnExperimentsLength)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSLControlDlg message handlers

BOOL CSLControlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	char display_string[200];

	p_SLControlDlg = this;

	// Set window size

	CRect work_area;
	CRect* p_work_area =& work_area;

	SystemParametersInfo(SPI_GETWORKAREA,0,p_work_area,0);

	SetWindowPos(NULL,NULL,0,work_area.Width(),work_area.Height(),SWP_SHOWWINDOW);

	read_calibration_parameters_from_file();

	// Perform automatic check if required

	if (AUTOMATIC_UPDATE_CHECK)
//		OnHelpCheckforupdate();		

	// Automatically open single_analysis window if the program called from the command line
	// with a data file

	if (NO_OF_COMMAND_LINE_DATA_FILES>0)
	{
		// Create data record

		p_analysis_record =& analysis_record;

		// Try to read data file

		if (p_analysis_record->read_data_from_file(COMMAND_LINE_DATA_FILE_STRING))
		{
			// Successfully read data from file

			// Create Analysis window

			if (p_analysis_display == NULL)
			{
				p_analysis_display = new Analysis_display(this,p_analysis_record);
				
				p_analysis_display->Create();
			}
			else
			{
				p_analysis_display->SetActiveWindow();
			}
		}
		else
		{
			// Couldn't read file

			sprintf(display_string,"File\n%s\ncould not be opened",COMMAND_LINE_DATA_FILE_STRING);
			MessageBox(display_string);
		}
	}

	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSLControlDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSLControlDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSLControlDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSLControlDlg::OnExperimentsSlsrec() 
{
	////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	// SL_SREC object

	if (p_SL_SREC == NULL)
	{
		p_SL_SREC = new SL_SREC(this);

		p_SL_SREC->Create();
	}
	else
	{
		MessageBox("Please close the existing SL_SREC dialog and try again",
			"CSLControlDlg::OnExperimentSL_SREC");
		p_SL_SREC->SetActiveWindow();
	}

	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////
}

void CSLControlDlg::SL_SREC_Window_Done()
{
	// Function is called when the SL_SREC window closes

	p_SL_SREC = NULL;
}

void CSLControlDlg::OnExperimentsLength()
{
	// Length_control object

	if (p_Length_control_dialog == NULL)
	{
		p_Length_control_dialog = new Length_control_dialog(this);

		p_Length_control_dialog->Create();
	}
	else
	{
		MessageBox("Please close the existing Length_control dialog and try again",
			"CSLControlDlg::OnExperimentsLength()");

		p_Length_control_dialog->SetActiveWindow();
	}
}

void CSLControlDlg::OnExperimentsTensioncontrol() 
{
	// Tension_control object

	if (p_Tension_control_dialog == NULL)
	{
		p_Tension_control_dialog = new Tension_control_dialog(this);

		p_Tension_control_dialog->Create();
	}
	else
	{
		MessageBox("Please close the existing Tension_control dialog and try again",
			"CSLControlDlg::OnExperimentsTensionControl()");

		p_Tension_control_dialog->SetActiveWindow();
	}
}

void CSLControlDlg::Length_control_Window_Done()
{
	// Function is called when the Length_control dialog closes

	p_Length_control_dialog = NULL;
}

void CSLControlDlg::Tension_control_Window_Done()
{
	// Function is called when the Tension_control window closes

	p_Tension_control_dialog = NULL;
}

void CSLControlDlg::OnExperimentsCyclical() 
{
	// Cyclical object

	if (p_Cyclical == NULL)
	{
		p_Cyclical = new Cyclical(this);

		p_Cyclical->Create();
	}
	else
	{
		MessageBox("Please close the existing Cyclical dialog and try again",
			"CSLControlDlg::OnExperimentsCyclical()");

		p_Cyclical->SetActiveWindow();
	}
}

void CSLControlDlg::Cyclical_Window_Done()
{
	// Function is called when the Cyclical window closes

	p_Cyclical = NULL;
}

void CSLControlDlg::OnExperimentsChirp() 
{
	// Chirp object

	if (p_Chirp == NULL)
	{
		p_Chirp = new Chirp(this);

		p_Chirp->Create();
	}
	else
	{
		MessageBox("Please close the existing Chirp dialog and try again",
			"CSLControlDlg::OnExperimentsChirp()");

		p_Chirp->SetActiveWindow();
	}
}

void CSLControlDlg::Chirp_Window_Done()
{
	// Function is called when the Chirp window closes

	p_Chirp = NULL;
}

void CSLControlDlg::OnExperimentsTriggers()
{
	// Trigger object

	if (p_Triggers == NULL)
	{
		p_Triggers = new Triggers(this);

		p_Triggers->Create();
	}
	else
	{
		MessageBox("Please close the existing Triggers dialog and try again",
			"CSLControlDlg::OnExperimentsTriggers()");

		p_Triggers->SetActiveWindow();
	}
}

void CSLControlDlg::Triggers_Window_Done(void)
{
	// Function is called when the Triggers window closes

	p_Triggers = NULL;
}

void CSLControlDlg::Analysis_display_Window_Done()
{
	// Function is called when the Analysis_display window closes

	p_analysis_display = NULL;
}

void CSLControlDlg::Batch_analysis_Window_Done(void)
{
	// Function is calle when the Batch_analysis window closes

	p_batch_analysis = NULL;
}

void CSLControlDlg::Superposition_Window_Done()
{
	// Function is called when the Analysis_display window closes

	p_superposition_display = NULL;
}

void CSLControlDlg::File_header_Window_Done()
{
	// Function is called when the File_header window closes

	p_file_header = NULL;
}

void CSLControlDlg::Update_check_Window_Done()
{
	// Function is called when the File_header window closes

	p_update_check = NULL;
}

void CSLControlDlg::OnFileExit() 
{
	////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	// Confirm exit

	int confirmation;
	
	confirmation=MessageBox("Are you sure you want to exit?",
		"Exit confirmation",MB_YESNO+MB_ICONQUESTION);

	if (confirmation==IDYES)
	{
		OnClose();
		OnOK();
	}

	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////
}

void CSLControlDlg::OnHelpAbout() 
{
	// TODO: Add your command handler code here

	////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	// Create an object

	CAboutDlg dlg;

	// Display

	dlg.DoModal();

	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////
}

void CSLControlDlg::OnHelpCalibrationparameters() 
{
	////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	// Code displays calibration parameters

	// Variables

	char display_string[400];
	char temp_string[100];

	// Code

	sprintf(display_string,
		"FL_POLARITY: %.0f\n\nFL_COMMAND (µm/V): %.3f\n\nFL_RESPONSE (µm/V): %.3f\n\nFORCE (N/V): %.3e\n\nLast Updated: %s\n\nFL_OR_SL_CHANNEL_MODE: %i\n\nSLControl Version: %s",
		FL_POLARITY,
		FL_COMMAND_CALIBRATION,
		FL_RESPONSE_CALIBRATION,
		FORCE_CALIBRATION,
		CALIBRATION_DATE_STRING,
		FL_OR_SL_CHANNEL_MODE,
		SLCONTROL_VERSION_STRING);

	if (!AUTOMATIC_UPDATE_CHECK)
	{
		sprintf(temp_string,"\n\nAUTOMATIC_UPDATE_CHECK: %i",AUTOMATIC_UPDATE_CHECK);
		strcat(display_string,temp_string);
	}

	MessageBox(display_string,"Current Calibration Values");

	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////
}

void CSLControlDlg::OnAnalysisSingledatafile() 
{
	// Variables

	char display_string[200];

	int return_value;

	CString data_file_string;

	// Initialises file open dialog

	CFileDialog fileDlg(TRUE,NULL,"*.slc;*.txt",
		OFN_HIDEREADONLY,
		"SLControl files (*.slc,*.txt)|*.slc;*.txt||",this);

	// Initialisation

	fileDlg.m_ofn.lpstrTitle="Single Data File Analysis";

	// Call Dialog

	if (fileDlg.DoModal()==IDOK)
	{
		data_file_string=fileDlg.GetPathName();

		// Create data record

		p_analysis_record =& analysis_record;

		// Try to read data file

		return_value=p_analysis_record->read_data_from_file(data_file_string);

		if (return_value==1)
		{
			// Successfully read data from file

			// Create Analysis window

			if (p_analysis_display == NULL)
			{
				p_analysis_display = new Analysis_display(this,p_analysis_record);
				
				p_analysis_display->Create();
			}
			else
			{
				MessageBox("Please close the existing Analysis_display window and try again");
				p_analysis_display->SetActiveWindow();
			}
		}
		else
		{
			// Couldn't read file

			if (return_value==2)
			{
				sprintf(display_string,"Is this an SLControl data file?\nIt does not seem to contain a 'Data' tag\n\n");
				strcat(display_string,"Please check you have selected the correct file before trying again.");
				MessageBox(display_string,data_file_string);
			}
			else
			{
				sprintf(display_string,"File\n%s\ncould not be opened",data_file_string);
				MessageBox(display_string);
			}
		}
	}
}

void CSLControlDlg::OnAnalysisReaddatafileheader() 
{
	// Variables

	CString data_file_string;

	// Initialises file open dialog

	CFileDialog fileDlg(TRUE,NULL,"*.slc;*.txt",OFN_HIDEREADONLY,"SLControl files (*.slc,*.txt)|*.slc;*.txt||",this);

	// Initialisation

	fileDlg.m_ofn.lpstrTitle="Inspect Single Data File Header";

	// Call Dialog

	if (fileDlg.DoModal()==IDOK)
	{
		data_file_string=fileDlg.GetPathName();

		// Create Analysis window

		if (p_file_header == NULL)
		{
			p_file_header = new File_header(this,data_file_string);
			
			p_file_header->Create();
		}
		else
		{
			p_file_header->SetActiveWindow();
		}
	}
}

void CSLControlDlg::OnAnalysisBatchanalysis() 
{
	// Batch analysis object

	if (p_batch_analysis == NULL)
	{
		p_batch_analysis = new Batch_analysis(this);

		p_batch_analysis->Create();
	}
	else
	{
		MessageBox("Please close the existing Batch_analysis dialog and try again",
			"CSLControlDlg::OnAnalysisBatchanalysisl()");

		p_batch_analysis->SetActiveWindow();
	}	

}

void CSLControlDlg::OnAnalysisSuperposition() 
{
	// Variables

	int i;
	int no_of_traces;

	CString data_file_string[MAX_SUPERPOSED_FILES];
	CString * p_data_file_string = data_file_string;
	CString temp_string;

	POSITION file_position;

	// Code

	CFileDialog fileDlg (TRUE,NULL,"*.slc;*.txt",
	    OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT,
	    "SLControl files (*.slc,*.txt)|*.slc;*.txt||",this);

	// Initialisation

	fileDlg.m_ofn.lpstrTitle="Select data files to superimpose";
	
	if (fileDlg.DoModal()==IDOK)
	{
		fileDlg.GetPathName();

		file_position=fileDlg.GetStartPosition();

		i=0;

		while ((file_position!=NULL)&&((i<MAX_SUPERPOSED_FILES)))
		{
			data_file_string[i]=fileDlg.GetNextPathName(file_position);
			i++;
		}

		no_of_traces=i;

		// Create Superposition window

		if (p_superposition_display == NULL)
		{
			p_superposition_display = new Superposition(this,p_data_file_string,no_of_traces);

			p_superposition_display->Create();
		}
		else
		{
			MessageBox("Please close the exisiting Superposition Window and try again",
				"Superposition File Selection Error",MB_ICONWARNING);

			p_superposition_display->SetActiveWindow();
		}
	}
}

void CSLControlDlg::read_calibration_parameters_from_file(void)
{
	// Variables

	char display_string[200];

	// Code

	if (read_apparatus_calibration_parameters()==0)
	{
		// Calibration failed

		sprintf(display_string,"Calibration parameters were not read from\napparatus_calibration_parameters.txt\nReverting to default values");
		MessageBox(display_string,"Calibration parameters failure",MB_ICONWARNING);
		MessageBox("Are you aware this is probably an error?","Calibration parameters failure",MB_ICONQUESTION);
	}

	// Display current parameters unless program started with a data file specified in the
	// command line

	if (NO_OF_COMMAND_LINE_DATA_FILES==0)
		OnHelpCalibrationparameters();
}

void CSLControlDlg::OnSystemchecksSquarewavefl() 
{
	// Creates Square_wave window

	Square_wave square_wave_dialog;

	square_wave_dialog.DoModal();
}

void CSLControlDlg::OnSystemchecksReadchannels() 
{
	// Creates Scan_channels window

	if (p_Scan_channels_dialog == NULL)
	{
		p_Scan_channels_dialog = new Scan_channels(this);

		p_Scan_channels_dialog->Create();
	}
	else
		p_Scan_channels_dialog->SetActiveWindow();
}

void CSLControlDlg::Scan_channels_Window_Done(void)
{
	p_Scan_channels_dialog=NULL;
}

void CSLControlDlg::OnHelpDocumentation() 
{
	// Launches HTML Browser at help page

	// Variables

	char temp_string[200];

	CString executable_string;
	CString documentation_string;

	// Code

	::set_executable_directory_string(temp_string);
	executable_string=temp_string;

	executable_string=executable_string.Left(executable_string.ReverseFind('\\'));
	documentation_string=executable_string+"\\Documentation\\index.html";

	HINSTANCE hTemp=ShellExecute(NULL,"open",documentation_string,NULL,NULL,SW_SHOWNORMAL);
}

void CSLControlDlg::OnHelpVersioninformation() 
{
	// Displays version information

	// Variables

	char display_string[300];

	// Code

	sprintf(display_string,"SLControl Version: %s",SLCONTROL_VERSION_STRING);

	MessageBox(display_string,"SLControl Version Information");
}

void CSLControlDlg::OnClose() 
{
	// Code shuts down any open windows
	
	if (p_SL_SREC!=NULL)
		p_SL_SREC->remote_shut_down();

	if (p_Length_control_dialog!=NULL)
		p_Length_control_dialog->remote_shut_down();

	if (p_Tension_control_dialog!=NULL)
		p_Tension_control_dialog->remote_shut_down();

	if (p_Cyclical!=NULL)
		p_Cyclical->remote_shut_down();

	if (p_Chirp!=NULL)
		p_Chirp->remote_shut_down();

	if (p_Triggers!=NULL)
		p_Triggers->remote_shut_down();

	if (p_analysis_display!=NULL)
		p_analysis_display->remote_shut_down();

	if (p_Scan_channels_dialog!=NULL)
		p_Scan_channels_dialog->remote_shut_down();

	if (p_superposition_display!=NULL)
		p_superposition_display->remote_shut_down();

	if (p_batch_analysis!=NULL)
		p_batch_analysis->remote_shut_down();

	if (p_file_header!=NULL)
		p_file_header->remote_shut_down();

	CDialog::OnClose();
}

void CSLControlDlg::OnSystemchecksSetmemstate()
{
	MessageBox("Setting mem state");
	oldMemState.Checkpoint();
}

void CSLControlDlg::OnSystemchecksCheckmemory()
{
	newMemState.Checkpoint();
    
	if( diffMemState.Difference( oldMemState, newMemState ) )
    {
        MessageBox("Memory has leaked");
		diffMemState.DumpStatistics();
    }
	else
	{
		MessageBox("Memory is okay");
	}
}

void CSLControlDlg::OnFileSaveexperimentprotocol()
{
	// Creates a parameter file which contains the current settings for every open experimental protocol

	// Variables

	CString parameter_file_string;

	FILE* parameter_file;

	char display_string[200];

	// Code

    // Initialises file save dialog

	CFileDialog fileDlg(FALSE,NULL,"*.slp",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"SLControl parameter files (*.slp)",this);

	// Initialisation

	fileDlg.m_ofn.lpstrTitle="SLControl Experimental Parameters File";
	

	// Call Dialog

	if (fileDlg.DoModal()==IDOK)
	{
		parameter_file_string=fileDlg.GetPathName();

		// Open and test file

		parameter_file=fopen(parameter_file_string,"w");

		if (parameter_file==NULL)
		{
			sprintf(display_string,"Parameter file\n%s\ncould not be opened",parameter_file_string);
			MessageBox(display_string);
			return;
		}
		else
		{
			fprintf(parameter_file,"SLCONTROL_PARAMETER_FILE\n\n");

			if (p_Triggers!=NULL)
				p_Triggers->append_parameters_to_file(parameter_file);


			fprintf(parameter_file,"\n_END_\n");
		}

		fclose(parameter_file);
	}
}

void CSLControlDlg::OnFileLoadexperimentprotocol()
{
	// Loads the current parameters for every open dialog

	// Variables

	CString parameter_file_string;

	FILE* parameter_file;

	char display_string[200];

	// Code

    // Initialises file open dialog

	CFileDialog fileDlg(TRUE,NULL,"*.slp",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"SLControl parameter files (*.slp)",this);

	// Initialisation

	fileDlg.m_ofn.lpstrTitle="SLControl Experimental Parameters File";
	

	// Call Dialog

	if (fileDlg.DoModal()==IDOK)
	{
		parameter_file_string=fileDlg.GetPathName();

		// Open and test file

		parameter_file=fopen(parameter_file_string,"r");

		if (parameter_file==NULL)
		{
			sprintf(display_string,"Parameter file\n%s\ncould not be opened",parameter_file_string);
			MessageBox(display_string);
			return;
		}
		else
		{
			if (p_Triggers!=NULL)
				p_Triggers->load_parameters_from_file(parameter_file);
		}

		fclose(parameter_file);
	}
}

void CSLControlDlg::OnHelpCheckforupdate()
//  Created: 4/21/2003  (rk)
//  Last modified: 4/23/2003  (rk)
//  Handle the event where user clicked on "Check for Updates"
//  Connects to FTP site to look for updates
//  Uses method posted by Paul DiLascia in April 2003 MSDN Magazine
{
	int iMaj,iMin,iRev;
	BOOL bRes;

	char display_string[400];
	char temp_string[200];

	// Code

	// Open message box, closed automatically if everything is okay

	if (p_update_check == NULL)
	{
		p_update_check = new Update_check(this);

		p_update_check->Create();
	}
	else
	{
		MessageBox("Please close the existing Update_check dialog and try again",
			"CSLControlDlg::OnHelpCheckforupdate()");

		p_update_check->SetActiveWindow();
	}	

	// Now check for update

	if (CWebVersion::Online()) 
	{
		CWaitCursor wait;                    // Display hourglass cursor
		CWebVersion webver(FTPSERVER);
		bRes=webver.ReadVersion(VERFILE);  // Get version info from net
		wait.Restore ();                     // Restore cursor
		if(bRes==TRUE)
		{
			// Close the check dialog

			p_update_check->remote_shut_down();

			// Get current program version

			sscanf(SLCONTROL_VERSION_STRING,"%d.%d.%d",&iMaj,&iMin,&iRev);

			if((webver.iMajor > iMaj) || 
				((webver.iMajor == iMaj) &&	(webver.iMinor > iMin)) ||
				(((webver.iMajor == iMaj) && (webver.iMinor == iMin)) && (webver.iRevLevel > iRev))) 
			{
				// Generate dialog box

				sprintf(display_string,"SLControl Version %i.%i.%i is now available on-line\n\n",
					webver.iMajor,webver.iMinor,webver.iRevLevel);
				sprintf(temp_string,"Your current version is %i.%i.%i\n\n",iMaj,iMin,iRev);
				strcat(display_string,temp_string);
				sprintf(temp_string,"Click OK to open your WWW browser at the appropriate download page\n\n");
				strcat(display_string,temp_string);
				sprintf(temp_string,"To enable/disable the automatic version check on start-up\nupdate the _AUTOMATIC_VERSION_CHECK_ tag\nin your calibration parameters file to 1/0 respectively\n\n");
				strcat(display_string,temp_string);
				sprintf(temp_string,"\n\nPlease remember to close this version of SLControl before installing the new version");
				strcat(display_string,temp_string);
				
				if (MessageBox(display_string,"SLControl update is available",MB_OKCANCEL)==IDOK)
				{
					::ShellExecute( m_hWnd, "open", "http://www.slcontrol.com/download/download_header.html",
						"", "", SW_SHOW );
				}
				else
				{
					MessageBox("Please download the new version of SLControl at your earliest convenience");
				}
			}
			else
			{
				AfxMessageBox("Your program is already up-to-date!",MB_OK|MB_ICONINFORMATION,0);
			}		
		} 
		else 
		{
			// Close the check dialog
			p_update_check->remote_shut_down();

			DWORD err = webver.GetError();
			if(err==ERROR_INTERNET_NAME_NOT_RESOLVED)
			{
				AfxMessageBox("Insider error No Internet connection available: please connect, then try again",MB_OK|MB_ICONEXCLAMATION,0);
				return;
			}
			else
			{
				AfxMessageBox("No update information available at this time, please try again later!",MB_OK|MB_ICONINFORMATION,0);
				return;
			}
		}
	} 
	else 
	{
		// Close the check dialog
		p_update_check->remote_shut_down();

		AfxMessageBox("No Internet connection available: please connect, then try again!",MB_OK|MB_ICONEXCLAMATION,0);
	}
    return;
}

