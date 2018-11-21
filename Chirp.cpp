// Chirp.cpp : implementation file
//

#include "stdafx.h"
#include "slcontrol.h"
#include "Chirp.h"
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
// Chirp dialog


Chirp::Chirp(CWnd* pParent /*=NULL*/)
	: CDialog(Chirp::IDD, pParent)
{
	// Constuctor

	m_pParent=pParent;
	m_nID=Chirp::IDD;

	// Create pointer to the data record

	p_Chirp_record =& Chirp_record;

	// Null daughter window pointers

	p_Calibration=NULL;
	p_Experiment_display=NULL;

	// Variables internal to constructor

	char* p_string;
	char temp_string[_MAX_PATH];

	//{{AFX_DATA_INIT(Chirp)
	sampling_rate_string = _T("");
	data_points = 0;
	duration_string = _T("");
	post_ktr_ms = 0.0f;
	length_duration_ms = 0.0f;
	begin_period_ms = 0.0f;
	end_period_ms = 0.0f;
	mid_period_ms = 0.0f;
	pre_length_ms = 0.0f;
	length_size_microns = 0.0f;
	length_offset_microns = 0.0f;
	length_function_string = _T("");
	clamp_mode_string = _T("");
	pre_servo_points = 0;
	proportional_gain = 0.0f;
	SL_volts_to_FL_command_volts = 0.0f;
	SL_volts_to_FL_microns_calibration = 0.0f;
	ktr_step_duration_ms = 0.0f;
	ktr_step_size_microns = 0.0f;
	pre_ktr_ms = 0.0f;
	length_servo_mode_string = _T("");
	chirp_sweep_string = _T("");
	chirp_mid_position = 0.0f;
	data_base_string = _T("");
	data_path_string = _T("");
	data_index = 0;
	//}}AFX_DATA_INIT

	// File strings

	// Set dir_path

	char display_string[_MAX_PATH];

	::set_executable_directory_string(display_string);
	strcat(display_string,"\\Chirp\\");

	Chirp_dir_path=display_string;

	header_file_string=Chirp_dir_path+"Chirp_header.txt";
	fill_pipes_file_string=Chirp_dir_path+"Chirp_fill_pipes.txt";
	create_pipes_file_string=Chirp_dir_path+"Chirp_creates_pipe.txt";
	control_file_string=Chirp_dir_path+"Chirp_control.txt";
	io_procedures_file_string=Chirp_dir_path+"Chirp_io_procs.txt";
	overflow_file_string=Chirp_dir_path+"Chirp_overflow.txt";
	DAPL_source_file_string=Chirp_dir_path+"dapl_source.txt";
	
	// Chirp display

	sampling_rate_string="1000";
	sampling_rate=1000.0;
	data_points=20000;
	time_increment=((float)(1.0/strtod(sampling_rate_string,&p_string)));
	duration_seconds=data_points*time_increment;
	sprintf(temp_string,"%.3f",duration_seconds);
	duration_string=temp_string;

	overflow_status=0;
	overflow_threshold=64500;

	// Filing parameters

	data_path_string="d:\\temp";
	data_base_string="test";
	data_index=1;

	// Servo parameters

	length_servo_mode_string="Fibre Length Control";
	length_servo_mode=FL_CONTROL;
	clamp_mode_string="Prevailing position";
	clamp_mode=PREVAILING_POSITION;
	proportional_gain=(float)0.001;
	pre_servo_points=100;

	// SL Calibration parameters

	SL_calibration=SL_NOT_CALIBRATED;

	// ktr parameters

	ktr_step_size_microns=-20.0;
	pre_ktr_ms=100.0;
	ktr_step_duration_ms=20.0;

	// Length variables

	pre_length_ms=100.0;
	length_duration_ms=10000.0;
	length_size_microns=100.0;
	length_offset_microns=0.0;
	length_function_string="Sine";

	// Chirp variables

	chirp_sweep_string="Logarithmic";
	chirp_sweep_mode=LOGARITHMIC_CHIRP;
	chirp_mid_position=0.5;

	begin_period_ms=500.0;
	mid_period_ms=100.0;
	end_period_ms=2000.0;

	// Set post_ktr_ms

	post_ktr_ms=(float)((1000.0*duration_seconds)-
		(pre_length_ms+length_duration_ms+pre_ktr_ms+ktr_step_duration_ms));

	// Status strings and counter

	status_middle_string="1) Chirp dialog initialising";
	status_bottom_string="2) Preparing for trials";
	status_counter=3;

	// Parameters valid

	parameters_valid=TRUE;
	previous_parameters_valid=TRUE;

	// Create pipes and code buffer

	pipe_servo = new float [MAX_DATA_POINTS+1];
	pipe_offset = new float [MAX_DATA_POINTS+1];
	pipe_fl = new float [MAX_DATA_POINTS+1];
	pipe_target = new float [MAX_DATA_POINTS+1];

	code_buffer = new char [15*MAX_DATA_POINTS];					// should be big enough for
																	// entire DAPL code since most
																	// lines will be like
																	// fill pipe 0 1 0 0 0 1 0 1
}

Chirp::~Chirp()
{
	// Destructor

	delete [] pipe_servo;
	delete [] pipe_offset;
	delete [] pipe_fl;
	delete [] pipe_target;

	delete [] code_buffer;

}

void Chirp::PostNcDestroy()
{
	delete this;
}

void Chirp::OnClose()
{
	// Close derived windows

	p_Calibration->close_calibration_window();
	p_Calibration=NULL;

	p_Experiment_display->close_experiment_display_window();
	p_Experiment_display=NULL;

	// Signal to parent that this window is closing

	((CSLControlDlg*)m_pParent)->Chirp_Window_Done();

	DestroyWindow();
}

BOOL Chirp::Create()
{
	return CDialog::Create(m_nID,m_pParent);
}

BOOL Chirp::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Variables

	int width=1022;
	int height=740;

	// Code

	// Set window size and position

	SetWindowPos(NULL,(int)(0.02*width),(int)(0.07*height),
		(int)(0.63*width),(int)(0.43*height),SWP_SHOWWINDOW);

	// Create the calibration window

	p_Calibration = new Calibration();
	p_Calibration->Create();

	// Create the Experiment_display window

	p_Experiment_display = new Experiment_display(this,p_Chirp_record);
	p_Experiment_display->Create();

	// Disable appropriate sampling rate controls

	GetDlgItem(IDC_DURATION)->EnableWindow(FALSE);
	GetDlgItem(IDC_POST_KTR)->EnableWindow(FALSE);

	// Disable servo controls

	GetDlgItem(IDC_PRE_SERVO_POINTS)->EnableWindow(FALSE);
	GetDlgItem(IDC_CLAMP_MODE)->EnableWindow(FALSE);
	GetDlgItem(IDC_PROPORTIONAL_GAIN)->EnableWindow(FALSE);

	// Disable calibration controls

	GetDlgItem(IDC_SL_volts_to_FL_volts)->EnableWindow(FALSE);
	GetDlgItem(IDC_SL_to_FL_microns_calibration)->EnableWindow(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void Chirp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Chirp)
	DDX_Control(pDX, IDC_CHIRP_SWEEP, CHIRP_SWEEP_CONTROL);
	DDX_Control(pDX, IDC_SERVO_MODE, LENGTH_SERVO_MODE_CONTROL);
	DDX_Control(pDX, IDC_CLAMP_MODE, CLAMP_MODE_CONTROL);
	DDX_Control(pDX, IDC_LENGTH_FUNCTION, LENGTH_FUNCTION_CONTROL);
	DDX_Control(pDX, IDC_SAMPLING_RATE, SAMPLING_RATE_CONTROL);
	DDX_CBString(pDX, IDC_SAMPLING_RATE, sampling_rate_string);
	DDX_Text(pDX, IDC_DATA_POINTS, data_points);
	DDV_MinMaxInt(pDX, data_points, 1, 500000);
	DDX_Text(pDX, IDC_DURATION, duration_string);
	DDX_Text(pDX, IDC_POST_KTR, post_ktr_ms);
	DDX_Text(pDX, IDC_LENGTH_DURATION, length_duration_ms);
	DDV_MinMaxFloat(pDX, length_duration_ms, 0.f, 1.e+006f);
	DDX_Text(pDX, IDC_BEGIN_PERIOD, begin_period_ms);
	DDX_Text(pDX, IDC_END_PERIOD, end_period_ms);
	DDX_Text(pDX, IDC_MID_PERIOD, mid_period_ms);
	DDX_Text(pDX, IDC_PRE_LENGTH, pre_length_ms);
	DDX_Text(pDX, IDC_LENGTH_SIZE, length_size_microns);
	DDX_Text(pDX, IDC_OFFSET, length_offset_microns);
	DDX_CBString(pDX, IDC_LENGTH_FUNCTION, length_function_string);
	DDX_CBString(pDX, IDC_CLAMP_MODE, clamp_mode_string);
	DDX_Text(pDX, IDC_PRE_SERVO_POINTS, pre_servo_points);
	DDX_Text(pDX, IDC_PROPORTIONAL_GAIN, proportional_gain);
	DDX_Text(pDX, IDC_SL_volts_to_FL_volts, SL_volts_to_FL_command_volts);
	DDX_Text(pDX, IDC_SL_to_FL_microns_calibration, SL_volts_to_FL_microns_calibration);
	DDX_Text(pDX, IDC_KTR_STEP_DURATION, ktr_step_duration_ms);
	DDX_Text(pDX, IDC_KTR_STEP_SIZE, ktr_step_size_microns);
	DDX_Text(pDX, IDC_PRE_KTR, pre_ktr_ms);
	DDX_CBString(pDX, IDC_SERVO_MODE, length_servo_mode_string);
	DDX_CBString(pDX, IDC_CHIRP_SWEEP, chirp_sweep_string);
	DDX_Text(pDX, IDC_MID_POSITION, chirp_mid_position);
	DDV_MinMaxFloat(pDX, chirp_mid_position, 0.f, 1.f);
	DDX_Text(pDX, IDC_DATA_BASE, data_base_string);
	DDX_Text(pDX, IDC_DATA_PATH, data_path_string);
	DDX_Text(pDX, IDC_DATA_INDEX, data_index);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Chirp, CDialog)
	//{{AFX_MSG_MAP(Chirp)
	ON_BN_CLICKED(IDC_Run_Experiment, OnRunExperiment)
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_SAMPLING_RATE, OnSelchangeSamplingRate)
	ON_EN_CHANGE(IDC_DATA_POINTS, OnChangeDataPoints)
	ON_CBN_SELCHANGE(IDC_CHIRP_SWEEP, OnSelchangeChirpSweep)
	ON_BN_CLICKED(IDC_SL_to_FL_Calibration, OnSLtoFLCalibration)
	ON_CBN_SELCHANGE(IDC_SERVO_MODE, OnSelchangeServoMode)
	ON_EN_CHANGE(IDC_KTR_STEP_DURATION, OnChangeKtrStepDuration)
	ON_EN_CHANGE(IDC_PRE_KTR, OnChangePreKtr)
	ON_EN_CHANGE(IDC_LENGTH_DURATION, OnChangeLengthDuration)
	ON_EN_CHANGE(IDC_PRE_LENGTH, OnChangePreLength)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Chirp message handlers

void Chirp::OnRunExperiment() 
{
	// TODO: Add your control notification handler code here

	// Variables

	int i;

	char display_string[200];

	char cursor_path_string[300];

	char single_character='a';												// used for sizeof operator

	short short_int=1;														// used for sizeof operator
	int holder;																// another counter
	int buffer_size;														// number of points in data
																			// buffer with 4 channels read
																			// from DAPL

	int points_read;														// data points read from DAP
	int bytes_available;													// bytes available from DAP
	int buffer_points;														// total number of data samples
																			// available from DAP
	int points_available;													// number of data points/channel
																			// available from DAP

	int characters_written;													// number of DAPL command characters
																			// transferred to DAP

	int return_value;														// DAP function return value

	clock_t start_fill,start_loop,stop_loop,last_display;					// clocks

	HDAP hdapSysGet,hdapSysPut,hdapBinGet;									// DAP communication pipes

	// DAP Structures

	TDapBufferPutEx command_stream;
	DapStructPrepare(command_stream);

	TDapBufferGetEx record_stream;
	DapStructPrepare(record_stream);

	// Code

	UpdateData(TRUE);

	// Allocate data buffer

	buffer_size=(4*data_points);

	short int* DAPL_data = new short int [buffer_size+4];

	// Initiate experiment

	// Check experiment is valid - if not, warn user, else run experiment

	if (parameters_valid<0)
	{
		MessageBox("Trial aborted - parameters not valid","Chirp::OnRunExperiment");
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

	// Switch to animated animated cursor

	::set_executable_directory_string(cursor_path_string);
	strcat(cursor_path_string,"\\sl_control.ani");

	HCURSOR hcurWait = (HCURSOR)LoadImage(AfxGetApp()->m_hInstance,
	cursor_path_string,IMAGE_CURSOR,0,0,
	LR_DEFAULTSIZE|LR_DEFAULTCOLOR|LR_LOADFROMFILE);
	const HCURSOR hcurSave=SetCursor(hcurWait);

	SetCursor(hcurWait);

	// Create and Fill code buffer

	sprintf(display_string,"Creating DAPL file and filling buffer");
	display_status(TRUE,display_string);
	
	start_fill=clock();

	create_DAPL_source_file();

	fill_code_buffer();

	sprintf(display_string,"Fill completed in %.4f",(float)(clock()-start_fill)/CLOCKS_PER_SEC);
	display_status(TRUE,display_string);

	// Open and flush DAP communication pipes

	hdapSysGet=DapHandleOpen("\\\\.\\Dap0\\$SysOut",DAPOPEN_READ);

	hdapSysPut=DapHandleOpen("\\\\.\\Dap0\\$SysIn",DAPOPEN_WRITE);
	
	hdapBinGet=DapHandleOpen("\\\\.\\Dap0\\$BinOut",DAPOPEN_READ);
	
	i=DapInputFlush(hdapSysGet);
	sprintf(display_string,"Flush hdapSysGet: %i",i);
	display_status(TRUE,display_string);

	i=DapInputFlush(hdapBinGet);
	sprintf(display_string,"Flush hdapBinGet: %i",i);
	display_status(TRUE,display_string);

	// Prepare DAP structures

	command_stream.iBytesPut=buffer_characters*sizeof(single_character);
	command_stream.dwTimeWait=10;
	command_stream.dwTimeOut=100;
	command_stream.iBytesMultiple=1;

	record_stream.iBytesGetMin=4*sizeof(short_int);
	record_stream.iBytesGetMax=10000*sizeof(short_int);
	record_stream.dwTimeWait=10;
	record_stream.dwTimeOut=100;
	record_stream.iBytesMultiple=4*sizeof(short_int);

	// Loop until all points have been read

	points_read=0;
	characters_written=0;
	last_display=clock();

	while (points_read<data_points)
	{
		start_loop=clock();														// record time

		// Try to send data if fill not complete

		if (characters_written<buffer_characters)
		{
			command_stream.iBytesPut=(buffer_characters-characters_written)*sizeof(single_character);

			return_value=DapBufferPutEx(hdapSysPut,&command_stream,&code_buffer[characters_written]);

			if (return_value>=0)
			{
				characters_written=characters_written+return_value;
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
				p_Chirp_record->data[TIME][points_read+holder]=((float)(points_read+holder-1)*time_increment);
				p_Chirp_record->data[FORCE][points_read+holder]=convert_DAPL_units_to_volts(DAPL_data[i]);
				p_Chirp_record->data[SL][points_read+holder]=convert_DAPL_units_to_volts(DAPL_data[i+1]);
				p_Chirp_record->data[FL][points_read+holder]=convert_DAPL_units_to_volts(DAPL_data[i+2]);
				p_Chirp_record->data[INTENSITY][points_read+holder]=convert_DAPL_units_to_volts(DAPL_data[i+3]);

				holder++;
			}

			points_read=(points_read+points_available);
		}

		stop_loop=clock();

		if (((float)(clock()-last_display)/(float)CLOCKS_PER_SEC)>1.0)
		{
			sprintf(display_string,"Last loop: %.0f ms, %.3f written %.3f read",
				(float)(1000.0*(stop_loop-start_loop)/CLOCKS_PER_SEC),
				(float)characters_written/(float)buffer_characters,
				(float)points_read/(float)data_points);
			display_status(TRUE,display_string);
			last_display=clock();
		}
	}

	// Copy data to record and update no_of_points

	p_Chirp_record->no_of_points=data_points;

	// Close DAPL pipes

	DapHandleClose(hdapSysGet);
	DapHandleClose(hdapSysPut);
	DapHandleClose(hdapBinGet);

	// Update plots

	p_Experiment_display->clear_window();

	for (i=0;i<=(p_Experiment_display->no_of_plots-1);i++)
	{
		p_Experiment_display->pointers_array[i]->first_drawn_point=1;
		p_Experiment_display->pointers_array[i]->last_drawn_point=
			p_Chirp_record->no_of_points;
	}

	p_Experiment_display->update_plots();

	write_data_to_file();

	// Free memory

	delete [] DAPL_data;

	// End cursor wait

	SetCursor(hcurSave);

	// Tidy up

	sprintf(display_string,"Trial completed");
	display_status(TRUE,display_string);
	MessageBeep(MB_ICONEXCLAMATION);

	UpdateData(FALSE);

	// Signal successful

	return;
}

void Chirp::write_data_to_file(void)
{
	// Code writes data to file

	// Variables

	int i,j;																// counter

	char index_string[20];												// index string
	char display_string[FILE_INFO_STRING_LENGTH];
	char file_info_string[FILE_INFO_STRING_LENGTH];

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

		p_Calibration->UpdateData(TRUE);

		fprintf(output_data_file,"_MUSCLE_DIMENSIONS_\n");
		fprintf(output_data_file,"Muscle_length_(µm): %.3e\n",p_Calibration->Calibration_fibre_length);
		fprintf(output_data_file,"Sarcomere_length_(nm): %.3e\n",p_Calibration->Calibration_sarcomere_length);
		fprintf(output_data_file,"Area_(m^2): %.3e\n\n",p_Calibration->Calibration_area);

		// Experiment parameters

		fprintf(output_data_file,"_CHIRP_\n");
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
		fprintf(output_data_file,"length_offset_(µm): %.3e\n\n",length_offset_microns);

		fprintf(output_data_file,"chirp_sweep_string: %s\n",chirp_sweep_string);
		fprintf(output_data_file,"chirp_mid_position: %f\n",chirp_mid_position);
		fprintf(output_data_file,"begin_period_(ms): %g\n",begin_period_ms);
		fprintf(output_data_file,"mid_period_(ms): %g\n",mid_period_ms);
		fprintf(output_data_file,"end_period_(ms): %g\n",end_period_ms);
			
		fprintf(output_data_file,"length_servo_mode: %i\n",length_servo_mode);
		fprintf(output_data_file,"pre_servo_points: %i\n",pre_servo_points);
		fprintf(output_data_file,"clamp_mode: %i\n",clamp_mode);
		fprintf(output_data_file,"proportional_gain: %.3e\n\n",proportional_gain);

		// Data

		fprintf(output_data_file,"Data\n");

		for (i=1;i<=p_Chirp_record->no_of_points;i++)
		{
			fprintf(output_data_file,"%f\t%f\t%f\t%f\t%f\n",
				p_Chirp_record->data[TIME][i],
				p_Chirp_record->data[FORCE][i],
				p_Chirp_record->data[SL][i],
				p_Chirp_record->data[FL][i],
				p_Chirp_record->data[INTENSITY][i]);
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

void Chirp::fill_code_buffer(void)
{
	// Fills code_buffer

	// Variables

	char single_character;
	char display_string[200];
		
	// Code

	FILE *code_file=fopen(DAPL_source_file_string,"r");

	if (code_file==NULL)
	{
		sprintf(display_string,"Code file\n%s\ncould not be opened",DAPL_source_file_string);
		MessageBox(display_string,"Chirp::fill_code_buffer");
	}

	// Loop

	buffer_characters=0;

	while (!feof(code_file))
	{
		single_character=fgetc(code_file);

		if (single_character!=EOF)
		{
			code_buffer[buffer_characters]=single_character;
			buffer_characters++;
		}
	}

	code_buffer[buffer_characters]='\0';
	buffer_characters--;

	fclose(code_file);
}

void Chirp::OnCancel() 
{
	// Ensures correct shut-down when window is dismissed with the Escape key

	OnClose();
}

void Chirp::remote_shut_down(void)
{
	// Called by parent to shut window

	OnClose();
}

void Chirp::OnPaint() 
{
	CPaintDC dc(this);

	// Redraw status message

	display_status(FALSE,"");
}

void Chirp::display_status(int update_mode,CString new_string)
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

void Chirp::set_parameters_valid(void)
{
	// Code sets parameters_valid if experiment seems plausible

	// Code

	// Update previous value

	previous_parameters_valid=parameters_valid;
	
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

// Internal processing functions

void Chirp::set_sampling_parameters(void)
{
	// Code sets sampling rate, time_increment and duration_seconds

	// Variables

	char* p_string;													// used to extract sampling rate
	CString value_string;											// from control
	char display_string[100];

	// Code

	SAMPLING_RATE_CONTROL.GetLBText(SAMPLING_RATE_CONTROL.GetCurSel(),value_string);
	sampling_rate=(float)strtod(value_string,&p_string);
	time_increment=(float)1.0/sampling_rate;
	duration_seconds=(float)data_points*time_increment;

	// Update display

	sprintf(display_string,"%.3f",duration_seconds);
	GetDlgItem(IDC_DURATION)->SetWindowText(display_string);

}

void Chirp::set_post_ktr_ms(void)
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

	// Check if parameters are valid

	set_parameters_valid();
}

int Chirp::convert_time_ms_to_sampling_points(float time_ms)
{
	// Returns no of sample points in a given time interval (expressed in ms)

	// Variables

	int points;													// sample points in time_ms

	// Code

	// Calculate points and return

	points=(int)((time_ms/1000.0)*sampling_rate);

	return(points);
}

void Chirp::create_pipes(void)
{
	// Code fills pipe arrays which are written to file by Chirp::write_fill_pipes_file

	// Variables

	int i;

	int ktr_initiation_points;											// integer data point at the beginning
																		// of the ktr step

	int sl_start,sl_stop;												// integer data points defining start
																		// and stop for sarcomere length
																		// control

	int length_start,length_stop;										// integer data points for the first
																		// and last points of the length
																		// waveform

	float begin_position_increment;										// phase_increments at beginning
	float mid_position_increment;										// mid position and end of chirps
	float end_position_increment;

	int first_phase_points;												// number of points in first and
	int second_phase_points;											// second phases of chirp

	float amplitude,position_offset;									// used as temporary values inside
																		// loops to prevent repeated
																		// calculations

	float denominator;													// temporary float used in exponent
																		// calculations

	float position;

	float* increment;													// position increment for each data
																		// point

	// Code

	increment = new float [MAX_DATA_POINTS+1];

	for (i=1;i<=MAX_DATA_POINTS;i++)
	{
		increment[i]=0.0;
	}

	// Set record points

	ktr_initiation_time_ms=pre_length_ms+length_duration_ms+pre_ktr_ms;
	ktr_initiation_points=convert_time_ms_to_sampling_points(ktr_initiation_time_ms)+1;

	length_start=convert_time_ms_to_sampling_points(pre_length_ms)+1;
	length_stop=convert_time_ms_to_sampling_points(pre_length_ms+length_duration_ms);

	// Set sl_start and stop points

	sl_start=pre_servo_points;
	sl_stop=ktr_initiation_points-1;

	// pservo

	if (length_servo_mode==SL_CONTROL)
	{
		for (i=1;i<=data_points;i++)
		{
			if ((i>sl_start)&&(i<=sl_stop))
			{
				pipe_servo[i]=1.0;
			}
			else
			{
				pipe_servo[i]=0.0;
			}
		}
	}
	else
	{
		for (i=1;i<=data_points;i++)
		{
			pipe_servo[i]=0.0;
		}
	}

	// poffset marker

	for (i=1;i<=data_points;i++)
	{
		if (i<=pre_servo_points)
		{
			pipe_offset[i]=1.0;
		}
		else
		{
			pipe_offset[i]=0.0;
		}
	}

	// pfl

	// Pre waveform

	::generate_waveform(CONSTANT,pipe_fl,1,length_start,1,0);

	// Waveform - pre-processing

	amplitude=(float)::convert_volts_to_DAPL_units(
						::convert_FL_COMMAND_microns_to_volts(length_size_microns));

	position_offset=(float)::convert_volts_to_DAPL_units(
			::convert_FL_COMMAND_microns_to_volts(length_offset_microns));

	begin_position_increment=((float)(2.0*PI)/((float)convert_time_ms_to_sampling_points(begin_period_ms)-(float)1.0));

	mid_position_increment=((float)(2.0*PI)/((float)convert_time_ms_to_sampling_points(mid_period_ms)-(float)1.0));

	end_position_increment=((float)(2.0*PI)/((float)convert_time_ms_to_sampling_points(end_period_ms)-(float)1.0));

	first_phase_points=convert_time_ms_to_sampling_points(chirp_mid_position*length_duration_ms);
	
	second_phase_points=convert_time_ms_to_sampling_points(length_duration_ms)-first_phase_points;

	// Calculate phase increments

	if (chirp_sweep_mode==LOGARITHMIC_CHIRP)
	{
		for (i=length_start;i<=(length_start+first_phase_points);i++)
		{
			denominator=((float)first_phase_points/(float)(log10(mid_position_increment/begin_position_increment)));

			increment[i]=begin_position_increment*(float)pow((float)10.0,((float)(i-length_start))/denominator);
		}

		for (i=(length_start+first_phase_points+1);i<=(length_start+first_phase_points+second_phase_points);i++)
		{
			denominator=((float)second_phase_points/(float)(log10(end_position_increment/mid_position_increment)));

			increment[i]=mid_position_increment*(float)pow((float)10.0,((float)(i-(length_start+first_phase_points+1)))/denominator);
		}
	}
	else															// Linear sweep
	{
		for (i=length_start;i<=(length_start+first_phase_points);i++)
		{
			increment[i]=begin_position_increment+
				(float)(i-length_start)*(mid_position_increment-begin_position_increment)/((float)first_phase_points-(float)1.0);
		}

		for (i=(length_start+first_phase_points+1);i<=(length_start+first_phase_points+second_phase_points);i++)
		{
			increment[i]=mid_position_increment+
				(float)(i-(length_start+first_phase_points+1))*(end_position_increment-mid_position_increment)/((float)second_phase_points-(float)1.0);
		}
	}

	// Generate length segment for pipe_fl

	position=0.0;

	if (length_function_string=="Sine")
	{
		for (i=length_start;i<=length_stop;i++)
		{
			pipe_fl[i]=(float)(amplitude*sin(position))+position_offset;
			position=position+increment[i];
		}
	}

	if (length_function_string=="Cosine")
	{
		for (i=length_start;i<=length_stop;i++)
		{
			pipe_fl[i]=(float)(amplitude*cos(position))+position_offset;
			position=position+increment[i];
		}
	}

	if (length_function_string=="Triangle")
	{
		for (i=length_start;i<=length_stop;i++)
		{
			pipe_fl[i]=(amplitude*return_triangle(position))+position_offset;
			position=position+increment[i];
		}
	}

	if (length_function_string=="Square")
	{
		for (i=length_start;i<=length_stop;i++)
		{
			pipe_fl[i]=(amplitude*return_square(position))+position_offset;
			position=position+increment[i];
		}
	}

	// Between length stop and ktr

	::generate_waveform(CONSTANT,pipe_fl,length_stop+1,ktr_initiation_points,1,0);

	// ktr

	::generate_waveform(CONSTANT,pipe_fl,ktr_initiation_points,
		(ktr_initiation_points+convert_time_ms_to_sampling_points(ktr_step_duration_ms)),1,
		(float)::convert_volts_to_DAPL_units(
		::convert_FL_COMMAND_microns_to_volts(ktr_step_size_microns)));

	// Post ktr

	::generate_waveform(CONSTANT,pipe_fl,
		(ktr_initiation_points+convert_time_ms_to_sampling_points(ktr_step_duration_ms))+1,data_points,
		1,0);

	// ptarget

	// Pre waveform

	::generate_waveform(CONSTANT,pipe_target,1,length_start,1,0);

	// Reuse pfl phase increments but calculate new amplitude and offset

	amplitude=(float)::convert_volts_to_DAPL_units(length_size_microns*SL_volts_to_FL_microns_calibration);

	position_offset=(float)::convert_volts_to_DAPL_units(length_offset_microns*
									SL_volts_to_FL_microns_calibration);

	// Calculate length segment

	position=0.0;

	if (length_function_string=="Sine")
	{
		for (i=length_start;i<=length_stop;i++)
		{
			pipe_target[i]=(float)(amplitude*sin(position))+position_offset;
			position=position+increment[i];
		}
	}

	if (length_function_string=="Cosine")
	{
		for (i=length_start;i<=length_stop;i++)
		{
			pipe_target[i]=(float)(amplitude*cos(position))+position_offset;
			position=position+increment[i];
		}
	}

	if (length_function_string=="Triangle")
	{
		for (i=length_start;i<=length_stop;i++)
		{
			pipe_target[i]=(amplitude*return_triangle(position))+position_offset;
			position=position+increment[i];
		}
	}

	if (length_function_string=="Square")
	{
		for (i=length_start;i<=length_stop;i++)
		{
			pipe_target[i]=(amplitude*return_square(position))+position_offset;
			position=position+increment[i];
		}
	}

	// Between length stop and end of record

	::generate_waveform(CONSTANT,pipe_target,length_stop,data_points,1,0);

	// Tidy up

	delete [] increment;
}

float Chirp::return_triangle(float position)
{
	// Code returns y-value for a triangular waveform of peak-height 1.0 repeating with period of 2*PI

	// Variables

	float remainder;

	// Code

	remainder=(float)(fmod((float)position,(float)(2.0*PI))/(2.0*PI));

	if (remainder<=0.25)
	{
		return remainder*(float)4.0;
	}

	if ((remainder>0.25)&&(remainder<0.75))
	{
		return (float)(1.0)-((float)4.0*(remainder-(float)0.25));
	}

	if (remainder>=0.75)
	{
		return (float)-1.0+((float)4.0*(remainder-(float)0.75));
	}

	return 0.0;
}

float Chirp::return_square(float position)
{
	// Code returns y_value for a square-wave repeating with period 2.0*PI

	// Variables

	float remainder;

	// Code

	remainder=(float)(fmod((float)position,(float)(2.0*PI))/(2.0*PI));

	if (remainder<=0.5)
	{
		return 1.0;
	}
	else
	{
		return -1.0;
	}
}

// Message Handlers

void Chirp::OnSelchangeSamplingRate() 
{
	// Controls for user changing sampling rate

	set_sampling_parameters();

	set_post_ktr_ms();
}

void Chirp::OnChangeDataPoints() 
{
	// Controls for user changing data_points

	UpdateData(TRUE);

	set_sampling_parameters();

	set_post_ktr_ms();
}

void Chirp::OnSelchangeChirpSweep() 
{
	// Updates chirp_sweep_mode when user changes options

	if (CHIRP_SWEEP_CONTROL.GetCurSel()==LOGARITHMIC_CHIRP)
	{
		chirp_sweep_mode=LOGARITHMIC_CHIRP;
	}
	else
	{
		chirp_sweep_mode=LINEAR_CHIRP;
	}
}

void Chirp::OnSLtoFLCalibration() 
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

void Chirp::OnSelchangeServoMode() 
{
	// Enables/disables servo controls as appropriate and sets length_servo_mode

	// Code

	if (LENGTH_SERVO_MODE_CONTROL.GetCurSel()==FL_CONTROL)
	{
		// Length Servo mode is FL_Control

		length_servo_mode=FL_CONTROL;

		GetDlgItem(IDC_PRE_SERVO_POINTS)->EnableWindow(FALSE);
		GetDlgItem(IDC_CLAMP_MODE)->EnableWindow(FALSE);
		GetDlgItem(IDC_PROPORTIONAL_GAIN)->EnableWindow(FALSE);
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

void Chirp::OnChangeKtrStepDuration() 
{
	UpdateData(TRUE);
	
	set_post_ktr_ms();
}

void Chirp::OnChangePreKtr() 
{
	UpdateData(TRUE);
	
	set_post_ktr_ms();
}

void Chirp::OnChangeLengthDuration() 
{
	UpdateData(TRUE);
	
	set_post_ktr_ms();
}

void Chirp::OnChangePreLength() 
{
	UpdateData(TRUE);
	
	set_post_ktr_ms();
}

// DAPL source file functions

int Chirp::create_DAPL_source_file(void)
{
	// Creates DAPL_source_file by combining 5 sub-files

	// Variables

	int i;

	int no_of_sub_files;

	char single_character;
	char display_string[200];

	// File pointers and array

	FILE* file_pointer[6];

	FILE* DAPL_source_file=fopen(DAPL_source_file_string,"w");

	// Code

	// Sets overflow_status

	if (data_points<=overflow_threshold)
	{
		overflow_status=0;
	}
	else
	{
		overflow_status=1;
	}

	// Create pipes

	create_pipes();

	// Now write files

	if ((write_header_file()*write_fill_pipes_file()*write_create_pipes_file()*
		write_control_file()*write_io_procedures_file()*write_overflow_file())==1)
	{
		// Sub-files written successfully - merge files

		if (overflow_status==0)
		{
			no_of_sub_files=5;
		}	
		else
		{
			no_of_sub_files=6;
		}

		// Check files

		if (DAPL_source_file==NULL)
		{
			sprintf(display_string,"%s could not be opened",DAPL_source_file_string);
			MessageBox(display_string,"Cyclical::create_DAPL_source_file");
			return(0);
		}

		FILE* header_file=fopen(header_file_string,"r");
		FILE* fill_pipes_file=fopen(fill_pipes_file_string,"r");
		FILE* create_pipes_file=fopen(create_pipes_file_string,"r");
		FILE* control_file=fopen(control_file_string,"r");
		FILE* io_procedures_file=fopen(io_procedures_file_string,"r");
		FILE* overflow_file=fopen(overflow_file_string,"r");

		file_pointer[0]=header_file;
		file_pointer[1]=fill_pipes_file;
		file_pointer[2]=create_pipes_file;
		file_pointer[3]=control_file;
		file_pointer[4]=io_procedures_file;
		file_pointer[5]=overflow_file;

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
		MessageBox(display_string,"Chirp::create_DAPL_source_file");
		return(0);
	}
}

int Chirp::write_header_file(void)
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

		fprintf(header_file,"OPTION SYSINECHO=OFF\n");

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

int Chirp::write_fill_pipes_file(void)
{
	// Code writes file

	// Variables

	char display_string[200];												// user display

	int fill_level;															// number of points to 
																			// put in fill commands in
																			// this level - remainder would
																			// go to overflow file if
																			// necessary

	//Code

	// File handling

	FILE* fill_pipes_file;

	fill_pipes_file=fopen(fill_pipes_file_string,"w");

	if (fill_pipes_file==NULL)
	{
		sprintf(display_string,"Fill_pipes_file\n%s\ncould not be opened",fill_pipes_file_string);
		MessageBox(display_string,"Chirp::write_fill_pipes_file");
		return 0;
	}
	else
	{
		if (data_points<=overflow_threshold)
		{
			fill_level=data_points;
		}
		else
		{
			fill_level=overflow_threshold;
		}

		// File output

		fprintf(fill_pipes_file,"\nfill ptrigger %i\n\n",convert_volts_to_DAPL_units((float)4.95));
																			// default high trigger

		fprintf(fill_pipes_file,"fill plastout 0\n");						// initial values
		fprintf(fill_pipes_file,"fill plastoffset 0\n\n");

		write_pipe_to_file(fill_pipes_file,"pservo",pipe_servo,fill_level);
		write_pipe_to_file(fill_pipes_file,"poffset_marker",pipe_offset,fill_level);
		write_pipe_to_file(fill_pipes_file,"pfl",pipe_fl,fill_level);
		write_pipe_to_file(fill_pipes_file,"ptarget",pipe_target,fill_level);
	}

	fclose(fill_pipes_file);

	return 1;
}

int Chirp::write_create_pipes_file(void)
{
	// Code writes file

	// Variables

	char display_string[200];

	FILE* create_pipes_file;

	// Code

	create_pipes_file=fopen(create_pipes_file_string,"w");

	if (create_pipes_file==NULL)
	{
		sprintf(display_string,"Create_pipes_file\n%s\ncould not be opened\n",create_pipes_file_string);
		MessageBox(display_string,"Chirp::write_create_pipes_file");
		return 0;
	}
	else
	{
		fprintf(create_pipes_file,"\npdef create_pipes\n\n");

		fprintf(create_pipes_file,"  ptrigger=0\n\n");								// set trigger low
		
		fprintf(create_pipes_file,"end\n\n");

		fprintf(create_pipes_file,";************************************************************\n\n");
	}

	fclose(create_pipes_file);

	return 1;
}

int Chirp::write_control_file(void)
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
		MessageBox(display_string,"Chirp::write_control_file");
		return(0);
	}
	else
	{
		fprintf(control_file,"\npdef control\n");

		if (sampling_rate<=5000)
		{
			fprintf(control_file,"  poffset=((1-poffset_marker)*plastoffset)+(poffset_marker*ip1)\n");
			fprintf(control_file,"  plastoffset=poffset\n\n");

			fprintf(control_file,"  pfeedback=plastout-(%.0f*(ip1-ptarget-((1-zeroclamp)*poffset))/10000)\n",
				1.0e4*proportional_gain/SL_volts_to_FL_command_volts);
			fprintf(control_file,"  pout=((1-pservo)*pfl)+(pservo*pfeedback)\n\n");
		}
		else
		{
			sprintf(display_string,"Open loop control only");
			display_status(TRUE,display_string);
			
			fprintf(control_file,"  pout=pfl\n");
		}

		fprintf(control_file,"  copy(pout,plastout,op1)\n");
		fprintf(control_file,"  copy(ptrigger,op0)\n\n");

		fprintf(control_file,"  merge(ip0,ip1,ip2,ip3,$binout)\n");
		fprintf(control_file,"end\n\n");

		fprintf(control_file,";************************************************************\n\n");
	}
	
	fclose(control_file);

	return 1;
}

int Chirp::write_io_procedures_file(void)
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

		fprintf(io_procedures_file,"start create_pipes,control,inputproc,outputproc\n\n");
	}

	fclose(io_procedures_file);

	return(1);
}

int Chirp::write_overflow_file(void)
{
	// Code writes overflow points as required

	// Variables

	int i;

	int fill_width=20;

	char display_string[200];

	// File handling

	if (overflow_status==1)
	{
		FILE* overflow_file;

		overflow_file=fopen(overflow_file_string,"w");

		if (overflow_file==NULL)
		{
			sprintf(display_string,"Overflow_file\n%s\ncould not be opened",overflow_file_string);
			MessageBox(display_string,"Chirp::write_overflow_file");
			return 0;
		}
		else
		{
			fprintf(overflow_file,"\n\n");	

			for (i=overflow_threshold+1;i<=data_points;i=i+fill_width)
			{
				::write_pipe_segment_to_file(overflow_file,"pservo",pipe_servo,i,i+fill_width-1);

				::write_pipe_segment_to_file(overflow_file,"poffset_marker",pipe_offset,i,i+fill_width-1);

				::write_pipe_segment_to_file(overflow_file,"pfl",pipe_fl,i,i+fill_width-1);

				::write_pipe_segment_to_file(overflow_file,"ptarget",pipe_target,i,i+fill_width-1);

				fprintf(overflow_file,"\n");
			}

			fprintf(overflow_file,"\n\n");
		}

		fclose(overflow_file);
	}

	return 1;
}


