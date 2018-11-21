// Length_control_dialog.cpp : implementation file
//

#include "stdafx.h"
#include <math.h>
#include "SLControl.h"
#include "Length_Control_dialog.h"
#include "Length_control_sheet.h"
#include "SLControlDlg.h"
#include ".\length_control_dialog.h"
#include "global_definitions.h"
#include "global_functions.h"
#include "dapio32.h"
#include <afxwin.h>

#include <ctime>

#include "Abort_multiple_trials.h"

#include "PathDialog.h"

extern float FL_POLARITY;
extern float FL_RESPONSE_CALIBRATION;
extern float FL_COMMAND_CALIBRATION;
extern float FORCE_CALIBRATION;
extern char CALIBRATION_DATE_STRING[100];
extern int FL_OR_SL_CHANNEL_MODE;
extern char SLCONTROL_VERSION_STRING[100];

// Length_control_dialog dialog

IMPLEMENT_DYNAMIC(Length_control_dialog, CDialog)
Length_control_dialog::Length_control_dialog(CWnd* pParent /*=NULL*/)
	: CDialog(Length_control_dialog::IDD, pParent)
{
	// Constructor

	// Variables

	int i;

	// Set pointer to parent window and set dialog IDD

	m_pParent=pParent;
	m_nID=Length_control_dialog::IDD;

	// Create pointers to data record

	p_Length_control_record =& Length_control_record;
	p_Preview_record =& Preview_record;

	// Create pointers to plot

	p_Preview_plot =& Preview_plot;

	// Thread pointer

	//p_thread=NULL;

	// Temp variables

	char temp_string[100];
	char* p_string;

	char path_string[_MAX_PATH];

	// Initialise variables

    LC_data_path_string="c:\\temp";
	data_base_string="test";
	data_index=1;
	inter_trial_interval_s=(float)3.0;

	ktr_initiation_points=1;
	trial_status=0;

	// File handling

	::set_executable_directory_string(path_string);
	DAPL_source_file_string=strcat(path_string,"\\LC_Dialog\\");

	DAPL_source_file_string=DAPL_source_file_string+"dapl_source.txt";

	no_of_trials=0;
	trials_left=0;

	lc_trials=1;

	status_middle_string="1) Length_control dialog initialised";
	status_bottom_string="2) Ready for input";
	status_counter=3;

	abort_signal=0;
	close_abort_window=0;

	sampling_rate_string="1000";
	sampling_rate=(float)1000.0;

	data_points=3000;
	time_increment=((float)(1.0/strtod(sampling_rate_string,&p_string)));
	duration_seconds=(data_points*time_increment);
	sprintf(temp_string,"%.3f",duration_seconds);
	duration_string=temp_string;

	lc_dialog_SL_volts_to_FL_microns_calibration=(float)999.9;
	lc_dialog_SL_volts_to_FL_command_volts=(float)999.9;

	lc_no_of_input_channels=4;

	// Reset record

	for (i=0;i<=MAX_DATA_POINTS-1;i++)
	{
		p_Preview_record->data[FL][i]=(float)0.0;
		p_Preview_record->data[TIME][i]=(float)(i*time_increment);
	}

	// Create batch mode arrays

	lc_dialog_max_trials=LC_MAX_TRIALS;

	lc_dialog_pre_ms_array = new float [lc_dialog_max_trials];
	lc_dialog_rel_size_array = new float [lc_dialog_max_trials];
	lc_dialog_rel_time_array = new float [lc_dialog_max_trials];
	lc_dialog_delay_array = new float [lc_dialog_max_trials];
	lc_dialog_ktr_rel_size_array = new float [lc_dialog_max_trials];
	lc_dialog_ktr_rel_duration_array = new float [lc_dialog_max_trials];
	lc_dialog_ktr_pre_array = new float [lc_dialog_max_trials];

	lc_freeform_file_string = new CString [lc_dialog_max_trials];
	lc_freeform_total_sample_points = new int [lc_dialog_max_trials];
	lc_freeform_no_of_channels = new int [lc_dialog_max_trials];
	lc_freeform_time_increment = new float [lc_dialog_max_trials];

	// Initialise triggered record

	max_triggered_points=65000;
	triggered_points=0;
	p_triggered_FL = new float [max_triggered_points];
	
}

Length_control_dialog::~Length_control_dialog()
{
	// Destructor

	delete lc_dialog_pre_ms_array;
	delete lc_dialog_rel_size_array;
	delete lc_dialog_rel_time_array;
	delete lc_dialog_delay_array;
	delete lc_dialog_ktr_rel_size_array;
	delete lc_dialog_ktr_rel_duration_array;
	delete lc_dialog_ktr_pre_array;

	delete [] lc_freeform_file_string;
	delete [] lc_freeform_total_sample_points;
	delete [] lc_freeform_no_of_channels;
	delete [] lc_freeform_time_increment;
	
	delete p_triggered_FL;
}

void Length_control_dialog::PostNcDestroy()
{
	delete this;
}

void Length_control_dialog::remote_shut_down(void)
{
	// Called by parent to shut down window

	OnClose();
}

void Length_control_dialog::OnClose()
{
	// Close derived windows

	p_Calibration->close_calibration_window();
	p_Calibration=NULL;

	p_Experiment_display->close_experiment_display_window();
	p_Experiment_display=NULL;

	// Close length control sheet

	// First return focus to first page
	// There is a memory leak on closing if this doesn't happen - don't know why?
	// Ken, 26 Dec, 2005
	p_lc_sheet->SetActivePage(p_lc_ramp_page);

	if (p_lc_sheet)
	{
		p_lc_sheet->RemovePage(p_lc_ramp_page);
		p_lc_sheet->RemovePage(p_lc_ktr_step_page);
		p_lc_sheet->RemovePage(p_lc_summary_page);
		p_lc_sheet->RemovePage(p_lc_triggered_page);
		p_lc_sheet->RemovePage(p_lc_dout_page);
		p_lc_sheet->RemovePage(p_lc_freeform_page);
	}

	p_lc_sheet=NULL;

	// Delete thread

	p_amt=NULL;
   
	// Signal to parent that this window is closing

	((CSLControlDlg*)m_pParent)->Length_control_Window_Done();

	DestroyWindow();
}


BOOL Length_control_dialog::Create()
{
	return CDialog::Create(m_nID, m_pParent);
}

void Length_control_dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_CBString(pDX,IDC_SAMPLING_RATE,sampling_rate_string);
	DDX_Text(pDX,IDC_DATA_POINTS,data_points);
	DDX_Text(pDX,IDC_DURATION,duration_string);
	DDV_MinMaxInt(pDX, data_points, 1, 65000);

	DDX_Text(pDX, IDC_DATA_PATH, LC_data_path_string);
	DDX_Control(pDX, IDC_LC_ABORT, ABORT_BUTTON);
	DDX_Control(pDX, IDC_SAMPLING_RATE, SAMPLING_RATE);

	DDX_Text(pDX,IDC_SL_to_FL_volts_calibration,lc_dialog_SL_volts_to_FL_microns_calibration);
	DDX_Text(pDX,IDC_SL_volts_to_FL_volts,lc_dialog_SL_volts_to_FL_command_volts);

	DDX_Control(pDX, IDC_DATA_BASE, DATA_BASE);
	DDX_Text(pDX, IDC_DATA_BASE, data_base_string);
	DDX_Control(pDX, IDC_DATA_INDEX, DATA_INDEX);
	DDX_Text(pDX, IDC_DATA_INDEX, data_index);
	DDX_Text(pDX, IDC_INTER_TRIAL_INTERVAL, inter_trial_interval_s);


}

BEGIN_MESSAGE_MAP(Length_control_dialog, CDialog)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_PAINT()

	ON_BN_CLICKED(IDC_Run_Experiment, OnBnClickedRunExperiment)
	ON_BN_CLICKED(IDC_LC_ABORT, OnBnClickedTest)
	ON_BN_CLICKED(IDC_PATH_BROWSE, OnBnClickedPathBrowse)
	ON_EN_CHANGE(IDC_DATA_POINTS, OnEnChangeDataPoints)
	ON_CBN_SELCHANGE(IDC_SAMPLING_RATE, OnCbnSelchangeSamplingRate)
END_MESSAGE_MAP()

// Length_control_dialog message handlers

BOOL Length_control_dialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Variables

	int i;

	int tab_counter=0;

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

	p_Experiment_display = new Experiment_display(this,p_Length_control_record);
	p_Experiment_display->Create();

	// Create the Length control property sheet

	p_lc_sheet = new Length_control_sheet("Length control sheet",this,0,this);
	
	p_lc_ramp_page = new LC_ramp_page(p_lc_sheet);
	p_lc_ktr_step_page = new LC_ktr_step_page(p_lc_sheet);
	p_lc_summary_page = new LC_summary_page(p_lc_sheet);
	p_lc_triggered_page = new LC_triggered_page(p_lc_sheet);
	p_lc_dout_page = new LC_dout_page(p_lc_sheet);
	p_lc_freeform_page = new LC_freeform_page(p_lc_sheet);
		
	p_lc_sheet->AddPage(p_lc_ramp_page);
	p_lc_ramp_page->tab_number=tab_counter;
	tab_counter++;

	p_lc_sheet->AddPage(p_lc_ktr_step_page);
	p_lc_ktr_step_page->tab_number=tab_counter;
	tab_counter++;

	p_lc_sheet->AddPage(p_lc_summary_page);
	p_lc_summary_page->tab_number=tab_counter;
	tab_counter++;

	p_lc_sheet->AddPage(p_lc_triggered_page);
	p_lc_triggered_page->tab_number=tab_counter;
	tab_counter++;

	p_lc_sheet->AddPage(p_lc_dout_page);
	p_lc_dout_page->tab_number=tab_counter;
	tab_counter++;

	p_lc_sheet->AddPage(p_lc_freeform_page);
	p_lc_freeform_page->tab_number=tab_counter;
	tab_counter++;

	no_of_tabs=tab_counter;

	// Modify control sheet

	p_lc_sheet->Create(this,WS_CHILD | WS_VISIBLE, 0);
	p_lc_sheet->ModifyStyle(0,WS_TABSTOP);
	p_lc_sheet->ModifyStyleEx(0, WS_EX_CONTROLPARENT);

	CRect rcSheet;
	GetDlgItem(IDC_LC_SHEET_FRAME)->GetWindowRect(&rcSheet);
	ScreenToClient(&rcSheet);
	
	p_lc_sheet->SetWindowPos(NULL,rcSheet.left,rcSheet.top,0,0,
		SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE );

	// Highlight ktr tab
	p_lc_sheet->highlight_tab(1,TRUE);
	p_lc_sheet->SetActivePage(p_lc_ktr_step_page);

	// Initialise preview plot

	CRect current_window;
	GetWindowRect(current_window);
	current_window.NormalizeRect();

	p_Preview_plot->initialise_plot(p_Preview_record,TIME,FL,AUTO_SCALE_ON,CURSOR_DISPLAY_OFF,TEXT_DISPLAY_OFF,
		current_window.Width(),current_window.Height(),(float)0.10,(float)0.52,(float)0.26,(float)0.075,
		"Time(s)","FL","FL v Time");
	p_Preview_plot->y_axis_label_separation=55;

	// Fill initial data arrays
	for (i=0;i<=lc_dialog_max_trials;i++)
	{
		lc_dialog_pre_ms_array[i]=p_lc_ramp_page->lc_ramp_pre_ms_array[i];
		lc_dialog_rel_size_array[i]=p_lc_ramp_page->lc_ramp_rel_size_array[i];
		lc_dialog_rel_time_array[i]=p_lc_ramp_page->lc_ramp_rel_time_array[i];
		lc_dialog_delay_array[i]=p_lc_ramp_page->lc_ramp_delay_array[i];
		lc_dialog_ktr_rel_size_array[i]=p_lc_ktr_step_page->lc_ktr_step_rel_size_array[i];
		lc_dialog_ktr_rel_duration_array[i]=p_lc_ktr_step_page->lc_ktr_step_rel_duration_array[i];
		lc_dialog_ktr_pre_array[i]=p_lc_ktr_step_page->lc_ktr_step_pre_array[i];
	}

	update_preview_record();

	// Status

	display_status(TRUE,"SL calibration not yet completed");

	// Update summary display

//	p_lc_summary_page->update_summary_display();

	// Disable inactive controls

	GetDlgItem(IDC_DURATION)->EnableWindow(FALSE);
	GetDlgItem(IDC_INTER_TRIAL_INTERVAL)->EnableWindow(FALSE);
	GetDlgItem(IDC_LC_ABORT)->EnableWindow(FALSE);

	// Load Necessary modules
/*
	HDAP hdap0;
	BOOL status;
	char module_file_string[200];

	hdap0=DapHandleOpen("\\\\.\\Dap0",DAPOPEN_WRITE);
	DapLastErrorTextGet(display_string,200);
	display_status(TRUE,display_string);

	//DapInputFlush(hdap0);

	::set_executable_directory_string(module_file_string);
	strcat(module_file_string,"kentrigmovem.dlm");
	sprintf(module_file_string,"kentrigmovem.m");
	display_status(TRUE,module_file_string);

	status=DapModuleInstall(hdap0,"kentrigmovem.dlm",0,NULL);
	
	sprintf(display_string,"Status: %i",status);
	display_status(TRUE,display_string);
	if (~status)
	{
		DapLastErrorTextGet(display_string,200);
		MessageBox(display_string);
	}	
	
	DapHandleClose(hdap0);
*/
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// Message handlers

void Length_control_dialog::OnBnClickedRunExperiment()
{
	// Variables

	UpdateData(TRUE);

	int i;

	// Code

	UpdateData(TRUE);
	p_Calibration->UpdateData(TRUE);

	if (p_lc_freeform_page->lc_freeform_mode_status==0)
	{
		// Normal run, set according to tab controls

		// Fill local array values

		no_of_trials=p_lc_ramp_page->no_of_lc_ramp_trials;

		for (i=0;i<no_of_trials;i++)
		{
			lc_dialog_pre_ms_array[i]=p_lc_ramp_page->lc_ramp_pre_ms_array[i];
			lc_dialog_rel_size_array[i]=p_lc_ramp_page->lc_ramp_rel_size_array[i];
			lc_dialog_rel_time_array[i]=p_lc_ramp_page->lc_ramp_rel_time_array[i];
			lc_dialog_delay_array[i]=p_lc_ramp_page->lc_ramp_delay_array[i];
		}

		trials_left=no_of_trials;

		// Enable/disable appropriate controls

		GetDlgItem(IDC_Run_Experiment)->EnableWindow(FALSE);
		GetDlgItem(IDC_LC_ABORT)->EnableWindow(TRUE);

		// OnBnClickedRunExperiment spawns p_run_trial_thread which runs the trial
		// If we are running more than one successive trial, OnBnClickedRunExperiment also spawns
		// p_delay_thread which looks after the inter-trial waits. The user can click Abort which will stop
		// subsequent trials by setting trials_left to 0. Communication between threads is through
		// thread_status in the base class.
		
		thread_status=1;
		p_delay_thread = AfxBeginThread(length_control_dialog_delay_as_thread,this,THREAD_PRIORITY_NORMAL,0,0,NULL);
		p_run_trial_thread = AfxBeginThread(length_control_dialog_run_trial_thread,this,THREAD_PRIORITY_NORMAL,0,0,NULL);
	}
	else
	{
		// Initiate a sequence of freeform trials

		trials_left=no_of_trials;

		// Enable/disable appropriate controls

		GetDlgItem(IDC_Run_Experiment)->EnableWindow(FALSE);
		GetDlgItem(IDC_LC_ABORT)->EnableWindow(TRUE);

		// OnBnClickedRunExperiment spawns p_run_trial_thread which runs the trial
		// If we are running more than one successive trial, OnBnClickedRunExperiment also spawns
		// p_delay_thread which looks after the inter-trial waits. The user can click Abort which will stop
		// subsequent trials by setting trials_left to 0. Communication between threads is through
		// thread_status in the base class.

		thread_status=1;
		p_delay_thread = AfxBeginThread(length_control_dialog_delay_as_thread,this,THREAD_PRIORITY_NORMAL,0,0,NULL);
		p_run_trial_thread = AfxBeginThread(length_control_dialog_run_freeform_trial_thread,this,THREAD_PRIORITY_NORMAL,0,0,NULL);
	}
}

UINT length_control_dialog_run_trial_thread(LPVOID set_pointer)
{
	char display_string[100];

	Length_control_dialog* p_lc_dialog = (Length_control_dialog*) set_pointer;

	while (p_lc_dialog->trials_left>0)
	{
		if (p_lc_dialog->thread_status>0)
		{
			p_lc_dialog->display_status(TRUE,"Starting experiment");
			if (p_lc_dialog->run_single_trial()==0)
			{
				// Something went wrong with the DAPL upload
				p_lc_dialog->trials_left=0;
			}
			if (p_lc_dialog->trials_left>0)
			{
				// Signal trial has finished
				MessageBeep(MB_ICONEXCLAMATION);
			}
		}
		Sleep(100);
	}
	
	p_lc_dialog->display_status(TRUE,"Run trial thread finished");
	p_lc_dialog->display_status(TRUE,"All trials completed");
		
	// Enable/disable appropriate controls

	p_lc_dialog->GetDlgItem(IDC_Run_Experiment)->EnableWindow(TRUE);
	p_lc_dialog->GetDlgItem(IDC_LC_ABORT)->EnableWindow(FALSE);
	sprintf(display_string,"%i",p_lc_dialog->data_index);
	p_lc_dialog->GetDlgItem(IDC_DATA_INDEX)->SetWindowText(display_string);

	// Warn user

	MessageBeep(MB_ICONASTERISK);
		
	return TRUE;
}

UINT length_control_dialog_run_freeform_trial_thread(LPVOID set_pointer)
{
	char display_string[100];

	Length_control_dialog* p_lc_dialog = (Length_control_dialog*) set_pointer;

	while (p_lc_dialog->trials_left>0)
	{
		if (p_lc_dialog->thread_status>0)
		{
			p_lc_dialog->display_status(TRUE,"Starting freeform trial");
			if (p_lc_dialog->run_freeform_trial()==0)
			{
				// Something went wrong with the DAPL upload
				p_lc_dialog->trials_left=0;
			}
			if (p_lc_dialog->trials_left>0)
			{
				// Signal trial has finished
				MessageBeep(MB_ICONEXCLAMATION);
			}
		}
		Sleep(100);
	}

	p_lc_dialog->display_status(TRUE,"Freeform trial thread finished");
	p_lc_dialog->display_status(TRUE,"All freeform trials completed");

	// Enable/disable appropriate controls

	p_lc_dialog->GetDlgItem(IDC_Run_Experiment)->EnableWindow(TRUE);
	p_lc_dialog->GetDlgItem(IDC_LC_ABORT)->EnableWindow(FALSE);
	sprintf(display_string,"%i",p_lc_dialog->data_index);
	p_lc_dialog->GetDlgItem(IDC_DATA_INDEX)->SetWindowText(display_string);

	// Warn user

	MessageBeep(MB_ICONASTERISK);
		
	return TRUE;
}

UINT length_control_dialog_delay_as_thread(LPVOID set_pointer)
{
	// Delays as a separate thread

	// Variables

	char display_string[200];
	Length_control_dialog* p_lc_dialog = (Length_control_dialog*)set_pointer;

	// Code

	while (p_lc_dialog->trials_left>0)
	{
		if (p_lc_dialog->thread_status<0)
		{
			sprintf(display_string,"Pausing for %.1f s",p_lc_dialog->inter_trial_interval_s);
			p_lc_dialog->display_status(TRUE,display_string);
			Sleep(DWORD(p_lc_dialog->inter_trial_interval_s*1000.0));

			// Signal wait is over
			p_lc_dialog->thread_status=1;
		}

		Sleep(100);
	}

	p_lc_dialog->display_status(TRUE,"Delay thread finished");

	return 1;
}

int Length_control_dialog::run_single_trial(void)
{
	// Variables

	int i,j;
	int holder;

	short int short_int;

	char display_string[200];

	int buffer_size=lc_no_of_input_channels*data_points;					// number of points in
																			// data buffer being read from DAPL
																			// (adapts to the number of data channels)

	clock_t start_upload;													// time at start of DAPL upload

	short int* DAPL_data = new short int [buffer_size];						// data array for DAPL processing

	HDAP hdapSysGet,hdapSysPut,hdapBinGet;									// DAPL communication pipes

	// Code

	// Update trial variables

//	p_lc_ramp_page->lc_pre_ramp_ms=lc_pre_ramp_ms_array[no_of_trials-trials_left];
//	p_lc_ramp_page->lc_risetime_ms=lc_risetime_ms_array[no_of_trials-trials_left];

	// Update p_Preview_record

	update_preview_record();

	// Open and flush DAPL communication pipes

	hdapSysGet=DapHandleOpen("\\\\.\\Dap0\\$SysOut",DAPOPEN_READ);
	hdapSysPut=DapHandleOpen("\\\\.\\Dap0\\$SysIn",DAPOPEN_WRITE);
	hdapBinGet=DapHandleOpen("\\\\.\\Dap0\\$BinOut",DAPOPEN_READ);

	DapInputFlush(hdapSysGet);
	DapInputFlush(hdapBinGet);
	DapInputFlush(hdapSysPut);

	// Run experiment by uploading file to DAP

	start_upload=clock();

	if (!create_DAPL_source_file())
	{
		sprintf(display_string,"DAPL source file\n%s\ncould not be created",
			DAPL_source_file_string);
		MessageBox(display_string,"Length_control_dialog::run_single_trial()",MB_ICONWARNING);
		delete [] DAPL_data;
		return 0;
	}

	display_status(TRUE,"Passing command file to DAP");

	i=DapConfig(hdapSysPut,DAPL_source_file_string);

	// Error Checking

	if (i==FALSE)
	{
		sprintf(display_string,"DAPL could not handle source file\n%s\nResulting records will be invalid",
			DAPL_source_file_string);
		MessageBox(display_string,"Length_control_dialog::run_single_trial()",MB_ICONWARNING);
		delete [] DAPL_data;
		return 0;
	}

	sprintf(display_string,"Trial started, DAPL upload took %.4f s",(float)(clock()-start_upload)/CLOCKS_PER_SEC);
	display_status(TRUE,display_string);

	i=DapBufferGet(hdapBinGet,buffer_size*sizeof(short_int),DAPL_data);

	// Display status

	sprintf(display_string,"%i Points/Channel read from DAPL processor",(i/(4*sizeof(short_int))));
	display_status(TRUE,display_string);

	// Copy data to p_Length_control_record and update no_of_points

	holder=1;

	for (i=0;i<buffer_size;i=i+lc_no_of_input_channels)
	{
		p_Length_control_record->data[TIME][holder]=((float)(holder-1)*time_increment);
		p_Length_control_record->data[FORCE][holder]=convert_DAPL_units_to_volts(DAPL_data[i]);
		p_Length_control_record->data[SL][holder]=convert_DAPL_units_to_volts(DAPL_data[i+1]);
		p_Length_control_record->data[FL][holder]=convert_DAPL_units_to_volts(DAPL_data[i+2]);
		p_Length_control_record->data[INTENSITY][holder]=convert_DAPL_units_to_volts(DAPL_data[i+3]);
		// Cope with the scenario of multiple channels
		if (lc_no_of_input_channels>4)
		{
			for (j=5;j<=lc_no_of_input_channels;j++)
                p_Length_control_record->data[j][holder]=convert_DAPL_units_to_volts(DAPL_data[i+j-1]);
		}
		holder++;
	}

	p_Length_control_record->no_of_points=data_points;

	// Close communication pipes

	DapHandleClose(hdapSysGet);
	DapHandleClose(hdapSysPut);
	DapHandleClose(hdapBinGet);

	// Write data to file

	write_data_to_file();

	// Update data plots and display

	p_Experiment_display->clear_window();

	for (i=0;i<=(p_Experiment_display->no_of_plots-1);i++)
	{
		p_Experiment_display->pointers_array[i]->first_drawn_point=1;
		
		p_Experiment_display->pointers_array[i]->last_drawn_point=
			p_Length_control_record->no_of_points;
	}

	p_Experiment_display->update_plots();

	// Display

	sprintf(display_string,"pre_ramp_ms: %f  rt: %f",p_lc_ramp_page->lc_pre_ramp_ms,p_lc_ramp_page->lc_risetime_ms);
	display_status(TRUE,display_string);

	// Free memory and tidy up

	delete [] DAPL_data;

	trials_left=trials_left-1;

	// Update thread_status
	thread_status=-1;

	return 1;
}

int Length_control_dialog::run_freeform_trial(void)
{
	// Variables

	int i,j;
	int holder;

	short int short_int;

	char display_string[200];

	char DAPL_file_string[1000];

	int buffer_size=lc_freeform_total_sample_points[no_of_trials-trials_left];

	clock_t start_upload;													// time at start of DAPL upload

	short int* DAPL_data = new short int [buffer_size];						// data array for DAPL processing

	HDAP hdapSysGet,hdapSysPut,hdapBinGet;									// DAPL communication pipes

	// Code

	// Open and flush DAPL communication pipes

	hdapSysGet=DapHandleOpen("\\\\.\\Dap0\\$SysOut",DAPOPEN_READ);
	hdapSysPut=DapHandleOpen("\\\\.\\Dap0\\$SysIn",DAPOPEN_WRITE);
	hdapBinGet=DapHandleOpen("\\\\.\\Dap0\\$BinOut",DAPOPEN_READ);

	DapInputFlush(hdapSysGet);
	DapInputFlush(hdapBinGet);
	DapInputFlush(hdapSysPut);

	// Copy DAPL file to data folder with an index string
	sprintf_s(DAPL_file_string,1000,"%s\\Dapl_file_%i.dap",
		LC_data_path_string,data_index);

	CopyFile(lc_freeform_file_string[no_of_trials-trials_left],DAPL_file_string,FALSE);

	// Run experiment by uploading file to DAP

	start_upload=clock();

	sprintf(display_string,"Passing command file to DAP: %s",lc_freeform_file_string[no_of_trials-trials_left]);
	display_status(TRUE,display_string);

	// Config the DAP
	i=DapConfig(hdapSysPut,lc_freeform_file_string[no_of_trials-trials_left]);

	// Error Checking

	if (i==FALSE)
	{
		sprintf(display_string,"DAPL could not handle source file\n%s\nResulting records will be invalid",
			lc_freeform_file_string[no_of_trials-trials_left]);
		MessageBox(display_string,"Length_control_dialog::run_freeform_trial()",MB_ICONWARNING);
		delete [] DAPL_data;
		return 0;
	}

	sprintf(display_string,"Trial started, DAPL upload took %.4f s",(float)(clock()-start_upload)/CLOCKS_PER_SEC);
	display_status(TRUE,display_string);

	i=DapBufferGet(hdapBinGet,buffer_size*sizeof(short_int),DAPL_data);

	// Display status

	sprintf(display_string,"%i Points/Channel read from DAPL processor",(i/(4*sizeof(short_int))));
	display_status(TRUE,display_string);

	// Copy data to p_Length_control_record and update no_of_points

	holder=1;

	for (i=0;i<buffer_size;i=i+lc_freeform_no_of_channels[no_of_trials-trials_left])
	{
		p_Length_control_record->data[TIME][holder] = (holder-1)*lc_freeform_time_increment[no_of_trials-trials_left];

		for (j=1;j<=lc_freeform_no_of_channels[no_of_trials-trials_left];j++)
			p_Length_control_record->data[j][holder]=convert_DAPL_units_to_volts(DAPL_data[i+j-1]);

		holder++;
	}

	// Update no of points
	p_Length_control_record->no_of_points =
		(lc_freeform_total_sample_points[no_of_trials-trials_left]/
			lc_freeform_no_of_channels[no_of_trials-trials_left]);

	// Update other data
	sampling_rate = (1.0 / lc_freeform_time_increment[no_of_trials-trials_left]);
	data_points = p_Length_control_record->no_of_points;

	// Close communication pipes

	DapHandleClose(hdapSysGet);
	DapHandleClose(hdapSysPut);
	DapHandleClose(hdapBinGet);

	// Write data to file

	write_data_to_file();

	// Update data plots and display

	p_Experiment_display->clear_window();

	for (i=0;i<=(p_Experiment_display->no_of_plots-1);i++)
	{
		p_Experiment_display->pointers_array[i]->first_drawn_point=1;
		
		p_Experiment_display->pointers_array[i]->last_drawn_point=
			p_Length_control_record->no_of_points;
	}

	p_Experiment_display->update_plots();

	// Display

	sprintf(display_string,"Freeform trial successfully completed: %s",lc_freeform_file_string[no_of_trials-trials_left]);
	display_status(TRUE,display_string);

	// Free memory and tidy up

	delete [] DAPL_data;

	trials_left=trials_left-1;

	// Update thread_status
	thread_status=-1;

	return 1;
}

void Length_control_dialog::write_data_to_file(void)
{
	// Code writes data to file

	// Variables

	int i,j;

	char index_string[20];
	char display_string[FILE_INFO_STRING_LENGTH];
	char file_info_string[FILE_INFO_STRING_LENGTH];

	// Code

	// Create file string

	sprintf(index_string,"%i",data_index);
	output_data_file_string=LC_data_path_string+"\\"+data_base_string+index_string+".slc";

	// File handling
	
	FILE* output_data_file=fopen(output_data_file_string,"w");
	
	if (output_data_file==NULL)
	{
		sprintf(display_string,"Cannot open file\n%s\nData can not be saved to disk",
			output_data_file_string);
		MessageBox(display_string,"Length_Control_Dialog::write_data_to_file()",MB_ICONWARNING);
	}
	else
	{
		// Output
		fprintf(output_data_file,"SLCONTROL_DATA_FILE\n\n");
		
		// Version number

		fprintf(output_data_file,"_VERSION_\n");
		fprintf(output_data_file,"%s\n\n",SLCONTROL_VERSION_STRING);

		// Input channels

		fprintf(output_data_file,"_NO_OF_INPUT_CHANNELS_\n");
		fprintf(output_data_file,"No_of_input_channels: %i\n\n",lc_no_of_input_channels);

		// File info string
		sprintf(display_string,"%s",p_Calibration->Calibration_file_info_string);
		// Tidy up file info string
		j=0;
		for (i=0;i<=(int)strlen(display_string);i++)
		{
			if (i==(int)strlen(display_string)-1)
			{
				if (display_string[i]!='\n')
				{
					file_info_string[j]=display_string[i];
					j++;
				}
			}
			else
			{
				if (display_string[i]!='\r')
				{
					file_info_string[j]=display_string[i];
					j++;
				}
			}
		}

		fprintf(output_data_file,"_FILE_INFO_START_\n");
		fprintf(output_data_file,"%s\n",file_info_string);
		fprintf(output_data_file,"_FILE_INFO_STOP_\n\n");

		// Time stamp

		CTime record_time=CTime::GetCurrentTime();

		fprintf(output_data_file,"_RECORD_TIME_\n");
		fprintf(output_data_file,"Day %i Month %i Year %i Time %i : %i : %i\n\n",
			record_time.GetDay(),record_time.GetMonth(),record_time.GetYear(),
			record_time.GetHour(),record_time.GetMinute(),record_time.GetSecond());

		// Calibrations

		fprintf(output_data_file,"_CALIBRATIONS_\n");
 		fprintf(output_data_file,"FL_COMMAND_CALIBRATION: %g\n",FL_COMMAND_CALIBRATION);
		fprintf(output_data_file,"FL_RESPONSE_CALIBRATION: %g\n",FL_RESPONSE_CALIBRATION);
		fprintf(output_data_file,"FORCE_CALIBRATION: %g\n",FORCE_CALIBRATION);
		fprintf(output_data_file,"FL_POLARITY: %f\n",FL_POLARITY);

		fprintf(output_data_file,"SL_volts_to_FL_COMMAND_volts: %g\n\n",
			lc_dialog_SL_volts_to_FL_command_volts);

		// Dimensions

		fprintf(output_data_file,"_MUSCLE_DIMENSIONS_\n");
		fprintf(output_data_file,"Muscle_length_(µm): %.3e\n",p_Calibration->Calibration_fibre_length);
		fprintf(output_data_file,"Sarcomere_length_(nm): %.3e\n",p_Calibration->Calibration_sarcomere_length);
		fprintf(output_data_file,"Area_(m^2): %.3e\n\n",p_Calibration->Calibration_area);

		// Experiment parameters

		// Switch depending on freeform mode
		if (p_lc_freeform_page->lc_freeform_mode_status==0)
		{
			fprintf(output_data_file,"_LENGTH_CONTROL_\n");
			fprintf(output_data_file,"sampling_rate_(Hz): %.3e\n",sampling_rate);
			fprintf(output_data_file,"data_points: %i\n",data_points);
			fprintf(output_data_file,"triggered_points: %i\n",triggered_points);
			fprintf(output_data_file,"ktr_step_(µm): %.3e\n",
				lc_dialog_ktr_rel_size_array[no_of_trials-trials_left]*
					p_lc_ktr_step_page->ktr_step_page_size_microns);
			fprintf(output_data_file,"ktr_duration_(ms): %.3e\n",
				lc_dialog_ktr_rel_duration_array[no_of_trials-trials_left]*
					p_lc_ktr_step_page->ktr_step_page_duration_ms);
			fprintf(output_data_file,"pre_ktr_(ms): %.3e\n",
				lc_dialog_ktr_pre_array[no_of_trials-trials_left]);

			fprintf(output_data_file,"no_of_triangles: %i\n",
				p_lc_ramp_page->lc_no_of_stretches);
			fprintf(output_data_file,"triangle_halftime_(ms): %.3e\n",
				p_lc_ramp_page->lc_risetime_ms);
			fprintf(output_data_file,"inter_triangle_interval_(ms): %.3e\n",
				lc_dialog_delay_array[no_of_trials-trials_left]);
			fprintf(output_data_file,"relative_first_tri_size: %.3e\n",
				lc_dialog_rel_size_array[no_of_trials-trials_left]);
			fprintf(output_data_file,"relative_first_tri_time: %.3e\n",
				lc_dialog_rel_time_array[no_of_trials-trials_left]);
			fprintf(output_data_file,"pre_triangle_(ms): %.3e\n",
				lc_dialog_pre_ms_array[no_of_trials-trials_left]);
			fprintf(output_data_file,"ramp_mode: %i\n",
				p_lc_ramp_page->lc_ramp_mode);
			fprintf(output_data_file,"hold_mode: %i\n",
				p_lc_ramp_page->lc_hold_mode);
			fprintf(output_data_file,"hold_(ms): %.3e\n",
				p_lc_ramp_page->lc_hold_period_ms);
/*
		for (i=0;i<p_channel_control->no_of_tab_pages;i++)
		{
			if (p_channel_control->tab_pages[i]->channel_active==TRUE)
			{
				fprintf(output_data_file,"Trigger_channel: %i\n",i+1);
				fprintf(output_data_file,"Mode:%i\n",p_channel_control->tab_pages[i]->RADIO_CONTROL);

				switch (p_channel_control->tab_pages[i]->RADIO_CONTROL)
				{
					case 0:
					{
						fprintf(output_data_file,"Start_level: %i\n",p_channel_control->tab_pages[i]->start_level);
						fprintf(output_data_file,"Channel_flip: %f\n",p_channel_control->tab_pages[i]->channel_flip);
						fprintf(output_data_file,"Channel_duration: %f\n",p_channel_control->tab_pages[i]->channel_duration);
					}
					break;

					case 1:
					{
						fprintf(output_data_file,"Train_frequency: %f\n",p_channel_control->tab_pages[i]->train_frequency);
						fprintf(output_data_file,"Train_duty_cycle: %f\n",p_channel_control->tab_pages[i]->train_duty_cycle);
						fprintf(output_data_file,"Train_gate_on: %f\n",p_channel_control->tab_pages[i]->train_gate_on);
						fprintf(output_data_file,"Train_gate_off: %f\n",p_channel_control->tab_pages[i]->train_gate_off);
					}
					break;

					case 2:
					{
						fprintf(output_data_file,"Free_form_file: %s\n\n",p_channel_control->tab_pages[i]->free_form_file_string);
					}
					break;

					default:
					{
					}
					break;
				}
			}
		}
*/
		}
		else
		{
			fprintf(output_data_file,"_FREEFORM_MODE_\n");
			fprintf(output_data_file,"freeform_file: %s\n",
				lc_freeform_file_string[no_of_trials-trials_left]);
		}

		// Data

		fprintf(output_data_file,"\nData\n");

		if (lc_no_of_input_channels==4)
		{
			for (i=1;i<=p_Length_control_record->no_of_points;i++)
			{
				fprintf(output_data_file,"%f\t%f\t%f\t%f\t%f\n",
					p_Length_control_record->data[TIME][i],
					p_Length_control_record->data[FORCE][i],
					p_Length_control_record->data[SL][i],
					p_Length_control_record->data[FL][i],
					p_Length_control_record->data[INTENSITY][i]);
			}
		}
		else
		{
			// Cope with additional input channels

			for (i=1;i<=p_Length_control_record->no_of_points;i++)
			{
				fprintf(output_data_file,"%f\t",p_Length_control_record->data[TIME][i]);

				for (j=1;j<lc_no_of_input_channels;j++)
					fprintf(output_data_file,"%f\t",p_Length_control_record->data[j][i]);
				
				fprintf(output_data_file,"%f\n",p_Length_control_record->data[lc_no_of_input_channels][i]);
			}
		}

		// Close file

		fclose(output_data_file);

		// Update counter and screen

		data_index++;
		//UpdateData(FALSE);

		// Update status

		sprintf(display_string,"Record written to %s",output_data_file_string);
		display_status(TRUE,display_string);
	}
}


int Length_control_dialog::create_DAPL_source_file(void)
{
	// Code creates source file

	// Variables

	int i;
	FILE *source_file;

	// Code

	// File handling

	source_file=fopen(DAPL_source_file_string,"w");

	if (source_file==NULL)										// Error
	{
		return(0);
	}

	fprintf(source_file,"RESET\n\n");
	fprintf(source_file,"OPTION UNDERFLOWQ=OFF\n");
	fprintf(source_file,"OPTION ERRORQ=OFF\n");
	fprintf(source_file,"OPTION SCHEDULING=FIXED\n");
	fprintf(source_file,"OPTION BUFFERING=OFF\n");
	fprintf(source_file,"OPTION QUANTUM=100\n\n");

	fprintf(source_file,";************************************************************\n\n");

	// Branch for triggered capture
	if (p_lc_triggered_page->triggered_capture)
	{
		// Triggered capture

		fprintf(source_file,"// Header for triggered capture\n\n");
		fprintf(source_file,"pipes pfl\n");
		fprintf(source_file,"pipes pfl2\n");
		fprintf(source_file,"pipes ptrigger\n");
		fprintf(source_file,"triggers t\n\n");

		fprintf(source_file,";************************************************************\n\n");
	    
		::write_pipe_to_file(source_file,"pfl",p_triggered_FL,triggered_points);

		fprintf(source_file,";************************************************************\n\n");

		fprintf(source_file,"pdef create_pipes\n");
		fprintf(source_file,"  ptrigger=0\n");
		fprintf(source_file,"  kentrigmove(pfl,t,%i,pfl2)\n",triggered_points);
		fprintf(source_file,"end\n\n");

		fprintf(source_file,";************************************************************\n\n");

		fprintf(source_file,"pdef control\n");
		fprintf(source_file,"  limit(ip%i,outside,%i,%i,t,outside,%i,%i)\n\n",
			p_lc_triggered_page->lc_analog_trigger_channel,
			::convert_volts_to_DAPL_units(ADC_MIN),::convert_volts_to_DAPL_units(p_lc_triggered_page->lc_analog_threshold),
			::convert_volts_to_DAPL_units(ADC_MIN),::convert_volts_to_DAPL_units(ADC_MAX));
		fprintf(source_file,"  copy(ptrigger,op0)\n");
		fprintf(source_file,"  copy(pfl2,op1)\n\n");
		fprintf(source_file,"  merge(ip(0..%i),$binout)\n",lc_no_of_input_channels-1);
		fprintf(source_file,"end\n\n");

		fprintf(source_file,";************************************************************\n\n");

		fprintf(source_file,"odef outputproc 2\n");
		fprintf(source_file,"  set op0 a0\n");
		fprintf(source_file,"  set op1 a1\n");
		fprintf(source_file,"  time %i\n",(int)(0.5*time_increment*1e6));
		fprintf(source_file,"  count %i\n",2*triggered_points);
		fprintf(source_file,"end\n\n");
	}
	else
	{
		// Non-triggered capture

		fprintf(source_file,";************************************************************\n\n");

		fprintf(source_file,"// Header for non-triggered capture\n\n");

		fprintf(source_file,"pipes pfl MAXSIZE=65500\n");
		fprintf(source_file,"pipes ptrigger MAXSIZE=65500\n");
		fprintf(source_file,"pipes pout MAXSIZE=65500\n\n");

		fprintf(source_file,";************************************************************\n\n");

		fprintf(source_file,"fill ptrigger %i\n\n",convert_volts_to_DAPL_units((float)4.95));

		fprintf(source_file,";************************************************************\n\n");

		::write_pipe_to_file(source_file,"pfl",p_Preview_record->data[FL],data_points);

		fprintf(source_file,";************************************************************\n\n");

		fprintf(source_file,"pdef create_pipes\n\n");
		fprintf(source_file,"  ptrigger=0\n");
		fprintf(source_file,"end\n\n");

		fprintf(source_file,";************************************************************\n\n");

		fprintf(source_file,"pdef control\n\n");
		fprintf(source_file,"  pout=pfl\n");
		fprintf(source_file,"  copy(pout,op1)\n");
		fprintf(source_file,"  copy(ptrigger,op0)\n\n");
		fprintf(source_file,"  merge(ip(0..%i),$binout)\n",lc_no_of_input_channels-1);
		fprintf(source_file,"end\n\n");

		fprintf(source_file,";************************************************************\n\n");
	    
		fprintf(source_file,"odef outputproc 2\n");
		fprintf(source_file,"  outputwait 2\n");
		fprintf(source_file,"  update burst\n");
		fprintf(source_file,"  set op0 a0\n");
		fprintf(source_file,"  set op1 a1\n");
		fprintf(source_file,"  time %i\n",(int)(0.5*time_increment*1e6));
		//fprintf(source_file,"  count %i\n",2*triggered_points);
		fprintf(source_file,"end\n\n");
	}

	fprintf(source_file,";************************************************************\n\n");

	fprintf(source_file,"idef inputproc %i\n",lc_no_of_input_channels);
	for (i=0;i<=lc_no_of_input_channels-1;i++)
	{
		fprintf(source_file,"  set ip%i s%i\n",i,i);
	}
	fprintf(source_file,"  count %i\n",lc_no_of_input_channels*data_points);
	fprintf(source_file,"  time %i\n",(int)(time_increment*1e6/lc_no_of_input_channels));
	fprintf(source_file,"end\n\n");

	fprintf(source_file,";************************************************************\n\n");

	fprintf(source_file,"start create_pipes,control,inputproc,outputproc\n\n");

	fclose(source_file);

	return 1;
}

void Length_control_dialog::OnBnClickedTest()
{
	// Aborts trials
	trials_left=0;
}

// Message Handlers and associated code

int Length_control_dialog::check_trial_status()
{
	int i;

	lc_trials=IMAX(p_lc_ramp_page->no_of_lc_ramp_trials,p_lc_ktr_step_page->no_of_ktr_step_trials);

	for (i=0;i<LC_MAX_TRIALS;i++)
	{
		// Ramp arrays
		lc_dialog_pre_ms_array[i]=p_lc_ramp_page->lc_ramp_pre_ms_array[i];
		lc_dialog_rel_size_array[i]=p_lc_ramp_page->lc_ramp_rel_size_array[i];
		lc_dialog_rel_time_array[i]=p_lc_ramp_page->lc_ramp_rel_time_array[i];
		lc_dialog_delay_array[i]=p_lc_ramp_page->lc_ramp_delay_array[i];

		// Ktr arrays
		lc_dialog_ktr_rel_size_array[i]=p_lc_ktr_step_page->lc_ktr_step_rel_size_array[i];
		lc_dialog_ktr_rel_duration_array[i]=p_lc_ktr_step_page->lc_ktr_step_rel_duration_array[i];
		lc_dialog_ktr_pre_array[i]=p_lc_ktr_step_page->lc_ktr_step_pre_array[i];
	}

	update_preview_record();

	if ((p_lc_ramp_page->no_of_lc_ramp_trials>1)||(p_lc_ktr_step_page->no_of_ktr_step_trials>1))
	{
		GetDlgItem(IDC_INTER_TRIAL_INTERVAL)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_INTER_TRIAL_INTERVAL)->EnableWindow(FALSE);
	}

	return 1;
}

void Length_control_dialog::OnEnChangeDataPoints()
{
	UpdateData(TRUE);

	update_preview_record();
}

void Length_control_dialog::OnCbnSelchangeSamplingRate()
{
	UpdateData(TRUE);

	update_preview_record();
}

void Length_control_dialog::update_preview_record(void)
{
	// Variables

	char* p_string;
	char temp_string[100];
	int temp;
	CString value_string;
	int i;

	float ramp_multiplier;
	float hold_multiplier;
	float tri_start_s[5];
	int tri_start_points[5];
	int tri_stop_points[5];
	int ramp_stop_points[5];

	float triggered_control_time_s;

	// Update values from daughter windows

	if (trials_left==0)
	{
		// Update preview array - if we're not in this loop, arrays have already
		// been updated by RunExperiment()

		i=0;
		lc_dialog_pre_ms_array[i]=p_lc_ramp_page->lc_ramp_pre_ms_array[i];
		lc_dialog_rel_size_array[i]=p_lc_ramp_page->lc_ramp_rel_size_array[i];
		lc_dialog_rel_time_array[i]=p_lc_ramp_page->lc_ramp_rel_time_array[i];
		lc_dialog_delay_array[i]=p_lc_ramp_page->lc_ramp_delay_array[i];
	}

	// Store local values for next record

	// Ramp values
	float local_pre_ms=lc_dialog_pre_ms_array[no_of_trials-trials_left];
	float local_rel_size=lc_dialog_rel_size_array[no_of_trials-trials_left];
	float local_rel_time=lc_dialog_rel_time_array[no_of_trials-trials_left];
	float local_delay_ms=lc_dialog_delay_array[no_of_trials-trials_left];
	float local_risetime_ms=p_lc_ramp_page->lc_risetime_ms;
	float local_hold_ms=p_lc_ramp_page->lc_hold_period_ms;
	float local_no_of_stretches=(float)(p_lc_ramp_page->lc_no_of_stretches);

	// Ktr values
	float local_ktr_duration_ms=lc_dialog_ktr_rel_duration_array[no_of_trials-trials_left]*
									p_lc_ktr_step_page->ktr_step_page_duration_ms;
	float local_ktr_size_microns=lc_dialog_ktr_rel_size_array[no_of_trials-trials_left]*
									p_lc_ktr_step_page->ktr_step_page_size_microns;
	float local_ktr_pre_ms=lc_dialog_ktr_pre_array[no_of_trials-trials_left];

	
	// Code

//	UpdateData(TRUE);

	// Error checking
	if (data_points>65000)
	{
		data_points=65000;
		sprintf(temp_string,"65000");
		GetDlgItem(IDC_DATA_POINTS)->SetWindowText(temp_string);
	}

	// Update values

	temp=SAMPLING_RATE.GetCurSel();
	SAMPLING_RATE.GetLBText(temp,value_string);
	time_increment=((float)(1.0/strtod(value_string,&p_string)));

	duration_seconds=(float)(data_points*time_increment);
	sprintf(temp_string,"%.3f",duration_seconds);
	duration_string=temp_string;
	GetDlgItem(IDC_DURATION)->SetWindowText(duration_string);

	// Update preview_record

	// First of all do time points

	for (i=1;i<=data_points;i++)
	{
		p_Preview_record->data[TIME][i]=(float)(i*time_increment);
	}

	// Now generate p_Preview_record->data[FL]
	// If we are going to do triggered capture, we'll copy that to p_triggered_FL at the end
	// and reset the p_Preview record

	// Set ramp_mulitplier and hold_multiplier
	// These allow us to generate different sorts of waveforms with one set of code

	if (p_lc_ramp_page->lc_ramp_mode==0)
		ramp_multiplier=(float)2.0;
	else
	{
		ramp_multiplier=(float)1.0;

		if (p_lc_ramp_page->lc_hold_mode==0)
			hold_multiplier=(float)0.0;
		else
			hold_multiplier=(float)1.0;
	}

	// First of all calculate the necessary time points
	// Set ktr_initiation_time and tri_start, tri_stop as required

	if (p_lc_ramp_page->lc_no_of_stretches==0)
		ktr_initiation_time_s=(float)(0.001*local_ktr_pre_ms);
	else
	{
		ktr_initiation_time_s=(float)(0.001*
			(local_pre_ms+
				(ramp_multiplier*local_rel_time*local_risetime_ms)+
				(ramp_multiplier*(local_no_of_stretches-1)*local_risetime_ms)+
				(hold_multiplier*local_no_of_stretches*local_hold_ms)+
				((local_no_of_stretches-1)*local_delay_ms)+
				local_ktr_pre_ms));

		for (i=1;i<=p_lc_ramp_page->lc_no_of_stretches;i++)
		{
			if (i==1)
				tri_start_s[i]=(float)(0.001*local_pre_ms);
			else
                tri_start_s[i]=tri_start_s[1]+
					(float)(0.001*((local_rel_time*local_risetime_ms*ramp_multiplier)+
						(i-1)*(local_delay_ms+(hold_multiplier*local_hold_ms))+
						(i-2)*(ramp_multiplier*local_risetime_ms)));

			tri_start_points[i]=nearest_integer(tri_start_s[i]/time_increment);

			if (i==1)
				tri_stop_points[i]=nearest_integer((tri_start_s[i]+
					ramp_multiplier*(float)0.001*local_rel_time*local_risetime_ms)/time_increment);
			else
				tri_stop_points[i]=nearest_integer((tri_start_s[i]+
					ramp_multiplier*(float)0.001*local_risetime_ms)/time_increment);

			ramp_stop_points[i]=nearest_integer((tri_start_s[i]+
				ramp_multiplier*(float)0.001*local_risetime_ms+
				hold_multiplier*(float)0.001*p_lc_ramp_page->lc_hold_period_ms)/time_increment);
		}
	}

	ktr_initiation_points=nearest_integer(ktr_initiation_time_s/time_increment);

	// Now do the amplitudes
	// First reset the record

	for (i=1;i<=data_points;i++)
	{
		p_Preview_record->data[FL][i]=(float)0.0;
	}

	// Next the stretches

	for (i=1;i<=p_lc_ramp_page->lc_no_of_stretches;i++)
	{
		if (p_lc_ramp_page->lc_ramp_mode==0)
		{
			// Ramp and potential hold

			if (i==1)
			{
				// Potentially different sized first one
				generate_waveform(TRIANGLE,p_Preview_record->data[FL],tri_start_points[i],tri_stop_points[i],
					(float)(2*(tri_stop_points[i]-tri_start_points[i])+1),
					(float)(local_rel_size*::convert_volts_to_DAPL_units(
						::convert_FL_COMMAND_microns_to_volts(p_lc_ramp_page->lc_stretch_size_microns))));
			}
			else
			{
				generate_waveform(TRIANGLE,p_Preview_record->data[FL],tri_start_points[i],tri_stop_points[i],
					(float)(2*(tri_stop_points[i]-tri_start_points[i])+1),
					(float)(::convert_volts_to_DAPL_units(
						::convert_FL_COMMAND_microns_to_volts(p_lc_ramp_page->lc_stretch_size_microns))));
			}
		}
		else
		{
			// Straightforward triangles

			if (i==1)
			{
				// Potentially different sized first one
				generate_waveform(TRIANGLE,p_Preview_record->data[FL],tri_start_points[i],tri_stop_points[i],
					(float)(4*(tri_stop_points[i]-tri_start_points[i])+1),
					(float)(local_rel_size*::convert_volts_to_DAPL_units(
						::convert_FL_COMMAND_microns_to_volts(p_lc_ramp_page->lc_stretch_size_microns))));

				generate_waveform(CONSTANT,p_Preview_record->data[FL],tri_stop_points[i],ramp_stop_points[i],(float)1,
					(float)(local_rel_size*::convert_volts_to_DAPL_units(
						(float)::convert_FL_COMMAND_microns_to_volts(
							p_lc_ramp_page->lc_stretch_size_microns))));
			}
			else
			{
				generate_waveform(TRIANGLE,p_Preview_record->data[FL],tri_start_points[i],tri_stop_points[i],
					(float)(4*(tri_stop_points[i]-tri_start_points[i])+1),
					(float)(::convert_volts_to_DAPL_units(
						::convert_FL_COMMAND_microns_to_volts(p_lc_ramp_page->lc_stretch_size_microns))));

				generate_waveform(CONSTANT,p_Preview_record->data[FL],tri_stop_points[i],ramp_stop_points[i],(float)1,
					(float)::convert_volts_to_DAPL_units(
						(float)::convert_FL_COMMAND_microns_to_volts(
							p_lc_ramp_page->lc_stretch_size_microns)));
			}

		}
	}

	// Now the ktr amplitude

	generate_waveform(CONSTANT,p_Preview_record->data[FL],ktr_initiation_points,
		ktr_initiation_points+nearest_integer((float)(local_ktr_duration_ms*0.001/time_increment)),(float)1,
		(float)::convert_volts_to_DAPL_units(::convert_FL_COMMAND_microns_to_volts(local_ktr_size_microns)));
		
	// Now do the necessary manipulations if it's a captured record

	if (p_lc_triggered_page->triggered_capture==1)
	{
		// Triggered Capture
		
		// First copy the p_Preview_record->data[FL] to p_triggered_FL and reset p_Preview_record

		for (i=1;i<=data_points;i++)
		{
			p_triggered_FL[i]=p_Preview_record->data[FL][i];
			p_Preview_record->data[FL][i]=(float)0.0;
		}
	
		// Set necessary parameters

		triggered_control_time_s=(float)(ktr_initiation_time_s+(float)(0.001*local_ktr_duration_ms));
		triggered_points=nearest_integer(triggered_control_time_s/time_increment)+1;
		triggered_points=IMIN(triggered_points,data_points);
	}
	else
	{
		// It's not triggered

		for (i=1;i<=data_points;i++)
		{
			p_triggered_FL[i]=0;
		}
		triggered_control_time_s=(float)0.0;
		triggered_points=0;
	}
		
	// Finished, so update the plot

	update_preview_plot();
}

void Length_control_dialog::update_preview_plot(void)
{
	// Variables

	int i;

	float min_p_triggered_FL;
	float max_p_triggered_FL;

	CClientDC dc(this);
	CClientDC* p_display_dc;
	p_display_dc =& dc;

	// Code

	if (triggered_points==0)
	{
		p_Preview_plot->draw_plot(p_display_dc,1,data_points);
	}
	else
	{
		// Deduce min and max triggered control points

		for (i=1;i<=triggered_points;i++)
		{
			if (i==1)
			{
				min_p_triggered_FL=p_triggered_FL[i];
				max_p_triggered_FL=p_triggered_FL[i];
			}

			if (p_triggered_FL[i]<min_p_triggered_FL)
				min_p_triggered_FL=p_triggered_FL[i];
			if (p_triggered_FL[i]>max_p_triggered_FL)
				max_p_triggered_FL=p_triggered_FL[i];
		}

		// Compare them to the normal plot and rescale

		// Y-axis

		if (min_p_triggered_FL==max_p_triggered_FL)
		{
			p_Preview_plot->y_min=min_p_triggered_FL-(float)0.1;
			p_Preview_plot->y_max=max_p_triggered_FL+(float)0.1;
		}
		else
		{
			p_Preview_plot->y_min=min_p_triggered_FL;
			p_Preview_plot->y_max=max_p_triggered_FL;
		}

		p_Preview_plot->y_scale=p_Preview_plot->y_length/
			(p_Preview_plot->y_max-p_Preview_plot->y_min);

		// X axis

		p_Preview_plot->x_min=p_Preview_record->data[TIME][1];
		p_Preview_plot->x_max=p_Preview_record->data[TIME][data_points];
		p_Preview_plot->x_scale=p_Preview_plot->x_length/
			(p_Preview_plot->x_max-p_Preview_plot->x_min);
		
		// Now re-draw sampling trace

		p_Preview_plot->draw_plot(p_display_dc,1,data_points,0);

		// Now re-draw p_triggered_FL

		CPen* pOriginalPen;
		CPen TracePen;
		TracePen.CreatePen(PS_SOLID,1,RGB(0,0,255));
		pOriginalPen = p_display_dc->SelectObject(&TracePen);

		p_display_dc->MoveTo(p_Preview_plot->x_origin,
			(p_Preview_plot->y_origin-(int)((p_triggered_FL[1]-p_Preview_plot->y_min)*p_Preview_plot->y_scale)));

		for (i=1;i<=triggered_points;i++)
		{
			p_display_dc->LineTo((p_Preview_plot->x_origin+(int)(p_Preview_record->data[TIME][i]*p_Preview_plot->x_scale)),
				(p_Preview_plot->y_origin-(int)((p_triggered_FL[i]-p_Preview_plot->y_min)*p_Preview_plot->y_scale)));
		}
		
		p_display_dc->SelectObject(pOriginalPen);
	}
}

void Length_control_dialog::display_status(int update_mode,CString new_string)
{
	// Scrolls new status messages to screen

	// Variables

	char display_string[300];
	int display_length=65;

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

		if (new_string.GetLength()>display_length)
		{
			sprintf(display_string,"%i) ... %s",status_counter,new_string.Right(display_length-10));
		}
		else
		{
			sprintf(display_string,"%i) %s",status_counter,new_string);
		}
        status_bottom_string=display_string;

		status_counter++;
	}
	//MessageBox(status_bottom_string);

	dc.TextOut((int)(0.52*current_window.Width()),(int)(0.773*current_window.Height()),status_top_string.Right(75));
	dc.TextOut((int)(0.52*current_window.Width()),(int)(0.813*current_window.Height()),status_middle_string.Right(75));
	dc.TextOut((int)(0.52*current_window.Width()),(int)(0.853*current_window.Height()),status_bottom_string.Right(75));
}

// Buttons

void Length_control_dialog::OnBnClickedPathBrowse(void)
{
	// Allows user to browse for filing path

	CString strInitialPath;
	CString strYourCaption(_T("Directory Browser"));
	CString strYourTitle(_T("Select path for data filing (or create a new directory by\ntyping in the edit box and clicking 'Ok')"));

	CPathDialog dlg(strYourCaption, strYourTitle, strInitialPath);

	if (dlg.DoModal()==IDOK)
	{
		LC_data_path_string=dlg.GetPathName();
	}

	UpdateData(FALSE);
}

// Utilities

void Length_control_dialog::OnPaint()
{
	CPaintDC dc(this);			// device context for painting

	update_preview_plot();

	display_status(FALSE,"");
}