// SL_SREC.cpp : implementation file
//

#include "stdafx.h"
#include "SLControl.h"
#include "SL_SREC.h"
#include "SLControlDlg.h"

////////////////////////////
// Ken's code starts here //
////////////////////////////

#include <stdio.h>
#include <iostream>
#include "dapio32.h"
#include "Calibration.h"
#include "global_functions.h"
#include "global_definitions.h"
#include <math.h>
#include <ctime>
#include <direct.h>
#include "string.h"

#include ".\sl_srec.h"

#include "PathDialog.h"

#include "Abort_multiple_trials.h"
#include "Abort_multiple_trials_dialog.h"

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
// SL_SREC dialog


SL_SREC::SL_SREC(CWnd* pParent /*=NULL*/)
	: CDialog(SL_SREC::IDD, pParent)
{
	m_pParent=pParent;
	m_nID=SL_SREC::IDD;

	// Variables

	char* p_string;												// p_string and temp_string
	char temp_string[100];										// used to set duration_string
																// which is a control and not
																// a value

	char display_string[_MAX_PATH];

	// Related to closing SL_SREC window

	p_SL_SREC = this;
	
	// Create pointer to data record

	p_SL_SREC_record =& SL_SREC_record;

	// File strings for dapl_source_file components

	// Set SL_SREC_dir_path

	::set_executable_directory_string(display_string);
	strcat(display_string,"\\SL_SREC\\");
	
	SL_SREC_dir_path=display_string;
	
	header_file_string=SL_SREC_dir_path+"SL_SREC_header.txt";
	fill_pipes_file_string=SL_SREC_dir_path+"SL_SREC_fill_pipes.txt";
	create_pipes_file_string=SL_SREC_dir_path+"SL_SREC_create_pipes.txt";
	control_file_string=SL_SREC_dir_path+"SL_SREC_control.txt";
	io_procedures_file_string=SL_SREC_dir_path+"SL_SREC_io_procs.txt";
	DAPL_source_file_string=SL_SREC_dir_path+"dapl_source.txt";


	// Set daughter window pointers

	p_Calibration=NULL;
	p_Experiment_display=NULL;

	// SL_SREC display

	servo_mode_string="Fibre Length Control";
	pre_ktr_servo_mode=FL_CONTROL;
	clamp_mode_string="Prevailing Position";
	pre_servo_points=50;
	proportional_gain=0.5;

	sampling_rate_string="1000";
	data_points=1000;
	time_increment=((float)(1.0/strtod(sampling_rate_string,&p_string)));
	duration_seconds=(data_points*time_increment);
	sprintf(temp_string,"%.3f",duration_seconds);
	duration_string=temp_string;

	no_of_triangles_string="0";
	no_of_triangles=strtol(no_of_triangles_string,&p_string,10);
	triangle_size_microns=20.0;
	triangle_halftime_ms=100.0;
	pre_triangle_ms=100.0;

	ramp_option=FALSE;
	ramp_mode=0;
	ramp_increment_option=FALSE;
	ramp_increment_mode=0;

	pre_ktr_ms=100.0;
	ktr_step_size_microns=-20.0;
	ktr_step_duration_ms=20;

	SL_SREC_SL_volts_to_FL_microns_calibration=(float)999.9;
	SL_volts_to_FL_command_volts=(float)999.9;

	data_path_string="c:\\temp";
	data_base_string="test_step";
	data_index=1;

	experiment_mode_string="Single";
	experiment_mode=SL_SREC_SINGLE_TRIAL;
	relative_first_tri_size=1.0;

	inter_triangle_interval_ms=100.0;
	single_trial_string="";
	delay_trials_string="c:\\temp\\interval.txt";
	size_trials_string="c:\\temp\\size.txt";
	velocity_trials_string="c:\\temp\\vel.txt";
	all_trials_string="c:\\temp\\all.txt";
	batch_file_string=single_trial_string;
	step_trials_string="c:\\temp\\step.txt";
	inter_trial_interval_seconds=2.0;

	ktr_slcontrol=FL_CONTROL;
	post_ktr_servo_mode=FL_CONTROL;
	ktr_slcontrol_duration_ms=200.0;
	post_ktr_ms_fl=5;

	if (no_of_triangles==0)
	{
		post_ktr_ms=((duration_seconds*(float)1000.0)-
			(pre_ktr_ms+ktr_step_duration_ms));
	}
	else
	{
		post_ktr_ms=((duration_seconds*(float)1000.0)-
			(pre_ktr_ms+ktr_step_duration_ms+
				(no_of_triangles*2*triangle_halftime_ms)+
				((no_of_triangles-1)*inter_triangle_interval_ms)+
				pre_triangle_ms));
	}

	sprintf(temp_string,"%.0f",post_ktr_ms);
	post_ktr_ms_string=temp_string;

	if (ktr_slcontrol==FL_CONTROL)
	{
		post_ktr_final_fl_ms=post_ktr_ms;
	}
	else
	{
		post_ktr_final_fl_ms=(post_ktr_ms-
			(post_ktr_ms_fl+ktr_slcontrol_duration_ms));
	}

	sprintf(temp_string,"%.0f",post_ktr_final_fl_ms);
	post_ktr_final_fl_ms_string=temp_string;
	
	status_middle_string="1) SL_SREC dialog initialised";
	status_bottom_string="2) Ready for input";
	status_counter=3;

	SL_calibration=SL_NOT_CALIBRATED;

	parameters_valid=1;
	previous_parameters_valid=1;

	// Create pipes and data buffer for Step trials

	overflow_threshold=65000;
	pipe_fl = new float [MAX_DATA_POINTS+1];
	pipe_overflow = new short [MAX_DATA_POINTS+1];

	// Thread stuff

	m_pThread=NULL;
}

SL_SREC::~SL_SREC()
{
	// Destructor

	delete [] pipe_fl;
	delete [] pipe_overflow;
}

void SL_SREC::remote_shut_down(void)
{
	// Called by parent to shut down window

	OnCancel();
}

void SL_SREC::PostNcDestroy() 
{
	DWORD dwExitCode;
	if (m_pThread!=NULL)
	{
		::GetExitCodeThread(m_pThread->m_hThread,&dwExitCode);
		if (dwExitCode == STILL_ACTIVE)
		{
			// Thread is still running
			MessageBox("Thread is still running");
		}
		else
		{
			if (dwExitCode == 3)
			{
				MessageBox("Ken's exit code");
			}
			else
			{
				MessageBox("Not Ken's exit code");
			}
		
			delete m_pThread;
		}
	}


	delete this;
}

BOOL SL_SREC::Create()
{
	return CDialog::Create(m_nID, m_pParent);
}

BOOL SL_SREC::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	// Variables

	int width=1022;																// screen width and height
	int height=740;																// and pixels

	// Set window size and position

	SetWindowPos(NULL,(int)(0.02*width),(int)(0.07*height),
		(int)(0.63*width),(int)(0.43*height),SWP_SHOWWINDOW);

	// Disable calibration control

	GetDlgItem(IDC_SL_to_FL_volts_calibration)->EnableWindow(FALSE);
	GetDlgItem(IDC_SL_volts_to_FL_volts)->EnableWindow(FALSE);

	// Disable post_ktr_points

	GetDlgItem(IDC_POST_KTR)->EnableWindow(FALSE);
	GetDlgItem(IDC_POST_KTR_FINAL_FL)->EnableWindow(FALSE);

	// Disable triangle controls

	GetDlgItem(IDC_TRI_SIZE)->EnableWindow(FALSE);
	GetDlgItem(IDC_TRI_HALFTIME)->EnableWindow(FALSE);
	GetDlgItem(IDC_PRE_TRI)->EnableWindow(FALSE);
	GetDlgItem(IDC_SERVO_MODE)->EnableWindow(FALSE);

	GetDlgItem(IDC_RAMP_CONTROL)->EnableWindow(FALSE);
	GetDlgItem(IDC_RAMP_INCREMENT_CONTROL)->EnableWindow(FALSE);

	GetDlgItem(IDC_INTER_TRIANGLE_INTERVAL)->EnableWindow(FALSE);

	// Disable servo controls

	GetDlgItem(IDC_CLAMP_MODE)->EnableWindow(FALSE);
	GetDlgItem(IDC_PRE_SERVO_POINTS)->EnableWindow(FALSE);
	GetDlgItem(IDC_PROPORTIONAL_GAIN)->EnableWindow(FALSE);

	GetDlgItem(IDC_KTR_SLCONTROL_DURATION_MS)->EnableWindow(FALSE);
	GetDlgItem(IDC_POST_KTR_FL)->EnableWindow(FALSE);

	// Disable record duration control

	GetDlgItem(IDC_DURATION)->EnableWindow(FALSE);

	// Disable multiple trials controls

	GetDlgItem(IDC_BATCH_FILE)->EnableWindow(FALSE);
	GetDlgItem(IDC_INTER_TRIAL_INTERVAL)->EnableWindow(FALSE);

	// Status

	display_status(TRUE,"SL calibration not yet completed");

	// Create the Calibration window

	p_Calibration = new Calibration();
	p_Calibration->Create();

	// Create Experiment_display window

	p_Experiment_display = new Experiment_display(this,p_SL_SREC_record);
	p_Experiment_display->Create();

	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SL_SREC::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SL_SREC)
	DDX_Control(pDX, IDC_EXPT_MODE, experiment_mode_control);
	DDX_Control(pDX, IDC_POST_KTR_FINAL_FL, post_ktr_final_fl_ms_control);
	DDX_Control(pDX, IDC_POST_KTR, post_ktr_ms_control);
	DDX_Control(pDX, IDC_DURATION, duration_control);
	DDX_Control(pDX, IDC_NO_OF_TRIANGLES, no_of_triangles_control);
	DDX_Control(pDX, IDC_SAMPLING_RATE, SAMPLING_RATE_CONTROL);
	DDX_Control(pDX, IDC_SERVO_MODE, SERVO_MODE_CONTROL);
	DDX_Text(pDX, IDC_SL_to_FL_volts_calibration, SL_SREC_SL_volts_to_FL_microns_calibration);
	DDX_Text(pDX, IDC_KTR_STEP_SIZE, ktr_step_size_microns);
	DDX_Text(pDX, IDC_KTR_STEP_DURATION, ktr_step_duration_ms);
	DDX_CBString(pDX, IDC_SERVO_MODE, servo_mode_string);
	DDX_CBString(pDX, IDC_CLAMP_MODE, clamp_mode_string);
	DDX_CBString(pDX, IDC_SAMPLING_RATE, sampling_rate_string);
	DDX_Text(pDX, IDC_DATA_POINTS, data_points);
	DDV_MinMaxInt(pDX, data_points, 1, 650000);
	DDX_Text(pDX, IDC_DURATION, duration_string);
	DDX_Text(pDX, IDC_TRI_SIZE, triangle_size_microns);
	DDX_Text(pDX, IDC_TRI_HALFTIME, triangle_halftime_ms);
	DDX_Text(pDX, IDC_PRE_SERVO_POINTS, pre_servo_points);
	DDX_Text(pDX, IDC_PROPORTIONAL_GAIN, proportional_gain);
	DDX_Text(pDX, IDC_DATA_PATH, data_path_string);
	DDX_Text(pDX, IDC_DATA_BASE, data_base_string);
	DDX_Text(pDX, IDC_DATA_INDEX, data_index);
	DDX_Text(pDX, IDC_PRE_TRI, pre_triangle_ms);
	DDX_Text(pDX, IDC_BATCH_FILE, batch_file_string);
	DDX_Text(pDX, IDC_INTER_TRIAL_INTERVAL, inter_trial_interval_seconds);
	DDX_Text(pDX, IDC_INTER_TRIANGLE_INTERVAL, inter_triangle_interval_ms);
	DDX_Text(pDX, IDC_KTR_SLCONTROL_DURATION_MS, ktr_slcontrol_duration_ms);
	DDX_Check(pDX, IDC_KTR_SLCONTROL, ktr_slcontrol);
	DDX_CBString(pDX, IDC_NO_OF_TRIANGLES, no_of_triangles_string);
	DDX_Text(pDX, IDC_PRE_KTR, pre_ktr_ms);
	DDX_Text(pDX, IDC_POST_KTR, post_ktr_ms_string);
	DDX_Text(pDX, IDC_POST_KTR_FL, post_ktr_ms_fl);
	DDV_MinMaxFloat(pDX, post_ktr_ms_fl, 0.f, 50.f);
	DDX_Text(pDX, IDC_POST_KTR_FINAL_FL, post_ktr_final_fl_ms_string);
	DDX_Text(pDX, IDC_SL_volts_to_FL_volts, SL_volts_to_FL_command_volts);
	DDX_CBString(pDX, IDC_EXPT_MODE, experiment_mode_string);
	DDX_Check(pDX, IDC_RAMP_CONTROL, ramp_option);
	DDX_Check(pDX, IDC_RAMP_INCREMENT_CONTROL, ramp_increment_option);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SL_SREC, CDialog)
	//{{AFX_MSG_MAP(SL_SREC)
	ON_BN_CLICKED(IDC_Run_Experiment, OnRunExperiment)
	ON_BN_CLICKED(IDC_SL_to_FL_Calibration_Override, OnSLtoFLCalibrationOverride)
	ON_CBN_SELCHANGE(IDC_SERVO_MODE, OnSelchangeServoMode)
	ON_CBN_SELCHANGE(IDC_SAMPLING_RATE, OnSelchangeSamplingRate)
	ON_EN_CHANGE(IDC_DATA_POINTS, OnChangeDataPoints)
	ON_BN_CLICKED(IDC_KTR_SLCONTROL, OnKtrSlcontrol)
	ON_CBN_SELCHANGE(IDC_NO_OF_TRIANGLES, OnSelchangeNoOfTriangles)
	ON_EN_CHANGE(IDC_INTER_TRIANGLE_INTERVAL, OnChangeInterTriangleInterval)
	ON_EN_CHANGE(IDC_KTR_STEP_DURATION, OnChangeKtrStepDuration)
	ON_EN_CHANGE(IDC_KTR_SLCONTROL_DURATION_MS, OnChangeKtrSlcontrolDurationMs)
	ON_EN_CHANGE(IDC_POST_KTR_FL, OnChangePostKtrFl)
	ON_EN_CHANGE(IDC_PRE_KTR, OnChangePreKtr)
	ON_EN_CHANGE(IDC_TRI_HALFTIME, OnChangeTriHalftime)
	ON_EN_CHANGE(IDC_PRE_TRI, OnChangePreTri)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_EXPT_MODE, OnSelchangeExptMode)
	ON_BN_CLICKED(IDC_RAMP_CONTROL, OnRampControl)
	ON_BN_CLICKED(IDC_RAMP_INCREMENT_CONTROL, OnRampIncrementControl)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_Abort, OnAbort)
	ON_BN_CLICKED(IDC_BROWSE_PATH, OnBnClickedBrowsePath)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SL_SREC message handlers

void SL_SREC::OnCancel() 
{
	// Close derived windows

	p_Calibration->close_calibration_window();
	p_Calibration=NULL;

	p_Experiment_display->close_experiment_display_window();
	p_Experiment_display=NULL;

	// Signal to parent that this window is closing

	((CSLControlDlg*)m_pParent)->SL_SREC_Window_Done();

	DestroyWindow();
}

void SL_SREC::OnBnClickedBrowsePath()
{
	CString strInitialPath;
	CString strYourCaption(_T("Directory Browser"));
	CString strYourTitle(_T("Select path for data filing\n(or create a new directory)"));

	CPathDialog dlg(strYourCaption, strYourTitle, strInitialPath);

	if (dlg.DoModal()==IDOK)
	{
		data_path_string=dlg.GetPathName();
	}

	UpdateData(FALSE);
}

void SL_SREC::OnRunExperiment() 
{
	////////////////////////////
	// Ken's code starts here //
	////////////////////////////
 
	// Code runs SL_SREC experiment based on parameter values set by user
	// in dialog window

	// Variables

	int i;											// counter
	
	float initial_inter_triangle_interval_ms;
	float initial_triangle_halftime_ms;
													// holds initial inter_triangle_interval_ms and
													// resets to this at the end of the trials

	float interval_ms[101];							// array holding inter_triangle intervals
	float first_tri_size[101];						// array holding relative first triangle size
	float relative_velocity[101];					// array holding relative velocities
	float first_step_time_ms[101];					// array holding step time
	float first_step_size_microns[101];
	float second_step_time_ms[101];
	float second_step_size_microns[101];

	char display_string[300];						// string for user output
	char cursor_path_string[300];					// string holding path for animated cursor
							
	// Code

	// Read screen

	UpdateData(TRUE);
	p_Calibration->UpdateData(TRUE);

	// Update initial values

	initial_inter_triangle_interval_ms=inter_triangle_interval_ms;
	initial_triangle_halftime_ms=triangle_halftime_ms;

	// Set sampling rate parameters

	set_sampling_parameters();

	// Pre KTR control mode
	
	if 	(SERVO_MODE_CONTROL.GetCurSel()==FL_CONTROL)							// Fibre Length Control
		pre_ktr_servo_mode=FL_CONTROL;
	else																		// Sarcomere Length Control
		pre_ktr_servo_mode=SL_CONTROL;

	// Post KTR control mode

	if (ktr_slcontrol==FL_CONTROL)
		post_ktr_servo_mode=FL_CONTROL;
	else
		post_ktr_servo_mode=SL_CONTROL;

	// Zero clamp mode

	if (clamp_mode_string=="Prevailing Position")
		clamp_mode=PREVAILING_POSITION;
	else
		clamp_mode=ZERO_VOLTS;

	// Single or multiple trial

	if (experiment_mode==SL_SREC_SINGLE_TRIAL)									// Single trial
	{
		no_of_trials=1;
		interval_ms[1]=inter_triangle_interval_ms;
		first_tri_size[1]=1.0;
		relative_velocity[1]=1.0;
	}
	else																		// Multiple trials
	{
		// Open and check file

		FILE* batch_file=fopen(batch_file_string,"r");

		if (batch_file==NULL)
		{
			sprintf(display_string,"Batch file\n%s\ncould not be opened\n",batch_file_string);
			MessageBox(display_string,"SL_SREC::OnRunExperiment()");
			return;
		}

		// Read in no_of_trials

		fscanf(batch_file,"%i",&no_of_trials);

		// Switch depending on experiment_mode

		switch (experiment_mode)
		{
			case (SL_SREC_DELAY_TRIALS):
			{
				// Read in intervals
			
				for (i=1;i<=no_of_trials;i++)
				{
					fscanf(batch_file,"%f",&interval_ms[i]);
					first_tri_size[i]=1.0;
					relative_velocity[i]=1.0;

					first_step_time_ms[i]=0.0;
					first_step_size_microns[i]=0.0;
					second_step_time_ms[i]=0.0;
					second_step_size_microns[i]=0.0;
				}
			}
			break;

			case (SL_SREC_SIZE_TRIALS):
			{
				// Read in sizes
			
				for (i=1;i<=no_of_trials;i++)
				{
					fscanf(batch_file,"%f",&first_tri_size[i]);
					interval_ms[i]=inter_triangle_interval_ms;
					relative_velocity[i]=1.0;

					first_step_time_ms[i]=0.0;
					first_step_size_microns[i]=0.0;
					second_step_time_ms[i]=0.0;
					second_step_size_microns[i]=0.0;
				}
			}
			break;

			case (SL_SREC_VELOCITY_TRIALS):
			{
				// Read in velocities

				for (i=1;i<=no_of_trials;i++)
				{
					fscanf(batch_file,"%f",&relative_velocity[i]);
					interval_ms[i]=inter_triangle_interval_ms;
					first_tri_size[i]=1.0;

					first_step_time_ms[i]=0.0;
					first_step_size_microns[i]=0.0;
					second_step_time_ms[i]=0.0;
					second_step_size_microns[i]=0.0;
				}
			}
			break;

			case (SL_SREC_ALL_TRIALS):
			{
				// Read in interval, size and velocity

				for (i=1;i<=no_of_trials;i++)
				{
					fscanf(batch_file,"%f",&interval_ms[i]);
					fscanf(batch_file,"%f",&first_tri_size[i]);
					fscanf(batch_file,"%f",&relative_velocity[i]);

					first_step_time_ms[i]=0.0;
					first_step_size_microns[i]=0.0;
					second_step_time_ms[i]=0.0;
					second_step_size_microns[i]=0.0;
				}
			}
			break;

			case (SL_SREC_STEP_TRIALS):
			{
				// Read in time and step_size

				for (i=1;i<=no_of_trials;i++)
				{
					fscanf(batch_file,"%f",&first_step_time_ms[i]);
					fscanf(batch_file,"%f",&first_step_size_microns[i]);
					fscanf(batch_file,"%f",&second_step_time_ms[i]);
					fscanf(batch_file,"%f",&second_step_size_microns[i]);

					interval_ms[i]=inter_triangle_interval_ms;
					first_tri_size[i]=1.0;
					relative_velocity[i]=1.0;
				}
			}
			break;
		}
		
		// Close file

		fclose(batch_file);
	}

	// Loop through trials running individual experiments

	// Switch cursor to animated cursor
	
	::set_executable_directory_string(cursor_path_string);
	strcat(cursor_path_string,"\\sl_control.ani");

/*
	HCURSOR hcurWait = (HCURSOR)LoadImage(AfxGetApp()->m_hInstance,
		cursor_path_string,IMAGE_CURSOR,0,0,
		LR_DEFAULTSIZE|LR_DEFAULTCOLOR|LR_LOADFROMFILE);
	const HCURSOR hcurSave=SetCursor(hcurWait);

	SetCursor(hcurWait);
*/
	// Add abort option

	if (no_of_trials>1)
	{
		MessageBox("Trials>1");

		// Create an abort window
	
		m_pThread = AfxBeginThread(RUNTIME_CLASS(Abort_multiple_trials),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
		m_pThread->m_bAutoDelete = FALSE;

		// Set pointer back to this window

		Abort_multiple_trials* p_amt;
		p_amt = (Abort_multiple_trials*) m_pThread;
//		p_amt->p_SL_SREC = this;

		m_pThread->ResumeThread();

		UpdateData(FALSE);
	}

/*
thread_structure test_struct;
test_struct.flag=0;
test_struct.p_SL_SREC_object=this;

sprintf(display_string,"Test structure flag address: %i",&test_struct.flag);
MessageBox(display_string);
*/
	// Loop through trials



	i=1;
	thread_problem=0;

	while ((i<=no_of_trials)&&(!thread_problem))
	{
        current_trial=i;

		inter_triangle_interval_ms=interval_ms[i];
		relative_first_tri_size=first_tri_size[i];
		triangle_halftime_ms=initial_triangle_halftime_ms/relative_velocity[i];
		
		first_step_ms=first_step_time_ms[i];
		first_step_microns=first_step_size_microns[i];
		second_step_ms=second_step_time_ms[i];
		second_step_microns=second_step_size_microns[i];

		// Status

		sprintf(display_string,"File %i/%i dt: %.0f ms Size: %.1f Vel: %.1f",
			i,no_of_trials,inter_triangle_interval_ms,relative_first_tri_size,relative_velocity[i]);
		display_status(TRUE,display_string);

		// Update values

		set_post_ktr_values();

		// Check post_ktr_final_fl_ms

		if (parameters_valid<0)
		{
			MessageBox("Trial aborted - parameters not valid","SL_SREC::OnRunExperiment");
			return;
		}

		// Create DAPL_source_file

		if (experiment_mode!=SL_SREC_STEP_TRIALS)
		{
            if (create_DAPL_source_file()==0)
			{
				sprintf(display_string,"DAPL_source_file could not be created\n");
				display_status(TRUE,display_string);
				MessageBox(display_string,"SL_SREC::OnRunExperiment");
				return;
			}
		}
		else
		{
			sprintf(display_string,"Step file :%i",create_step_DAPL_source_file());
			display_status(TRUE,display_string);
		}

		// If SL feedback required, check SL is calibrated

		if (((pre_ktr_servo_mode==SL_CONTROL)||(post_ktr_servo_mode==SL_CONTROL)) &&
			(SL_calibration==SL_NOT_CALIBRATED))
		{
			sprintf(display_string,"SL not yet calibrated - Abort experiment");
			display_status(TRUE,display_string);
			MessageBox(display_string,"SL_SREC::OnRunExperiment");
			return;
		}
		
		// Run Experiment as a separate thread

		thread_running=1;
		thread_problem=0;

		// Run normal thread unless it is a step trial
		
		CWinThread* pThread;

		if (experiment_mode!=SL_SREC_STEP_TRIALS)
		{
			if (data_points<65000)
				pThread = AfxBeginThread(run_experiment_as_thread,p_SL_SREC,THREAD_PRIORITY_NORMAL,0,0,NULL);
			else
				pThread = AfxBeginThread(run_tri_experiment_as_thread,p_SL_SREC,THREAD_PRIORITY_NORMAL,0,0,NULL);
		}
		else
		{
			pThread = AfxBeginThread(run_step_experiment_as_thread,p_SL_SREC,THREAD_PRIORITY_NORMAL,0,0,NULL);
		}
		
		// Wait until the thread signals that it has finished, checking every 50 ms

		while (thread_running)
		{
			Sleep(500);

			if (thread_problem)
			{
				display_status(TRUE,"Experimental thread failed");
			}
		}
		
		// Now delay if not the last file in the series

//		if ((i<no_of_trials)&&(!thread_problem))

		if ((i<no_of_trials))
		{
			thread_running=1;
			thread_problem=0;

			pThread=AfxBeginThread(delay_as_thread,p_SL_SREC,THREAD_PRIORITY_NORMAL,0,0,NULL);

			while (thread_running)
			{
				Sleep(50);
			}
		}

		// Update counter

		i++;
	}

	// Close abort window if necessary

	if (no_of_trials>1)
	{
		//p_Abort_multiple_trials->close_Abort_multiple_trials_window();
		//p_Abort_multiple_trials=NULL;
	}

	// End Cursor wait

//	SetCursor(hcurSave);

	// Tidy up

	sprintf(display_string,"Trials completed");
	display_status(TRUE,display_string);
	MessageBeep(MB_ICONEXCLAMATION);

	// Reset relative_first_tri_size to default

	relative_first_tri_size=1.0;
	inter_triangle_interval_ms=initial_inter_triangle_interval_ms;
	triangle_halftime_ms=initial_triangle_halftime_ms;

	UpdateData(FALSE);

	return;

	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////
}

UINT run_experiment_as_thread(LPVOID set_pointer)
{
	SL_SREC* p_SL_SREC=(SL_SREC*)set_pointer;

	// Code runs a single trial as a separate thread
	// Parent function constructs the DAPL command file

	// Variables

	char display_string[300];												// user input

	int i;																	// counter
	short short_int=1;														// used for sizeof operator
	int holder;																// another counter
	int buffer_size=(4*p_SL_SREC->data_points);								// number of points in
																			// data buffer with 4 channels
																			// being read from DAPL

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

	// Run experiment

	start_upload=clock();
	p_SL_SREC->display_status(TRUE,"Passing command file to DAP");

	i=DapConfig(hdapSysPut,p_SL_SREC->DAPL_source_file_string);

	// Error Checking

	if (i==FALSE)
	{
		p_SL_SREC->thread_running=0;
		p_SL_SREC->thread_problem=1;
		sprintf(display_string,"DAPL could not handle source file\n%s\nResulting records will be invalid",
			p_SL_SREC->DAPL_source_file_string);
		p_SL_SREC->m_pParent->MessageBox(display_string,"SL_SREC::run_single_trial()",MB_ICONWARNING);
		delete [] DAPL_data;
		return 0;
	}

	sprintf(display_string,"Trial started, DAPL upload took %.4f s",(float)(clock()-start_upload)/CLOCKS_PER_SEC);
	p_SL_SREC->display_status(TRUE,display_string);

	i=DapBufferGet(hdapBinGet,buffer_size*sizeof(short_int),DAPL_data);

	// Display status

	sprintf(display_string,"%i Points/Channel read from DAPL processor",(i/(4*sizeof(short_int))));
	p_SL_SREC->display_status(TRUE,display_string);

	// Copy data to SL_SREC_record and update no_of_points

	holder=1;

	for (i=0;i<buffer_size;i=i+4)
	{
		p_SL_SREC->p_SL_SREC_record->data[TIME][holder]=((float)(holder-1)*p_SL_SREC->time_increment);
		p_SL_SREC->p_SL_SREC_record->data[FORCE][holder]=convert_DAPL_units_to_volts(DAPL_data[i]);
		p_SL_SREC->p_SL_SREC_record->data[SL][holder]=convert_DAPL_units_to_volts(DAPL_data[i+1]);
		p_SL_SREC->p_SL_SREC_record->data[FL][holder]=convert_DAPL_units_to_volts(DAPL_data[i+2]);
		p_SL_SREC->p_SL_SREC_record->data[INTENSITY][holder]=convert_DAPL_units_to_volts(DAPL_data[i+3]);

		holder++;
	}

	p_SL_SREC->p_SL_SREC_record->no_of_points=p_SL_SREC->data_points;

	// Close communication pipes

	DapHandleClose(hdapSysGet);
	DapHandleClose(hdapSysPut);
	DapHandleClose(hdapBinGet);

	// Write data to file

	p_SL_SREC->write_data_to_file();

	// Update data plots

	p_SL_SREC->p_Experiment_display->clear_window();

	for (i=0;i<=(p_SL_SREC->p_Experiment_display->no_of_plots-1);i++)
	{
		p_SL_SREC->p_Experiment_display->pointers_array[i]->first_drawn_point=1;
		
		p_SL_SREC->p_Experiment_display->pointers_array[i]->last_drawn_point=
			p_SL_SREC->p_SL_SREC_record->no_of_points;
	}

	p_SL_SREC->p_Experiment_display->update_plots();

	// Free memory

	delete [] DAPL_data;

	// Signal thread is finishing

	p_SL_SREC->thread_running=0;
	p_SL_SREC->display_status(TRUE,"Finished first thread");

	return 1;
}

UINT run_step_experiment_as_thread(LPVOID set_pointer)
{
	SL_SREC* p_SL_SREC=(SL_SREC*)set_pointer;

	// Code runs a single trial as a separate thread
	// Parent function constructs the DAPL command file

	// Variables

	char display_string[300];												// user input

	int i;																	// counter
	short short_int=1;														// used for sizeof operator
	int holder;																// another counter
	int buffer_size=(4*p_SL_SREC->data_points);								// number of points in
																			// data buffer with 4 channels
																			// being read from DAPL

	int return_value;
	int bytes_available;
	int points_available;
	int buffer_points;
	int points_read;
	int points_written;

	clock_t start_upload;													// time at start of DAPL upload

	clock_t start_loop,stop_loop,last_display;
	
	short int* DAPL_data = new short int [buffer_size];						// data array for DAPL processing

	HDAP hdapSysGet,hdapSysPut,hdapBinGet,hdapBinPut;						// DAPL communication pipes

	// Dap structures

	TDapBufferPutEx command_stream;
	DapStructPrepare(command_stream);

	TDapBufferGetEx record_stream;
	DapStructPrepare(record_stream);

	// Code

	// Open and flush DAPL communication pipes

	hdapSysGet=DapHandleOpen("\\\\.\\Dap0\\$SysOut",DAPOPEN_READ);
	hdapSysPut=DapHandleOpen("\\\\.\\Dap0\\$SysIn",DAPOPEN_WRITE);
	hdapBinGet=DapHandleOpen("\\\\.\\Dap0\\$BinOut",DAPOPEN_READ);
	hdapBinPut=DapHandleOpen("\\\\.\\Dap0\\$BinIn",DAPOPEN_WRITE);

	DapInputFlush(hdapSysGet);
	DapInputFlush(hdapSysPut);
	DapInputFlush(hdapBinGet);
	DapInputFlush(hdapBinPut);

	// Prepare DAP structures

	command_stream.iBytesPut=100*sizeof(short_int);
	command_stream.dwTimeWait=10;
	command_stream.dwTimeOut=100;
	command_stream.iBytesMultiple=1*sizeof(short_int);

	record_stream.iBytesGetMin=4*sizeof(short_int);
	record_stream.iBytesGetMax=1000*sizeof(short_int);
	record_stream.dwTimeWait=10;
	record_stream.dwTimeOut=100;
	record_stream.iBytesMultiple=4*sizeof(short_int);

	// First send original file

	start_upload=clock();
	p_SL_SREC->display_status(TRUE,"Passing command file to DAP");

	i=DapConfig(hdapSysPut,p_SL_SREC->DAPL_source_file_string);

	// Error Checking

	if (i==FALSE)
	{
		p_SL_SREC->thread_running=0;
		p_SL_SREC->thread_problem=1;
		sprintf(display_string,"DAPL could not handle source file\n%s\nResulting records will be invalid",
			p_SL_SREC->DAPL_source_file_string);
		p_SL_SREC->m_pParent->MessageBox(display_string,"SL_SREC::run_single_trial()",MB_ICONWARNING);
		delete [] DAPL_data;
		return 0;
	}

	sprintf(display_string,"Trial started, DAPL upload took %.4f s",(float)(clock()-start_upload)/CLOCKS_PER_SEC);
	p_SL_SREC->display_status(TRUE,display_string);

	// Now loop until all points have been read

	points_read=0;
	points_written=0;
	last_display=clock();
    
	while (points_read<p_SL_SREC->data_points)
	{
		start_loop=clock();										// record time

		// Try to send data if not yet complete

		if (points_written<p_SL_SREC->overflow_points)
		{
			if ((p_SL_SREC->overflow_points-points_written)>1000)
				command_stream.iBytesPut=1000*sizeof(short_int);
			else
				command_stream.iBytesPut=(p_SL_SREC->overflow_points-points_written)*sizeof(short_int);
			

			return_value=DapBufferPutEx(hdapBinPut,&command_stream,&p_SL_SREC->pipe_overflow[points_written]);

			if (return_value>=0)
			{
				points_written=points_written+return_value/sizeof(short_int);
			}
		}

		// Try to read data back

		bytes_available=DapBufferGetEx(hdapBinGet,&record_stream,DAPL_data);

		if (bytes_available>0)
		{
			points_available=bytes_available/(4*sizeof(short_int));
			buffer_points=bytes_available/(sizeof(short_int));

			holder=1;

			for (i=0;i<buffer_points;i=i+4)
			{
				p_SL_SREC->p_SL_SREC_record->data[TIME][points_read+holder]=((float)(points_read+holder-1)*p_SL_SREC->time_increment);
				p_SL_SREC->p_SL_SREC_record->data[FORCE][points_read+holder]=convert_DAPL_units_to_volts(DAPL_data[i]);
				p_SL_SREC->p_SL_SREC_record->data[SL][points_read+holder]=convert_DAPL_units_to_volts(DAPL_data[i+1]);
				p_SL_SREC->p_SL_SREC_record->data[FL][points_read+holder]=convert_DAPL_units_to_volts(DAPL_data[i+2]);
				p_SL_SREC->p_SL_SREC_record->data[INTENSITY][points_read+holder]=convert_DAPL_units_to_volts(DAPL_data[i+3]);

				holder++;
			}

			points_read=(points_read+points_available);
		}

		stop_loop=clock();

		if (((float)(clock()-last_display)/(float)CLOCKS_PER_SEC)>1.0)
		{
			sprintf(display_string,"Last loop %.0f ms, Left to write %i, %.3f read",
				(float)(1000.0*(stop_loop-start_loop)/CLOCKS_PER_SEC),
				(p_SL_SREC->overflow_points-points_written),
				(float)points_read/(float)p_SL_SREC->data_points);
			p_SL_SREC->display_status(TRUE,display_string);

			last_display=clock();
		}
	}

//fprintf(output_file,"%i points_written",points_written);

//fclose(output_file);

	p_SL_SREC->p_SL_SREC_record->no_of_points=p_SL_SREC->data_points;

	// Close communication pipes

	DapHandleClose(hdapSysGet);
	DapHandleClose(hdapSysPut);
	DapHandleClose(hdapBinGet);
	DapHandleClose(hdapBinPut);

	// Write data to file

	p_SL_SREC->write_data_to_file();

	// Update data plots

	p_SL_SREC->p_Experiment_display->clear_window();

	for (i=0;i<=(p_SL_SREC->p_Experiment_display->no_of_plots-1);i++)
	{
		p_SL_SREC->p_Experiment_display->pointers_array[i]->first_drawn_point=1;
		
		p_SL_SREC->p_Experiment_display->pointers_array[i]->last_drawn_point=
			p_SL_SREC->p_SL_SREC_record->no_of_points;
	}

	p_SL_SREC->p_Experiment_display->update_plots();

	// Free memory

	delete [] DAPL_data;

	// Signal thread is finishing

	p_SL_SREC->thread_running=0;
	p_SL_SREC->display_status(TRUE,"Finished first thread");

	return 1;
}

UINT run_tri_experiment_as_thread(LPVOID set_pointer)
{
	SL_SREC* p_SL_SREC=(SL_SREC*)set_pointer;

	// Code runs a single trial as a separate thread
	// Parent function constructs the DAPL command file

	// Variables

	char display_string[300];												// user input

	int i;																	// counter
	short short_int=1;														// used for sizeof operator
	int holder;																// another counter
	int buffer_size=(4*p_SL_SREC->data_points);								// number of points in
																			// data buffer with 4 channels
																			// being read from DAPL

	int return_value;
	int bytes_available;
	int points_available;
	int buffer_points;
	int points_read;
	int points_written;

	clock_t start_upload;													// time at start of DAPL upload

	clock_t start_loop,stop_loop,last_display;
	
	short int* DAPL_data = new short int [buffer_size];						// data array for DAPL processing

	HDAP hdapSysGet,hdapSysPut,hdapBinGet,hdapBinPut;						// DAPL communication pipes

	// Dap structures

	TDapBufferPutEx command_stream;
	DapStructPrepare(command_stream);

	TDapBufferGetEx record_stream;
	DapStructPrepare(record_stream);

	// Code

	// Open and flush DAPL communication pipes

	hdapSysGet=DapHandleOpen("\\\\.\\Dap0\\$SysOut",DAPOPEN_READ);
	hdapSysPut=DapHandleOpen("\\\\.\\Dap0\\$SysIn",DAPOPEN_WRITE);
	hdapBinGet=DapHandleOpen("\\\\.\\Dap0\\$BinOut",DAPOPEN_READ);
	hdapBinPut=DapHandleOpen("\\\\.\\Dap0\\$BinIn",DAPOPEN_WRITE);

	DapInputFlush(hdapSysGet);
	DapInputFlush(hdapSysPut);
	DapInputFlush(hdapBinGet);
	DapInputFlush(hdapBinPut);

	// Prepare DAP structures

	command_stream.iBytesPut=100*sizeof(short_int);
	command_stream.dwTimeWait=10;
	command_stream.dwTimeOut=100;
	command_stream.iBytesMultiple=1*sizeof(short_int);

	record_stream.iBytesGetMin=4*sizeof(short_int);
	record_stream.iBytesGetMax=1000*sizeof(short_int);
	record_stream.dwTimeWait=10;
	record_stream.dwTimeOut=100;
	record_stream.iBytesMultiple=4*sizeof(short_int);

	// First send original file

	start_upload=clock();
	p_SL_SREC->display_status(TRUE,"Passing command file to DAP");

	i=DapConfig(hdapSysPut,p_SL_SREC->DAPL_source_file_string);

	// Error Checking

	if (i==FALSE)
	{
		p_SL_SREC->thread_running=0;
		p_SL_SREC->thread_problem=1;
		sprintf(display_string,"DAPL could not handle source file\n%s\nResulting records will be invalid",
			p_SL_SREC->DAPL_source_file_string);
		p_SL_SREC->m_pParent->MessageBox(display_string,"SL_SREC::run_single_trial()",MB_ICONWARNING);
		delete [] DAPL_data;
		return 0;
	}

	sprintf(display_string,"Trial started, DAPL upload took %.4f s",(float)(clock()-start_upload)/CLOCKS_PER_SEC);
	p_SL_SREC->display_status(TRUE,display_string);

	// Now loop until all points have been read

	points_read=0;
	points_written=0;
	last_display=clock();
    
	while (points_read<p_SL_SREC->data_points)
	{
		start_loop=clock();										// record time

		// Try to send data if not yet complete

		if (points_written<p_SL_SREC->overflow_points)
		{
			if ((p_SL_SREC->overflow_points-points_written)>1000)
				command_stream.iBytesPut=1000*sizeof(short_int);
			else
				command_stream.iBytesPut=(p_SL_SREC->overflow_points-points_written)*sizeof(short_int);
			

			return_value=DapBufferPutEx(hdapBinPut,&command_stream,&p_SL_SREC->pipe_overflow[points_written]);

			if (return_value>=0)
			{
				points_written=points_written+return_value/sizeof(short_int);
			}
		}

		// Try to read data back

		bytes_available=DapBufferGetEx(hdapBinGet,&record_stream,DAPL_data);

		if (bytes_available>0)
		{
			points_available=bytes_available/(4*sizeof(short_int));
			buffer_points=bytes_available/(sizeof(short_int));

			holder=1;

			for (i=0;i<buffer_points;i=i+4)
			{
				p_SL_SREC->p_SL_SREC_record->data[TIME][points_read+holder]=((float)(points_read+holder-1)*p_SL_SREC->time_increment);
				p_SL_SREC->p_SL_SREC_record->data[FORCE][points_read+holder]=convert_DAPL_units_to_volts(DAPL_data[i]);
				p_SL_SREC->p_SL_SREC_record->data[SL][points_read+holder]=convert_DAPL_units_to_volts(DAPL_data[i+1]);
				p_SL_SREC->p_SL_SREC_record->data[FL][points_read+holder]=convert_DAPL_units_to_volts(DAPL_data[i+2]);
				p_SL_SREC->p_SL_SREC_record->data[INTENSITY][points_read+holder]=convert_DAPL_units_to_volts(DAPL_data[i+3]);

				holder++;
			}

			points_read=(points_read+points_available);
		}

		stop_loop=clock();

		if (((float)(clock()-last_display)/(float)CLOCKS_PER_SEC)>1.0)
		{
			sprintf(display_string,"Last loop %.0f ms, Left to write %i, %.3f read",
				(float)(1000.0*(stop_loop-start_loop)/CLOCKS_PER_SEC),
				(p_SL_SREC->overflow_points-points_written),
				(float)points_read/(float)p_SL_SREC->data_points);
			p_SL_SREC->display_status(TRUE,display_string);

			last_display=clock();
		}
	}

//fprintf(output_file,"%i points_written",points_written);

//fclose(output_file);

	p_SL_SREC->p_SL_SREC_record->no_of_points=p_SL_SREC->data_points;

	// Close communication pipes

	DapHandleClose(hdapSysGet);
	DapHandleClose(hdapSysPut);
	DapHandleClose(hdapBinGet);
	DapHandleClose(hdapBinPut);

	// Write data to file

	p_SL_SREC->write_data_to_file();

	// Update data plots

	p_SL_SREC->p_Experiment_display->clear_window();

	for (i=0;i<=(p_SL_SREC->p_Experiment_display->no_of_plots-1);i++)
	{
		p_SL_SREC->p_Experiment_display->pointers_array[i]->first_drawn_point=1;
		
		p_SL_SREC->p_Experiment_display->pointers_array[i]->last_drawn_point=
			p_SL_SREC->p_SL_SREC_record->no_of_points;
	}

	p_SL_SREC->p_Experiment_display->update_plots();

	// Free memory

	delete [] DAPL_data;

	// Signal thread is finishing

	p_SL_SREC->thread_running=0;
	p_SL_SREC->display_status(TRUE,"Finished first thread");

	return 1;
}

UINT delay_as_thread(LPVOID set_pointer)
{
	// Delays as a separate thread

	// Variables

	SL_SREC* p_SL_SREC = (SL_SREC*)set_pointer;
	
	char display_string[200];

	// Code

	sprintf(display_string,"File %i of %i.  Pausing for %.2f seconds",
		p_SL_SREC->current_trial,p_SL_SREC->no_of_trials,p_SL_SREC->inter_trial_interval_seconds);
	p_SL_SREC->display_status(TRUE,display_string);
	
	// Sleep

	Sleep(DWORD(p_SL_SREC->inter_trial_interval_seconds*1000.0));

	// Signal that thread is finishing and shut down

	p_SL_SREC->thread_running=0;

	return 1;
}

void SL_SREC::write_data_to_file(void)
{
	// Code writes data record to file

	// Variables

	int i,j;															// counter

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
		MessageBox(display_string,"SL_SREC.cpp");
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
		fprintf(output_data_file,"SL_volts_to_FL_COMMAND_volts: %g\n\n",SL_volts_to_FL_command_volts);

		// Dimensions

		//p_Calibration->UpdateData(TRUE);

		fprintf(output_data_file,"_MUSCLE_DIMENSIONS_\n");
		fprintf(output_data_file,"Muscle_length_(µm): %.3e\n",p_Calibration->Calibration_fibre_length);
		fprintf(output_data_file,"Sarcomere_length_(nm): %.3e\n",p_Calibration->Calibration_sarcomere_length);
		fprintf(output_data_file,"Area_(m^2): %.3e\n\n",p_Calibration->Calibration_area);

		// Experiment parameters

		fprintf(output_data_file,"_SL_SREC_\n");
		fprintf(output_data_file,"experiment_mode: %s\n",experiment_mode_string);
		fprintf(output_data_file,"sampling_rate_(Hz): %.3e\n",sampling_rate);
		fprintf(output_data_file,"data_points: %i\n",data_points);
		fprintf(output_data_file,"ktr_step_(µm): %.3e\n",ktr_step_size_microns);
		fprintf(output_data_file,"ktr_duration_(ms): %.3e\n",ktr_step_duration_ms);
		fprintf(output_data_file,"pre_ktr_(ms): %.3e\n",pre_ktr_ms);

		fprintf(output_data_file,"no_of_triangles: %i\n",no_of_triangles);
		
		fprintf(output_data_file,"triangle_size_(µm): %.3e\n",triangle_size_microns);
		fprintf(output_data_file,"triangle_halftime_(ms): %.3e\n",triangle_halftime_ms);
		fprintf(output_data_file,"inter_triangle_interval_(ms): %.3e\n",inter_triangle_interval_ms);
		fprintf(output_data_file,"relative_first_tri_size: %.3e\n",relative_first_tri_size);
		fprintf(output_data_file,"pre_triangle_(ms): %.3e\n",pre_triangle_ms);
		fprintf(output_data_file,"pre_ktr_servo_mode: %i\n",pre_ktr_servo_mode);
		fprintf(output_data_file,"ramp_mode: %i\n",ramp_mode);
		fprintf(output_data_file,"pre_ktr_servo_mode: %i\n",pre_ktr_servo_mode);
		fprintf(output_data_file,"post_ktr_servo_mode: %i\n",post_ktr_servo_mode);
		fprintf(output_data_file,"proportional_gain: %f\n",proportional_gain);
		fprintf(output_data_file,"ktr_initiation_time_ms: %f\n",ktr_initiation_time_ms);
		fprintf(output_data_file,"proportional_gain: %f\n\n",proportional_gain);

		fprintf(output_data_file,"first_step_ms: %f\n",first_step_ms);
		fprintf(output_data_file,"first_step_microns: %f\n\n",first_step_microns);
		fprintf(output_data_file,"second_step_ms: %f\n",second_step_ms);
		fprintf(output_data_file,"second_step_microns: %f\n\n",second_step_microns);

		// Data

		fprintf(output_data_file,"Data\n");

		for (i=1;i<=p_SL_SREC_record->no_of_points;i++)
		{
			fprintf(output_data_file,"%f\t%f\t%f\t%f\t%f\n",
				p_SL_SREC_record->data[TIME][i],
				p_SL_SREC_record->data[FORCE][i],
				p_SL_SREC_record->data[SL][i],
				p_SL_SREC_record->data[FL][i],
				p_SL_SREC_record->data[INTENSITY][i]);
		}

		// Close file

		fclose(output_data_file);

		// Update counter and screen

		data_index++;
		//UpdateData(FALSE);

		// Update staus

		sprintf(display_string,"Record written to %s",output_data_file_string);
		display_status(TRUE,display_string);
	}
}

void SL_SREC::OnSLtoFLCalibrationOverride() 
{
	// TODO: Add your control notification handler code here

	////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	// Retrieve data from Calibration window and update SL_SREC value

	p_Calibration->UpdateData(TRUE);

	// Retrieve data from SL_SREC window as well

	UpdateData(TRUE);

	SL_SREC_SL_volts_to_FL_microns_calibration=
		p_Calibration->Calibration_SL_volts_to_FL_microns_calibration;

	SL_volts_to_FL_command_volts=
		(FL_POLARITY*SL_SREC_SL_volts_to_FL_microns_calibration*FL_COMMAND_CALIBRATION);

	// Update screen

	UpdateData(FALSE);

	// Update status

	SL_calibration=SL_CALIBRATED;
	display_status(TRUE,"SL calibrated");

	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////
}

void SL_SREC::OnKtrSlcontrol() 
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);

	if (ktr_slcontrol==SL_CONTROL)
	{
		// Post KTR is SL_CONTROL

		GetDlgItem(IDC_KTR_SLCONTROL_DURATION_MS)->EnableWindow(TRUE);
		GetDlgItem(IDC_POST_KTR_FL)->EnableWindow(TRUE);

		GetDlgItem(IDC_CLAMP_MODE)->EnableWindow(TRUE);
		GetDlgItem(IDC_PROPORTIONAL_GAIN)->EnableWindow(TRUE);

		post_ktr_servo_mode=SL_CONTROL;
	}
	else
	{
		// Post KTR is FL_CONTROL

		GetDlgItem(IDC_KTR_SLCONTROL_DURATION_MS)->EnableWindow(FALSE);
		GetDlgItem(IDC_POST_KTR_FL)->EnableWindow(FALSE);

		if ((pre_ktr_servo_mode==SL_CONTROL)&&(no_of_triangles>0))
		{
			GetDlgItem(IDC_CLAMP_MODE)->EnableWindow(TRUE);
			GetDlgItem(IDC_PROPORTIONAL_GAIN)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_CLAMP_MODE)->EnableWindow(FALSE);
			GetDlgItem(IDC_PROPORTIONAL_GAIN)->EnableWindow(FALSE);
		}

		post_ktr_servo_mode=FL_CONTROL;
	}

	set_post_ktr_values();
}

void SL_SREC::OnSelchangeServoMode() 
{
	// TODO: Add your control notification handler code here
	
	////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	if 	(SERVO_MODE_CONTROL.GetCurSel()==0)
	{
		// Servo mode is Fibre Length Control

		GetDlgItem(IDC_PRE_SERVO_POINTS)->EnableWindow(FALSE);

		if (post_ktr_servo_mode==FL_CONTROL)
		{
			GetDlgItem(IDC_CLAMP_MODE)->EnableWindow(FALSE);
			GetDlgItem(IDC_PROPORTIONAL_GAIN)->EnableWindow(FALSE);
		}
		
		pre_ktr_servo_mode=FL_CONTROL;
	}
	else
	{
		// Servo mode is Sarcomere Length Control

		GetDlgItem(IDC_PRE_SERVO_POINTS)->EnableWindow(TRUE);

		GetDlgItem(IDC_CLAMP_MODE)->EnableWindow(TRUE);
		GetDlgItem(IDC_PROPORTIONAL_GAIN)->EnableWindow(TRUE);

		pre_ktr_servo_mode=SL_CONTROL;
	}

	// Check parameters valid

	set_parameters_valid();

	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////
}

void SL_SREC::OnSelchangeNoOfTriangles() 
{
	// TODO: Add your control notification handler code here

	////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	// Variables

	// Code

	set_no_of_triangles();													// set no_of_triangles

	// post_ktr values
	
	set_post_ktr_values();

	// Disable triangle controls if zero triangles selected

	if (no_of_triangles==0)
	{
		GetDlgItem(IDC_INTER_TRIANGLE_INTERVAL)->EnableWindow(FALSE);
		GetDlgItem(IDC_TRI_SIZE)->EnableWindow(FALSE);
		GetDlgItem(IDC_PRE_TRI)->EnableWindow(FALSE);
		GetDlgItem(IDC_TRI_HALFTIME)->EnableWindow(FALSE);

		GetDlgItem(IDC_RAMP_CONTROL)->EnableWindow(FALSE);
		GetDlgItem(IDC_RAMP_INCREMENT_CONTROL)->EnableWindow(FALSE);

		GetDlgItem(IDC_SERVO_MODE)->EnableWindow(FALSE);
		GetDlgItem(IDC_PRE_SERVO_POINTS)->EnableWindow(FALSE);

		if (post_ktr_servo_mode==FL_CONTROL)
		{
			GetDlgItem(IDC_CLAMP_MODE)->EnableWindow(FALSE);
			GetDlgItem(IDC_PROPORTIONAL_GAIN)->EnableWindow(FALSE);
		}
	}
	else
	{
		if ((no_of_triangles>1)&&(experiment_mode!=SL_SREC_DELAY_TRIALS))
			GetDlgItem(IDC_INTER_TRIANGLE_INTERVAL)->EnableWindow(TRUE);
		else
			GetDlgItem(IDC_INTER_TRIANGLE_INTERVAL)->EnableWindow(FALSE);
		
		GetDlgItem(IDC_TRI_SIZE)->EnableWindow(TRUE);
		GetDlgItem(IDC_PRE_TRI)->EnableWindow(TRUE);
		GetDlgItem(IDC_TRI_HALFTIME)->EnableWindow(TRUE);

		GetDlgItem(IDC_RAMP_CONTROL)->EnableWindow(TRUE);
		GetDlgItem(IDC_RAMP_INCREMENT_CONTROL)->EnableWindow(TRUE);

		GetDlgItem(IDC_SERVO_MODE)->EnableWindow(TRUE);

		if (pre_ktr_servo_mode==SL_CONTROL)
		{
			GetDlgItem(IDC_PRE_SERVO_POINTS)->EnableWindow(TRUE);
			GetDlgItem(IDC_CLAMP_MODE)->EnableWindow(TRUE);
			GetDlgItem(IDC_PROPORTIONAL_GAIN)->EnableWindow(TRUE);
		}
	}
	
	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////
}

void SL_SREC::OnSelchangeSamplingRate() 
{
	////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	// Variables

	char* p_string;
	char temp_string[100];
	int temp;

	CString value_string;

	temp=SAMPLING_RATE_CONTROL.GetCurSel();
	SAMPLING_RATE_CONTROL.GetLBText(temp,value_string);

	// Updates record parameters

	// duration_seconds

	duration_seconds=(float)(data_points/strtod(value_string,&p_string));
	sprintf(temp_string,"%.3f",duration_seconds);
	duration_string=temp_string;
	GetDlgItem(IDC_DURATION)->SetWindowText(temp_string);

	// post_ktr_ms

	set_post_ktr_values();
	
	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////
}

void SL_SREC::OnChangeInterTriangleInterval() 
{
	////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	// Code

	UpdateData(TRUE);

	// post_ktr_ms

	set_post_ktr_values();

	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////
}

void SL_SREC::OnChangeDataPoints() 
{
	////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	char* p_string;
	char temp_string[100];
	int temp;

	CString value_string;

	UpdateData(TRUE);

	temp=SAMPLING_RATE_CONTROL.GetCurSel();
	SAMPLING_RATE_CONTROL.GetLBText(temp,value_string);

	duration_seconds=(float)(data_points/strtod(value_string,&p_string));
	sprintf(temp_string,"%.3f",duration_seconds);
	duration_string=temp_string;
	GetDlgItem(IDC_DURATION)->SetWindowText(duration_string);

	// post_ktr

	set_post_ktr_values();

	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////
}

int SL_SREC::convert_time_ms_to_sampling_points(float time_ms)
{
	// Returns no of sample points in a given time interval (expressed in ms)

	// Variables

	int points;													// sample points in time_ms

	// Code

	// Extract sampling_rate

	set_sampling_parameters();

	// Calculate points and return

	points=(int)((time_ms/1000.0)*sampling_rate);

	return(points);
}

void SL_SREC::set_no_of_triangles(void)
{
	// Code returns no_of_triangles (set by control)

	// Variables

	char* p_string;

	int temp;

	CString value_string;

	// Code

	UpdateData(TRUE);

	temp=no_of_triangles_control.GetCurSel();
	no_of_triangles_control.GetLBText(temp,value_string);

	no_of_triangles=strtol(value_string,&p_string,10);
}

void SL_SREC::set_sampling_parameters(void)
{
	// Code sets sampling_rate, time_increment and duration_seconds from SAMPLING_RATE_CONTROL

	// Variables

	char* p_string;									// used to extract sampling rate
	CString value_string;							// from control

	// Code

	// Extract sampling_rate and time_increment

	SAMPLING_RATE_CONTROL.GetLBText(SAMPLING_RATE_CONTROL.GetCurSel(),value_string);
	sampling_rate=(float)strtod(value_string,&p_string);
	time_increment=(float)1.0/sampling_rate;
	duration_seconds=data_points*time_increment;
}

void SL_SREC::set_post_ktr_values(void)
{
	// Code sets post_ktr_ms and post_ktr_final_fl_ms and updates display
	
	// Variables

	char temp_string[100];
	char temp2_string[100];

	float ramp_multiplier;										// 1 if single ramp
																// 2 if triangle

	// Code

	// Set multiplier

	if (ramp_mode==0)
	{
		ramp_multiplier=2.0;
	}
	else
	{
		ramp_multiplier=1.0;
	}

	// Set values

	if (no_of_triangles==0)
	{
		post_ktr_ms=((duration_seconds*(float)1000.0)-
			(pre_ktr_ms+ktr_step_duration_ms));
	}
	else
	{
		post_ktr_ms=((duration_seconds*(float)1000.0)-
			(pre_ktr_ms+ktr_step_duration_ms+
				((float)fabs(relative_first_tri_size)*ramp_multiplier*triangle_halftime_ms)+
				((float)(no_of_triangles-1)*ramp_multiplier*triangle_halftime_ms)+
				((float)(no_of_triangles-1)*inter_triangle_interval_ms)+
				pre_triangle_ms));
	}

	if (ktr_slcontrol==FL_CONTROL)
	{
		post_ktr_final_fl_ms=post_ktr_ms;
	}
	else
	{
		post_ktr_final_fl_ms=(post_ktr_ms-
			(post_ktr_ms_fl+ktr_slcontrol_duration_ms));
	}

	// Update display

	sprintf(temp_string,"%.0f",post_ktr_ms);
	post_ktr_ms_string=temp_string;
	GetDlgItem(IDC_POST_KTR)->SetWindowText(temp_string);

	sprintf(temp2_string,"%.0f",post_ktr_final_fl_ms);
	post_ktr_final_fl_ms_string=temp2_string;
	GetDlgItem(IDC_POST_KTR_FINAL_FL)->SetWindowText(temp2_string);

	// Check values are valid

	set_parameters_valid();
}

void SL_SREC::display_status(int update_mode,CString new_string)
{
	// Scrolls new status messages to screen

	// Variables

	char display_string[300];
	int display_length=75;

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
			sprintf(display_string,"%i) ... %s",status_counter,new_string.Right(display_length-7));
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


void SL_SREC::OnChangeKtrStepDuration() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here

		////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	// Code

	UpdateData(TRUE);

	// post_ktr_ms

	set_post_ktr_values();

	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////
}

void SL_SREC::OnChangeKtrSlcontrolDurationMs() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	
		////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	// Code

	UpdateData(TRUE);

	// post_ktr_ms

	set_post_ktr_values();

	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////
}

void SL_SREC::OnChangePostKtrFl() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here

		////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	// Code

	UpdateData(TRUE);

	// post_ktr_ms

	set_post_ktr_values();

	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////
}

void SL_SREC::OnChangePreKtr() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	
	////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	// Code

	UpdateData(TRUE);

	// post_ktr_ms

	set_post_ktr_values();

	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////
}

void SL_SREC::OnChangeTriHalftime() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here

	////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	// Code

	UpdateData(TRUE);

	// post_ktr_ms

	set_post_ktr_values();

	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////
	
}

void SL_SREC::OnChangePreTri() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	
	////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	// Code

	UpdateData(TRUE);

	// post_ktr_ms

	set_post_ktr_values();

	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////
}

int SL_SREC::create_DAPL_source_file(void)
{
	// Code creates dapl_source_file by combining 4 sub files

	// Variables

	int i;

	int no_of_sub_files=5;

	char single_character;
	char display_string[200];

	// File pointers and array

	FILE* file_pointer[5];

	FILE* DAPL_source_file=fopen(DAPL_source_file_string,"w");

	// Code
		
	if (write_header_file()*write_fill_pipes_file()*write_create_pipes_file()*
			write_control_file()*write_io_procedures_file()==1)
	{
		// Sub-files written successfully - merge files

		// Check files

		if (DAPL_source_file==NULL)
		{
			sprintf(display_string,"%s\ncould not be opened",DAPL_source_file_string);
			MessageBox(display_string,"SL_SREC::create_DAPL_source_file");
			return(0);
		}

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

		for (i=0;i<=(no_of_sub_files-1);i++)
		{
			if (file_pointer[i]==NULL)
			{
				sprintf(display_string,"File %i could not be opened\n",i);
				MessageBox(display_string,"SL_SREC::create_DAPL_source_file");
				return(0);
			}
		}

		// Merge files

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

		return(1);
	}
	else
	{
		sprintf(display_string,"Create_DAPL_source_file failed\n");
		MessageBox(display_string,"create_DAPL_source_file");
		return(0);
	}
}

int SL_SREC::create_step_DAPL_source_file(void)
{
	// Code creates dapl_source_file for step experiments

	// Variables

	int i;

	int ktr_initiation_points;
	int first_step_point=convert_time_ms_to_sampling_points(first_step_ms);
	int second_step_point=convert_time_ms_to_sampling_points(second_step_ms);

	float ktr_increment;
	int  ktr_start;
	int ktr_stop;
	float first_step_increment;
	float second_step_increment;

	char display_string[300];
	clock_t start_pipe;
	
	// File pointers and array

	FILE* DAPL_source_file=fopen(DAPL_source_file_string,"w");

	// Write file

	if (DAPL_source_file==NULL)
	{
		// Error

		sprintf(display_string,"DAPL step file\n%s\n could not be opened\n",DAPL_source_file_string);
		MessageBox(display_string,"SL_SREC::create_DAPL_source_file");
		return(0);
	}
	
	fprintf(DAPL_source_file,"RESET\n\n");
	fprintf(DAPL_source_file,"OPTION SYSINECHO=OFF\n");
	fprintf(DAPL_source_file,"OPTION UNDERFLOWQ=OFF\n");
	fprintf(DAPL_source_file,"OPTION ERRORQ=OFF\n");
	fprintf(DAPL_source_file,"OPTION SCHEDULING=FIXED\n");
	fprintf(DAPL_source_file,"OPTION BUFFERING=OFF\n");
	fprintf(DAPL_source_file,"OPTION QUANTUM=200\n\n");

	fprintf(DAPL_source_file,";************************************************************\n\n");

	fprintf(DAPL_source_file,"// Header\n\n");

	fprintf(DAPL_source_file,"pipes pfl MAXSIZE=65500\n");
	fprintf(DAPL_source_file,"pipes ptrigger MAXSIZE=65500\n");
	fprintf(DAPL_source_file,"pipes pout MAXSIZE=65500\n\n");

	fprintf(DAPL_source_file,";************************************************************\n\n");

	fprintf(DAPL_source_file,"fill ptrigger %i\n\n",convert_volts_to_DAPL_units((float)4.95));

	fprintf(DAPL_source_file,";************************************************************\n\n");

	// Create pipe

	sprintf(display_string,"Starting to fill pfl");
	display_status(TRUE,display_string);
	start_pipe=clock();

	ktr_initiation_time_ms=pre_ktr_ms;
	ktr_initiation_points=convert_time_ms_to_sampling_points(ktr_initiation_time_ms);

	ktr_increment=(float)::convert_volts_to_DAPL_units(::convert_FL_COMMAND_microns_to_volts(ktr_step_size_microns));
	ktr_start=ktr_initiation_points;
	ktr_stop=convert_time_ms_to_sampling_points(ktr_initiation_time_ms+ktr_step_duration_ms);

	for (i=1;i<=ktr_start;i++)
	{
		pipe_fl[i]=(float)0.0;
	}
	for (i=ktr_start+1;i<=ktr_stop;i++)
	{
		pipe_fl[i]=ktr_increment;
	}
	for (i=ktr_stop+1;i<=data_points;i++)
	{
		pipe_fl[i]=(float)0.0;
	}
	
	first_step_increment=(float)::convert_volts_to_DAPL_units(
				::convert_FL_COMMAND_microns_to_volts(first_step_microns));
	second_step_increment=(float)::convert_volts_to_DAPL_units(
				::convert_FL_COMMAND_microns_to_volts(second_step_microns));

	for (i=first_step_point+1;i<=second_step_point;i++)
	{
		pipe_fl[i]=pipe_fl[i]+first_step_increment;
	}

	for (i=second_step_point+1;i<data_points;i++)
	{
		pipe_fl[i]=pipe_fl[i]+first_step_increment+second_step_increment;
	}

	sprintf(display_string,"Created array, %.3f",(clock()-start_pipe)/CLOCKS_PER_SEC);
	display_status(TRUE,display_string);
		
	// Write as much of the pipe to file as possible

	if (data_points<=overflow_threshold)
	{
		write_pipe_to_file(DAPL_source_file,"pfl",pipe_fl,data_points);
		overflow_points=0;
	}
	else
	{
		write_pipe_to_file(DAPL_source_file,"pfl",pipe_fl,overflow_threshold);

		overflow_points=(data_points-overflow_threshold);

		for (i=1;i<=overflow_points;i++)
		{
			pipe_overflow[i-1]=(short)pipe_fl[overflow_threshold+i];
		}
	}

FILE *output_file=fopen("c:\\temp\\overflow.txt","w");
for (i=1;i<=overflow_points;i++)
	fprintf(output_file,"%i\n",pipe_overflow[i]);
fclose(output_file);


	fprintf(DAPL_source_file,";************************************************************\n\n");

	fprintf(DAPL_source_file,"pdef create_pipes\n\n");
	fprintf(DAPL_source_file,"  ptrigger=0\n");
	//fprintf(DAPL_source_file,"  separate($binin,pfl)\n\n");
	fprintf(DAPL_source_file,"  pfl=$binin\n\n");
	fprintf(DAPL_source_file,"end\n\n");

	fprintf(DAPL_source_file,";************************************************************\n\n");

	fprintf(DAPL_source_file,"pdef control\n\n");
	fprintf(DAPL_source_file,"  pout=pfl\n");
	fprintf(DAPL_source_file,"  copy(pout,op1)\n");
	fprintf(DAPL_source_file,"  copy(ptrigger,op0)\n\n");
	fprintf(DAPL_source_file,"  merge(ip(0..3),$binout)\n");
	fprintf(DAPL_source_file,"end\n\n");

	fprintf(DAPL_source_file,";************************************************************\n\n");

	fprintf(DAPL_source_file,"\nodef outputproc 2\n");
	fprintf(DAPL_source_file,"  outputwait 2\n");
	fprintf(DAPL_source_file,"  update burst\n");
	fprintf(DAPL_source_file,"  set op0 a0\n");
	fprintf(DAPL_source_file,"  set op1 a1\n");
	fprintf(DAPL_source_file,"  time %.0f\n",(0.5*time_increment*1.0e6));
	fprintf(DAPL_source_file,"end\n\n");

	fprintf(DAPL_source_file,";************************************************************\n\n");

	fprintf(DAPL_source_file,"\n idef inputproc 4\n");
	fprintf(DAPL_source_file,"  set ip0 s0\n");
	fprintf(DAPL_source_file,"  set ip1 s1\n");
	fprintf(DAPL_source_file,"  set ip2 s2\n");
	fprintf(DAPL_source_file,"  set ip3 s3\n");
	fprintf(DAPL_source_file,"  count %i\n",(4*data_points));
	fprintf(DAPL_source_file,"  time %.0f\n",(0.25*time_increment*1.0e6));
	fprintf(DAPL_source_file,"end\n\n");

	fprintf(DAPL_source_file,";************************************************************\n\n");

	fprintf(DAPL_source_file,"start create_pipes,control,inputproc,outputproc\n\n");


	sprintf(display_string,"Finished pfl, %.3f",(clock()-start_pipe)/CLOCKS_PER_SEC);
	display_status(TRUE,display_string);
		
	// close file

	fclose(DAPL_source_file);

	return overflow_points;
}

int SL_SREC::write_header_file(void)
{
	// Code creates header file

	// Variables

	char display_string[200];

	FILE *header_file;

	// Code

	// File handling

	header_file=fopen(header_file_string,"w");

	if (header_file==NULL)													// Error
	{
		sprintf(display_string,"Header file\n%s\ncould not be opened\n",header_file_string);
		MessageBox(display_string,"SL_SREC::write_header_file");
		return(0);
	}
	else
	{
		fprintf(header_file,"RESET\n\n");

		fprintf(header_file,"OPTION UNDERFLOWQ=OFF\n");
		fprintf(header_file,"OPTION ERRORQ=OFF\n");
		fprintf(header_file,"OPTION SCHEDULING=FIXED\n");
		fprintf(header_file,"OPTION BUFFERING=OFF\n");
		fprintf(header_file,"OPTION QUANTUM=100\n\n");

		fprintf(header_file,";************************************************************\n\n");

		fprintf(header_file,"// Header\n\n");

		fprintf(header_file,"pipes pfl MAXSIZE=65500\n");
		fprintf(header_file,"pipes ptarget MAXSIZE=65500\n\n");
		
		fprintf(header_file,"pipes pservo MAXSIZE=65500\n\n");

		fprintf(header_file,"pipes poffset_marker MAXSIZE=65500\n");
		fprintf(header_file,"pipes poffset MAXSIZE=65500\n");
		fprintf(header_file,"pipes plastoffset MAXSIZE=65500\n\n");

		fprintf(header_file,"pipes pout MAXSIZE=65500\n");
		fprintf(header_file,"pipes plastout MAXSIZE=65500\n\n");

		fprintf(header_file,"pipes pfeedback MAXSIZE=65500\n\n");
		
		fprintf(header_file,"pipes ptrigger MAXSIZE=65500\n\n");

		fprintf(header_file,";************************************************************\n\n");

		fprintf(header_file,"variables zeroclamp\n\n");

		fprintf(header_file,";************************************************************\n\n");

		fprintf(header_file,"let zeroclamp=%i\n\n",clamp_mode);

		fprintf(header_file,";************************************************************\n\n");
	}

	// Close file

	fclose(header_file);

	return(1);
}

int SL_SREC::write_fill_pipes_file(void)
{
	// Code writes fill pipes

	// Variables

	int i;																		// counter

	float * pipe = new float [data_points+1];									// pipe array
																				// filled in turn with
																				// pservo, poffset_marker,
																				// pfl and ptarget and
																				// transferred to DAPL

	float ramp_multiplier;														// 1 if ramp mode
																				// 2 if triangles

	int ktr_initiation_points;													// integer data point at the
																				// beginning of the ktr step

	int ktr_sl_start, ktr_sl_stop;												// integer values defining the
																				// first and last points under
																				// sl control for ktr recovery

	int tri_sl_start, tri_sl_stop;												// integer values defining the
																				// first and last points under
																				// sl control for triangle
																				// waveforms

	int tri_1_start,tri_2_start,tri_3_start;									// integer values defining the
	int tri_1_stop,tri_2_stop,tri_3_stop;										// first and last points of
																				// each triangle

	int first_step_point;
	int second_step_point;

	char display_string[200];													// user output

	FILE *fill_pipes_file;														// file pointer

	// Code

	// Set ramp_multiplier

	if (ramp_mode==0)
	{
		ramp_multiplier=2.0;
	}
	else
	{
		ramp_multiplier=1.0;
	}

	// Set ktr_initiation_time and points

	if (no_of_triangles==0)
	{
		ktr_initiation_time_ms=pre_ktr_ms;
	}
	else
	{
		ktr_initiation_time_ms=pre_ktr_ms+pre_triangle_ms+
			((float)fabs(relative_first_tri_size)*ramp_multiplier*triangle_halftime_ms)+
			((float)(no_of_triangles-1)*ramp_multiplier*triangle_halftime_ms)+
			((float)(no_of_triangles-1)*inter_triangle_interval_ms);
	}

	ktr_initiation_points=convert_time_ms_to_sampling_points(ktr_initiation_time_ms);

	// Set ktr_sl start and stop values

	ktr_sl_start=convert_time_ms_to_sampling_points(ktr_initiation_time_ms+
		ktr_step_duration_ms+post_ktr_ms_fl);

	ktr_sl_stop=convert_time_ms_to_sampling_points(ktr_initiation_time_ms+
		ktr_step_duration_ms+post_ktr_ms_fl+ktr_slcontrol_duration_ms);

	// Set tri_sl_start and stop values

	tri_sl_start=pre_servo_points;
	tri_sl_stop=convert_time_ms_to_sampling_points(ktr_initiation_time_ms)-1;

	// File handling

	fill_pipes_file=fopen(fill_pipes_file_string,"w");

	if (fill_pipes_file==NULL)													// Error
	{
		sprintf(display_string,"Fill_pipes_file\n%s\ncould not be opened\n",fill_pipes_file_string);
		MessageBox(display_string,"SL_SREC::write_fill_pipes_file");
		return(0);
	}
	else																		// Write file
	{
	
		fprintf(fill_pipes_file,"\nfill ptrigger %i\n\n",convert_volts_to_DAPL_units((float)4.95));
																				// default high trigger

		fprintf(fill_pipes_file,"fill plastout 0\n");							// initial values
		fprintf(fill_pipes_file,"fill plastoffset 0\n\n");

		// Switch to different cases depending on the no_of_triangles

		if (no_of_triangles==0)													// ktr only
		{
			// pservo

			if (post_ktr_servo_mode==FL_CONTROL)								// No pre SL control option
			{
				for (i=1;i<=data_points;i++)
				{
					pipe[i]=0;													// servo always off
				}		
			}
			else																// ktr SL control
			{
				for (i=1;i<=data_points;i++)
				{
					if ((i>ktr_sl_start)&&(i<=ktr_sl_stop))						// servo on
					{
						pipe[i]=1;
					}
					else														// servo off
					{
						pipe[i]=0;
					}
				}
			}

			::write_pipe_to_file(fill_pipes_file,"pservo",pipe,data_points);

			// poffset_marker

			for (i=1;i<=data_points;i++)
			{
				if (i<ktr_initiation_points)
				{
					pipe[i]=1;
				}
				else
				{
					pipe[i]=0;
				}
			}

			::write_pipe_to_file(fill_pipes_file,"poffset_marker",pipe,data_points);

			// pfl

			for (i=1;i<=data_points;i++)
			{
				if ((i>ktr_initiation_points)&&
						(i<=convert_time_ms_to_sampling_points(ktr_initiation_time_ms+ktr_step_duration_ms)))
				{
					pipe[i]=(float)::convert_volts_to_DAPL_units(
								::convert_FL_COMMAND_microns_to_volts(ktr_step_size_microns));
				}
				else
				{
					pipe[i]=0;
				}
			}

// Correct for step mode

if (experiment_mode==SL_SREC_STEP_TRIALS)
{
	first_step_point=convert_time_ms_to_sampling_points(first_step_ms);
	second_step_point=convert_time_ms_to_sampling_points(second_step_ms);

	for (i=1;i<=data_points;i++)
	{
		if ((i>first_step_point)&&(i<data_points))
		{
			pipe[i]=pipe[i]+(float)::convert_volts_to_DAPL_units(
				::convert_FL_COMMAND_microns_to_volts(first_step_microns));
		}
		
		if ((i>second_step_point)&&(i<data_points))
		{
			pipe[i]=pipe[i]+(float)::convert_volts_to_DAPL_units(
				::convert_FL_COMMAND_microns_to_volts(second_step_microns));
		}
	}
}
			::write_pipe_to_file(fill_pipes_file,"pfl",pipe,data_points);

			// ptarget

			for (i=1;i<=data_points;i++)
			{
				pipe[i]=0;
			}

// Correct for step mode

if (experiment_mode==SL_SREC_STEP_TRIALS)
{
	first_step_point=convert_time_ms_to_sampling_points(first_step_ms);
	second_step_point=convert_time_ms_to_sampling_points(second_step_ms);

	for (i=1;i<=data_points;i++)
	{
		if ((i>first_step_point)&&(i<data_points))
		{
			pipe[i]=pipe[i]+
				(float)::convert_volts_to_DAPL_units(first_step_microns*SL_SREC_SL_volts_to_FL_microns_calibration);
		}

		if ((i>second_step_point)&&(i<data_points))
		{
			pipe[i]=pipe[i]+
				(float)::convert_volts_to_DAPL_units(second_step_microns*SL_SREC_SL_volts_to_FL_microns_calibration);
		}
	}
}

			::write_pipe_to_file(fill_pipes_file,"ptarget",pipe,data_points);
		}


		else																// triangles
		{
			// pservo

			if ((pre_ktr_servo_mode==FL_CONTROL)&&(post_ktr_servo_mode==FL_CONTROL))
			{
				for (i=1;i<=data_points;i++)
				{
					pipe[i]=0;
				}
			}

			if ((pre_ktr_servo_mode==FL_CONTROL)&&(post_ktr_servo_mode==SL_CONTROL))
			{
				
				for (i=1;i<=data_points;i++)
				{
					if ((i>ktr_sl_start)&&(i<=ktr_sl_stop))						// servo on
					{
						pipe[i]=1;
					}
					else														// servo off
					{
						pipe[i]=0;
					}
				}
			}

			if ((pre_ktr_servo_mode==SL_CONTROL)&&(post_ktr_servo_mode==FL_CONTROL))
			{
				for (i=1;i<=data_points;i++)
				{
					if ((i>tri_sl_start)&&(i<=tri_sl_stop))
					{
						pipe[i]=1;												// servo on
					}
					else
					{
						pipe[i]=0;												// servo off
					}
				}
			}

			if ((pre_ktr_servo_mode==SL_CONTROL)&&(post_ktr_servo_mode==SL_CONTROL))
			{
				for (i=1;i<=data_points;i++)
				{
					if (((i>tri_sl_start)&&(i<=tri_sl_stop)) ||
						((i>ktr_sl_start)&&(i<=ktr_sl_stop)))
					{
						pipe[i]=1;												// servo on
					}
					else
					{
						pipe[i]=0;												// servo off
					}
				}
			}

			::write_pipe_to_file(fill_pipes_file,"pservo",pipe,data_points);

			// poffset_marker

			if (pre_ktr_servo_mode==FL_CONTROL)								
			{
				// SL control for ktr only - offset just before ktr step

				for (i=1;i<=data_points;i++)
				{
					if (i<ktr_initiation_points)
					{
						pipe[i]=1;
					}
					else
					{
						pipe[i]=0;
					}
				}
			}
			else
			{
				// Otherwise offset to pre_servo_points

				for (i=1;i<=data_points;i++)
				{
					if (i<pre_servo_points)
					{
						pipe[i]=1;
					}
					else
					{
						pipe[i]=0;
					}
				}
			}

			::write_pipe_to_file(fill_pipes_file,"poffset_marker",pipe,data_points);

			// pfl and ptarget

			tri_1_start=convert_time_ms_to_sampling_points(pre_triangle_ms);
			tri_1_stop=tri_1_start+
					convert_time_ms_to_sampling_points(ramp_multiplier*(float)fabs(relative_first_tri_size)
														*triangle_halftime_ms);

			tri_2_start=tri_1_stop+
					convert_time_ms_to_sampling_points(inter_triangle_interval_ms);
			tri_2_stop=tri_2_start+
					convert_time_ms_to_sampling_points(ramp_multiplier*triangle_halftime_ms);

			tri_3_start=tri_2_stop+
					convert_time_ms_to_sampling_points(inter_triangle_interval_ms);
			tri_3_stop=tri_3_start+
					convert_time_ms_to_sampling_points(ramp_multiplier*triangle_halftime_ms);

			switch (no_of_triangles)
			{
				case 1:
				{
					// pFL

					::generate_waveform(CONSTANT,pipe,
						1,tri_1_start,
						1,0);

					::generate_waveform(TRIANGLE,pipe,
						tri_1_start,tri_1_stop,
						(float)convert_time_ms_to_sampling_points((float)4.0*(float)fabs(relative_first_tri_size)*triangle_halftime_ms),
							(float)::convert_volts_to_DAPL_units(
								::convert_FL_COMMAND_microns_to_volts(relative_first_tri_size*triangle_size_microns)));

					::generate_waveform(CONSTANT,pipe,
						tri_1_stop,ktr_initiation_points,
						1,0);

					::generate_waveform(CONSTANT,pipe,
						ktr_initiation_points,
							(ktr_initiation_points+convert_time_ms_to_sampling_points(ktr_step_duration_ms)),1,
							(float)::convert_volts_to_DAPL_units(::convert_FL_COMMAND_microns_to_volts(ktr_step_size_microns)));

					::generate_waveform(CONSTANT,pipe,
						(ktr_initiation_points+convert_time_ms_to_sampling_points(ktr_step_duration_ms)),
							data_points,
						1,0);

					// Correct for ramp_increment

					if (ramp_increment_mode==1)
					{
						for (i=tri_1_stop;i<=ktr_initiation_points;i++)
						{
							pipe[i]=pipe[i]+::convert_volts_to_DAPL_units(
								::convert_FL_COMMAND_microns_to_volts(relative_first_tri_size*triangle_size_microns));
						}
					}

					::write_pipe_to_file(fill_pipes_file,"pfl",pipe,data_points);

					// ptarget

					::generate_waveform(CONSTANT,pipe,
						1,tri_1_start,
						1,0);

					::generate_waveform(TRIANGLE,pipe,
						tri_1_start,tri_1_stop,
						(float)convert_time_ms_to_sampling_points((float)4.0*(float)fabs(relative_first_tri_size)*triangle_halftime_ms),
							(float)::convert_volts_to_DAPL_units(relative_first_tri_size*triangle_size_microns*SL_SREC_SL_volts_to_FL_microns_calibration));

					::generate_waveform(CONSTANT,pipe,
						tri_1_stop,data_points,
						1,0);

					// Correct for ramp_increment

					if (ramp_increment_mode==1)
					{
						for (i=tri_1_stop;i<=ktr_initiation_points;i++)
						{
							pipe[i]=pipe[i]+::convert_volts_to_DAPL_units(
								relative_first_tri_size*triangle_size_microns*SL_SREC_SL_volts_to_FL_microns_calibration);
						}
					}

					::write_pipe_to_file(fill_pipes_file,"ptarget",pipe,data_points);
				}
				break;

				case 2:
				{
					// pfl

					::generate_waveform(CONSTANT,pipe,
						1,tri_1_start,
						1,0);

					::generate_waveform(TRIANGLE,pipe,
						tri_1_start,tri_1_stop,
						(float)convert_time_ms_to_sampling_points((float)4.0*(float)fabs(relative_first_tri_size)*triangle_halftime_ms),
							(float)::convert_volts_to_DAPL_units(::convert_FL_COMMAND_microns_to_volts(relative_first_tri_size*triangle_size_microns)));

					::generate_waveform(CONSTANT,pipe,
						tri_1_stop,tri_2_start,
						1,0);

					::generate_waveform(TRIANGLE,pipe,
						tri_2_start,tri_2_stop,
						(float)convert_time_ms_to_sampling_points(4*triangle_halftime_ms),
							(float)::convert_volts_to_DAPL_units(::convert_FL_COMMAND_microns_to_volts(triangle_size_microns)));

					::generate_waveform(CONSTANT,pipe,
						tri_2_stop,ktr_initiation_points,
						1,0);

					::generate_waveform(CONSTANT,pipe,
						ktr_initiation_points,
							(ktr_initiation_points+convert_time_ms_to_sampling_points(ktr_step_duration_ms)),1,
							(float)::convert_volts_to_DAPL_units(::convert_FL_COMMAND_microns_to_volts(ktr_step_size_microns)));

					::generate_waveform(CONSTANT,pipe,
						(ktr_initiation_points+convert_time_ms_to_sampling_points(ktr_step_duration_ms)),
							data_points,
						1,0);

					// Correct for ramp_increment

					if (ramp_increment_mode==1)
					{
						for (i=tri_1_stop;i<=ktr_initiation_points;i++)
						{
							pipe[i]=pipe[i]+::convert_volts_to_DAPL_units(
								::convert_FL_COMMAND_microns_to_volts(relative_first_tri_size*triangle_size_microns));
						}

						for (i=tri_2_stop;i<=ktr_initiation_points;i++)
						{
							pipe[i]=pipe[i]+::convert_volts_to_DAPL_units(
								::convert_FL_COMMAND_microns_to_volts(triangle_size_microns));
						}
					}
						
					::write_pipe_to_file(fill_pipes_file,"pfl",pipe,data_points);

					// ptarget

					::generate_waveform(CONSTANT,pipe,
						1,tri_1_start,
						1,0);

					::generate_waveform(TRIANGLE,pipe,
						tri_1_start,tri_1_stop,
						(float)convert_time_ms_to_sampling_points((float)4.0*(float)fabs(relative_first_tri_size)*triangle_halftime_ms),
							(float)::convert_volts_to_DAPL_units(relative_first_tri_size*triangle_size_microns*SL_SREC_SL_volts_to_FL_microns_calibration));

					::generate_waveform(CONSTANT,pipe,
						tri_1_stop,tri_2_start,
						1,0);

					::generate_waveform(TRIANGLE,pipe,
						tri_2_start,tri_2_stop,
						(float)convert_time_ms_to_sampling_points((float)4.0*triangle_halftime_ms),
							(float)::convert_volts_to_DAPL_units(triangle_size_microns*SL_SREC_SL_volts_to_FL_microns_calibration));

					::generate_waveform(CONSTANT,pipe,
						tri_2_stop,data_points,
						1,0);

					// Correct for ramp_increment

					if (ramp_increment_mode==1)
					{
						for (i=tri_1_stop;i<=ktr_initiation_points;i++)
						{
							pipe[i]=pipe[i]+::convert_volts_to_DAPL_units(
								relative_first_tri_size*triangle_size_microns*SL_SREC_SL_volts_to_FL_microns_calibration);
						}

						for (i=tri_2_stop;i<=ktr_initiation_points;i++)
						{
							pipe[i]=pipe[i]+::convert_volts_to_DAPL_units(
								(triangle_size_microns*SL_SREC_SL_volts_to_FL_microns_calibration));
						}

					}

					::write_pipe_to_file(fill_pipes_file,"ptarget",pipe,data_points);
				}
				break;

				case 3:
				{
					// pfl

					::generate_waveform(CONSTANT,pipe,
						1,tri_1_start,
						1,0);

					::generate_waveform(TRIANGLE,pipe,
						tri_1_start,tri_1_stop,
						(float)convert_time_ms_to_sampling_points((float)4.0*(float)fabs(relative_first_tri_size)*triangle_halftime_ms),
							(float)::convert_volts_to_DAPL_units(::convert_FL_COMMAND_microns_to_volts(relative_first_tri_size*triangle_size_microns)));

					::generate_waveform(CONSTANT,pipe,
						tri_1_stop,tri_2_start,
						1,0);

					::generate_waveform(TRIANGLE,pipe,
						tri_2_start,tri_2_stop,
						(float)convert_time_ms_to_sampling_points((float)4.0*triangle_halftime_ms),
							(float)::convert_volts_to_DAPL_units(::convert_FL_COMMAND_microns_to_volts(triangle_size_microns)));

					::generate_waveform(CONSTANT,pipe,
						tri_2_stop,tri_3_start,
						1,0);

					::generate_waveform(TRIANGLE,pipe,
						tri_3_start,tri_3_stop,
						(float)convert_time_ms_to_sampling_points((float)4.0*triangle_halftime_ms),
							(float)::convert_volts_to_DAPL_units(::convert_FL_COMMAND_microns_to_volts(triangle_size_microns)));

					::generate_waveform(CONSTANT,pipe,
						tri_3_stop,ktr_initiation_points,
						1,0);

					::generate_waveform(CONSTANT,pipe,
						ktr_initiation_points,
							(ktr_initiation_points+convert_time_ms_to_sampling_points(ktr_step_duration_ms)),1,
							(float)::convert_volts_to_DAPL_units(::convert_FL_COMMAND_microns_to_volts(ktr_step_size_microns)));

					::generate_waveform(CONSTANT,pipe,
						(ktr_initiation_points+convert_time_ms_to_sampling_points(ktr_step_duration_ms)),
							data_points,
						1,0);

					// Correct for ramp_increment

					if (ramp_increment_mode==1)
					{
						for (i=tri_1_stop;i<=ktr_initiation_points;i++)
						{
							pipe[i]=pipe[i]+::convert_volts_to_DAPL_units(
								(float)::convert_FL_COMMAND_microns_to_volts(relative_first_tri_size*triangle_size_microns));
						}

						for (i=tri_2_stop;i<=ktr_initiation_points;i++)
						{
							pipe[i]=pipe[i]+::convert_volts_to_DAPL_units(
								(float)::convert_FL_COMMAND_microns_to_volts(triangle_size_microns));
						}

						for (i=tri_3_stop;i<=ktr_initiation_points;i++)
						{
							pipe[i]=pipe[i]+::convert_volts_to_DAPL_units(
								(float)::convert_FL_COMMAND_microns_to_volts(triangle_size_microns));
						}
					}
						
					::write_pipe_to_file(fill_pipes_file,"pfl",pipe,data_points);

					// ptarget

					::generate_waveform(CONSTANT,pipe,
						1,tri_1_start,
						1,0);

					::generate_waveform(TRIANGLE,pipe,
						tri_1_start,tri_1_stop,
						(float)convert_time_ms_to_sampling_points((float)4.0*(float)fabs(relative_first_tri_size)*triangle_halftime_ms),
							(float)::convert_volts_to_DAPL_units(relative_first_tri_size*triangle_size_microns*SL_SREC_SL_volts_to_FL_microns_calibration));

					::generate_waveform(CONSTANT,pipe,
						tri_1_stop,tri_2_start,
						1,0);

					::generate_waveform(TRIANGLE,pipe,
						tri_2_start,tri_2_stop,
						(float)convert_time_ms_to_sampling_points((float)4.0*triangle_halftime_ms),
							(float)::convert_volts_to_DAPL_units(triangle_size_microns*SL_SREC_SL_volts_to_FL_microns_calibration));

					::generate_waveform(CONSTANT,pipe,
						tri_2_stop,tri_3_start,
						1,0);

					::generate_waveform(TRIANGLE,pipe,
						tri_3_start,tri_3_stop,
						(float)convert_time_ms_to_sampling_points((float)4.0*triangle_halftime_ms),
							(float)(::convert_volts_to_DAPL_units(triangle_size_microns*SL_SREC_SL_volts_to_FL_microns_calibration)));

					::generate_waveform(CONSTANT,pipe,
						tri_3_stop,data_points,
						1,0);

					// Correct for ramp_increment

					if (ramp_increment_mode==1)
					{
						for (i=tri_1_stop;i<=ktr_initiation_points;i++)
						{
							pipe[i]=pipe[i]+::convert_volts_to_DAPL_units(
								relative_first_tri_size*triangle_size_microns*SL_SREC_SL_volts_to_FL_microns_calibration);
						}

						for (i=tri_2_stop;i<=ktr_initiation_points;i++)
						{
							pipe[i]=pipe[i]+::convert_volts_to_DAPL_units(
								(triangle_size_microns*SL_SREC_SL_volts_to_FL_microns_calibration));
						}

						for (i=tri_3_stop;i<=ktr_initiation_points;i++)
						{
							pipe[i]=pipe[i]+::convert_volts_to_DAPL_units(
								(triangle_size_microns*SL_SREC_SL_volts_to_FL_microns_calibration));
						}
					}

					::write_pipe_to_file(fill_pipes_file,"ptarget",pipe,data_points);
				}
				break;
			}
		}

		fprintf(fill_pipes_file,";************************************************************\n\n");
	}


	// Tidy up

	delete [] pipe;

	fclose(fill_pipes_file);

	return(1);
}

int SL_SREC::write_create_pipes_file(void)
{
	// Code creates header file

	// Variables

	char display_string[200];

	FILE *create_pipes_file;

	// Code

	// File handling

	create_pipes_file=fopen(create_pipes_file_string,"w");

	if (create_pipes_file==NULL)													// Error
	{
		sprintf(display_string,"Create_pipes_file\n%s\ncould not be opened\n",create_pipes_file_string);
		MessageBox(display_string,"SL_SREC::write_create_pipes_file");
		return(0);
	}
	else
	{
		fprintf(create_pipes_file,"\npdef create_pipes\n\n");

		fprintf(create_pipes_file,"  ptrigger=0\n\n");								// set trigger low
		
		fprintf(create_pipes_file,"end\n\n");

		fprintf(create_pipes_file,";************************************************************\n\n");
	}

	fclose(create_pipes_file);

	return(1);
}

int SL_SREC::write_control_file(void)
{
	// Code creates control_file

	// Variables

	char display_string[200];

	FILE *control_file;

	// Code

	// File handling

	control_file=fopen(control_file_string,"w");

	if (control_file==NULL)												// Error
	{
		sprintf(display_string,"Control file\n%s\ncould not be opened\n",control_file_string);
		MessageBox(display_string,"SL_SREC::write_control_file");
		return(0);
	}
	else
	{
		fprintf(control_file,"\npdef control\n");

		if (sampling_rate<=2500)
		{
			fprintf(control_file,"  poffset=((1-poffset_marker)*plastoffset)+(poffset_marker*ip1)\n");
			fprintf(control_file,"  plastoffset=poffset\n\n");

			fprintf(control_file,"  pfeedback=plastout-(%.0f*(ip1-ptarget-((1-zeroclamp)*poffset))/10000)\n",
				1.0e4*proportional_gain/SL_volts_to_FL_command_volts);
			fprintf(control_file,"  pout=((1-pservo)*pfl)+(pservo*pfeedback)\n\n");
		}
		else
		{
			sprintf(display_string,"Sampling rate: %f: Open loop control only");
			display_status(TRUE,display_string);

			fprintf(control_file,"  pout=pfl\n\n");
		}

		fprintf(control_file,"  copy(pout,plastout,op1)\n");
		fprintf(control_file,"  copy(ptrigger,op0)\n\n");

		fprintf(control_file,"  merge(ip0,ip1,ip2,ip3,$binout)\n");
		fprintf(control_file,"end\n\n");

		fprintf(control_file,";************************************************************\n\n");
	}
	
	fclose(control_file);

	return(1);
}

int SL_SREC::write_io_procedures_file(void)
{
	// Code creates io_procedures_file

	// Variables

	char display_string[200];

	FILE *io_procedures_file;

	// Code

	// File handling

	io_procedures_file=fopen(io_procedures_file_string,"w");

	if (io_procedures_file==NULL)													// Error
	{
		sprintf(display_string,"IO_Procedures file\n%s\ncould not be opened\n",io_procedures_file_string);
		MessageBox(display_string,"SL_SREC::write_io_procedures_file");
		return(0);
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

		fprintf(io_procedures_file,"start create_pipes,control,outputproc,inputproc\n");
	}

	fclose(io_procedures_file);

	return(1);
}

void SL_SREC::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here

	// Redraws status messages

	display_status(FALSE,"");

	// Do not call CDialog::OnPaint() for painting messages
}

void SL_SREC::OnSelchangeExptMode() 
{
	// Variables

	// Code

	// Sets experiment mode

	experiment_mode=experiment_mode_control.GetCurSel();
	
	// Enables/disables controls

	switch (experiment_mode)
	{
		case SL_SREC_SINGLE_TRIAL:
		{
			GetDlgItem(IDC_BATCH_FILE)->EnableWindow(FALSE);
			GetDlgItem(IDC_INTER_TRIAL_INTERVAL)->EnableWindow(FALSE);
			batch_file_string=single_trial_string;

			if (no_of_triangles>1)
				GetDlgItem(IDC_INTER_TRIANGLE_INTERVAL)->EnableWindow(TRUE);
			else
				GetDlgItem(IDC_INTER_TRIANGLE_INTERVAL)->EnableWindow(FALSE);
		}
		break;

		case SL_SREC_DELAY_TRIALS:
		{
			GetDlgItem(IDC_BATCH_FILE)->EnableWindow(TRUE);
			GetDlgItem(IDC_INTER_TRIAL_INTERVAL)->EnableWindow(TRUE);
			batch_file_string=delay_trials_string;

			GetDlgItem(IDC_INTER_TRIANGLE_INTERVAL)->EnableWindow(FALSE);
		}
		break;

		case SL_SREC_SIZE_TRIALS:
		{
			GetDlgItem(IDC_BATCH_FILE)->EnableWindow(TRUE);
			GetDlgItem(IDC_INTER_TRIAL_INTERVAL)->EnableWindow(TRUE);
			batch_file_string=size_trials_string;

			if (no_of_triangles>1)
				GetDlgItem(IDC_INTER_TRIANGLE_INTERVAL)->EnableWindow(TRUE);
			else
				GetDlgItem(IDC_INTER_TRIANGLE_INTERVAL)->EnableWindow(FALSE);
		}
		break;

		case SL_SREC_VELOCITY_TRIALS:
		{
			GetDlgItem(IDC_BATCH_FILE)->EnableWindow(TRUE);
			GetDlgItem(IDC_INTER_TRIAL_INTERVAL)->EnableWindow(TRUE);
			batch_file_string=velocity_trials_string;

			if (no_of_triangles>1)
				GetDlgItem(IDC_INTER_TRIANGLE_INTERVAL)->EnableWindow(TRUE);
			else
				GetDlgItem(IDC_INTER_TRIANGLE_INTERVAL)->EnableWindow(FALSE);
		}
		break;

		case SL_SREC_ALL_TRIALS:
		{
			GetDlgItem(IDC_BATCH_FILE)->EnableWindow(TRUE);
			GetDlgItem(IDC_INTER_TRIAL_INTERVAL)->EnableWindow(TRUE);
			batch_file_string=all_trials_string;

			GetDlgItem(IDC_INTER_TRIANGLE_INTERVAL)->EnableWindow(FALSE);
		}
		break;

		case SL_SREC_STEP_TRIALS:
		{
			GetDlgItem(IDC_BATCH_FILE)->EnableWindow(TRUE);
			GetDlgItem(IDC_INTER_TRIAL_INTERVAL)->EnableWindow(TRUE);
			batch_file_string=step_trials_string;

			GetDlgItem(IDC_INTER_TRIANGLE_INTERVAL)->EnableWindow(FALSE);
		}
		break;
	}

	GetDlgItem(IDC_BATCH_FILE)->SetWindowText(batch_file_string);
}

void SL_SREC::OnRampControl() 
{
	// Sets ramp_mode and enables/disables ramp_increment control

	UpdateData(TRUE);

	if (ramp_option==FALSE)												// Triangles
	{
		ramp_mode=0;

		// Increment option is nonsensical with triangles

		ramp_increment_mode=0;
		ramp_increment_option=FALSE;
		GetDlgItem(IDC_RAMP_INCREMENT_CONTROL)->EnableWindow(FALSE);
	}
	else																// Ramps
	{
		ramp_mode=1;

		if (no_of_triangles>=1)
		{
			GetDlgItem(IDC_RAMP_INCREMENT_CONTROL)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_RAMP_INCREMENT_CONTROL)->EnableWindow(FALSE);
		}
	}

	// Update Screen

	UpdateData(FALSE);

	// Recalculates post_ktr values

	set_post_ktr_values();
}

void SL_SREC::OnRampIncrementControl() 
{
	// Sets ramp_increment_mode

	UpdateData(TRUE);

	if (ramp_increment_option==FALSE)
	{
		ramp_increment_mode=0;
	}
	else
	{
		ramp_increment_mode=1;
	}
}
void SL_SREC::set_parameters_valid(void)
{
	// Code sets parameters_valid if experiment seems plausible

	// Variables

	int length_servo_mode;

	// Code

	// Update previous value

	previous_parameters_valid=parameters_valid;

	// Set length_servo_mode to 1 if either pre_ktr_servo_mode or post_ktr_servo_mode==1

	length_servo_mode=(pre_ktr_servo_mode||post_ktr_servo_mode);
	
	// Valid records need post_ktr_ms>0 and SL_calibrated if the experiment uses
	// sarcomere length control

	if (post_ktr_ms>0)
	{
		// Record length is valid

		if (length_servo_mode==SL_CONTROL)
		{
			// Calibration required

			if (SL_calibration==SL_CALIBRATED)
			{
				// Valid record

				parameters_valid=1;
			}
			else
			{
				// Invalid - correct record length but not calibrated 

				parameters_valid=-1;
			}
		}
		else
		{
			// Calibration not required so a valid record

			parameters_valid=1;
		}
	}
	else
	{
		// Record length is invalid

		if (length_servo_mode==SL_CONTROL)
		{
			// Calibration required

			if (SL_calibration==SL_CALIBRATED)
			{
				// Invalid - short record length but calibrated appropriately

				parameters_valid=-2;
			}
			else
			{
				// Invalid = short record length and calibration required

				parameters_valid=-3;
			}
		}
		else
		{
			// Invalid - short record length but calibration not required

			parameters_valid=-2;
		}
	}

	// Display appropriate message

	if (parameters_valid==1)
	{
		if (previous_parameters_valid<0)
		{
			MessageBeep(0xFFFFFFFF); 
			display_status(TRUE,"Parameters now valid");
		}
	}
	else
	{
		if ((parameters_valid==-1)&&(previous_parameters_valid!=-1))
		{
			MessageBeep(MB_ICONEXCLAMATION);
			display_status(TRUE,"Record length valid but SL calibration required");
		}

		if ((parameters_valid==-2)&&(previous_parameters_valid!=-2))
		{
			MessageBeep(MB_ICONEXCLAMATION);
			display_status(TRUE,"Record length too short but SL calibrated or not required");
		}

		if ((parameters_valid==-3)&&(previous_parameters_valid!=-3))
		{
			MessageBeep(MB_ICONEXCLAMATION);
			display_status(TRUE,"Record length too short and SL calibration required");
		}
	}
}

void SL_SREC::OnAbort()
{
	// TODO: Add your control notification handler code here

	
	CRuntimeClass* p_run_time = RUNTIME_CLASS(Abort_multiple_trials);

	m_pThread = AfxBeginThread(p_run_time,THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
	m_pThread->m_bAutoDelete = FALSE;
	m_pThread->ResumeThread();


char display_string[200];
sprintf(display_string,"SL_SREC: %i",this);
MessageBox(display_string);

Abort_multiple_trials* p_amt;
p_amt = (Abort_multiple_trials*) m_pThread;

//p_amt->p_SL_SREC = this;

p_run_time->m_pBaseClass;
MessageBox(display_string);
/*
	Abort_multiple_trials_dialog* p_amtd;

	p_amtd = new Abort_multiple_trials_dialog();
	p_amtd->Create();

	MessageBox("Window created");

	p_amtd->close_calibration_window();
*/

	DWORD dwExitCode;
	::GetExitCodeThread(m_pThread->m_hThread,&dwExitCode);
	if (dwExitCode == STILL_ACTIVE)
	{
		// Thread is still running
		MessageBox("Thread is still running");
	}
	else
	{
		

		delete m_pThread;
	}
}


