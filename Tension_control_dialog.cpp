// Tension_control_dialog.cpp : implementation file
//

#include "stdafx.h"
#include "slcontrol.h"
#include "Tension_control_dialog.h"
#include "SLControlDlg.h"

////////////////////////////
// Ken's code starts here //
////////////////////////////

#include <math.h>

#include "dapio32.h"
#include "Calibration.h"
#include "global_functions.h"
#include "global_definitions.h"

extern float FL_POLARITY;
extern float FL_RESPONSE_CALIBRATION;
extern float FL_COMMAND_CALIBRATION;
extern float FORCE_CALIBRATION;
extern char CALIBRATION_DATE_STRING[100];
extern int FL_OR_SL_CHANNEL_MODE;
extern char SLCONTROL_VERSION_STRING[100];

////////////////////////////
//  Ken's code ends here  //
////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Tension_control_dialog dialog


Tension_control_dialog::Tension_control_dialog(CWnd* pParent /*=NULL*/)
	: CDialog(Tension_control_dialog::IDD, pParent)
{
	m_pParent=pParent;
	m_nID=Tension_control_dialog::IDD;

	//{{AFX_DATA_INIT(Tension_control_dialog)
	data_points = 0;
	isotonic_level_volts = 0.0f;
	proportional_gain = 0.0f;
	sampling_rate_string = _T("");
	duration_string = _T("");
	ktr_step_size_microns = 0.0f;
	ktr_step_duration_ms = 0.0f;
	pre_servo_ms = 0.0f;
	post_ktr_ms_string = _T("");
	servo_ms = 0.0f;
	data_index = 0;
	data_base_string = _T("");
	data_path_string = _T("");
	SL_volts_to_FL_microns_calibration = 0.0f;
	SL_volts_to_FL_COMMAND_volts = 0.0f;
	batch_file_string = _T("");
	inter_trial_interval_seconds = 0.0f;
	experiment_mode_string = _T("");
	triangle_halftime_ms = 0.0f;
	triangle_size_volts = 0.0f;
	no_of_triangles_string = _T("");
	post_triangle_ms = 0.0f;
	pre_triangle_ms = 0.0f;
	pre_tension_servo_points = 0;
	inter_pull_interval_ms = 0.0f;
	ramp_increment_option = FALSE;
	ramp_option = FALSE;
	integral_gain = 0.0f;
	//}}AFX_DATA_INIT

	// Variables

	char* p_string;											// used for screen display
	char temp_string[100];
	
	char path_string[_MAX_PATH];
	
	// Code

	// Related to closing SL_SREC window

	p_Tension_control_dialog = this;

	// Create pointer to data record

	p_Tension_control_record =& Tension_control_record;

	// Set file strings for DAPL_source_file

	::set_executable_directory_string(path_string);
	strcat(path_string,"\\Tension_control\\");

	Tension_control_dir_path=path_string;

	DAPL_source_file_string=Tension_control_dir_path+"dapl_source.txt";
	header_file_string=Tension_control_dir_path+"Tension_control_header.txt";
	fill_pipes_file_string=Tension_control_dir_path+"Tension_control_fill_pipes.txt";
	create_pipes_file_string=Tension_control_dir_path+"Tension_control_create_pipes.txt";
	control_file_string=Tension_control_dir_path+"Tension_control_control.txt";
	io_procedures_file_string=Tension_control_dir_path+"Tension_control_io_procedures.txt";

	// Tension_control display

	sampling_rate_string="1000";

	data_points=300;

	isotonic_level_volts=(float)0.1;

	proportional_gain=(float)0.001;
	integral_gain=(float)0.001;

	pre_servo_ms=(float)100.0;
	servo_ms=(float)100.0;

	ktr_step_size_microns=(float)-20.0;
	ktr_step_duration_ms=(float)20.0;

	// Triangle parameters

	triangle_halftime_ms=(float)100.0;
	triangle_size_volts=(float)0.1;
	no_of_triangles_string="0";
	no_of_triangles=0;
	post_triangle_ms=(float)100.0;
	pre_triangle_ms =(float)100.0;
	pre_tension_servo_points=50;
	inter_pull_interval_ms=(float)100.0;

	// Set sampling parameters

	time_increment=(float)(1.0/strtod(sampling_rate_string,&p_string));
	duration_seconds=(float)(data_points*time_increment);
	sprintf(temp_string,"%.3f",duration_seconds);
	duration_string=temp_string;

	// Post_ktr_ms

	post_ktr_ms=((duration_seconds*(float)1000.0)-(pre_servo_ms+servo_ms+ktr_step_duration_ms));
	sprintf(temp_string,"%.0f",post_ktr_ms);
	post_ktr_ms_string=temp_string;

	// Data file parameters

	data_path_string="d:\\temp";
	data_base_string="test";
	data_index=1;

	// Batch parameters

	force_velocity_file_string="c:\\temp\\for_vel.txt";
	batch_pulls_file_string="c:\\temp\\batch_pulls.txt";
	batch_velocity_file_string="c:\\temp\\batch_velocity.txt";

	experiment_mode_string="Single_Hold";
	experiment_mode=SINGLE_HOLD;
	batch_file_string=force_velocity_file_string;
	inter_trial_interval_seconds=(float)3.0;

	// Ramp options

	ramp_option=FALSE;
	ramp_mode=0;
	ramp_increment_option=FALSE;
	ramp_increment_mode=0;

	ramp_multiplier=(float)2.0;

	// Status

	status_middle_string="1) Tension_control dialog initialised";
	status_bottom_string="2) SL Calibration not yet completed";
	status_counter=3;
}

Tension_control_dialog::~Tension_control_dialog()
{

}

void Tension_control_dialog::PostNcDestroy() 
{
	delete this;
}

BOOL Tension_control_dialog::Create()
{
	return CDialog::Create(m_nID, m_pParent);
}

void Tension_control_dialog::remote_shut_down(void)
{
	// Called by parent to shut down window

	OnClose();
}

void Tension_control_dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Tension_control_dialog)
	DDX_Control(pDX, IDC_NO_OF_TRIANGLES, no_of_triangles_control);
	DDX_Control(pDX, IDC_EXPT_MODE, EXPERIMENT_MODE_CONTROL);
	DDX_Control(pDX, IDC_DURATION_CONTROL, DURATION_CONTROL);
	DDX_Control(pDX, IDC_SAMPLING_RATE, SAMPLING_RATE_CONTROL);
	DDX_Text(pDX, IDC_DATA_POINTS, data_points);
	DDV_MinMaxInt(pDX, data_points, 1, 65000);
	DDX_Text(pDX, IDC_ISOTONIC_LEVEL, isotonic_level_volts);
	DDX_Text(pDX, IDC_GAIN, proportional_gain);
	DDX_CBString(pDX, IDC_SAMPLING_RATE, sampling_rate_string);
	DDX_Text(pDX, IDC_DURATION_CONTROL, duration_string);
	DDX_Text(pDX, IDC_KTR_STEP_SIZE, ktr_step_size_microns);
	DDX_Text(pDX, IDC_KTR_STEP_DURATION, ktr_step_duration_ms);
	DDX_Text(pDX, IDC_PRE_SERVO, pre_servo_ms);
	DDX_Text(pDX, IDC_POST_KTR, post_ktr_ms_string);
	DDX_Text(pDX, IDC_SERVO, servo_ms);
	DDX_Text(pDX, IDC_DATA_INDEX, data_index);
	DDX_Text(pDX, IDC_DATA_BASE, data_base_string);
	DDX_Text(pDX, IDC_DATA_PATH, data_path_string);
	DDX_Text(pDX, IDC_SL_volts_to_FL_microns, SL_volts_to_FL_microns_calibration);
	DDX_Text(pDX, IDC_SL_volts_to_FL_COMMAND_volts, SL_volts_to_FL_COMMAND_volts);
	DDX_Text(pDX, IDC_BATCH_FILE, batch_file_string);
	DDX_Text(pDX, IDC_INTER_TRIAL_INTERVAL, inter_trial_interval_seconds);
	DDX_CBString(pDX, IDC_EXPT_MODE, experiment_mode_string);
	DDX_Text(pDX, IDC_TRI_HALFTIME, triangle_halftime_ms);
	DDX_Text(pDX, IDC_TRIANGLE_SIZE, triangle_size_volts);
	DDX_CBString(pDX, IDC_NO_OF_TRIANGLES, no_of_triangles_string);
	DDX_Text(pDX, IDC_POST_TRI, post_triangle_ms);
	DDX_Text(pDX, IDC_PRE_TRI, pre_triangle_ms);
	DDX_Text(pDX, IDC_PRE_TENSION_SERVO, pre_tension_servo_points);
	DDX_Text(pDX, IDC_INTER_PULL_INTERVAL, inter_pull_interval_ms);
	DDX_Check(pDX, IDC_RAMP_INCREMENT_CONTROL, ramp_increment_option);
	DDX_Check(pDX, IDC_RAMP_CONTROL, ramp_option);
	DDX_Text(pDX, IDC_INTEGRAL_GAIN, integral_gain);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(Tension_control_dialog, CDialog)
	//{{AFX_MSG_MAP(Tension_control_dialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_Run_Experiment, OnRunExperiment)
	ON_CBN_SELCHANGE(IDC_SAMPLING_RATE, OnSelchangeSamplingRate)
	ON_EN_CHANGE(IDC_DATA_POINTS, OnChangeDataPoints)
	ON_EN_CHANGE(IDC_KTR_STEP_DURATION, OnChangeKtrStepDuration)
	ON_EN_CHANGE(IDC_SERVO, OnChangeServo)
	ON_EN_CHANGE(IDC_PRE_SERVO, OnChangePreServo)
	ON_BN_CLICKED(IDC_SL_to_FL_Calibration_Override, OnSLtoFLCalibrationOverride)
	ON_CBN_SELCHANGE(IDC_EXPT_MODE, OnSelchangeExptMode)
	ON_CBN_SELCHANGE(IDC_NO_OF_TRIANGLES, OnSelchangeNoOfTriangles)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_RAMP_CONTROL, OnRampControl)
	ON_EN_CHANGE(IDC_PRE_TRI, OnChangePreTri)
	ON_EN_CHANGE(IDC_POST_TRI, OnChangePostTri)
	ON_EN_CHANGE(IDC_INTER_PULL_INTERVAL, OnChangeInterPullInterval)
	ON_EN_CHANGE(IDC_TRI_HALFTIME, OnChangeTriHalftime)
//	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	ON_BN_CLICKED(IDC_RAMP_INCREMENT_CONTROL, OnRampIncrementControl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Tension_control_dialog message handlers

BOOL Tension_control_dialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	// Set window size and position

	int width=1022;
	int height=740;
	
	SetWindowPos(NULL,(int)(0.02*width),(int)(0.07*height),
		(int)(0.63*width),(int)(0.43*height),SWP_SHOWWINDOW);

	// Disable appropriate controls

	GetDlgItem(IDC_DURATION_CONTROL)->EnableWindow(FALSE);
	GetDlgItem(IDC_POST_KTR)->EnableWindow(FALSE);

	GetDlgItem(IDC_BATCH_FILE)->EnableWindow(FALSE);
	GetDlgItem(IDC_INTER_TRIAL_INTERVAL)->EnableWindow(FALSE);

	GetDlgItem(IDC_SL_volts_to_FL_COMMAND_volts)->EnableWindow(FALSE);
	GetDlgItem(IDC_SL_volts_to_FL_microns)->EnableWindow(FALSE);

	GetDlgItem(IDC_TRI_HALFTIME)->EnableWindow(FALSE);
	GetDlgItem(IDC_TRIANGLE_SIZE)->EnableWindow(FALSE);
	GetDlgItem(IDC_NO_OF_TRIANGLES)->EnableWindow(FALSE);
	GetDlgItem(IDC_POST_TRI)->EnableWindow(FALSE);
	GetDlgItem(IDC_PRE_TRI)->EnableWindow(FALSE);
	GetDlgItem(IDC_PRE_TENSION_SERVO)->EnableWindow(FALSE);
	GetDlgItem(IDC_INTER_PULL_INTERVAL)->EnableWindow(FALSE);
	GetDlgItem(IDC_RAMP_CONTROL)->EnableWindow(FALSE);
	GetDlgItem(IDC_RAMP_INCREMENT_CONTROL)->EnableWindow(FALSE);

	// Update status

	display_status(TRUE,"Ready for input");

	// Create the Calibration window

	p_Calibration = new Calibration();
	p_Calibration->Create();

	// Create Experiment_display window

	p_Experiment_display = new Experiment_display(this,p_Tension_control_record);
	p_Experiment_display->Create();


	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void Tension_control_dialog::OnClose()
{
	// Close derived windows

	p_Calibration->close_calibration_window();
	p_Calibration=NULL;

	p_Experiment_display->close_experiment_display_window();
	p_Experiment_display=NULL;

	// Signal to parent that this window is closing

	((CSLControlDlg*)m_pParent)->Tension_control_Window_Done();

	DestroyWindow();
}

BOOL Tension_control_dialog::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class

	// Traps escape key presses to prevent dialog box closing

	if (pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE)
	{
		return TRUE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void Tension_control_dialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Redraws status messages

	display_status(FALSE,"");

	// Do not call CDialog::OnPaint() for painting messages
}

void Tension_control_dialog::OnSLtoFLCalibrationOverride() 
{
	// Retrieve data from Calibration window and update local value

	p_Calibration->UpdateData(TRUE);

	// Update local values

	SL_volts_to_FL_microns_calibration=
		p_Calibration->Calibration_SL_volts_to_FL_microns_calibration;

	SL_volts_to_FL_COMMAND_volts=
		(FL_POLARITY*SL_volts_to_FL_microns_calibration*FL_COMMAND_CALIBRATION);

	// Update status

	display_status(TRUE,"SL calibrated");

	// Update screen

	UpdateData(FALSE);
}

void Tension_control_dialog::OnRunExperiment() 
{
	// Code runs Tension_control experiment based on parameter values set by user
	// in dialog window

	// Variables

	int i;																	// counter

	int no_of_trials;														// no of trials to perform

	float relative_tension[101];											// relative tension for
																			// each trial

	float batch_interval[101];												// inter_pull interval for
																			// each trial (ms)

	float relative_velocity[101];											// relative velocity for
																			// each trial

	float relative_gain[101];												// relative gain for force_velocity
																			// trials

	float initial_isotonic_level_volts;										// initial isotonic hold level
	float initial_inter_pull_interval_ms;									// initial inter_pull_interval_ms
	float initial_proportional_gain;										// initial proportional_gain
	float initial_rise_time;												// initial rise time

	clock_t start;															// time at end of individual
																			// trial

	clock_t delay;															// inter_trial interval

	char display_string[200];												// user output
	char cursor_path_string[_MAX_PATH];										// string holding path for
																			// animated cursor

	// Code

	// Read screen and hold initial values

	UpdateData(TRUE);

	initial_isotonic_level_volts=isotonic_level_volts;
	initial_inter_pull_interval_ms=inter_pull_interval_ms;
	initial_proportional_gain=proportional_gain;
	initial_rise_time=triangle_halftime_ms;

	// Single or multiple trials

	if ((experiment_mode==SINGLE_HOLD)||(experiment_mode==SINGLE_PULLS))
	{
		no_of_trials=1;
		relative_tension[1]=1.0;
		relative_gain[1]=1.0;
		batch_interval[1]=initial_inter_pull_interval_ms;
		relative_velocity[1]=1.0;
	}
	else
	{
		// Multiple trials

		// Open and check batch file

		FILE *batch_file=fopen(batch_file_string,"r");

		if (batch_file==NULL)
		{
			sprintf(display_string,"Batch file\n%s\ncould not be opened",batch_file_string);
			MessageBox(display_string,"Tension_control::OnRunExperiment()");
			return;
		}

		// Read in trials

		fscanf(batch_file,"%i",&no_of_trials);

		// Read in batch file values values

		switch (experiment_mode)
		{
			case FORCE_VELOCITY:
			{
				for (i=1;i<=no_of_trials;i++)
				{	
					fscanf(batch_file,"%f",&relative_tension[i]);
					fscanf(batch_file,"%f",&relative_gain[i]);
					batch_interval[i]=initial_inter_pull_interval_ms;
					relative_velocity[i]=1.0;
				}
			}
			break;

			case BATCH_PULLS:
			{
				for (i=1;i<=no_of_trials;i++)
				{
					fscanf(batch_file,"%f",&batch_interval[i]);
					relative_tension[i]=1.0;
					relative_gain[i]=1.0;
					relative_velocity[i]=1.0;
				}
			}
			break;

			case BATCH_VELOCITY:
			{
				for (i=1;i<=no_of_trials;i++)
				{
					fscanf(batch_file,"%f",&relative_velocity[i]);
					fscanf(batch_file,"%f",&relative_gain[i]);
					relative_tension[i]=1.0;
					batch_interval[i]=initial_inter_pull_interval_ms;
				}
			}
			break;
		}

		// Close file

		fclose(batch_file);
	}

	// Set up for first experiment

			// Check experiment values are valid

		set_post_ktr_values();

		if (post_ktr_ms<0)														// Abort if necessary
		{
			sprintf(display_string,"Post_ktr_ms<0\n");
			MessageBox(display_string,"Tension_control::OnRunExperiment");
			return;
		}


	// Switch cursor to animated cursor

	::set_executable_directory_string(cursor_path_string);
	strcat(cursor_path_string,"\\sl_control.ani");

	HCURSOR hcurWait = (HCURSOR)LoadImage(AfxGetApp()->m_hInstance,cursor_path_string,IMAGE_CURSOR,0,0,
		LR_DEFAULTSIZE|LR_DEFAULTCOLOR|LR_LOADFROMFILE);
	const HCURSOR hcurSave=SetCursor(hcurWait);

	sprintf(display_string,"hcurSave: %i",hcurSave);
	MessageBox(display_string);

	// Now loop through trials

	for (i=1;i<=no_of_trials;i++)
	{
		isotonic_level_volts=(relative_tension[i]*initial_isotonic_level_volts);
		proportional_gain=(relative_gain[i]*initial_proportional_gain);
		inter_pull_interval_ms=batch_interval[i];
		triangle_halftime_ms=(initial_rise_time/relative_velocity[i]);

		// Status

		sprintf(display_string,"File %i/%i, Ten: %.1f Int: %.1f ms Rel_gain: %.1f Rel_vel: %.1f",
			i,no_of_trials,relative_tension[i],inter_pull_interval_ms,relative_gain[i],relative_velocity[i]);
		display_status(TRUE,display_string);

		// Create DAPL_source_file

		if (create_DAPL_source_file()==0)
		{
			sprintf(display_string,"DAPL_source_file could not be created\n");
			display_status(TRUE,display_string);
			MessageBox(display_string,"Tension_control::OnRunExperiment");
			return;
		}
	
		// Run Experiment

		run_single_trial();

		// Delay if not the last trial

		if (i<no_of_trials)
		{
			start=clock();

			// User information

			MessageBeep(0xFFFFFFFF);
			sprintf(display_string,"Pausing for %.1f s. File %i of %i",
				inter_trial_interval_seconds,i,no_of_trials);
			display_status(TRUE,display_string);

			// Delay

			delay=(long)(inter_trial_interval_seconds*CLOCKS_PER_SEC);

			while ((clock()-start)<delay)
				;														// dummy line
		}

	}

	// Switch back to normal cursor

	SetCursor(hcurSave);

	// Tidy up

	sprintf(display_string,"%i trials successfully completed",no_of_trials);
	display_status(TRUE,display_string);
	MessageBeep(MB_ICONEXCLAMATION);

	// Reset default values

	isotonic_level_volts=initial_isotonic_level_volts;
	inter_pull_interval_ms=initial_inter_pull_interval_ms;
	proportional_gain=initial_proportional_gain;
	triangle_halftime_ms=initial_rise_time;

	UpdateData(FALSE);

	// Finished successfully

	return;
}

void Tension_control_dialog::run_single_trial(void)
{
	// Code runs single trial
	// Parent function constructs DAPL command file

	// Variables

	char display_string[200];

	int i,holder;															// counters

	short int short_int=1;													// a short integer

	int buffer_size=(4*data_points);										// number of points in a
																			// data buffer with 4 channels

	short int* DAPL_data = new short int [buffer_size];						// data array for DAPL
																			// processing

	clock_t start_upload;													// time at start of DAPL
																			// upload

	HDAP hdapSysGet,hdapSysPut,hdapBinGet;									// DAPL communication pipes

	// Code

	// Open and flush DAPL pipes

	hdapSysGet=DapHandleOpen("\\\\.\\Dap0\\$SysOut",DAPOPEN_READ);
	hdapSysPut=DapHandleOpen("\\\\.\\Dap0\\$SysIn",DAPOPEN_WRITE);
	hdapBinGet=DapHandleOpen("\\\\.\\Dap0\\$BinOut",DAPOPEN_READ);

	DapInputFlush(hdapSysGet);
	DapInputFlush(hdapSysPut);
	DapInputFlush(hdapBinGet);
	
	// Run Experiment

	// Time upload

	start_upload=clock();
	sprintf(display_string,"Passing command file to DAPL");
	display_status(TRUE,display_string);

	// Upload

	i=DapConfig(hdapSysPut,DAPL_source_file_string);

	// Error checking

	if (i==FALSE)
	{
		sprintf(display_string,"DAPL could not handle source file\n%s\nResulting record is invalid",
			DAPL_source_file_string);
		MessageBox(display_string,"Tension_control::run_single_trial()",MB_ICONWARNING);
	}

	// Display upload time as experiment is running

	sprintf(display_string,"Trial started, DAPL upload took %.4f s",
		(float)(clock()-start_upload)/CLOCKS_PER_SEC);
	display_status(TRUE,display_string);

	// Read data back from DAPL and display status

	i=DapBufferGet(hdapBinGet,buffer_size*sizeof(short_int),DAPL_data);

	sprintf(display_string,"%i Points/Channel read from DAPL processor",
		(i/(4*sizeof(short_int))));
	display_status(TRUE,display_string);

	// Copy data to Tension_control_record and update no_of_points;

	holder=1;

	for (i=0;i<buffer_size;i=i+4)
	{
		p_Tension_control_record->data[TIME][holder]=((float)(holder-1)*time_increment);
		p_Tension_control_record->data[FORCE][holder]=convert_DAPL_units_to_volts(DAPL_data[i]);
		p_Tension_control_record->data[SL][holder]=convert_DAPL_units_to_volts(DAPL_data[i+1]);
		p_Tension_control_record->data[FL][holder]=convert_DAPL_units_to_volts(DAPL_data[i+2]);
		p_Tension_control_record->data[INTENSITY][holder]=convert_DAPL_units_to_volts(DAPL_data[i+3]);

		holder++;
	}

	p_Tension_control_record->no_of_points=data_points;

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
			p_Tension_control_record->no_of_points;
	}

	p_Experiment_display->update_plots();

	// Write data file

	write_data_to_file();

	// Free memory

	delete [] DAPL_data;
}

void Tension_control_dialog::write_data_to_file(void)
{
	// Code writes data record to file

	// Variables

	int i,j;																// counter

	char index_string[200];
	char display_string[FILE_INFO_STRING_LENGTH];
	char file_info_string[FILE_INFO_STRING_LENGTH];
	
	// Code

	sprintf(index_string,"%i",data_index);

	// Create file string

	output_data_file_string=data_path_string+"\\"+data_base_string+index_string+".slc";

	// File handling

	FILE* output_data_file=fopen(output_data_file_string,"w");

	if (output_data_file==NULL)
	{
		sprintf(display_string,"Cannot open file\n%s",output_data_file_string);
		MessageBox(display_string,"Tension_control::write_data_to_file");
	}
	else
	{
		// Output

		fprintf(output_data_file,"SLCONTROL_DATA_FILE\n\n");

		// Version Number

		fprintf(output_data_file,"_VERSION_\n");
		fprintf(output_data_file,"%s\n\n",SLCONTROL_VERSION_STRING);

		// File info

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
		fprintf(output_data_file,"SL_volts_to_FL_COMMAND_volts: %g\n\n",SL_volts_to_FL_COMMAND_volts);

		// Dimensions

		p_Calibration->UpdateData(TRUE);

		fprintf(output_data_file,"_MUSCLE_DIMENSIONS_\n");
		fprintf(output_data_file,"Muscle_length_(µm): %.3e\n",p_Calibration->Calibration_fibre_length);
		fprintf(output_data_file,"Sarcomere_length_(nm): %.3e\n",p_Calibration->Calibration_sarcomere_length);
		fprintf(output_data_file,"Area_(m^2): %.3e\n\n",p_Calibration->Calibration_area);

		// Experiment parameters

		fprintf(output_data_file,"_TENSION_CONTROL_\n");
		fprintf(output_data_file,"sampling_rate_(Hz): %.3e\n",sampling_rate);
		fprintf(output_data_file,"data_points: %i\n",data_points);
		fprintf(output_data_file,"ktr_step_(µm): %.3e\n",ktr_step_size_microns);
		fprintf(output_data_file,"ktr_duration_(ms): %.3e\n",ktr_step_duration_ms);

		fprintf(output_data_file,"experiment_mode: %s\n",experiment_mode_string);
		fprintf(output_data_file,"pre_servo_(ms): %.3e\n",pre_servo_ms);
		fprintf(output_data_file,"servo_(ms): %.3e\n",servo_ms);
		fprintf(output_data_file,"isotonic_hold_(V): %.3e\n",isotonic_level_volts);
		fprintf(output_data_file,"no_of_triangles: %i\n",no_of_triangles);
		fprintf(output_data_file,"triangle_size_(V): %.3e\n",triangle_size_volts);
		fprintf(output_data_file,"triangle_halftime_(ms): %.3e\n",triangle_halftime_ms);
		fprintf(output_data_file,"pre_triangle_(ms): %.3e\n",pre_triangle_ms);
		fprintf(output_data_file,"post_triangle_(ms): %.3e\n",post_triangle_ms);
		fprintf(output_data_file,"inter_pull_interval_(ms): %.3e\n",inter_pull_interval_ms);
		fprintf(output_data_file,"pre_tension_servo_points: %i\n",pre_tension_servo_points);
		fprintf(output_data_file,"ramp_mode: %i\n",ramp_mode);
		fprintf(output_data_file,"ramp_increment_mode: %i\n",ramp_increment_mode);
		fprintf(output_data_file,"ktr_initiation_time_ms: %f\n\n",ktr_initiation_time_ms);
		fprintf(output_data_file,"proportional_gain: %f\n",proportional_gain);
		fprintf(output_data_file,"integral_gain: %f\n",integral_gain);
		


		// Data

		fprintf(output_data_file,"Data\n");

		for (i=1;i<=p_Tension_control_record->no_of_points;i++)
		{
			fprintf(output_data_file,"%f\t%f\t%f\t%f\t%f\n",
				p_Tension_control_record->data[TIME][i],
				p_Tension_control_record->data[FORCE][i],
				p_Tension_control_record->data[SL][i],
				p_Tension_control_record->data[FL][i],
				p_Tension_control_record->data[INTENSITY][i]);
		}
	
		// Close file

		fclose(output_data_file);

		// Update counter and screen

		data_index++;
		UpdateData(FALSE);

		// Update staus

		sprintf(display_string,"Record written to %s",output_data_file_string);
		display_status(TRUE,display_string);
	}
}

void Tension_control_dialog::OnSelchangeSamplingRate() 
{
	// Code updates sampling parameters and changes record_duration on screen

	set_sampling_parameters();
}

void Tension_control_dialog::OnSelchangeExptMode() 
{
	// Code enables and disables appropriate controls

	// Variables

	// Code

	// Sets experiment_mode and enables/disables appropriate controls

	UpdateData(TRUE);

	switch (EXPERIMENT_MODE_CONTROL.GetCurSel())
	{
		case 0:
		{
			experiment_mode=SINGLE_HOLD;

			GetDlgItem(IDC_BATCH_FILE)->EnableWindow(FALSE);
			GetDlgItem(IDC_INTER_TRIAL_INTERVAL)->EnableWindow(FALSE);

			GetDlgItem(IDC_PRE_SERVO)->EnableWindow(TRUE);
			GetDlgItem(IDC_SERVO)->EnableWindow(TRUE);
			GetDlgItem(IDC_ISOTONIC_LEVEL)->EnableWindow(TRUE);

			GetDlgItem(IDC_TRI_HALFTIME)->EnableWindow(FALSE);
			GetDlgItem(IDC_TRIANGLE_SIZE)->EnableWindow(FALSE);
			GetDlgItem(IDC_NO_OF_TRIANGLES)->EnableWindow(FALSE);
			GetDlgItem(IDC_POST_TRI)->EnableWindow(FALSE);
			GetDlgItem(IDC_PRE_TRI)->EnableWindow(FALSE);
			GetDlgItem(IDC_PRE_TENSION_SERVO)->EnableWindow(FALSE);
			GetDlgItem(IDC_INTER_PULL_INTERVAL)->EnableWindow(FALSE);
			GetDlgItem(IDC_RAMP_CONTROL)->EnableWindow(FALSE);
			GetDlgItem(IDC_RAMP_INCREMENT_CONTROL)->EnableWindow(FALSE);
		}
		break;

		case 1:
		{
			experiment_mode=FORCE_VELOCITY;

			batch_file_string=force_velocity_file_string;

			GetDlgItem(IDC_BATCH_FILE)->EnableWindow(TRUE);
			GetDlgItem(IDC_INTER_TRIAL_INTERVAL)->EnableWindow(TRUE);

			GetDlgItem(IDC_PRE_SERVO)->EnableWindow(TRUE);
			GetDlgItem(IDC_SERVO)->EnableWindow(TRUE);
			GetDlgItem(IDC_ISOTONIC_LEVEL)->EnableWindow(TRUE);

			GetDlgItem(IDC_TRI_HALFTIME)->EnableWindow(FALSE);
			GetDlgItem(IDC_TRIANGLE_SIZE)->EnableWindow(FALSE);
			GetDlgItem(IDC_NO_OF_TRIANGLES)->EnableWindow(FALSE);
			GetDlgItem(IDC_POST_TRI)->EnableWindow(FALSE);
			GetDlgItem(IDC_PRE_TRI)->EnableWindow(FALSE);
			GetDlgItem(IDC_PRE_TENSION_SERVO)->EnableWindow(FALSE);
			GetDlgItem(IDC_INTER_PULL_INTERVAL)->EnableWindow(FALSE);
			GetDlgItem(IDC_RAMP_CONTROL)->EnableWindow(FALSE);
			GetDlgItem(IDC_RAMP_INCREMENT_CONTROL)->EnableWindow(FALSE);
		}
		break;

		case 2:
		{
			experiment_mode=SINGLE_PULLS;

			GetDlgItem(IDC_BATCH_FILE)->EnableWindow(FALSE);
			GetDlgItem(IDC_INTER_TRIAL_INTERVAL)->EnableWindow(FALSE);

			GetDlgItem(IDC_PRE_SERVO)->EnableWindow(FALSE);
			GetDlgItem(IDC_SERVO)->EnableWindow(FALSE);
			GetDlgItem(IDC_ISOTONIC_LEVEL)->EnableWindow(FALSE);

			GetDlgItem(IDC_NO_OF_TRIANGLES)->EnableWindow(TRUE);
			OnSelchangeNoOfTriangles();

			OnRampControl();
		}
		break;

		case 3:
		{
			experiment_mode=BATCH_PULLS;

			GetDlgItem(IDC_BATCH_FILE)->EnableWindow(TRUE);
			GetDlgItem(IDC_INTER_TRIAL_INTERVAL)->EnableWindow(TRUE);

			GetDlgItem(IDC_PRE_SERVO)->EnableWindow(FALSE);
			GetDlgItem(IDC_SERVO)->EnableWindow(FALSE);
			GetDlgItem(IDC_ISOTONIC_LEVEL)->EnableWindow(FALSE);

			GetDlgItem(IDC_NO_OF_TRIANGLES)->EnableWindow(TRUE);
			OnSelchangeNoOfTriangles();

			OnRampControl();

			batch_file_string=batch_pulls_file_string;
		}
		break;

		case 4:
		{
			experiment_mode=BATCH_VELOCITY;

			GetDlgItem(IDC_BATCH_FILE)->EnableWindow(TRUE);
			GetDlgItem(IDC_INTER_TRIAL_INTERVAL)->EnableWindow(TRUE);

			GetDlgItem(IDC_PRE_SERVO)->EnableWindow(FALSE);
			GetDlgItem(IDC_SERVO)->EnableWindow(FALSE);
			GetDlgItem(IDC_ISOTONIC_LEVEL)->EnableWindow(FALSE);

			GetDlgItem(IDC_NO_OF_TRIANGLES)->EnableWindow(TRUE);
			OnSelchangeNoOfTriangles();

			OnRampControl();

			batch_file_string=batch_velocity_file_string;
		}
		break;
	}

	// Update batch_file

	GetDlgItem(IDC_BATCH_FILE)->SetWindowText(batch_file_string);

	// Update parameters

	set_sampling_parameters();
}


void Tension_control_dialog::OnRampControl() 
{
	// Sets ramp_mode and enables/disables ramp_increment control

	UpdateData(TRUE);

	if (ramp_option==FALSE)													// Triangles
	{
		ramp_mode=0;

		// Increment option is nonsensical unless we are doing ramps and there is more than one of them

		ramp_increment_mode=0;
		ramp_increment_option=FALSE;
		GetDlgItem(IDC_RAMP_INCREMENT_CONTROL)->EnableWindow(FALSE);
	}
	else																	// Ramps
	{
		ramp_mode=1;

		if (no_of_triangles>1)
		{
			GetDlgItem(IDC_RAMP_INCREMENT_CONTROL)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_RAMP_INCREMENT_CONTROL)->EnableWindow(FALSE);
		}
	}

	// Set multiplier

	set_ramp_multiplier();

	/// Sets ramp_increment_mode

	OnRampIncrementControl();

	// Recalculate sampling parameters

	set_sampling_parameters();
}

void Tension_control_dialog::OnRampIncrementControl() 
{
	// Sets ramp_increment_mode
	
	UpdateData(TRUE);

	if (ramp_increment_option==FALSE)												// Does not increment
	{
		ramp_increment_mode=0;
	}
	else
	{
		ramp_increment_mode=1;
	}
}


void Tension_control_dialog::set_ramp_multiplier(void)
{
	// Sets ramp_multiplier

	UpdateData(TRUE);

	if (ramp_mode==0)													// Triangles
	{
		ramp_multiplier=2.0;
	}
	else																// Ramps
	{
		ramp_multiplier=1.0;
	}
}

void Tension_control_dialog::OnChangeDataPoints(void) 
{
	// Code updates sampling parameters

	UpdateData(TRUE);

	set_sampling_parameters();
}

void Tension_control_dialog::OnChangeKtrStepDuration(void) 
{
	// Code updates sampling parameters

	UpdateData(TRUE);

	set_sampling_parameters();	
}

void Tension_control_dialog::OnChangeServo() 
{
	// Code updates sampling parameters

	UpdateData(TRUE);

	set_sampling_parameters();
}

void Tension_control_dialog::OnChangePreServo() 
{
	// Code updates sampling parameters

	UpdateData(TRUE);

	set_sampling_parameters();
}

void Tension_control_dialog::OnSelchangeNoOfTriangles() 
{
	// Code sets no_of_triangles

	// Variables

	char* p_string;

	int temp;

	CString value_string;

	// Code

	UpdateData(TRUE);

	temp=no_of_triangles_control.GetCurSel();
	no_of_triangles_control.GetLBText(temp,value_string);

	no_of_triangles=strtol(value_string,&p_string,10);

	// Enable/disable controls appropriately

	GetDlgItem(IDC_PRE_TRI)->EnableWindow(TRUE);
	GetDlgItem(IDC_POST_TRI)->EnableWindow(TRUE);
	GetDlgItem(IDC_PRE_TENSION_SERVO)->EnableWindow(TRUE);
	
	if (no_of_triangles>0)
	{
		GetDlgItem(IDC_TRI_HALFTIME)->EnableWindow(TRUE);
		GetDlgItem(IDC_TRIANGLE_SIZE)->EnableWindow(TRUE);
		GetDlgItem(IDC_RAMP_CONTROL)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_TRI_HALFTIME)->EnableWindow(FALSE);
		GetDlgItem(IDC_TRIANGLE_SIZE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RAMP_CONTROL)->EnableWindow(FALSE);
		GetDlgItem(IDC_RAMP_INCREMENT_CONTROL)->EnableWindow(FALSE);
	}

	if (no_of_triangles>1)
	{
		GetDlgItem(IDC_INTER_PULL_INTERVAL)->EnableWindow(TRUE);

		if (ramp_option==FALSE)
		{
			GetDlgItem(IDC_RAMP_INCREMENT_CONTROL)->EnableWindow(FALSE);
		}
		else
		{
			GetDlgItem(IDC_RAMP_INCREMENT_CONTROL)->EnableWindow(TRUE);
		}
	}
	else
	{
		GetDlgItem(IDC_INTER_PULL_INTERVAL)->EnableWindow(FALSE);
		GetDlgItem(IDC_RAMP_INCREMENT_CONTROL)->EnableWindow(FALSE);
	}

	// Update parameters

	set_sampling_parameters();
}

void Tension_control_dialog::OnChangePreTri() 
{
	// Code updates sampling parameters

	UpdateData(TRUE);

	set_sampling_parameters();	
}

void Tension_control_dialog::OnChangePostTri() 
{
	// Code updates sampling parameters

	UpdateData(TRUE);

	set_sampling_parameters();	
}

void Tension_control_dialog::OnChangeInterPullInterval() 
{
	// Code updates sampling parameters

	UpdateData(TRUE);

	set_sampling_parameters();	
}

void Tension_control_dialog::OnChangeTriHalftime() 
{
	// Code updates sampling parameters

	UpdateData(TRUE);

	set_sampling_parameters();	
}

void Tension_control_dialog::set_sampling_parameters(void)
{
	// Code sets sampling_rate, time_increment and duration_seconds from SAMPLING_RATE_CONTROL

	// Variables

	char* p_string;									// used to extract sampling rate
	CString value_string;							// from control

	char temp_string[100];							// for display

	// Code

	// Extract sampling_rate and time_increment

	SAMPLING_RATE_CONTROL.GetLBText(SAMPLING_RATE_CONTROL.GetCurSel(),value_string);
	sampling_rate=(float)strtod(value_string,&p_string);
	time_increment=(float)1.0/sampling_rate;
	duration_seconds=data_points*time_increment;

	// Update record_duration

	sprintf(temp_string,"%.3f",duration_seconds);
	duration_string=temp_string;
	GetDlgItem(IDC_DURATION_CONTROL)->SetWindowText(temp_string);

	// Update post_ktr_ms

	set_post_ktr_values();
}

void Tension_control_dialog::set_post_ktr_values(void)
{
	// Code sets post_ktr_ms

	// Variables

	char temp_string[100];											// user display

	// Code

	if ((experiment_mode==SINGLE_HOLD)||(experiment_mode==FORCE_VELOCITY))
	{
		// Single Holds

		post_ktr_ms=((duration_seconds*(float)1000.0)-(pre_servo_ms+servo_ms+ktr_step_duration_ms));
	}
	else
	{
		// Triangles or ramps

		if (no_of_triangles>1)
		{
			post_ktr_ms=((duration_seconds*(float)1000.0)-
				(pre_triangle_ms+((float)no_of_triangles*ramp_multiplier*triangle_halftime_ms)+
					((float)(no_of_triangles-1)*inter_pull_interval_ms)+
					post_triangle_ms+ktr_step_duration_ms));
		}
		else
		{
			post_ktr_ms=((duration_seconds*(float)1000.0)-
				(pre_triangle_ms+((float)no_of_triangles*ramp_multiplier*triangle_halftime_ms)+
					post_triangle_ms+ktr_step_duration_ms));
		}
	}

	// Update display

	sprintf(temp_string,"%.0f",post_ktr_ms);
	post_ktr_ms_string=temp_string;
	GetDlgItem(IDC_POST_KTR)->SetWindowText(temp_string);
}

int Tension_control_dialog::convert_time_ms_to_sampling_points(float time_ms)
{
	// Returns no of sample points in a given time interval (expressed in ms)

	// Variables

	int points;

	// Code

	// Calculate sampling_rate

	set_sampling_parameters();

	// Calculate points and return

	points=(int)((time_ms/(float)1000.0)*sampling_rate);

	return(points);
}

void Tension_control_dialog::display_status(int update_mode,CString new_string)
{
	// Code scrolls new status message to screen

	// Variables

	char display_string[300];

	// Code

	// Create display_dc

	CClientDC dc(this);

	// Create font

	dc.SetBkMode(TRANSPARENT);
	CFont display_font;
	display_font.CreateFont(12,0,0,0,400,FALSE,FALSE,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_SWISS,"Arial");
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

	// Update and display

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

int Tension_control_dialog::create_DAPL_source_file(void)
{
	// Code creates dapl_source_file by combining a number of sub files

	// Variables

	int i;																// counter

	int no_of_sub_files=5;

	char single_character;
	char display_string[200];											// user output

	// File pointer and an array of pointers

	FILE* DAPL_source_file=fopen(DAPL_source_file_string,"w");

	FILE* file_pointer[5];

	// Code

	if (write_header_file()*write_fill_pipes_file()*write_create_pipes_file()*
		write_control_file()*write_io_procedures_file()==1)
	{
		// Sub files written successfully - now merge them into the DAPL_source_file

		// Check source file

		if (DAPL_source_file==NULL)
		{
			sprintf(display_string,"%s\n could not be opened",DAPL_source_file_string);
			MessageBox(display_string,"Tension_control::create_DAPL_source_file");
			return 0;
		}

		// Sub files

		FILE* header_file=fopen(header_file_string,"r");
		FILE* fill_pipes_file=fopen(fill_pipes_file_string,"r");
		FILE* create_pipes_file=fopen(create_pipes_file_string,"r");
		FILE* control_file=fopen(control_file_string,"r");
		FILE* io_procedures_file=fopen(io_procedures_file_string,"r");

		file_pointer[0]=header_file;
		file_pointer[1]=fill_pipes_file;
		file_pointer[2]=create_pipes_file;
		file_pointer[3]=control_file;
		file_pointer[4]=io_procedures_file;

		// Check them too

		for (i=0;i<=(no_of_sub_files-1);i++)
		{
			if (file_pointer[i]==NULL)
			{
				sprintf(display_string,"File %i could not be opened",i);
				MessageBox(display_string,"Tension_control::create_DAPL_source_file");
				return 0;
			}
		}

		// Merge them

		for (i=0;i<=(no_of_sub_files-1);i++)
		{
			while (!feof(file_pointer[i]))
			{
				single_character=fgetc(file_pointer[i]);

				if (single_character!=EOF)
					fputc(single_character,DAPL_source_file);
			}
		}

		// close files

		fclose(DAPL_source_file);

		for (i=0;i<=(no_of_sub_files-1);i++)
		{
			fclose(file_pointer[i]);
		}

		// Return

		return 1;
	}
	else
	{
		// Writing sub_files failed

		sprintf(display_string,"Create_DAPL_source_file failed\n");
		MessageBox(display_string,"Tension_control::create_DAPL_source_file()");
		return 0;
	}
}

int Tension_control_dialog::write_header_file(void)
{
	// Code creates header file

	// Variables

	char display_string[200];

	FILE* header_file;

	// Code

	// File handling

	header_file=fopen(header_file_string,"w");

	if (header_file==NULL)
	{
		// File is not open

		sprintf(display_string,"Header file\n%s\ncould not be opened",header_file_string);
		MessageBox(display_string,"Tension_control::write_header_file");
		return 0;
	}
	else
	{
		// Write file

		fprintf(header_file,"RESET\n\n");

		fprintf(header_file,"OPTION UNDERFLOWQ=OFF\n");
		fprintf(header_file,"OPTION ERRORQ=OFF\n");
		fprintf(header_file,"OPTION SCHEDULING=FIXED\n");
		fprintf(header_file,"OPTION BUFFERING=OFF\n");
		fprintf(header_file,"OPTION QUANTUM=100\n\n");

		fprintf(header_file,";************************************************************\n\n");

		fprintf(header_file,"// Header\n\n");

		fprintf(header_file,"pipes pfl MAXSIZE=65500\n");
		fprintf(header_file,"pipes ptarget_force MAXSIZE=65500\n\n");
		
		fprintf(header_file,"pipes pservo MAXSIZE=65500\n\n");

		fprintf(header_file,"pipes poffset_marker MAXSIZE=65500\n");
		fprintf(header_file,"pipes poffset MAXSIZE=65500\n");
		fprintf(header_file,"pipes plastoffset MAXSIZE=65500\n\n");

		fprintf(header_file,"pipes pout MAXSIZE=65500\n");
		fprintf(header_file,"pipes plastout MAXSIZE=65500\n\n");

		fprintf(header_file,"pipes pfeedback MAXSIZE=65500\n\n");
		
		fprintf(header_file,"pipes ptrigger MAXSIZE=65500\n\n");

		fprintf(header_file,"pipes pdifference MAXSIZE=65500\n");
		fprintf(header_file,"pipes pamplified_diff MAXSIZE=65500\n");
		fprintf(header_file,"pipes pintegral MAXSIZE=65500\n\n");

		fprintf(header_file,";************************************************************\n\n");

		// Close file and return

		fclose(header_file);

		return 1;
	}
}

int Tension_control_dialog::write_fill_pipes_file(void)
{
	// Code creates fill_pipes_file

	// Variables

	int i;																	// counter

	float* pipe = new float [data_points+1];								// pipe array, which is
																			// filled in turn with
																			// pservo, poffset_marker,
																			// pfl and ptarget_force and
																			// transferred to DAPL

	// Time points

	int servo_initiation_points;										// integer defining the number of
																		// sampling points at which the
																		// tension servo initiates
	
	int ktr_initiation_points;											// integer values defining the
	int ktr_finish_points;												// first and last points of the
																		// ktr step

	int tri_1_start,tri_2_start,tri_3_start;							// integer values defining the
	int tri_1_stop,tri_2_stop,tri_3_stop;								// first and last points for each
																		// triangle

	char display_string[200];

	FILE* fill_pipes_file;

	// Code

	// File handling

	fill_pipes_file=fopen(fill_pipes_file_string,"w");

	if (fill_pipes_file==NULL)
	{
		// File is not open

		sprintf(display_string,"Fill_pipes file\n%s\ncould not be opened",fill_pipes_file_string);
		MessageBox(display_string,"Tension_control::write_fill_pipes_file");
		return 0;
	}
	else
	{
		// Write file

		fprintf(fill_pipes_file,"\nfill ptrigger %i\n\n",::convert_volts_to_DAPL_units((float)4.95));
																			// default high trigger

		fprintf(fill_pipes_file,"fill plastout 0\n");						// initial values
		fprintf(fill_pipes_file,"fill plastoffset 0\n\n");

		fprintf(fill_pipes_file,"fill pdifference 0\n");
		fprintf(fill_pipes_file,"fill pintegral 0 0 0 0 0 0 0 0 0\n\n");

		if ((experiment_mode==SINGLE_HOLD)||(experiment_mode==FORCE_VELOCITY))
		{
			// Isotonic hold
		
			servo_initiation_points=convert_time_ms_to_sampling_points(pre_servo_ms);

			ktr_initiation_time_ms=pre_servo_ms+servo_ms;

			ktr_initiation_points=servo_initiation_points+
				convert_time_ms_to_sampling_points(servo_ms);
			
			ktr_finish_points=ktr_initiation_points+
				convert_time_ms_to_sampling_points(ktr_step_duration_ms);

			// pservo

			for (i=1;i<=data_points;i++)
			{
				if ((i>servo_initiation_points)&&(i<=ktr_initiation_points))
				{
					pipe[i]=1;
				}
				else
				{
					pipe[i]=0;
				}
			}

			write_pipe_to_file(fill_pipes_file,"pservo",pipe,data_points);

			// poffset_marker

			for (i=1;i<=data_points;i++)
			{
				if ((i>servo_initiation_points)&&(i<=ktr_initiation_points))
				{
					pipe[i]=0;
				}
				else
				{
					pipe[i]=1;
				}
			}

			write_pipe_to_file(fill_pipes_file,"poffset_marker",pipe,data_points);

			// pfl

			for (i=1;i<=data_points;i++)
			{
				if ((i>ktr_initiation_points)&&(i<=ktr_finish_points))
				{
					pipe[i]=(float)::convert_volts_to_DAPL_units(
						::convert_FL_COMMAND_microns_to_volts(ktr_step_size_microns));
				}
				else
				{
					pipe[i]=0;
				}
			}

			write_pipe_to_file(fill_pipes_file,"pfl",pipe,data_points);
		
			// ptarget_for

			for (i=1;i<=data_points;i++)
			{
				if ((i>servo_initiation_points)&&(i<=ktr_initiation_points))
				{
					pipe[i]=(float)::convert_volts_to_DAPL_units(isotonic_level_volts);
				}
				else
				{
					pipe[i]=0;
				}
			}

			write_pipe_to_file(fill_pipes_file,"ptarget_force",pipe,data_points);
		}
		else
		{
			// Tension control pulls

			servo_initiation_points=pre_tension_servo_points;

			if (no_of_triangles==0)
			{
				ktr_initiation_points=convert_time_ms_to_sampling_points(pre_triangle_ms+post_triangle_ms);

				ktr_finish_points=ktr_initiation_points+
					convert_time_ms_to_sampling_points(ktr_step_duration_ms);
			}
			else
			{
				ktr_initiation_time_ms=pre_triangle_ms+
						((float)no_of_triangles*ramp_multiplier*triangle_halftime_ms)+
						((float)(no_of_triangles-1)*inter_pull_interval_ms)+
						post_triangle_ms;

				ktr_initiation_points=convert_time_ms_to_sampling_points(ktr_initiation_time_ms);
					
				ktr_finish_points=ktr_initiation_points+
					convert_time_ms_to_sampling_points(ktr_step_duration_ms);
			}

			// pservo

			for (i=1;i<=data_points;i++)
			{
				if ((i>servo_initiation_points)&&(i<=ktr_initiation_points))
				{
					pipe[i]=1;
				}
				else
				{
					pipe[i]=0;
				}
			}

			write_pipe_to_file(fill_pipes_file,"pservo",pipe,data_points);

			// poffset_marker

			for (i=1;i<=data_points;i++)
			{
				if ((i>servo_initiation_points)&&(i<=ktr_initiation_points))
				{
					pipe[i]=0;
				}
				else
				{
					pipe[i]=1;
				}
			}

			write_pipe_to_file(fill_pipes_file,"poffset_marker",pipe,data_points);

			// pfl

			for (i=1;i<=data_points;i++)
			{
				if ((i>ktr_initiation_points)&&(i<=ktr_finish_points))
				{
					pipe[i]=(float)::convert_volts_to_DAPL_units(
						::convert_FL_COMMAND_microns_to_volts(ktr_step_size_microns));
				}
				else
				{
					pipe[i]=0;
				}
			}

			write_pipe_to_file(fill_pipes_file,"pfl",pipe,data_points);

			// ptarget_for

			switch (no_of_triangles)
			{
				case 0:
				{
					for (i=1;i<=data_points;i++)
					{
						pipe[i]=0;
					}
				}
				break;

				case 1:
				{
					tri_1_start=convert_time_ms_to_sampling_points(pre_triangle_ms);
					tri_1_stop=tri_1_start+
						(int)(ramp_multiplier*convert_time_ms_to_sampling_points(triangle_halftime_ms));

					::generate_waveform(CONSTANT,pipe,1,tri_1_start,1,0);

					::generate_waveform(TRIANGLE,pipe,tri_1_start,tri_1_stop,
						(float)convert_time_ms_to_sampling_points((float)4.0*triangle_halftime_ms),
						(float)::convert_volts_to_DAPL_units(triangle_size_volts));

					::generate_waveform(CONSTANT,pipe,tri_1_stop,data_points,1,0);
				}
				break;

				case 2:
				{
					tri_1_start=convert_time_ms_to_sampling_points(pre_triangle_ms);
					tri_1_stop=tri_1_start+
						(int)(ramp_multiplier*convert_time_ms_to_sampling_points(triangle_halftime_ms));

					tri_2_start=tri_1_stop+convert_time_ms_to_sampling_points(inter_pull_interval_ms);
					tri_2_stop=tri_2_start+
						(int)(ramp_multiplier*convert_time_ms_to_sampling_points(triangle_halftime_ms));

					::generate_waveform(CONSTANT,pipe,1,tri_1_start,1,0);

					::generate_waveform(TRIANGLE,pipe,tri_1_start,tri_1_stop,
						(float)convert_time_ms_to_sampling_points((float)4.0*triangle_halftime_ms),
						(float)::convert_volts_to_DAPL_units(triangle_size_volts));

					::generate_waveform(CONSTANT,pipe,tri_1_stop,tri_2_start,1,0);

					::generate_waveform(TRIANGLE,pipe,tri_2_start,tri_2_stop,
						(float)convert_time_ms_to_sampling_points((float)4.0*triangle_halftime_ms),
						(float)::convert_volts_to_DAPL_units(triangle_size_volts));

					::generate_waveform(CONSTANT,pipe,tri_2_stop,data_points,1,0);
						

					// Correct for ramp_increment

					if (ramp_increment_mode==1)
					{
						for (i=tri_1_stop;i<=ktr_initiation_points;i++)
						{
							pipe[i]=(pipe[i]+::convert_volts_to_DAPL_units(triangle_size_volts));
						}

						for (i=tri_2_stop;i<=ktr_initiation_points;i++)
						{
							pipe[i]=(pipe[i]+::convert_volts_to_DAPL_units(triangle_size_volts));
						}
					}
				}
				break;

				case 3:
				{
					tri_1_start=convert_time_ms_to_sampling_points(pre_triangle_ms);
					tri_1_stop=tri_1_start+
						(int)(ramp_multiplier*convert_time_ms_to_sampling_points(triangle_halftime_ms));

					tri_2_start=tri_1_stop+convert_time_ms_to_sampling_points(inter_pull_interval_ms);
					tri_2_stop=tri_2_start+
						(int)(ramp_multiplier*convert_time_ms_to_sampling_points(triangle_halftime_ms));

					tri_3_start=tri_2_stop+convert_time_ms_to_sampling_points(inter_pull_interval_ms);
					tri_3_stop=tri_3_start+
						(int)(ramp_multiplier*convert_time_ms_to_sampling_points(triangle_halftime_ms));

					::generate_waveform(CONSTANT,pipe,1,tri_1_start,1,0);

					::generate_waveform(TRIANGLE,pipe,tri_1_start,tri_1_stop,
						(float)convert_time_ms_to_sampling_points((float)4.0*triangle_halftime_ms),
						(float)::convert_volts_to_DAPL_units(triangle_size_volts));

					::generate_waveform(CONSTANT,pipe,tri_1_stop,tri_2_start,1,0);

					::generate_waveform(TRIANGLE,pipe,tri_2_start,tri_2_stop,
						(float)convert_time_ms_to_sampling_points((float)4.0*triangle_halftime_ms),
						(float)::convert_volts_to_DAPL_units(triangle_size_volts));

					::generate_waveform(CONSTANT,pipe,tri_2_stop,tri_3_start,1,0);

					::generate_waveform((int)TRIANGLE,pipe,(int)tri_3_start,(int)tri_3_stop,
						(float)convert_time_ms_to_sampling_points((float)4.0*triangle_halftime_ms),
						(float)(::convert_volts_to_DAPL_units(triangle_size_volts)));
						
					// Correct for ramp_increment

					if (ramp_increment_mode==1)
					{
						for (i=tri_1_stop;i<=ktr_initiation_points;i++)
						{
							pipe[i]=(pipe[i]+::convert_volts_to_DAPL_units(triangle_size_volts));
						}

						for (i=tri_2_stop;i<=ktr_initiation_points;i++)
						{
							pipe[i]=(pipe[i]+::convert_volts_to_DAPL_units(triangle_size_volts));
						}

						for (i=tri_3_stop;i<=ktr_initiation_points;i++)
						{
							pipe[i]=(pipe[i]+::convert_volts_to_DAPL_units(triangle_size_volts));
						}
					}
				}
				break;
			}

			write_pipe_to_file(fill_pipes_file,"ptarget_force",pipe,data_points);
		}

		fprintf(fill_pipes_file,"\n\n");

		fprintf(fill_pipes_file,";************************************************************\n\n");
	}

	// Tidy Up

	fclose(fill_pipes_file);

	delete [] pipe;

	return 1;
}

int Tension_control_dialog::write_create_pipes_file(void)
{
	// Code creates create_pipes_file

	// Variables

	char display_string[200];

	FILE* create_pipes_file;

	// Code

	// File handling

	create_pipes_file=fopen(create_pipes_file_string,"w");

	if (create_pipes_file==NULL)
	{
		// File is not open

		sprintf(display_string,"Create_pipes file\n%s\ncould not be opened",create_pipes_file_string);
		MessageBox(display_string,"Tension_control::write_create_pipes_file");
		return 0;
	}
	else
	{
		// Write file

		fprintf(create_pipes_file,"\npdef create_pipes\n\n");

		fprintf(create_pipes_file,"  ptrigger=0\n\n");				// set trigger low

		fprintf(create_pipes_file,"end\n\n");

		fprintf(create_pipes_file,";************************************************************\n\n");

		// Close file and return

		fclose(create_pipes_file);

		return 1;
	}
}

int Tension_control_dialog::write_control_file(void)
{
	// Code creates control_file

	// Variables

	char display_string[200];

	FILE* control_file;

	// Code

	// File handling

	control_file=fopen(control_file_string,"w");

	if (control_file==NULL)
	{
		// File is not open

		sprintf(display_string,"Control file\n%s\ncould not be opened",control_file_string);
		MessageBox(display_string,"Tension_control::write_control_file");
		return 0;
	}
	else
	{
		// Write file

		fprintf(control_file,"\npdef control\n");
		fprintf(control_file,"  poffset=((1-poffset_marker)*plastoffset)+(poffset_marker*ip0)\n");
		fprintf(control_file,"  plastoffset=poffset\n\n");

		fprintf(control_file,"  pdifference=pservo*(ip0-ptarget_force-poffset)\n");
		fprintf(control_file,"  pamplified_diff=5*pdifference\n");
		fprintf(control_file,"  raverage(pamplified_diff,10,pintegral)\n\n");

		fprintf(control_file,"  pfeedback=plastout+(%.0f*(%.0f*(ip0-ptarget_force-poffset)/10000))+(%.0f*(%.0f*pintegral)/10000)\n",
			-FL_POLARITY,1.0e4*proportional_gain,-FL_POLARITY,1.0e4*integral_gain);
		fprintf(control_file,"  pout=((1-pservo)*pfl)+(pservo*pfeedback)\n\n");

		fprintf(control_file,"  copy(pout,plastout,op1)\n");
		fprintf(control_file,"  copy(ptrigger,op0)\n\n");

		fprintf(control_file,"  merge(ip0,ip1,ip2,ip3,$binout)\n");
		fprintf(control_file,"end\n\n");

		fprintf(control_file,";************************************************************\n\n");

		// Close file and return

		fclose(control_file);

		return 1;
	}
}

int Tension_control_dialog::write_io_procedures_file(void)
{
	// Code creates io_procedures_file

	// Variables

	char display_string[200];

	FILE* io_procedures_file;

	// Code

	// File handling

	io_procedures_file=fopen(io_procedures_file_string,"w");

	if (io_procedures_file==NULL)
	{
		// File is not open

		sprintf(display_string,"IO_procedures file\n%s\ncould not be opened",io_procedures_file_string);
		MessageBox(display_string,"Tension_control::write_io_procedures_file");
		return 0;
	}
	else
	{
		// Output Proc

		fprintf(io_procedures_file,"\nodef outputproc 2\n");
		fprintf(io_procedures_file,"  outputwait 2\n");
		fprintf(io_procedures_file,"  update burst\n");
		fprintf(io_procedures_file,"  set op0 a0\n");
		fprintf(io_procedures_file,"  set op1 a1\n");
		fprintf(io_procedures_file,"  time %.0f\n",(0.5*time_increment*1.0e6));
		fprintf(io_procedures_file,"end\n\n");

		fprintf(io_procedures_file,";************************************************************\n\n");

		// Input Proc

		fprintf(io_procedures_file,"idef inputproc 4\n");
		fprintf(io_procedures_file,"  set ip0 s0\n");
		fprintf(io_procedures_file,"  set ip1 s1\n");
		fprintf(io_procedures_file,"  set ip2 s2\n");
		fprintf(io_procedures_file,"  set ip3 s3\n");
		fprintf(io_procedures_file,"  count %i\n",(4*data_points));
		fprintf(io_procedures_file,"  time %.0f\n",(0.25*time_increment*1.0e6));
		fprintf(io_procedures_file,"end\n\n");

		fprintf(io_procedures_file,";************************************************************\n\n");

		// Start tasks

		fprintf(io_procedures_file,"start create_pipes,control,inputproc,outputproc\n");

		// Close file and return

		fclose(io_procedures_file);

		return 1;
	}
}

