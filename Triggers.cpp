// Triggers.cpp : implementation file
//

#include "stdafx.h"
#include "SLControl.h"
#include "Triggers.h"
#include "SLControlDlg.h"

#include "Channel_control.h"
#include "dapio32.h"
#include "Calibration.h"
#include "global_functions.h"
#include "global_definitions.h"
#include <math.h>

extern float FL_POLARITY;
extern float FL_RESPONSE_CALIBRATION;
extern float FL_COMMAND_CALIBRATION;
extern float FORCE_CALIBRATION;
extern char CALIBRATION_DATE_STRING[100];
extern int FL_OR_SL_CHANNEL_MODE;
extern char SLCONTROL_VERSION_STRING[100];

// Triggers dialog

IMPLEMENT_DYNAMIC(Triggers, CDialog)
Triggers::Triggers(CWnd* pParent /*=NULL*/)
	: CDialog(Triggers::IDD, pParent)
{
	m_pParent=pParent;
	m_nID=Triggers::IDD;

	// Variables internal to constructor

	char* p_string;
	char temp_string[_MAX_PATH];

	// Create pointer to the data record

	p_triggers_record =& triggers_record;

	// Null daughter window pointers

	p_Calibration=NULL;
	p_Experiment_display=NULL;

	// Default values
    
	sampling_rate_string="1000";
	sampling_rate=(float)1000.0;
	data_points=2000;
	time_increment=((float)(1.0/strtod(sampling_rate_string,&p_string)));
	duration_seconds=data_points*time_increment;
	sprintf(temp_string,"%.3f",duration_seconds);
	duration_string=temp_string;

	// ktr parameters

	ktr_initiation_ms=1000.0;
	ktr_step_duration_ms=20.0;
	ktr_step_size_microns=-20.0;
	post_ktr_ms=(duration_seconds*(float)1000.0)-(ktr_initiation_ms+ktr_step_duration_ms);

	// Filing parameters

	data_path_string="c:\\temp";
	data_base_string="test";
	data_index=1;

	// Status strings and counter

	status_middle_string="1) Triggers dialog initialising";
	status_bottom_string="2) Preparing for trials";
	status_counter=3;

	// Set Trigger_dir_path

	set_executable_directory_string(temp_string);
	strcat(temp_string,"\\Triggers\\");

	Triggers_dir_path=temp_string;

	// Set DAPL source file strings

	header_file_string=Triggers_dir_path+"Trigger_header.txt";
	fill_pipes_file_string=Triggers_dir_path+"Trigger_fill_pipes.txt";
	control_file_string=Triggers_dir_path+"Trigger_control.txt";
	io_procedures_file_string=Triggers_dir_path+"Trigger_io_procs.txt";
	DAPL_source_file_string=Triggers_dir_path+"dapl_source.txt";

	// Experiment_status

	previous_experiment_status=0;
	experiment_status=0;

	for (int i=0;i<15;i++)
		channel_status[i]=0;

	SINGLE_SHOT_OVER_RUN=1;
	SINGLE_SHOT_SHORT_PERIODS=2;
	TRAIN_OFF_GT_ON=3;
	TRAIN_OFF_GT_DURATION=4;
	TRAIN_CYCLE_TOO_HIGH=5;
	TRAIN_CYCLE_TOO_LOW=6;
	FREEFORM_POINTS_MISMATCH=7;
	FREEFORM_FILE_PROBLEM=8;
	POST_KTR_NEGATIVE=9;
	TRIGGERS_INVALID=10;
}

Triggers::~Triggers()
{

}

BOOL Triggers::Create()
{
	return CDialog::Create(m_nID, m_pParent);
}

void Triggers::PostNcDestroy() 
{
	delete this;
}


void Triggers::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHANNEL_TAB_CONTROL, CHANNEL_TAB_CONTROL);
	//DDX_Control(pDX, IDC_CHANNEL_TAB_CONTROL, CHANNEL_TAB_CONTROL);
	DDX_Text(pDX, IDC_DATA_POINTS, data_points);
	DDX_Text(pDX, IDC_DATA_PATH, data_path_string);
	DDX_Text(pDX, IDC_DATA_BASE, data_base_string);
	DDX_Text(pDX, IDC_DATA_INDEX, data_index);
	DDX_Control(pDX, IDC_SAMPLING_RATE, SAMPLING_RATE_CONTROL);
	DDX_CBString(pDX, IDC_SAMPLING_RATE, sampling_rate_string);
	DDX_Text(pDX, IDC_DURATION, duration_string);
	DDX_Text(pDX, IDC_KTR_INITIATION, ktr_initiation_ms);
	DDX_Text(pDX, IDC_POST_KTR, post_ktr_ms);
	DDX_Text(pDX, IDC_KTR_STEP_DURATION, ktr_step_duration_ms);
	DDX_Text(pDX, IDC_KTR_STEP_SIZE, ktr_step_size_microns);
}


BEGIN_MESSAGE_MAP(Triggers, CDialog)
	ON_BN_CLICKED(IDC_Run_Experiment, OnBnClickedRunExperiment)
	ON_WM_PAINT()
	ON_EN_CHANGE(IDC_DATA_POINTS, OnEnChangeDataPoints)
	ON_CBN_SELCHANGE(IDC_SAMPLING_RATE, OnCbnSelchangeSamplingRate)
	ON_EN_CHANGE(IDC_KTR_INITIATION, OnEnChangeKtrInitiation)
	ON_EN_CHANGE(IDC_KTR_STEP_DURATION, OnEnChangeKtrStepDuration)
END_MESSAGE_MAP()


// Triggers message handlers

BOOL Triggers::OnInitDialog() 
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

	GetDlgItem(IDC_DURATION)->EnableWindow(FALSE);
	GetDlgItem(IDC_POST_KTR)->EnableWindow(FALSE);

	// Update status

	display_status(TRUE,"Ready for input");

	// Create the Calibration window

	p_Calibration = new Calibration();
	p_Calibration->Create();

	// Create Experiment_display window

	p_Experiment_display = new Experiment_display(this,p_triggers_record);
	p_Experiment_display->Create();

	// Create Channel control

	p_channel_control =& CHANNEL_TAB_CONTROL;

	p_channel_control->InsertItem(0, _T("1"));
	p_channel_control->InsertItem(1, _T("2"));
	p_channel_control->InsertItem(2, _T("3"));
	p_channel_control->InsertItem(3, _T("4"));
	p_channel_control->InsertItem(4, _T("5"));
	p_channel_control->InsertItem(5, _T("6"));
	p_channel_control->InsertItem(6, _T("7"));
	p_channel_control->InsertItem(7, _T("8"));
	p_channel_control->InsertItem(8, _T("9"));
	p_channel_control->InsertItem(9, _T("10"));
	p_channel_control->InsertItem(10, _T("11"));
	p_channel_control->InsertItem(11, _T("12"));
	p_channel_control->InsertItem(12, _T("13"));
	p_channel_control->InsertItem(13, _T("14"));
	p_channel_control->InsertItem(14, _T("15"));
	p_channel_control->InsertItem(15, _T("16"));
	
	p_channel_control->p_triggers=this;
	p_channel_control->Init();

	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void Triggers::OnClose()
{
	// Close derived windows

	p_Calibration->close_calibration_window();
	p_Calibration=NULL;

	p_Experiment_display->close_experiment_display_window();
	p_Experiment_display=NULL;

	// Signal to parent that this window is closing

	((CSLControlDlg*)m_pParent)->Triggers_Window_Done();

	DestroyWindow();
}

BOOL Triggers::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class

	// Traps escape key presses to prevent dialog box closing

	if (pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE)
	{
		return TRUE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void Triggers::OnCancel()
{
	OnClose();
}

void Triggers::remote_shut_down(void)
{
	// Called by parent to shut down window

	OnClose();
}

// Run experiment

void Triggers::OnBnClickedRunExperiment()
{
	// Code runs a single experiment

	// Variables

	int i,holder;

	short short_int=1;									// a short int for sizeof calculation

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

	// Check parameters are valid, proceed if appropriate, abort if not

	if ((experiment_status==0)&&(create_DAPL_source_file()))
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
			MessageBox(display_string,"Triggers::OnBinClickedRunExperiment");

			sprintf(display_string,"Upload to Dap failed");
			display_status(TRUE,display_string);

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
			p_triggers_record->data[TIME][holder]=((float)(holder-1)*time_increment);
			p_triggers_record->data[FORCE][holder]=convert_DAPL_units_to_volts(DAPL_data[i]);
			p_triggers_record->data[SL][holder]=convert_DAPL_units_to_volts(DAPL_data[i+1]);
			p_triggers_record->data[FL][holder]=convert_DAPL_units_to_volts(DAPL_data[i+2]);
			p_triggers_record->data[INTENSITY][holder]=convert_DAPL_units_to_volts(DAPL_data[i+3]);

			holder++;
		}

		p_triggers_record->no_of_points=data_points;

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
				p_triggers_record->no_of_points;
		}

		p_Experiment_display->update_plots();

		// Write data file

		write_data_to_file();
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

void Triggers::write_data_to_file(void)
{
	// Code writes data to file

	// Variables

	int i,j;

	char index_string[20];
	char display_string[FILE_INFO_STRING_LENGTH];
	char file_info_string[FILE_INFO_STRING_LENGTH];

	float SL_volts_to_FL_command_volts;										// extracted from Calibration window - not part of Trigger class
																			// because Trigger doesn't update under SL control

	// Code

	// Create file string

	sprintf(index_string,"%i",data_index);
	output_data_file_string=data_path_string+"\\"+data_base_string+index_string+".slc";

	// File handling

	FILE* output_data_file=fopen(output_data_file_string,"w");

	if (output_data_file==NULL)
	{
		sprintf(display_string,"Cannot open file\n%s\nData can not be saved to disk",
			output_data_file_string);
		MessageBox(display_string,"Triggers::write_data_to_file()",MB_ICONWARNING);
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

		// Retrieve data from Calibration window to calculate SL_volts_to_FL_command_volts
	
		p_Calibration->UpdateData(TRUE);
		SL_volts_to_FL_command_volts=
			(FL_POLARITY*(p_Calibration->Calibration_SL_volts_to_FL_microns_calibration)*FL_COMMAND_CALIBRATION);
		fprintf(output_data_file,"SL_volts_to_FL_COMMAND_volts: %g\n\n",SL_volts_to_FL_command_volts);

		// Dimensions

		p_Calibration->UpdateData(TRUE);

		fprintf(output_data_file,"_MUSCLE_DIMENSIONS_\n");
		fprintf(output_data_file,"Muscle_length_(µm): %.3e\n",p_Calibration->Calibration_fibre_length);
		fprintf(output_data_file,"Sarcomere_length_(nm): %.3e\n",p_Calibration->Calibration_sarcomere_length);
		fprintf(output_data_file,"Area_(m^2): %.3e\n\n",p_Calibration->Calibration_area);

		// Experiment parameters

		fprintf(output_data_file,"_TRIGGERS_\n");
		fprintf(output_data_file,"sampling_rate_(Hz): %.3e\n",sampling_rate);
		fprintf(output_data_file,"data_points: %i\n",data_points);
		fprintf(output_data_file,"ktr_step_(µm): %.3e\n",ktr_step_size_microns);
		fprintf(output_data_file,"ktr_step_duration_(ms): %.3e\n",ktr_step_duration_ms);
		fprintf(output_data_file,"ktr_initiation_time_(ms): %.3e\n\n",ktr_initiation_ms);

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

		// Data

		fprintf(output_data_file,"\nData\n");

		for (i=1;i<=p_triggers_record->no_of_points;i++)
		{
			fprintf(output_data_file,"%f\t%f\t%f\t%f\t%f\n",
				p_triggers_record->data[TIME][i],
				p_triggers_record->data[FORCE][i],
				p_triggers_record->data[SL][i],
				p_triggers_record->data[FL][i],
				p_triggers_record->data[INTENSITY][i]);
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

// Display functions

void Triggers::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	// Redraw status message

	display_status(FALSE,"");
}

void Triggers::display_status(int update_mode,CString new_string)
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

// Message Handlers

void Triggers::OnEnChangeDataPoints()
{
	UpdateData(TRUE);

	// Error checking

	if (data_points>65000)
	{
		char display_string[200];
		sprintf(display_string,"Max data points is 65000");
		MessageBox(display_string);

		data_points=65000;
		UpdateData(FALSE);
	}

	set_sampling_parameters();

	set_post_ktr_ms();

	p_channel_control->update_tab_calculations();
}

void Triggers::OnCbnSelchangeSamplingRate()
{
	// Controls for user changing sampling rate

	set_sampling_parameters();

	set_post_ktr_ms();

	p_channel_control->update_tab_calculations();
}

void Triggers::OnEnChangeKtrInitiation()
{
	// Controls for user changing ktr initiation time

	UpdateData(TRUE);

	set_post_ktr_ms();

	set_parameters_valid();
}

void Triggers::OnEnChangeKtrStepDuration()
{
	// Controls for user changing ktr step duration

	UpdateData(TRUE);

	set_post_ktr_ms();

	set_parameters_valid();
}

// Internal Processing functions

void Triggers::set_sampling_parameters(void)
{
	// Code sets sampling rate, time_increment and duration_seconds
		
	// Variables

	char* p_string;
	CString value_string;
	char display_string[20];

	// Code

	SAMPLING_RATE_CONTROL.GetLBText(SAMPLING_RATE_CONTROL.GetCurSel(),value_string);
	sampling_rate=(float)strtod(value_string,&p_string);
	time_increment=(float)1.0/sampling_rate;
	duration_seconds=(float)data_points*time_increment;

	// Update display

	sprintf(display_string,"%.3f",duration_seconds);
	GetDlgItem(IDC_DURATION)->SetWindowText(display_string);
}

int Triggers::convert_time_ms_to_sampling_points(float time_ms)
{
	// Returns no of sample points in a given time interval (expressed in ms)

	// Variables

	int points;

	// Code

	points=(int)((time_ms/1000.0)*sampling_rate);

	return points;
}

void Triggers::set_post_ktr_ms(void)
{
	// Code sets post_ktr_ms

	char display_string[200];

	post_ktr_ms=(float)1000.0*duration_seconds-(ktr_initiation_ms+ktr_step_duration_ms);

	// Update display

	sprintf(display_string,"%.1f",post_ktr_ms);
	GetDlgItem(IDC_POST_KTR)->SetWindowText(display_string);
}

int Triggers::set_parameters_valid(void)
{
	// Code sets parameters_valid if experiment seems plausible

	int i;
	int status=0;

	char display_string[100];

	// Code

	for (i=0;i<p_channel_control->no_of_tab_pages;i++)
	{
		if (p_channel_control->tab_pages[i]->channel_active==TRUE)
		{
			// Radio control
	
			if (p_channel_control->tab_pages[i]->RADIO_CONTROL==SINGLE_SHOT)
			{
				int flip_point;
				int flop_point;

				flip_point=(int)ceil(((p_channel_control->tab_pages[i]->channel_flip/1000.0)/time_increment))+1;
				flop_point=flip_point+(int)ceil(((p_channel_control->tab_pages[i]->channel_duration/1000.0)/time_increment));
				
				if (flop_point>data_points)
				{
					status++;
					
					if (channel_status[i]!=SINGLE_SHOT_OVER_RUN)
					{
                        sprintf(display_string,"Single_shot over-run - Channel %i",i+1);
						display_status(TRUE,display_string);
						MessageBeep(MB_ICONEXCLAMATION);

						channel_status[i]=SINGLE_SHOT_OVER_RUN;
					}
				}

				if (fmod(((p_channel_control->tab_pages[i]->channel_duration/1000.0)/time_increment),1)<0.5)
				{
					status++;
					
					if (channel_status[i]!=SINGLE_SHOT_SHORT_PERIODS)
					{
                        sprintf(display_string,"Single_shot timing intervals are too short - Channel %i",i+1);
						display_status(TRUE,display_string);
						MessageBeep(MB_ICONEXCLAMATION);

						channel_status[i]=SINGLE_SHOT_SHORT_PERIODS;
					}
				}
			}

			// Train

			if (p_channel_control->tab_pages[i]->RADIO_CONTROL==TRAIN)
			{
				if ((p_channel_control->tab_pages[i]->train_gate_on)>=
					(p_channel_control->tab_pages[i]->train_gate_off))
				{
					status++;

					if (channel_status[i]!=TRAIN_OFF_GT_ON)
					{
						sprintf(display_string,"Train gate on > gate off - Channel %i",i+1);
						display_status(TRUE,display_string);
						MessageBeep(MB_ICONEXCLAMATION);

						channel_status[i]=TRAIN_OFF_GT_ON;
					}
				}

				if ((p_channel_control->tab_pages[i]->train_gate_off)>=duration_seconds)
				{
					status++;
					
					if (channel_status[i]!=TRAIN_OFF_GT_DURATION)
					{
						sprintf(display_string,"Train gate off > record duration - Channel %i",i+1);
						display_status(TRUE,display_string);
						MessageBeep(MB_ICONEXCLAMATION);

						channel_status[i]=TRAIN_OFF_GT_DURATION;
					}
				}

				if (((1.0-p_channel_control->tab_pages[i]->train_duty_cycle)*
						(1.0/p_channel_control->tab_pages[i]->train_frequency))<time_increment)
				{
					status++;
					
					if (channel_status[i]!=TRAIN_CYCLE_TOO_HIGH)
					{
						sprintf(display_string,"Duty cycle too high - Channel %i",i+1);
						display_status(TRUE,display_string);
						MessageBeep(MB_ICONEXCLAMATION);
						
						channel_status[i]=TRAIN_CYCLE_TOO_HIGH;
					}
				}

				if ((p_channel_control->tab_pages[i]->train_duty_cycle)*
						(1.0/p_channel_control->tab_pages[i]->train_frequency)<time_increment)
				{
					status++;
					
					if (channel_status[i]!=TRAIN_CYCLE_TOO_LOW)
					{
						sprintf(display_string,"Duty cycle too low - Channel %i",i+1);
						display_status(TRUE,display_string);
						MessageBeep(MB_ICONEXCLAMATION);

						channel_status[i]=TRAIN_CYCLE_TOO_LOW;
					}
				}
			}

			// Free-form

			if (p_channel_control->tab_pages[i]->RADIO_CONTROL==FREEFORM)
			{
				if ((p_channel_control->tab_pages[i]->free_form_points)!=data_points)
				{
					status++;

					if (p_channel_control->tab_pages[i]->free_form_points>=0)
					{
						if ((channel_status[i]!=FREEFORM_POINTS_MISMATCH)||
							(p_channel_control->tab_pages[i]->free_form_points!=p_channel_control->tab_pages[i]->previous_free_form_points))
						{
							sprintf(display_string,"File Mismatch - File points: %i  Data points: %i Channel %i",
								p_channel_control->tab_pages[i]->free_form_points,
								data_points,i+1);
							display_status(TRUE,display_string);
							MessageBeep(MB_ICONEXCLAMATION);

							channel_status[i]=FREEFORM_POINTS_MISMATCH;
						}
					}
				}

				if ((p_channel_control->tab_pages[i]->free_form_points)<0)
				{
					status++;

					if (channel_status[i]!=FREEFORM_FILE_PROBLEM)
					{
						sprintf(display_string,"File handling problem - Channel %i",i+1);
						display_status(TRUE,display_string);
						MessageBeep(MB_ICONEXCLAMATION);

						channel_status[i]=FREEFORM_FILE_PROBLEM;
					}
				}
			}
		}
	}

	// post_ktr

	if (post_ktr_ms<=0)
	{
		status++;

		if (previous_experiment_status!=POST_KTR_NEGATIVE)
		{
			sprintf(display_string,"Post_ktr_ms < 0");
			display_status(TRUE,display_string);
			MessageBeep(MB_ICONEXCLAMATION);
		}
	}

	// Tidy up

	if ((previous_experiment_status>0)&&(status==0))
	{
		sprintf(display_string,"Experiment parameters now valid");
		display_status(TRUE,display_string);
		MessageBeep(0xFFFFFFFF);

		for (i=0;i<p_channel_control->no_of_tab_pages;i++)
			channel_status[i]=0;
	}

	if (status>0)
	{
		experiment_status=status;

		if (post_ktr_ms<=0)
			previous_experiment_status=POST_KTR_NEGATIVE;
		else
			previous_experiment_status=TRIGGERS_INVALID;
	}
	else
	{
		experiment_status=0;
        previous_experiment_status=0;
	}

	return status;
}

// File handling functions

int Triggers::create_DAPL_source_file(void)
{
	// Creates DAPL_source_file by combining 4 sub-files

	// Variables

	int i;

	int no_of_sub_files=4;

	char single_character;
	char display_string[200];

	// File pointers and array

	FILE* file_pointer[5];

	FILE* DAPL_source_file=fopen(DAPL_source_file_string,"w");

	// Code

	if (write_header_file()*write_fill_pipes_file()*write_control_file()*write_io_procedures_file()==1)
	{
		// Sub-files written successfully - merge files

		// Check files

		if (DAPL_source_file==NULL)
		{
			sprintf(display_string,"%s could not be opened",DAPL_source_file_string);
			MessageBox(display_string,"Triggers::create_DAPL_source_file");
			return 0;
		}

		FILE *header_file=fopen(header_file_string,"r");
		FILE* fill_pipes_file=fopen(fill_pipes_file_string,"r");
		FILE* control_file=fopen(control_file_string,"r");
		FILE* io_procedures_file=fopen(io_procedures_file_string,"r");

		file_pointer[0]=header_file;
		file_pointer[1]=fill_pipes_file;
		file_pointer[2]=control_file;
		file_pointer[3]=io_procedures_file;

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

		return 1;
	}
	else
	{
		sprintf(display_string,"Create_DAPL_source_file failed\nA component file was not created successfully");
		MessageBox(display_string,"Triggers::create_DAPL_source_file");
		return 0;
	}
}

int Triggers::write_header_file(void)
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
		// Error

		sprintf(display_string,"Header file\n%s\ncould not be opened\n",header_file_string);
		MessageBox(display_string,"Triggers::write_header_file()");
		return 0;
	}
	else
	{
		// Normal function

		fprintf(header_file,"RESET\n\n");

		fprintf(header_file,"OPTION SYSINECHO=OFF\n");
		fprintf(header_file,"OPTION UNDERFLOWQ=OFF\n");
		fprintf(header_file,"OPTION ERRORQ=OFF\n");
		fprintf(header_file,"OPTION SCHEDULING=FIXED\n");
		fprintf(header_file,"OPTION BUFFERING=OFF\n");
		fprintf(header_file,"OPTION QUANTUM=100\n");

		fprintf(header_file,";***********************************************************\n\n");

		fprintf(header_file,"// Header\n\n");
		
		fprintf(header_file,"pipes ptriggers MAXSIZE=65500\n");
		fprintf(header_file,"pipes pfl MAXSIZE=65500\n");
		
		fprintf(header_file,";***********************************************************\n\n");

		fprintf(header_file,"OUTPORT 0..15 TYPE=0\n\n");

		fprintf(header_file,";***********************************************************\n\n");
	}

	fclose(header_file);

	return 1;
}

int Triggers::write_fill_pipes_file(void)
{
	// Code writes fill_pipes_file

	// Variables

	int i,j;

	int ktr_initiation_points;										// integer data points at beginning of ktr step

	float * pipe_fl = new float [data_points+1];					// array used for file output
	float * pipe_triggers = new float [data_points+1];				// array used for digital triggers

	char display_string[200];

	FILE* fill_pipes_file;

	// Code

	// File handling

	fill_pipes_file=fopen(fill_pipes_file_string,"w");

	if (fill_pipes_file==NULL)
	{
		// Error

		sprintf(display_string,"Fill_pipes_file\n%s\ncould not be opened",fill_pipes_file_string);
		MessageBox(display_string,"Triggers::fill_pipes_file()");
		return 0;
	}
	else
	{
		// Normal code execution

		ktr_initiation_points=convert_time_ms_to_sampling_points(ktr_initiation_ms)+1;

		// pipe_fl

		::generate_waveform(CONSTANT,pipe_fl,1,ktr_initiation_points,1,0);

		::generate_waveform(CONSTANT,pipe_fl,ktr_initiation_points,
				(ktr_initiation_points+convert_time_ms_to_sampling_points(ktr_step_duration_ms)),1,
				(float)::convert_volts_to_DAPL_units(
					::convert_FL_COMMAND_microns_to_volts(ktr_step_size_microns)));

		::generate_waveform(CONSTANT,pipe_fl,
			(ktr_initiation_points+convert_time_ms_to_sampling_points(ktr_step_duration_ms)+1),data_points,1,0);

		write_pipe_to_file(fill_pipes_file,"pfl",pipe_fl,data_points);

		// pipe_triggers

		// First reset pipe

		for (i=1;i<=data_points;i++)
			pipe_triggers[i]=0;

		// Now fill pipe from tab_pages[]->trigger_array

		for (i=0;i<p_channel_control->no_of_tab_pages;i++)
		{
			if (p_channel_control->tab_pages[i]->channel_active==TRUE)
			{
				if (i==0)	// First channel
				{
					for (j=1;j<=data_points;j++)
						pipe_triggers[j]=p_channel_control->tab_pages[i]->trigger_array[j];
				}
				else
				{
					for (j=1;j<=data_points;j++)
						pipe_triggers[j]=pipe_triggers[j]+
								(float)pow(((float)2.0*p_channel_control->tab_pages[i]->trigger_array[j]),i);
				}
			}
		}
		
		write_pipe_to_file(fill_pipes_file,"ptriggers",pipe_triggers,data_points);

		fprintf(fill_pipes_file,";***********************************************************\n\n");
	}

	// Tidy up

	fclose(fill_pipes_file);

	delete [] pipe_fl;
	delete [] pipe_triggers;

	return 1;
}

int Triggers::write_control_file(void)
{
	// Code creates control file

	// Variables

	char display_string[200];
	FILE* control_file;

	// Code

	// File handling

	control_file=fopen(control_file_string,"w");

	if (control_file==NULL)
	{
		// Error

		sprintf(display_string,"Control file\n%s\ncould not be opened\n",control_file_string);
		MessageBox(display_string,"Triggers::write_control_file()");
		return 0;
	}
	else
	{
		// Normal function

		fprintf(control_file,"\npdef control\n");

		fprintf(control_file,"  op0=pfl\n");
		fprintf(control_file,"  op1=ptriggers\n");
		
		fprintf(control_file,"  merge(ip0,ip1,ip2,ip3,$binout)\n");
		fprintf(control_file,"end\n\n");

		fprintf(control_file,";***********************************************************\n\n");
	}

	fclose(control_file);

	return 1;
}

int Triggers::write_io_procedures_file(void)
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
		MessageBox(display_string,"Triggers::write_io_procedures_file");
		return(0);
	}
	else
	{
		// Output Proc

		fprintf(io_procedures_file,"\nodef outputproc 2\n");
		fprintf(io_procedures_file,"  outputwait 2\n");
		fprintf(io_procedures_file,"  update burst\n");
		fprintf(io_procedures_file,"  set op0 a1\n");
		fprintf(io_procedures_file,"  set op1 b0\n");
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

		fprintf(io_procedures_file,"start control,inputproc,outputproc\n");
	}

	fclose(io_procedures_file);

	return 1;
}

void Triggers::append_parameters_to_file(FILE* parameter_file)
{
	// Appends current parameters to data file

	// Variables

	int i;

	// Code
	
	fprintf(parameter_file,"_TRIGGERS_\n\n");

	fprintf(parameter_file,"sampling_rate: %f\n",sampling_rate);
	fprintf(parameter_file,"data_points: %i\n",data_points);
	fprintf(parameter_file,"ktr_initiation_ms: %f\n",ktr_initiation_ms);
	fprintf(parameter_file,"ktr_step_duration_ms: %f\n",ktr_step_duration_ms);
	fprintf(parameter_file,"ktr_step_size_microns: %f\n",ktr_step_size_microns);
	fprintf(parameter_file,"data_path_string: %s\n",data_path_string);
	fprintf(parameter_file,"data_base_string: %s\n",data_base_string);
	fprintf(parameter_file,"data_index: %i\n\n",data_index);

	// Tab dialogs

	for (i=0;i<p_channel_control->no_of_tab_pages;i++)
	{
		if (p_channel_control->tab_pages[i]->channel_active)
		{
			fprintf(parameter_file,"Tab: %i\n",p_channel_control->tab_pages[i]->channel_number);
			fprintf(parameter_file,"Mode: %i\n",p_channel_control->tab_pages[i]->RADIO_CONTROL);

			switch (p_channel_control->tab_pages[i]->RADIO_CONTROL)
			{
				case 0:
				{
					fprintf(parameter_file,"Start_level: %i\n",p_channel_control->tab_pages[i]->start_level);
					fprintf(parameter_file,"Channel_flip: %f\n",p_channel_control->tab_pages[i]->channel_flip);
					fprintf(parameter_file,"Channel_duration: %f\n",p_channel_control->tab_pages[i]->channel_duration);
				}
				break;

				case 1:
				{
					fprintf(parameter_file,"Train_frequency: %f\n",p_channel_control->tab_pages[i]->train_frequency);
					fprintf(parameter_file,"Train_duty_cycle: %f\n",p_channel_control->tab_pages[i]->train_duty_cycle);
					fprintf(parameter_file,"Train_gate_on: %f\n",p_channel_control->tab_pages[i]->train_gate_on);
					fprintf(parameter_file,"Train_gate_off: %f\n",p_channel_control->tab_pages[i]->train_gate_off);
				}
				break;

				case 2:
				{
					fprintf(parameter_file,"Free_form_file: %s\n\n",p_channel_control->tab_pages[i]->free_form_file_string);
				}
				break;

				default:
				{
				}
				break;
			}

			fprintf(parameter_file,"\n");
		}
	}

	fprintf(parameter_file,"\n_END_TRIGGERS_\n");
}

void Triggers::load_parameters_from_file(FILE* parameter_file)
{
	// Loads parameters from data file

	// Variables

	int i;
	int tab,mode;

	char temp_string[100]="";
	char temp2_string[100]="";
	char end_string[]="_END_TRIGGERS_";

	// Code

	// Scan through the file looking for the Triggers tag

	rewind(parameter_file);

	sprintf(temp_string,"");

	while ((strcmp("_TRIGGERS_",temp_string)!=0)&&(strcmp(end_string,temp_string)!=0))
	{
		fscanf(parameter_file,"%s",&temp_string);
	}

	if (strcmp(end_string,temp_string)!=0)
	{
		// Successfully found tag, read values from file

		fscanf(parameter_file,"%s",&temp_string);
		fscanf(parameter_file,"%f",&sampling_rate);
		sprintf(temp_string,"%.0f",sampling_rate);
		sampling_rate_string=temp_string;

		fscanf(parameter_file,"%s",&temp_string);
		fscanf(parameter_file,"%i",&data_points);

		fscanf(parameter_file,"%s",&temp_string);
		fscanf(parameter_file,"%f",&ktr_initiation_ms);

		fscanf(parameter_file,"%s",&temp_string);
		fscanf(parameter_file,"%f",&ktr_step_duration_ms);

		fscanf(parameter_file,"%s",&temp_string);
		fscanf(parameter_file,"%f",&ktr_step_size_microns);

		fscanf(parameter_file,"%s",&temp_string);
		fscanf(parameter_file,"%s",&temp_string);
		data_path_string=temp_string;

		fscanf(parameter_file,"%s",&temp_string);
		fscanf(parameter_file,"%s",&temp_string);
		data_base_string=temp_string;

		fscanf(parameter_file,"%s",&temp_string);
		fscanf(parameter_file,"%i",&data_index);

		// First switch of all tabs

		for (i=0;i<p_channel_control->no_of_tab_pages;i++)
		{
			p_channel_control->tab_pages[i]->channel_active=FALSE;
			p_channel_control->tab_pages[i]->UpdateData(FALSE);
			p_channel_control->tab_pages[i]->OnBnClickedChannelActive();
		}

		// Now scan for active tabs and switch them on

		while (strcmp(end_string,temp_string)!=0)
		{
			fscanf(parameter_file,"%s",&temp_string);

			if (strcmp("Tab:",temp_string)==0)
			{
				// Found tab, read channel number, then mode

				fscanf(parameter_file,"%i",&tab);
				fscanf(parameter_file,"%s",&temp2_string);
				fscanf(parameter_file,"%i",&mode);

				p_channel_control->tab_pages[tab-1]->channel_active=TRUE;
				p_channel_control->tab_pages[tab-1]->RADIO_CONTROL=mode;

				switch (mode)
				{
					case 0:
					{
						fscanf(parameter_file,"%s",&temp2_string);

						fscanf(parameter_file,"%i",&i);
						if (i==0)
							p_channel_control->tab_pages[tab-1]->start_level_string="Low";
						else
							p_channel_control->tab_pages[tab-1]->start_level_string="High";

						fscanf(parameter_file,"%s",&temp2_string);
						fscanf(parameter_file,"%f",&p_channel_control->tab_pages[tab-1]->channel_flip);

						fscanf(parameter_file,"%s",&temp2_string);
						fscanf(parameter_file,"%f",&p_channel_control->tab_pages[tab-1]->channel_duration);
					}
					break;

					case 1:
					{
						fscanf(parameter_file,"%s",&temp2_string);
						fscanf(parameter_file,"%f",&p_channel_control->tab_pages[tab-1]->train_frequency);

						fscanf(parameter_file,"%s",&temp2_string);
						fscanf(parameter_file,"%f",&p_channel_control->tab_pages[tab-1]->train_duty_cycle);

						fscanf(parameter_file,"%s",&temp2_string);
						fscanf(parameter_file,"%f",&p_channel_control->tab_pages[tab-1]->train_gate_on);

						fscanf(parameter_file,"%s",&temp2_string);
						fscanf(parameter_file,"%f",&p_channel_control->tab_pages[tab-1]->train_gate_off);
					}
					break;

					case 2:
					{
						fscanf(parameter_file,"%s",&temp2_string);
						fscanf(parameter_file,"%s",&temp2_string);
						p_channel_control->tab_pages[tab-1]->free_form_file_string=temp2_string;
					}
					break;

					default:
					{
					}
					break;
				}

				p_channel_control->tab_pages[tab-1]->UpdateData(FALSE);
				p_channel_control->tab_pages[tab-1]->OnBnClickedChannelActive();
			}
		}

		UpdateData(FALSE);

		set_sampling_parameters();

		set_post_ktr_ms();

		set_parameters_valid();

		display_status(TRUE,"Trigger parameters successfully updated");
	}


}

