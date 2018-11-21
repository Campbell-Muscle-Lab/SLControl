// Square_wave.cpp : implementation file
//

#include "stdafx.h"
#include "slcontrol.h"
#include "Square_wave.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////
// Ken's code starts here //
////////////////////////////

#include "global_functions.h"
#include "dapio32.h"

////////////////////////////
//  Ken's code ends here  //
////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Square_wave dialog


Square_wave::Square_wave(CWnd* pParent /*=NULL*/)
	: CDialog(Square_wave::IDD, pParent)
{
	//{{AFX_DATA_INIT(Square_wave)
	fl_step_duration_ms = 0.0f;
	fl_step_size_microns = 0.0f;
	//}}AFX_DATA_INIT

	char temp_string[200];

	// Initialisation

	fl_step_duration_ms=500.0;
	fl_step_size_microns=20.0;

	time_increment_ms=1.0;

	square_wave_active=0;

	// Set file strings

	// Set path_string

	set_executable_directory_string(temp_string);
	strcat(temp_string,"\\SQUARE\\");
	
	Square_wave_path_string=temp_string;

	// Set file strings
	
	start_file_string=Square_wave_path_string+"dapl_start.txt";
	stop_file_string=Square_wave_path_string+"dapl_stop.txt";
}

void Square_wave::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Square_wave)
	DDX_Text(pDX, IDC_FL_STEP_DURATION, fl_step_duration_ms);
	DDX_Text(pDX, IDC_FL_STEP_SIZE, fl_step_size_microns);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Square_wave, CDialog)
	//{{AFX_MSG_MAP(Square_wave)
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Square_wave message handlers

BOOL Square_wave::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	// Disable Stop button

	GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void Square_wave::OnStart() 
{
	// Starts Square wave

	// Variables

	int i;

	char display_string[200];

	HDAP hdapSysGet,hdapSysPut,hdapBinGet;									// DAPL communication pipes

	// Code

	// Try to write control files, run expt if successful

	if (write_start_file()*write_stop_file())
	{
		// Open and flush DAPL communication pipes

		hdapSysGet=DapHandleOpen("\\\\.\\Dap0\\$SysOut",DAPOPEN_READ);
		hdapSysPut=DapHandleOpen("\\\\.\\Dap0\\$SysIn",DAPOPEN_WRITE);
		hdapBinGet=DapHandleOpen("\\\\.\\Dap0\\$BinOut",DAPOPEN_READ);

		DapInputFlush(hdapSysGet);
		DapInputFlush(hdapBinGet);
		DapInputFlush(hdapSysPut);

		// Upload file
		
		i=DapConfig(hdapSysPut,start_file_string);

		if (i!=1)															// DAPL file error
		{
			sprintf(display_string,"DAPL could not handle start_file");
			MessageBox(display_string,"Square_wave::OnStart()");
		}

		// Close communication pipes

		DapHandleClose(hdapSysGet);
		DapHandleClose(hdapSysPut);
		DapHandleClose(hdapBinGet);

		// Disable Start button and controls

		GetDlgItem(IDC_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_FL_STEP_DURATION)->EnableWindow(FALSE);
		GetDlgItem(IDC_FL_STEP_SIZE)->EnableWindow(FALSE);

		// Enable Stop button

		GetDlgItem(IDC_STOP)->EnableWindow(TRUE);

		// Update square_wave_active

		square_wave_active=1;
	}
	else
	{
		sprintf(display_string,"Warning\nStart and/or stop files could not be created\n");
		MessageBox(display_string,"Square_wave::OnStart()");
	}
}

void Square_wave::OnStop() 
{
	// Code stops square wave	

	// Variables

	HDAP hdapSysGet,hdapSysPut,hdapBinGet;									// DAPL communication pipes

	int i;

	char display_string[200];

	// Code

	// Open and flush DAPL communication pipes

	hdapSysGet=DapHandleOpen("\\\\.\\Dap0\\$SysOut",DAPOPEN_READ);
	hdapSysPut=DapHandleOpen("\\\\.\\Dap0\\$SysIn",DAPOPEN_WRITE);
	hdapBinGet=DapHandleOpen("\\\\.\\Dap0\\$BinOut",DAPOPEN_READ);

	DapInputFlush(hdapSysGet);
	DapInputFlush(hdapBinGet);
	DapInputFlush(hdapSysPut);

	// Upload file
		
	i=DapConfig(hdapSysPut,stop_file_string);

	if (i!=1)															// DAPL file error
	{
		sprintf(display_string,"DAPL could not handle stop_file");
		MessageBox(display_string,"Square_wave::OnStop()");
	}

	// Close communication pipes

	DapHandleClose(hdapSysGet);
	DapHandleClose(hdapSysPut);
	DapHandleClose(hdapBinGet);

	// Disable Stop button

	GetDlgItem(IDC_STOP)->EnableWindow(FALSE);

	// Enable Start button

	GetDlgItem(IDC_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_FL_STEP_DURATION)->EnableWindow(TRUE);
	GetDlgItem(IDC_FL_STEP_SIZE)->EnableWindow(TRUE);

	// Update square_wave_active

	square_wave_active=0;
}


int Square_wave::write_start_file(void)
{
	// Code writes start_file

	// Variables

	char display_string[200];

	FILE *start_file;

	// Code

	UpdateData(TRUE);

	// File handling

	start_file=fopen(start_file_string,"w");

	if (start_file==NULL)															// Error
	{
		sprintf(display_string,"Start_file\n%s\ncould not be opened\n",start_file_string);
		MessageBox(display_string,"Square_wave::write_start_file");
		return(0);
	}
	else
	{
		fprintf(start_file,"RESET\n\n");

		fprintf(start_file,"OPTION UNDERFLOWQ=OFF\n");
		fprintf(start_file,"OPTION ERRORQ=OFF\n");
		fprintf(start_file,"OPTION SCHEDULING=FIXED\n");
		fprintf(start_file,"OPTION BUFFERING=OFF\n");
		fprintf(start_file,"OPTION QUANTUM=100\n\n");

		fprintf(start_file,";************************************************************\n\n");
		fprintf(start_file,"pipes phigh\n");
		fprintf(start_file,"pipes pplus\n");
		fprintf(start_file,"pipes pneg\n");
		fprintf(start_file,"pipes pzero\n");
		fprintf(start_file,"pipes pzero2\n");
		fprintf(start_file,"pipes pzero3\n\n");

		fprintf(start_file,"pipes pout\n");
		fprintf(start_file,"pipes ptrig\n\n");

		fprintf(start_file,"pipes ptrig_high\n");
		fprintf(start_file,"pipes ptrig_low\n\n");

		fprintf(start_file,"constants duration=%.0f\n",fl_step_duration_ms);
		fprintf(start_file,"constants fl_step=%i\n\n",
			convert_volts_to_DAPL_units(convert_FL_COMMAND_microns_to_volts(fl_step_size_microns)));

		fprintf(start_file,"constants trigger_level=%i\n",(int)convert_volts_to_DAPL_units((float)4.95));
		fprintf(start_file,"constants trig_high=%.0f\n",(float)1.0);
		fprintf(start_file,"constants trig_low=%.0f\n\n",(float)((2.0*fl_step_duration_ms)-1.0));

		fprintf(start_file,";*************************************************************\n\n");

		fprintf(start_file,"pdef create_pipes\n");
		fprintf(start_file,"  pplus=fl_step\n");
		fprintf(start_file,"  pneg=-fl_step\n");
		fprintf(start_file,"  pzero=0\n\n");

		fprintf(start_file,"  ptrig_high=trigger_level\n");
		fprintf(start_file,"  ptrig_low=0\n");

		fprintf(start_file,"  nmerge(duration,pneg,duration,pplus,pout)\n");
		fprintf(start_file,"  nmerge(trig_high,ptrig_high,trig_low,ptrig_low,ptrig)\n");

		fprintf(start_file,"end\n\n");

		fprintf(start_file,";*************************************************************\n\n");

		fprintf(start_file,"pdef control\n");
		fprintf(start_file,"  copy(ptrig,op0)\n");
		fprintf(start_file,"  copy(pout,op1)\n");
		fprintf(start_file,"end\n\n");

		fprintf(start_file,";*************************************************************\n\n");

		fprintf(start_file,"odef output_proc 2\n");
		fprintf(start_file,"  set op0 a0\n");
		fprintf(start_file,"  set op1 a1\n");
		fprintf(start_file,"  time 500\n");
		fprintf(start_file,"end\n\n");
		
		fprintf(start_file,";*************************************************************\n\n");

		fprintf(start_file,"pdef zero_control\n");
		fprintf(start_file,"  pzero2=0\n");
		fprintf(start_file,"  pzero3=0\n\n");

		fprintf(start_file,"  copy(pzero2,op0)\n");
		fprintf(start_file,"  copy(pzero3,op1)\n");
		fprintf(start_file,"end\n\n");

		fprintf(start_file,";*************************************************************\n\n");

		fprintf(start_file,"odef output_zero 2\n");
		fprintf(start_file,"  set op0 a0\n");
		fprintf(start_file,"  set op1 a1\n");
		fprintf(start_file,"  time 500\n");
		fprintf(start_file,"  count 2\n");
		fprintf(start_file,"end\n\n");

		fprintf(start_file,";*************************************************************\n\n");

		fprintf(start_file,"start create_pipes,control,output_proc\n\n");
	}

	fclose(start_file);

	return(1);
}

int Square_wave::write_stop_file(void)
{
	// Code writes stop_file

	// Variables

	char display_string[200];

	FILE *stop_file;

	// Code

	// File handling

	stop_file=fopen(stop_file_string,"w");

	if (stop_file==NULL)															// Error
	{
		sprintf(display_string,"Stop_file\n%s\ncould not be opened\n",stop_file_string);
		MessageBox(display_string,"Square_wave::write_stop_file");
		return(0);
	}
	else
	{
		fprintf(stop_file,"stop\n\n");

		fprintf(stop_file,"start output_zero,zero_control\n\n");

		fprintf(stop_file,"pause 5\n");

		fprintf(stop_file,"RESET\n\n");
	}

	fclose(stop_file);

	return(1);
}

void Square_wave::OnCancel() 
{
	// TODO: Add extra cleanup here

	if (square_wave_active==1)
		OnStop();														// stop square wave before
																		// exiting
	CDialog::OnCancel();
}

BOOL Square_wave::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class

	// Traps Escape key presses to prevent dialog box closing

	if (pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE)
	{
		return TRUE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}
