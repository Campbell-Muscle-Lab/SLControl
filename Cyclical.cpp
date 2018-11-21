// Cyclical.cpp : implementation file
//

#include "stdafx.h"
#include "slcontrol.h"
#include "Cyclical.h"
#include "SLControlDlg.h"
#include "Calibration.h"
#include "dapio32.h"
#include "global_definitions.h"
#include "global_functions.h"
#include <math.h>

extern float FL_POLARITY;
extern float FL_RESPONSE_CALIBRATION;
extern float FL_COMMAND_CALIBRATION;
extern float FORCE_CALIBRATION;
extern char CALIBRATION_DATE_STRING[100];
extern int FL_OR_SL_CHANNEL_MODE;
extern char SLCONTROL_VERSION_STRING[100];

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Cyclical dialog


Cyclical::Cyclical(CWnd* pParent /*=NULL*/)
	: CDialog(Cyclical::IDD, pParent)
{
	// Constructor

	m_pParent=pParent;
	m_nID=Cyclical::IDD;

	// Create pointer to the data record

	p_Cyclical_record =& Cyclical_record;

	// Null daughter window pointers

	p_Calibration=NULL;
	p_Experiment_display=NULL;

	//{{AFX_DATA_INIT(Cyclical)
	data_path_string = _T("");
	data_base_string = _T("");
	data_index = 0;
	sampling_rate_string = _T("");
	data_points = 0;
	duration_seconds = 0.0f;
	SL_volts_to_FL_command_volts = 0.0f;
	SL_volts_to_FL_microns_calibration = 0.0f;
	ktr_step_size_microns = 0.0f;
	ktr_slcontrol_duration_ms = 0.0f;
	ktr_step_duration_ms = 0.0f;
	ktr_slcontrol = FALSE;
	ktr_fl_settle_ms = 0.0f;
	pre_ktr_ms = 0.0f;
	pre_length_ms = 0.0f;
	length_size_microns = 0.0f;
	period_ms = 0.0f;
	phase_degrees = 0.0f;
	length_function_string = _T("");
	proportional_gain = 0.0f;
	length_servo_mode_string = _T("");
	post_ktr_ms = 0.0f;
	length_duration_ms = 0.0f;
	clamp_mode_string = _T("");
	pre_servo_points = 0;
	post_ktr_final_fl_ms = 0.0f;
	length_offset_microns = 0.0f;
	//}}AFX_DATA_INIT

	// Variables

	char* p_string;													// p_string and temp_string
	char temp_string[100];											// used to set and read control
																	// variables

	// File strings

	// Set Cyclical_dir_path

	set_executable_directory_string(temp_string);
	strcat(temp_string,"\\Cyclical\\");

	Cyclical_dir_path=temp_string;

	header_file_string=Cyclical_dir_path+"Cyclical_header.txt";
	fill_pipes_file_string=Cyclical_dir_path+"Cyclical_fill_pipes.txt";
	create_pipes_file_string=Cyclical_dir_path+"Cyclical_creates_pipe.txt";
	control_file_string=Cyclical_dir_path+"Cyclical_control.txt";
	io_procedures_file_string=Cyclical_dir_path+"Cyclical_io_procs.txt";
	DAPL_source_file_string=Cyclical_dir_path+"dapl_source.txt";

	// Default values

	// Sampling rate etc

	sampling_rate_string="1000";
	data_points=4000;

	time_increment=(float)(1.0/strtod(sampling_rate_string,&p_string));
	duration_seconds=((float)data_points*time_increment);

	// SL control parameters

	clamp_mode_string="Prevailing Position";
	clamp_mode=PREVAILING_POSITION;
	proportional_gain=(float)0.001;
	
	// ktr parameters

	ktr_step_size_microns=-20.0;
	ktr_step_duration_ms=20.0;
	ktr_slcontrol_duration_ms=200.0;
	ktr_slcontrol=FL_CONTROL;
	post_ktr_servo_mode=FL_CONTROL;
	ktr_fl_settle_ms=5.0;
	pre_ktr_ms=100.0;

	// Length change parameters

	pre_length_ms=100.0;
	length_duration_ms=2000.0;
	length_size_microns=10.0;
	period_ms=500.0;
	phase_degrees=0.0;
	length_offset_microns=0.0;

	length_function_string="Sine";
	length_function_mode=SINE_WAVE;

	length_servo_mode_string="Fibre Length Control";
	length_servo_mode=FL_CONTROL;
	pre_servo_points=100;

	// Calibration mode

	SL_calibration=SL_NOT_CALIBRATED;
	
	// Set post_ktr_ms and post_ktr_final_fl_ms

	post_ktr_ms=(float)((1000.0*duration_seconds)-
			(pre_length_ms+length_duration_ms+pre_ktr_ms+ktr_step_duration_ms));

	post_ktr_final_fl_ms=post_ktr_ms;

	// Status strings

	status_middle_string="1) Cyclical dialog initialising";
	status_bottom_string="2) Preparing for trials";
	status_counter=3;

	// Filing parameters

	data_path_string="d:\\temp";
	data_base_string="test";
	data_index=1;

	// Parameters check

	parameters_valid=TRUE;
	previous_parameters_valid=TRUE;
}

Cyclical::~Cyclical()
{
	// Destructor
}

void Cyclical::PostNcDestroy()
{
	delete this;
}

void Cyclical::OnClose() 
{
	// Close derived windows

	p_Calibration->close_calibration_window();
	p_Calibration=NULL;

	p_Experiment_display->close_experiment_display_window();
	p_Experiment_display=NULL;

	// Signal to parent that this window is closing

	((CSLControlDlg*)m_pParent)->Cyclical_Window_Done();

	DestroyWindow();
}

BOOL Cyclical::Create()
{
	return CDialog::Create(m_nID,m_pParent);
}

BOOL Cyclical::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Variables

	int width=1022;														// screen width and height
	int height=740;														// in pixels

	// Code

	// Set window size and position

	SetWindowPos(NULL,(int)(0.02*width),(int)(0.07*height),
		(int)(0.63*width),(int)(0.43*height),SWP_SHOWWINDOW);

	// Create the Calibration window

	p_Calibration = new Calibration();
	p_Calibration->Create();

	// Create the Experiment_display window

	p_Experiment_display = new Experiment_display(this,p_Cyclical_record);
	p_Experiment_display->Create();

	// Status

	display_status(TRUE,"SL Calibration not yet completed");

	// Update post_ktr values

	set_post_ktr_ms();

	// Enable/disable appropriate controls

	GetDlgItem(IDC_DURATION)->EnableWindow(FALSE);
	GetDlgItem(IDC_POST_KTR)->EnableWindow(FALSE);
	GetDlgItem(IDC_POST_KTR_FINAL_FL)->EnableWindow(FALSE);

	GetDlgItem(IDC_SL_volts_to_FL_volts)->EnableWindow(FALSE);
	GetDlgItem(IDC_SL_to_FL_microns_calibration)->EnableWindow(FALSE);

	GetDlgItem(IDC_KTR_SLCONTROL_DURATION)->EnableWindow(FALSE);
	GetDlgItem(IDC_KTR_FL_SETTLE)->EnableWindow(FALSE);

	GetDlgItem(IDC_PROPORTIONAL_GAIN)->EnableWindow(FALSE);

	GetDlgItem(IDC_CLAMP_MODE)->EnableWindow(FALSE);
	GetDlgItem(IDC_PRE_SERVO_POINTS)->EnableWindow(FALSE);
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void Cyclical::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Cyclical)
	DDX_Control(pDX, IDC_CLAMP_MODE, CLAMP_MODE_CONTROL);
	DDX_Control(pDX, IDC_SERVO_MODE, LENGTH_SERVO_MODE_CONTROL);
	DDX_Control(pDX, IDC_LENGTH_FUNCTION, LENGTH_FUNCTION_CONTROL);
	DDX_Control(pDX, IDC_SAMPLING_RATE, SAMPLING_RATE_CONTROL);
	DDX_Text(pDX, IDC_DATA_PATH, data_path_string);
	DDX_Text(pDX, IDC_DATA_BASE, data_base_string);
	DDX_Text(pDX, IDC_DATA_INDEX, data_index);
	DDX_CBString(pDX, IDC_SAMPLING_RATE, sampling_rate_string);
	DDX_Text(pDX, IDC_DATA_POINTS, data_points);
	DDV_MinMaxInt(pDX, data_points, 1, 65000);
	DDX_Text(pDX, IDC_DURATION, duration_seconds);
	DDX_Text(pDX, IDC_SL_volts_to_FL_volts, SL_volts_to_FL_command_volts);
	DDX_Text(pDX, IDC_SL_to_FL_microns_calibration, SL_volts_to_FL_microns_calibration);
	DDX_Text(pDX, IDC_KTR_STEP_SIZE, ktr_step_size_microns);
	DDX_Text(pDX, IDC_KTR_SLCONTROL_DURATION, ktr_slcontrol_duration_ms);
	DDX_Text(pDX, IDC_KTR_STEP_DURATION, ktr_step_duration_ms);
	DDX_Check(pDX, IDC_KTR_SLCONTROL, ktr_slcontrol);
	DDX_Text(pDX, IDC_KTR_FL_SETTLE, ktr_fl_settle_ms);
	DDX_Text(pDX, IDC_PRE_KTR, pre_ktr_ms);
	DDX_Text(pDX, IDC_PRE_LENGTH, pre_length_ms);
	DDV_MinMaxFloat(pDX, pre_length_ms, 0.f, 50000.f);
	DDX_Text(pDX, IDC_LENGTH_SIZE, length_size_microns);
	DDX_Text(pDX, IDC_PERIOD, period_ms);
	DDX_Text(pDX, IDC_PHASE, phase_degrees);
	DDX_CBString(pDX, IDC_LENGTH_FUNCTION, length_function_string);
	DDX_Text(pDX, IDC_PROPORTIONAL_GAIN, proportional_gain);
	DDX_CBString(pDX, IDC_SERVO_MODE, length_servo_mode_string);
	DDX_Text(pDX, IDC_POST_KTR, post_ktr_ms);
	DDX_Text(pDX, IDC_LENGTH_DURATION, length_duration_ms);
	DDX_CBString(pDX, IDC_CLAMP_MODE, clamp_mode_string);
	DDX_Text(pDX, IDC_PRE_SERVO_POINTS, pre_servo_points);
	DDX_Text(pDX, IDC_POST_KTR_FINAL_FL, post_ktr_final_fl_ms);
	DDX_Text(pDX, IDC_OFFSET, length_offset_microns);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Cyclical, CDialog)
	//{{AFX_MSG_MAP(Cyclical)
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_SAMPLING_RATE, OnSelchangeSamplingRate)
	ON_EN_CHANGE(IDC_DATA_POINTS, OnChangeDataPoints)
	ON_BN_CLICKED(IDC_SL_to_FL_Calibration, OnSLtoFLCalibration)
	ON_BN_CLICKED(IDC_KTR_SLCONTROL, OnKtrSlcontrol)
	ON_CBN_SELCHANGE(IDC_SERVO_MODE, OnSelchangeServoMode)
	ON_EN_CHANGE(IDC_LENGTH_DURATION, OnChangeLengthDuration)
	ON_BN_CLICKED(IDC_Run_Experiment, OnRunExperiment)
	ON_EN_CHANGE(IDC_KTR_FL_SETTLE, OnChangeKtrFlSettle)
	ON_EN_CHANGE(IDC_KTR_SLCONTROL_DURATION, OnChangeKtrSlcontrolDuration)
	ON_EN_CHANGE(IDC_KTR_STEP_DURATION, OnChangeKtrStepDuration)
	ON_EN_CHANGE(IDC_PRE_KTR, OnChangePreKtr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Cyclical message handlers

void Cyclical::OnRunExperiment() 
{
	// Runs experiment

	// Variables

	int i;															// counter

	char display_string[100];										// user display

	char cursor_path_string[300];									// string holding path for animated cursor

	clock_t start_upload;											// time at start of file upload

	short short_int=1;												// used for sizeof operator
	int holder;														// another counter
	int buffer_size;												// number of points in data buffer with
																	// 4 channels read from DAPL

	HDAP hdapSysGet,hdapSysPut,hdapBinGet;							// DAPL communication pipes
	
	// Code

	// Update parameters

	UpdateData(TRUE);

	buffer_size=(4*data_points);

	// Allocate data buffer

	short int* DAPL_data = new short int [buffer_size];

	// Check experiment is valid - if not, warn user, else run experiment

	if (parameters_valid<0)
	{
		MessageBox("Trial aborted - parameters not valid","Cyclical::OnRunExperiment");
		return;
	}

	// Sets clamp_mode

	if (clamp_mode_string=="Prevailing Position")
	{
		clamp_mode=PREVAILING_POSITION;
	}
	else
	{
		clamp_mode=ZERO_VOLTS;
	}

	// Initiate experiment

	// Open and flush DAPL communication pipes

	hdapSysGet=DapHandleOpen("\\\\.\\Dap0\\$SysOut",DAPOPEN_READ);
	hdapSysPut=DapHandleOpen("\\\\.\\Dap0\\$SysIn",DAPOPEN_WRITE);
	hdapBinGet=DapHandleOpen("\\\\.\\Dap0\\$BinOut",DAPOPEN_READ);

	DapInputFlush(hdapSysGet);
	DapInputFlush(hdapBinGet);
	DapInputFlush(hdapSysPut);

	// Switch to animated animated cursor

	::set_executable_directory_string(cursor_path_string);
	strcat(cursor_path_string,"\\sl_control.ani");

	HCURSOR hcurWait = (HCURSOR)LoadImage(AfxGetApp()->m_hInstance,
		cursor_path_string,IMAGE_CURSOR,0,0,
		LR_DEFAULTSIZE|LR_DEFAULTCOLOR|LR_LOADFROMFILE);
	const HCURSOR hcurSave=SetCursor(hcurWait);

	SetCursor(hcurWait);

	// Start timer and display status

	start_upload=clock();
	sprintf(display_string,"Creating and passing command file to DAP");
	display_status(TRUE,display_string);

	// Create DAPL_source_file

	if (create_DAPL_source_file()==0)
	{
		sprintf(display_string,"DAPL_source_file could not be created - trial aborted");
		display_status(TRUE,display_string);
		MessageBox(display_string,"Cyclical::OnRunExperiment");
		return;
	}

	// Pass to DAP and update status

	if (DapConfig(hdapSysPut,DAPL_source_file_string)==FALSE)
	{
		sprintf(display_string,"DAP could not handle source file\n%s\nResulting records are invalid",
			DAPL_source_file_string);
		MessageBox(display_string,"Cyclical::OnRunExperiment",MB_ICONWARNING);
	}

	sprintf(display_string,"Trial started, DAPL upload took %.4f s",
		(float)(clock()-start_upload)/CLOCKS_PER_SEC);
	display_status(TRUE,display_string);

	// Read data back and update status

	i=DapBufferGet(hdapBinGet,buffer_size*sizeof(short_int),DAPL_data);

	sprintf(display_string,"%i Points/Channel read from DAP",(i/(4*sizeof(short_int))));
	display_status(TRUE,display_string);

	// Copy data to record and update no_of_points

	holder=1;

	for (i=0;i<=buffer_size;i=i+4)
	{
		p_Cyclical_record->data[TIME][holder]=((float)(holder-1)*time_increment);
		p_Cyclical_record->data[FORCE][holder]=convert_DAPL_units_to_volts(DAPL_data[i]);
		p_Cyclical_record->data[SL][holder]=convert_DAPL_units_to_volts(DAPL_data[i+1]);
		p_Cyclical_record->data[FL][holder]=convert_DAPL_units_to_volts(DAPL_data[i+2]);
		p_Cyclical_record->data[INTENSITY][holder]=convert_DAPL_units_to_volts(DAPL_data[i+3]);

		holder++;
	}

	p_Cyclical_record->no_of_points=data_points;

	// Close communication pipes

	DapHandleClose(hdapSysGet);
	DapHandleClose(hdapSysPut);
	DapHandleClose(hdapBinGet);

	// Write data to file

	write_data_to_file();

	// Update plots

	p_Experiment_display->clear_window();

	for (i=0;i<=(p_Experiment_display->no_of_plots-1);i++)
	{
		p_Experiment_display->pointers_array[i]->first_drawn_point=1;
		
		p_Experiment_display->pointers_array[i]->last_drawn_point=
			p_Cyclical_record->no_of_points;
	}

	p_Experiment_display->update_plots();

	// Free memory

	delete [] DAPL_data;

	// End cursor wait

	SetCursor(hcurSave);

	// Tidy up

	sprintf(display_string,"Trial completed");
	display_status(TRUE,display_string);
	MessageBeep(MB_ICONEXCLAMATION);

	UpdateData(FALSE);
	
	// Experiment successful

	return;
}

void Cyclical::write_data_to_file(void)
{
	// Code writes data to file

	// Variables

	int i;																// counter

	char index_string[20];												// index string
	char display_string[100];											// user display

	// Code

	// Create file string

	sprintf(index_string,"%i",data_index);

	output_data_file_string=data_path_string+"\\"+data_base_string+index_string+".slc";

	// File handling

	FILE* output_data_file=fopen(output_data_file_string,"w");

	if (output_data_file==NULL)
	{
		sprintf(display_string,"Cannot open file\n%s\nData file could not be saved to disk",
			output_data_file_string);
		MessageBox(display_string,"Cylical::write_data_to_file",MB_ICONWARNING);
	}
	else
	{
		// Output

		fprintf(output_data_file,"SLCONTROL_DATA_FILE\n\n");

		// Version number

		fprintf(output_data_file,"_VERSION_\n");
		fprintf(output_data_file,"%s\n\n",SLCONTROL_VERSION_STRING);

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

		p_Calibration->UpdateData(TRUE);

		fprintf(output_data_file,"_MUSCLE_DIMENSIONS_\n");
		fprintf(output_data_file,"Muscle_length_(µm): %.3e\n",p_Calibration->Calibration_fibre_length);
		fprintf(output_data_file,"Sarcomere_length_(nm): %.3e\n",p_Calibration->Calibration_sarcomere_length);
		fprintf(output_data_file,"Area_(m^2): %.3e\n\n",p_Calibration->Calibration_area);

		// Experiment parameters

		fprintf(output_data_file,"_CYCLICAL_\n");
		fprintf(output_data_file,"sampling_rate_(Hz): %.3e\n",sampling_rate);
		fprintf(output_data_file,"data_points: %i\n",data_points);
		fprintf(output_data_file,"ktr_step_(µm): %.3e\n",ktr_step_size_microns);
		fprintf(output_data_file,"ktr_duration_(ms): %.3e\n",ktr_step_duration_ms);
		fprintf(output_data_file,"pre_ktr_(ms): %.3e\n",pre_ktr_ms);
		fprintf(output_data_file,"ktr_initiation_time_(ms): %.3e\n\n",ktr_initiation_time_ms);

		fprintf(output_data_file,"length_function: %s\n",length_function_string);
		fprintf(output_data_file,"pre_length_(ms): %.3e\n",pre_length_ms);
		fprintf(output_data_file,"length_duration_(ms): %.3e\n",length_duration_ms);
		fprintf(output_data_file,"length_size_(µm): %.3e\n",length_size_microns);
		fprintf(output_data_file,"length_period_(ms): %.3e\n",period_ms);
		fprintf(output_data_file,"length_phase_(deg): %.3e\n",phase_degrees);
		fprintf(output_data_file,"length_offset_(µm): %.3e\n\n",length_offset_microns);

		fprintf(output_data_file,"post_ktr_servo_mode: %i\n",post_ktr_servo_mode);
		fprintf(output_data_file,"ktr_sl_control_duration_(ms): %.3e\n",ktr_slcontrol_duration_ms);
		fprintf(output_data_file,"ktr_fl_settle_(ms): %.3e\n",ktr_fl_settle_ms);
		fprintf(output_data_file,"length_servo_mode: %i\n",length_servo_mode);
		fprintf(output_data_file,"pre_servo_points: %i\n",pre_servo_points);
		fprintf(output_data_file,"clamp_mode: %i\n",clamp_mode);
		fprintf(output_data_file,"proportional_gain: %.3e\n\n",proportional_gain);

		// Data

		fprintf(output_data_file,"Data\n");

		for (i=1;i<=p_Cyclical_record->no_of_points;i++)
		{
			fprintf(output_data_file,"%f\t%f\t%f\t%f\t%f\n",
				p_Cyclical_record->data[TIME][i],
				p_Cyclical_record->data[FORCE][i],
				p_Cyclical_record->data[SL][i],
				p_Cyclical_record->data[FL][i],
				p_Cyclical_record->data[INTENSITY][i]);
		}

		// Close file

		fclose(output_data_file);

		// Update counter and screen

		data_index++;
		UpdateData(FALSE);

		// Update status

		sprintf(display_string,"Record written to %s",output_data_file_string);
		display_status(TRUE,display_string);
	}
}

void Cyclical::OnCancel() 
{
	// Ensures correct shut-down when dialog is dismissed with the Escape key

	OnClose();
}

void Cyclical::remote_shut_down(void)
{
	// Called by parent to shut down window

	OnClose();
}

void Cyclical::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// Redraw status message

	display_status(FALSE,"");
}

void Cyclical::display_status(int update_mode,CString new_string)
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

void Cyclical::OnSLtoFLCalibration() 
{
	// Variables

	char display_string[100];

	// Code

	// Retrieve data from Calibration window and update Cyclical SL calibration values
	
	p_Calibration->UpdateData(TRUE);

	SL_volts_to_FL_microns_calibration=
		p_Calibration->Calibration_SL_volts_to_FL_microns_calibration;

	SL_volts_to_FL_command_volts=
		(FL_POLARITY*SL_volts_to_FL_microns_calibration*FL_COMMAND_CALIBRATION);

	// Update screen

	sprintf(display_string,"%.4g",SL_volts_to_FL_microns_calibration);
	GetDlgItem(IDC_SL_to_FL_microns_calibration)->SetWindowText(display_string);	

	sprintf(display_string,"%.4g",SL_volts_to_FL_command_volts);
	GetDlgItem(IDC_SL_volts_to_FL_volts)->SetWindowText(display_string);

	// Update status

	SL_calibration=SL_CALIBRATED;
	
	sprintf(display_string,"SL calibrated - SL(V):FL_C(V) = %.4g",SL_volts_to_FL_command_volts);
	display_status(TRUE,display_string);

	// Update valid parameters?

	set_parameters_valid();
}

void Cyclical::set_sampling_parameters(void)
{
	// Code sets sampling_rate, time_increment and duration_seconds
	// and is called whenever an appropriate control is altered

	// Variables

	char *p_string;												// used to extract sampling rate
	CString value_string;										// from control and for display
	char display_string[100];

	// Code

	SAMPLING_RATE_CONTROL.GetLBText(SAMPLING_RATE_CONTROL.GetCurSel(),value_string);
	sampling_rate=(float)strtod(value_string,&p_string);
	time_increment=(float)(1.0/sampling_rate);
	duration_seconds=(float)data_points*time_increment;

	// Update display

	sprintf(display_string,"%.3f",duration_seconds);
	GetDlgItem(IDC_DURATION)->SetWindowText(display_string);
}

void Cyclical::set_post_ktr_ms(void)
{
	// Function sets post_ktr_ms and post_ktr_final_fl_ms and is called whenever
	// a parameter which affects this value is adjusted

	// Variables

	char display_string[100];

	// Code

	// First update the sampling parameters
		
	set_sampling_parameters();

	// Now calculate post_ktr_ms

	post_ktr_ms=(float)((1000.0*duration_seconds)-
		(pre_length_ms+length_duration_ms+pre_ktr_ms+ktr_step_duration_ms));

	// Update display

	sprintf(display_string,"%.1f",post_ktr_ms);
	GetDlgItem(IDC_POST_KTR)->SetWindowText(display_string);

	// Calculate post_ktr_final_fl_ms

	if (post_ktr_servo_mode==0)
	{
		post_ktr_final_fl_ms=post_ktr_ms;
	}
	else
	{
		post_ktr_final_fl_ms=(post_ktr_ms-(ktr_fl_settle_ms+ktr_slcontrol_duration_ms));
	}

	// Update display

	sprintf(display_string,"%.1f",post_ktr_final_fl_ms);
	GetDlgItem(IDC_POST_KTR_FINAL_FL)->SetWindowText(display_string);

	// Check if parameters are valid

	set_parameters_valid();
}

int Cyclical::convert_time_ms_to_sampling_points(float time_ms)
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

void Cyclical::set_parameters_valid(void)
{
	// Code sets parameters_valid if experiment seems plausible

	// Code

	// Update previous value

	previous_parameters_valid=parameters_valid;
	
	// Valid records need post_ktr_final_fl_ms>0 and SL_calibrated if the experiment uses
	// sarcomere length control

	if (post_ktr_final_fl_ms>0)
	{
		// Record length is valid

		if ((length_servo_mode==SL_CONTROL)||(post_ktr_servo_mode==SL_CONTROL))
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

		if ((length_servo_mode==SL_CONTROL)||(post_ktr_servo_mode==SL_CONTROL))
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

// MFC Updates - update linked fields when one is changed

void Cyclical::OnSelchangeSamplingRate() 
{
	set_sampling_parameters();

	set_post_ktr_ms();
}

void Cyclical::OnChangeDataPoints() 
{
	UpdateData(TRUE);

	set_sampling_parameters();

	set_post_ktr_ms();
}

void Cyclical::OnKtrSlcontrol() 
{
	// User has toggled ktr_slcontrol

	UpdateData(TRUE);

	if (ktr_slcontrol==SL_CONTROL)
	{
		// Post ktr is SL_CONTROL

		GetDlgItem(IDC_KTR_SLCONTROL_DURATION)->EnableWindow(TRUE);
		GetDlgItem(IDC_KTR_FL_SETTLE)->EnableWindow(TRUE);

		GetDlgItem(IDC_CLAMP_MODE)->EnableWindow(TRUE);
		GetDlgItem(IDC_PROPORTIONAL_GAIN)->EnableWindow(TRUE);

		post_ktr_servo_mode=SL_CONTROL;
	}
	else
	{
		// Post ktr is FL_CONTROL

		GetDlgItem(IDC_KTR_SLCONTROL_DURATION)->EnableWindow(FALSE);
		GetDlgItem(IDC_KTR_FL_SETTLE)->EnableWindow(FALSE);

		if (length_servo_mode==FL_CONTROL)
		{
			GetDlgItem(IDC_CLAMP_MODE)->EnableWindow(FALSE);
			GetDlgItem(IDC_PROPORTIONAL_GAIN)->EnableWindow(FALSE);
		}

		post_ktr_servo_mode=FL_CONTROL;
	}

	// Update post_ktr_ms

	set_post_ktr_ms();
}
	
void Cyclical::OnSelchangeServoMode() 
{
	// Enables/disables servo controls as appropriate and sets length_servo_mode

	// Code

	if (LENGTH_SERVO_MODE_CONTROL.GetCurSel()==FL_CONTROL)
	{
		// Length Servo mode is FL_Control

		length_servo_mode=FL_CONTROL;

		GetDlgItem(IDC_PRE_SERVO_POINTS)->EnableWindow(FALSE);

		if (post_ktr_servo_mode==FL_CONTROL)
		{
			GetDlgItem(IDC_CLAMP_MODE)->EnableWindow(FALSE);
			GetDlgItem(IDC_PROPORTIONAL_GAIN)->EnableWindow(FALSE);
		}
	}
	else
	{
		// Length Servo mode is SL_CONTROL

		length_servo_mode=SL_CONTROL;

		GetDlgItem(IDC_PRE_SERVO_POINTS)->EnableWindow(TRUE);

		GetDlgItem(IDC_CLAMP_MODE)->EnableWindow(TRUE);
		GetDlgItem(IDC_PROPORTIONAL_GAIN)->EnableWindow(TRUE);
	}

	// Set parameters valid

	set_parameters_valid();
}

void Cyclical::OnChangeLengthDuration() 
{
	UpdateData(TRUE);

	set_sampling_parameters();

	set_post_ktr_ms();
}

void Cyclical::OnChangeKtrSlcontrolDuration() 
{
	UpdateData(TRUE);
	
	set_post_ktr_ms();
}

void Cyclical::OnChangeKtrFlSettle() 
{
	UpdateData(TRUE);
	
	set_post_ktr_ms();
}

void Cyclical::OnChangeKtrStepDuration() 
{
	UpdateData(TRUE);

	set_post_ktr_ms();
}

void Cyclical::OnChangePreKtr() 
{
	UpdateData(TRUE);

	set_post_ktr_ms();
}

// Dapl source file functions

int Cyclical::create_DAPL_source_file(void)
{
	// Special fast-sampling version
//	char disp_string[1000];
//	sprintf(disp_string,"time_increment: %g",time_increment);
//	MessageBox(disp_string);

	if (time_increment<2.5e-4)
		return fast_sampling_DAPL_source_file();
	else
	{

	// Creates DAPL_source_file by combining 5 sub-files

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
			sprintf(display_string,"%s could not be opened",DAPL_source_file_string);
			MessageBox(display_string,"Cyclical::create_DAPL_source_file");
			return(0);
		}

		FILE *header_file=fopen(header_file_string,"r");
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
		MessageBox(display_string,"Cyclical::create_DAPL_source_file");
		return(0);
	}
	}
}

int Cyclical::fast_sampling_DAPL_source_file(void)
{
	// This is a special case where the sampling rate is too fast for the
	// normal control algorithm

	// Variables
	int i;

	FILE * source_file;

	float * pipe = new float [data_points+1];									// array used for file output
	float * phase_correction_array = new float [100000];						// array used for dealing with
																				// phase correction

	int length_start,length_stop;												// integer data points for the
																				// first and last points of the
																				// length waveform

	int sl_start,sl_stop;														// integer points defining
																				// first and last points for
																				// sl control pre ktr

	int ktr_initiation_points;													// integer data point at the
																				// beginning of the ktr step

	int ktr_sl_start,ktr_sl_stop;												// integer data points for the
																				// first and last points under
																				// slcontrol for the ktr
																				// recovery

	int start_phase_array,stop_phase_array;										// first and last points in
																				// phase_correction_array

	float amplitude;															// used as temporary values
	int phase_offset;															// in a loop to prevent
	float position_offset;														// repeated calculation

	// Set record points

	ktr_initiation_time_ms=pre_length_ms+length_duration_ms+pre_ktr_ms;
	ktr_initiation_points=convert_time_ms_to_sampling_points(ktr_initiation_time_ms)+1;

	sl_start=pre_servo_points;
	sl_stop=ktr_initiation_points-1;

	length_start=convert_time_ms_to_sampling_points(pre_length_ms)+1;
	length_stop=convert_time_ms_to_sampling_points(pre_length_ms+length_duration_ms);
																				
	ktr_sl_start=convert_time_ms_to_sampling_points(ktr_initiation_time_ms+
			ktr_step_duration_ms+ktr_fl_settle_ms);
	ktr_sl_stop=convert_time_ms_to_sampling_points(ktr_initiation_time_ms+
			ktr_step_duration_ms+ktr_fl_settle_ms+ktr_slcontrol_duration_ms);

	start_phase_array=1;
	stop_phase_array=convert_time_ms_to_sampling_points(length_duration_ms+period_ms);


	// File handling
	source_file=fopen(DAPL_source_file_string,"w");
	if (source_file==NULL)
		return(0);

	fprintf(source_file,"RESET\n\n");
	fprintf(source_file,"OPTION UNDERFLOWQ=OFF\n");
	fprintf(source_file,"OPTION ERRORQ=OFF\n");
	fprintf(source_file,"OPTION SCHEDULING=FIXED\n");
	fprintf(source_file,"OPTION BUFFERING=OFF\n");
	fprintf(source_file,"OPTION QUANTUM=100\n\n");
	fprintf(source_file,";************************************************************\n\n");

	fprintf(source_file,"// Header for non-triggered capture\n\n");

	fprintf(source_file,"pipes pfl MAXSIZE=65500\n");
	fprintf(source_file,"pipes ptrigger MAXSIZE=65500\n");
	fprintf(source_file,"pipes pout MAXSIZE=65500\n\n");

	fprintf(source_file,";************************************************************\n\n");

	fprintf(source_file,"fill ptrigger %i\n\n",convert_volts_to_DAPL_units((float)4.95));

	fprintf(source_file,";************************************************************\n\n");

	// Generate pfl
	// Pre waveform
	::generate_waveform(CONSTANT,pipe,1,length_start,1,0);
	// Generate waveform, one cycle longer than required and of unit amplitude
	if (length_function_string=="Sine")
		generate_waveform(SINE_WAVE,phase_correction_array,start_phase_array,stop_phase_array,
			(float)convert_time_ms_to_sampling_points(period_ms),1);
	
	if (length_function_string=="Cosine")
		generate_waveform(COS_WAVE,phase_correction_array,start_phase_array,stop_phase_array,
			(float)convert_time_ms_to_sampling_points(period_ms),1);

	if (length_function_string=="Triangle")
		generate_waveform(TRIANGLE,phase_correction_array,start_phase_array,stop_phase_array,
			(float)convert_time_ms_to_sampling_points(period_ms),1);

	if (length_function_string=="Square")
		generate_waveform(SQUARE_WAVE,phase_correction_array,start_phase_array,stop_phase_array,
			(float)convert_time_ms_to_sampling_points(period_ms),1);

	// Now select appropriate section to account for phase delay, multiply by amplitude and
	// apply offset
	amplitude=(float)::convert_volts_to_DAPL_units(
		::convert_FL_COMMAND_microns_to_volts(length_size_microns));

	phase_offset=(int)((phase_degrees/360.0)*convert_time_ms_to_sampling_points(period_ms));

	position_offset=(float)::convert_volts_to_DAPL_units(
		::convert_FL_COMMAND_microns_to_volts(length_offset_microns));

	for (i=length_start;i<length_stop;i++)
	{
		pipe[i]=(amplitude*phase_correction_array[i-length_start+1+phase_offset])+position_offset;
	}

	// Between length stop and ktr
	::generate_waveform(CONSTANT,pipe,length_stop,ktr_initiation_points,1,0);

	// ktr
	::generate_waveform(CONSTANT,pipe,ktr_initiation_points,
		(ktr_initiation_points+convert_time_ms_to_sampling_points(ktr_step_duration_ms)),1,
		(float)::convert_volts_to_DAPL_units(
			::convert_FL_COMMAND_microns_to_volts(ktr_step_size_microns)));

	// Post ktr
	::generate_waveform(CONSTANT,pipe,
		(ktr_initiation_points+convert_time_ms_to_sampling_points(ktr_step_duration_ms)),data_points,
			1,0);

	// Write pipe to file
	::write_pipe_to_file(source_file,"pfl",pipe,data_points);
	fprintf(source_file,";************************************************************\n\n");

	fprintf(source_file,"pdef create_pipes\n\n");
	fprintf(source_file,"  ptrigger=0\n");
	fprintf(source_file,"end\n\n");

	fprintf(source_file,";************************************************************\n\n");

	fprintf(source_file,"pdef control\n\n");
	fprintf(source_file,"  pout=pfl\n");
	fprintf(source_file,"  copy(pout,op1)\n");
	fprintf(source_file,"  copy(ptrigger,op0)\n\n");
	fprintf(source_file,"  merge(ip0,ip1,ip2,ip3,$binout)\n");
	fprintf(source_file,"end\n\n");

	fprintf(source_file,";************************************************************\n\n");
    
	fprintf(source_file,"odef outputproc 2\n");
	fprintf(source_file,"  outputwait 2\n");
	fprintf(source_file,"  update burst\n");
	fprintf(source_file,"  set op0 a0\n");
	fprintf(source_file,"  set op1 a1\n");
	fprintf(source_file,"  time %i\n",(int)(0.5*time_increment*1e6));
	fprintf(source_file,"end\n\n");

	fprintf(source_file,"idef inputproc 4\n");
	fprintf(source_file,"  set ip0 s0\n");
	fprintf(source_file,"  set ip1 s1\n");
	fprintf(source_file,"  set ip2 s2\n");
	fprintf(source_file,"  set ip3 s3\n");
	fprintf(source_file,"  count %i\n",(4*data_points));
	fprintf(source_file,"  time %.0f\n",(0.25*time_increment*1.0e6));
	fprintf(source_file,"end\n\n");

	fprintf(source_file,";************************************************************\n\n");

	fprintf(source_file,"start create_pipes,control,inputproc,outputproc\n\n");

	fclose(source_file);

	return 1;
}

int Cyclical::write_header_file(void)
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
		MessageBox(display_string,"Cyclical::write_header_file");
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

int Cyclical::write_fill_pipes_file(void)
{
	// Code writes fill pipes file

	// Variables

	int i;																		// counter

	char display_string[300];													// user display

	FILE *fill_pipes_file;														// file pointer

	float * pipe = new float [data_points+1];									// array used for file output
	float * phase_correction_array = new float [100000];						// array used for dealing with
																				// phase correction

	int length_start,length_stop;												// integer data points for the
																				// first and last points of the
																				// length waveform

	int sl_start,sl_stop;														// integer points defining
																				// first and last points for
																				// sl control pre ktr

	int ktr_initiation_points;													// integer data point at the
																				// beginning of the ktr step

	int ktr_sl_start,ktr_sl_stop;												// integer data points for the
																				// first and last points under
																				// slcontrol for the ktr
																				// recovery

	int start_phase_array,stop_phase_array;										// first and last points in
																				// phase_correction_array

	float amplitude;															// used as temporary values
	int phase_offset;															// in a loop to prevent
	float position_offset;														// repeated calculation


	// Code

	// Set record points

	ktr_initiation_time_ms=pre_length_ms+length_duration_ms+pre_ktr_ms;
	ktr_initiation_points=convert_time_ms_to_sampling_points(ktr_initiation_time_ms)+1;

	sl_start=pre_servo_points;
	sl_stop=ktr_initiation_points-1;

	length_start=convert_time_ms_to_sampling_points(pre_length_ms)+1;
	length_stop=convert_time_ms_to_sampling_points(pre_length_ms+length_duration_ms);
																				
	ktr_sl_start=convert_time_ms_to_sampling_points(ktr_initiation_time_ms+
			ktr_step_duration_ms+ktr_fl_settle_ms);
	ktr_sl_stop=convert_time_ms_to_sampling_points(ktr_initiation_time_ms+
			ktr_step_duration_ms+ktr_fl_settle_ms+ktr_slcontrol_duration_ms);

	start_phase_array=1;
	stop_phase_array=convert_time_ms_to_sampling_points(length_duration_ms+period_ms);

	// File handling

	fill_pipes_file=fopen(fill_pipes_file_string,"w");

	if (fill_pipes_file==NULL)													// Error
	{
		sprintf(display_string,"Fill_pipes_file\n%s\ncould not be opened\n",fill_pipes_file_string);
		MessageBox(display_string,"Cyclical::write_fill_pipes_file");
		return(0);
	}
	else																		// Write file
	{
	
		fprintf(fill_pipes_file,"\nfill ptrigger %i\n\n",convert_volts_to_DAPL_units((float)4.95));
																				// default high trigger
			
		fprintf(fill_pipes_file,"fill plastout 0\n");							// initial values
		fprintf(fill_pipes_file,"fill plastoffset 0\n\n");

		// pservo

		if ((length_servo_mode==FL_CONTROL)&&(post_ktr_servo_mode==FL_CONTROL))
		{
			for (i=1;i<=data_points;i++)		// No servo control
				pipe[i]=0;
		}

		if ((length_servo_mode==SL_CONTROL)&&(post_ktr_servo_mode==FL_CONTROL))
		{
			for (i=1;i<=data_points;i++)		// Servo control for length function but not ktr
			{
				if ((i>sl_start)&&(i<=sl_stop))
					pipe[i]=1;
				else
					pipe[i]=0;
			}
		}

		if ((length_servo_mode==FL_CONTROL)&&(post_ktr_servo_mode==SL_CONTROL))
		{
			for (i=1;i<=data_points;i++)
			{
				if ((i>ktr_sl_start)&&(i<=ktr_sl_stop))
					pipe[i]=1;
				else
					pipe[i]=0;
			}
		}

		if ((length_servo_mode==SL_CONTROL)&&(post_ktr_servo_mode==SL_CONTROL))
		{
			for (i=1;i<=data_points;i++)		// Servo control pre and post ktr
			{
				if (((i>sl_start)&&(i<=sl_stop)) || ((i>ktr_sl_start)&&(i<=ktr_sl_stop)))
					pipe[i]=1;
				else
					pipe[i]=0;
			}	
		}

		::write_pipe_to_file(fill_pipes_file,"pservo",pipe,data_points);

		// poffset_marker

		if (length_servo_mode==FL_CONTROL)
		{
			// SL control for ktr only or not at all - offset just before the ktr step

			for (i=1;i<=data_points;i++)
			{
				if (i<ktr_initiation_points)
					pipe[i]=1;
				else
					pipe[i]=0;
			}
		}
		else
		{
			// Offset to pre_servo_points

			for (i=1;i<=data_points;i++)
			{
				if (i<pre_servo_points)
					pipe[i]=1;
				else
					pipe[i]=0;
			}
		}

		::write_pipe_to_file(fill_pipes_file,"poffset_marker",pipe,data_points);

		// pfl

		// Pre waveform

		::generate_waveform(CONSTANT,pipe,1,length_start,1,0);

		// Generate waveform, one cycle longer than required and of unit amplitude

		if (length_function_string=="Sine")
			generate_waveform(SINE_WAVE,phase_correction_array,start_phase_array,stop_phase_array,
				(float)convert_time_ms_to_sampling_points(period_ms),1);
		
		if (length_function_string=="Cosine")
			generate_waveform(COS_WAVE,phase_correction_array,start_phase_array,stop_phase_array,
				(float)convert_time_ms_to_sampling_points(period_ms),1);

		if (length_function_string=="Triangle")
			generate_waveform(TRIANGLE,phase_correction_array,start_phase_array,stop_phase_array,
				(float)convert_time_ms_to_sampling_points(period_ms),1);

		if (length_function_string=="Square")
			generate_waveform(SQUARE_WAVE,phase_correction_array,start_phase_array,stop_phase_array,
				(float)convert_time_ms_to_sampling_points(period_ms),1);

		// Now select appropriate section to account for phase delay, multiply by amplitude and
		// apply offset

		amplitude=(float)::convert_volts_to_DAPL_units(
			::convert_FL_COMMAND_microns_to_volts(length_size_microns));

		phase_offset=(int)((phase_degrees/360.0)*convert_time_ms_to_sampling_points(period_ms));

		position_offset=(float)::convert_volts_to_DAPL_units(
			::convert_FL_COMMAND_microns_to_volts(length_offset_microns));

		for (i=length_start;i<length_stop;i++)
		{
			pipe[i]=(amplitude*phase_correction_array[i-length_start+1+phase_offset])+position_offset;
		}

		// Between length stop and ktr

		::generate_waveform(CONSTANT,pipe,length_stop,ktr_initiation_points,1,0);

		// ktr

		::generate_waveform(CONSTANT,pipe,ktr_initiation_points,
			(ktr_initiation_points+convert_time_ms_to_sampling_points(ktr_step_duration_ms)),1,
			(float)::convert_volts_to_DAPL_units(
				::convert_FL_COMMAND_microns_to_volts(ktr_step_size_microns)));

		// Post ktr

		::generate_waveform(CONSTANT,pipe,
			(ktr_initiation_points+convert_time_ms_to_sampling_points(ktr_step_duration_ms)),data_points,
			1,0);

		// Write pipe to file

		::write_pipe_to_file(fill_pipes_file,"pfl",pipe,data_points);

		// ptarget

		// Pre waveform

		::generate_waveform(CONSTANT,pipe,1,length_start,1,0);

		// Generate waveform, one cycle longer than required and of unit amplitude

		if (length_function_string=="Sine")
			generate_waveform(SINE_WAVE,phase_correction_array,start_phase_array,stop_phase_array,
				(float)convert_time_ms_to_sampling_points(period_ms),1);
		
		if (length_function_string=="Cosine")
			generate_waveform(COS_WAVE,phase_correction_array,start_phase_array,stop_phase_array,
				(float)convert_time_ms_to_sampling_points(period_ms),1);

		if (length_function_string=="Triangle")
			generate_waveform(TRIANGLE,phase_correction_array,start_phase_array,stop_phase_array,
				(float)convert_time_ms_to_sampling_points(period_ms),1);

		if (length_function_string=="Square")
			generate_waveform(SQUARE_WAVE,phase_correction_array,start_phase_array,stop_phase_array,
				(float)convert_time_ms_to_sampling_points(period_ms),1);

		// Now select appropriate section to account for phase delay, multiply by amplitude
		// and apply offset

		amplitude=(float)::convert_volts_to_DAPL_units(length_size_microns*SL_volts_to_FL_microns_calibration);

		phase_offset=(int)((phase_degrees/360.0)*convert_time_ms_to_sampling_points(period_ms));

		position_offset=(float)::convert_volts_to_DAPL_units(length_offset_microns*
									SL_volts_to_FL_microns_calibration);

		for (i=length_start;i<length_stop;i++)
		{
			pipe[i]=(amplitude*phase_correction_array[i-length_start+1+phase_offset])+
						position_offset;
		}

		// Between length stop and end of record

		::generate_waveform(CONSTANT,pipe,length_stop,data_points,1,0);

		// Write pipe to file

		::write_pipe_to_file(fill_pipes_file,"ptarget",pipe,data_points);
	}

	// Tidy up

	delete [] pipe;
	delete [] phase_correction_array;

	fclose(fill_pipes_file);

	return(1);
}

int Cyclical::write_create_pipes_file(void)
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
		MessageBox(display_string,"Cyclical::write_create_pipes_file");
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

int Cyclical::write_control_file(void)
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
		MessageBox(display_string,"Cyclical::write_control_file");
		return(0);
	}
	else
	{
		fprintf(control_file,"\npdef control\n");
		fprintf(control_file,"  poffset=((1-poffset_marker)*plastoffset)+(poffset_marker*ip1)\n");
		fprintf(control_file,"  plastoffset=poffset\n\n");

		fprintf(control_file,"  pfeedback=plastout-(%.0f*(ip1-ptarget-((1-zeroclamp)*poffset))/10000)\n",
			1.0e4*proportional_gain/SL_volts_to_FL_command_volts);
		fprintf(control_file,"  pout=((1-pservo)*pfl)+(pservo*pfeedback)\n\n");

		fprintf(control_file,"  copy(pout,plastout,op1)\n");
		fprintf(control_file,"  copy(ptrigger,op0)\n\n");

		fprintf(control_file,"  merge(ip0,ip1,ip2,ip3,$binout)\n");
		fprintf(control_file,"end\n\n");

		fprintf(control_file,";************************************************************\n\n");
	}
	
	fclose(control_file);

	return(1);
}

int Cyclical::write_io_procedures_file(void)
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
		MessageBox(display_string,"Cyclical::write_io_procedures_file");
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

		fprintf(io_procedures_file,"start create_pipes,control,inputproc,outputproc\n");
	}

	fclose(io_procedures_file);

	return(1);
}





