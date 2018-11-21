// Stimulus_dialog.cpp : implementation file
//

#include "stdafx.h"
#include "SLControl.h"
#include "Stimulus_dialog.h"
#include ".\stimulus_dialog.h"
#include "SLControlDlg.h"
#include "dapio32.h"
#include "global_functions.h"
#include "global_definitions.h"


// Stimulus_dialog dialog

IMPLEMENT_DYNAMIC(Stimulus_dialog, CDialog)
Stimulus_dialog::Stimulus_dialog(CWnd* pParent /*=NULL*/)
	: CDialog(Stimulus_dialog::IDD, pParent)
{
	// Constructor

	// Set pointer to parent window and set dialog IDD

	m_pParent=pParent;
	m_nID=Stimulus_dialog::IDD;

	// Create pointer to data record

	p_Stimulus_record =& Stimulus_record;

	// Status strings and counter

	status_middle_string="1) Stimulus dialog initialising";
	status_bottom_string="2) Preparing for trials";
	status_counter=3;

}

Stimulus_dialog::~Stimulus_dialog()
{
	// Destructor
}

BOOL Stimulus_dialog::Create()
{
	return CDialog::Create(m_nID, m_pParent);
}

void Stimulus_dialog::PostNcDestroy()
{
	delete this;
}

void Stimulus_dialog::remote_shut_down(void)
{
	// Called by parent to shut down window

	OnClose();
}

void Stimulus_dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Stimulus_dialog, CDialog)
	ON_WM_CLOSE()
//	ON_WM_CREATE()
ON_BN_CLICKED(IDC_RUN, OnBnClickedRun)
END_MESSAGE_MAP()


// Stimulus_dialog message handlers

void Stimulus_dialog::OnClose()
{
	// Close derived windows

	p_Calibration->close_calibration_window();
	p_Calibration=NULL;

	p_Experiment_display->close_experiment_display_window();
	p_Experiment_display=NULL;

	// Signal to parent that this window is closing

	((CSLControlDlg*)m_pParent)->Stimulus_Window_Done();

	DestroyWindow();
}

BOOL Stimulus_dialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set window size and position

	int width=1022;
	int height=740;
	
	SetWindowPos(NULL,(int)(0.02*width),(int)(0.07*height),
		(int)(0.63*width),(int)(0.43*height),SWP_SHOWWINDOW);

	// Create derived windows

		// Create the Calibration window

	p_Calibration = new Calibration();
	p_Calibration->Create();

	// Create Experiment_display window

	p_Experiment_display = new Experiment_display(this,p_Stimulus_record);
	p_Experiment_display->Create();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void Stimulus_dialog::display_status(int update_mode,CString new_string)
{
	// Scrolls new status messages to screen

	// Variables

	char display_string[300];

	// Code

	// Create display dc

	CClientDC dc(this);
	
	// Create font

	dc.SetBkMode(TRANSPARENT);
	CFont display_font;
	display_font.CreateFont(12,0,0,0,400,FALSE,FALSE,0,ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_SWISS,"Arial");
	CFont* pOldFont = dc.SelectObject(&display_font);

	// Check window dimensions

	CRect current_window;
	GetWindowRect(current_window);
	current_window.NormalizeRect();

	// Draws over status display area in grey

	CPen* pOriginalPen;
	CPen BackgroundPen;
	BackgroundPen.CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNFACE));
	pOriginalPen = dc.SelectObject(&BackgroundPen);

	CBrush* pOriginalBrush;
	CBrush BackgroundBrush;
	BackgroundBrush.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	pOriginalBrush = dc.SelectObject(&BackgroundBrush);

	dc.Rectangle((int)(0.50*current_window.Width()),(int)(0.77*current_window.Height()),
		(int)(0.96*current_window.Width()),(int)(0.89*current_window.Height()));
	
	dc.SelectObject(pOriginalPen);

	if (update_mode==TRUE)
	{
		// Update strings and display

		status_top_string=status_middle_string;
		status_middle_string=status_bottom_string;
		sprintf(display_string,"%i) %s",status_counter,new_string);
		status_bottom_string=display_string;
		status_counter++;
	}
			
	dc.TextOut((int)(0.52*current_window.Width()),(int)(0.773*current_window.Height()),status_top_string);
	dc.TextOut((int)(0.52*current_window.Width()),(int)(0.813*current_window.Height()),status_middle_string);
	dc.TextOut((int)(0.52*current_window.Width()),(int)(0.853*current_window.Height()),status_bottom_string);
}

void Stimulus_dialog::OnBnClickedRun()
{
		// Code runs a single experiment

	// Variables

	int i,holder;

	short short_int=1;									// a short int for sizeof calculation

int data_points=5000;

	int buffer_size=4*data_points;						// number of points in a data
														// buffer with 4 channels

	short* DAPL_data = new short [buffer_size];			// data buffer for processing

	clock_t start_upload;								// time at start of DAPL upload

	HDAP hdapSysGet,hdapSysPut,hdapBinGet;				// DAPL pipes

	char display_string[200];
	char cursor_path_string[_MAX_PATH];

	// Code

	UpdateData(TRUE);

	// Switch cursor to animated cursor

	::set_executable_directory_string(cursor_path_string);
	strcat(cursor_path_string,"\\sl_control.ani");

	HCURSOR hcurWait = (HCURSOR)LoadImage(AfxGetApp()->m_hInstance,cursor_path_string,IMAGE_CURSOR,0,0,
		LR_DEFAULTSIZE|LR_DEFAULTCOLOR|LR_LOADFROMFILE);
	const HCURSOR hcurSave=SetCursor(hcurWait);

int experiment_status=0;
DAPL_source_file_string="c:\\ken\\DAPL_misc\\ken_test2.txt";
float time_increment=0.001;


	// Check parameters are valid, proceed if appropriate, abort if not

	//if ((experiment_status==0)&&(create_DAPL_source_file()))
if (experiment_status==0)
	{
		// Proceed with experiment

		// Open and flush DAPL pipes

		hdapSysGet=DapHandleOpen("\\\\.\\Dap0\\$SysOut",DAPOPEN_READ);
		hdapSysPut=DapHandleOpen("\\\\.\\Dap0\\$SysIn",DAPOPEN_WRITE);
		hdapBinGet=DapHandleOpen("\\\\.\\Dap0\\$BinOut",DAPOPEN_READ);

		DapInputFlush(hdapSysGet);
		DapInputFlush(hdapSysPut);
		DapInputFlush(hdapBinGet);

		// Time upload

		start_upload=clock();
		sprintf(display_string,"Passing command file to DAPL");
		display_status(TRUE,display_string);

		// Upload

		i=DapConfig(hdapSysPut,DAPL_source_file_string);

		// Error checking

		if (i==FALSE)
		{
			sprintf(display_string,"DAPL could not handle source file string: %s\n",
				DAPL_source_file_string);
			MessageBox(display_string,"Stimulus_dialog::OnBinClickedRunExperiment");

			sprintf(display_string,"Upload to Dap failed");
			//display_status(TRUE,display_string);

			delete [] DAPL_data;

			return;
		}

		// Display upload time as experiment is running

		sprintf(display_string,"Trial started, DAPL upload took %.4f s",
			(float)(clock()-start_upload)/CLOCKS_PER_SEC);
		display_status(TRUE,display_string);

		// Read data bck from DAPL and display status

		i=DapBufferGet(hdapBinGet,buffer_size*sizeof(short_int),DAPL_data);

		sprintf(display_string,"%i Points/Channel read from DAPL processor",
			(i/(4*sizeof(short_int))));
		display_status(TRUE,display_string);

		// Copy data to Triggers record and update no_of_points

		holder=1;

		for (i=0;i<buffer_size;i=i+4)
		{
			p_Stimulus_record->data[TIME][holder]=((float)(holder-1)*time_increment);
			p_Stimulus_record->data[FORCE][holder]=convert_DAPL_units_to_volts(DAPL_data[i]);
			p_Stimulus_record->data[SL][holder]=convert_DAPL_units_to_volts(DAPL_data[i+1]);
			p_Stimulus_record->data[FL][holder]=convert_DAPL_units_to_volts(DAPL_data[i+2]);
			p_Stimulus_record->data[INTENSITY][holder]=convert_DAPL_units_to_volts(DAPL_data[i+3]);

			holder++;
		}

		p_Stimulus_record->no_of_points=data_points;

		// Close communication pipes

		DapHandleClose(hdapSysGet);
		DapHandleClose(hdapSysPut);
		DapHandleClose(hdapBinGet);

		// Updata data plots

		p_Experiment_display->clear_window();

		for (i=0;i<=(p_Experiment_display->no_of_plots-1);i++)
		{
			p_Experiment_display->pointers_array[i]->first_drawn_point=1;
			p_Experiment_display->pointers_array[i]->last_drawn_point=
				p_Stimulus_record->no_of_points;
		}

		p_Experiment_display->update_plots();

		// Write data file

		//write_data_to_file();
	}
	else
	{
		if (experiment_status>0)
		{
			sprintf(display_string,"Experiment parameters are invalid, Aborting experiment");
			MessageBox(display_string,"Triggers::OnBnClickedRunExperiment");
		}
	}

	// Free memory

	delete [] DAPL_data;

	// Switch back to normal cursor

	SetCursor(hcurSave);

	// Finished succesfully

	return;
}
