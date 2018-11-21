// Batch_analysis.cpp : implementation file
//

#include "stdafx.h"
#include "slcontrol.h"
#include "Batch_analysis.h"

#include "SLControlDlg.h"
#include "global_definitions.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Batch_analysis dialog


Batch_analysis::Batch_analysis(CWnd* pParent /*=NULL*/)
	: CDialog(Batch_analysis::IDD, pParent)
{
	m_pParent = pParent;										// set pointer to parent window
	m_nID = Batch_analysis::IDD;

	//{{AFX_DATA_INIT(Batch_analysis)
	list_file_string = _T("");
	output_file_string = _T("");
	ktr_skip_points = 0;
	fit_points = 0;
	stretch_number = 0;
	fv_last_pre_ktr_points = 0;
	//}}AFX_DATA_INIT

	// Set variables

	// Data record

	p_batch_record =& batch_record;

	// Display

	p_batch_display = NULL;

	// Experiment modes

	no_of_experiment_modes=4;
	
	experiment_mode_string[SIMPLE_KTR]="Simple ktr (Length control)";
	experiment_mode_string[STRETCH_LENGTH_CONTROL]="Ramps/Triangles (Length control)";
	experiment_mode_string[ISOTONIC_HOLD]="Isotonic hold (Tension control)";
	experiment_mode_string[STRETCH_TENSION_CONTROL]="Ramps/Triangles (Tension control)";

	// Analysis options

	analysis_option_string[ISOMETRIC_TENSION]="Isometric tension";
	analysis_option_string[KTR_RATE]="Ktr Rate";
	analysis_option_string[STIFFNESS]="Stiffness";
	analysis_option_string[SHORTENING_VELOCITY]="Shortening velocity";
	analysis_option_string[RELATIVE_PRE_STRETCH_TENSION]="Relative Pre-Stretch Tension";
	analysis_option_string[PEAK_TENSION]="Stretch peak tension";
	analysis_option_string[TAKE_OFF]="V_max take-off";

	// Combo box strings

	linear_regression_string="Linear Regression";
	robust_linear_string="Robust Linear";
	single_exponential_string="Single Exponential";
	double_exponential_string="Double Exponential";

	// File options

	list_file_string="c:\\temp\\list.txt";
	output_file_string="c:\\temp\\output.txt";

	// Fit parameters

	ktr_skip_points=10;
	fit_points=5000;
	fv_last_pre_ktr_points=5;

	// Stretch number

	stretch_number=1;
}

Batch_analysis::~Batch_analysis(void)
{
	// Destructor
}

BOOL Batch_analysis::Create()
{
	return CDialog::Create(m_nID, m_pParent);
}

void Batch_analysis::PostNcDestroy() 
{
	delete this;
}

void Batch_analysis::remote_shut_down(void)
{
	// Called by parent to shut down window

	OnClose();
}

void Batch_analysis::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Batch_analysis)
	DDX_Control(pDX, IDC_SPIN_STRETCH_NUMBER, spin_stretch_number_control);
	DDX_Control(pDX, IDC_FIT_FUNCTION, function_list_control);
	DDX_Control(pDX, IDC_ANALYSIS_OPTIONS, analysis_options_control);
	DDX_Control(pDX, IDC_EXPERIMENT_MODE, experiment_mode_control);
	DDX_Text(pDX, IDC_LIST_FILE, list_file_string);
	DDX_Text(pDX, IDC_OUTPUT_FILE, output_file_string);
	DDX_Text(pDX, IDC_KTR_SKIP_POINTS, ktr_skip_points);
	DDV_MinMaxInt(pDX, ktr_skip_points, 0, 1000);
	DDX_Text(pDX, IDC_FIT_POINTS, fit_points);
	DDV_MinMaxInt(pDX, fit_points, 1, 100000);
	DDX_Text(pDX, IDC_STRETCH_NUMBER, stretch_number);
	DDV_MinMaxInt(pDX, stretch_number, 0, 5);
	DDX_Text(pDX, IDC_FV_PRE_KTR_POINTS, fv_last_pre_ktr_points);
	DDV_MinMaxInt(pDX, fv_last_pre_ktr_points, 0, 10000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Batch_analysis, CDialog)
	//{{AFX_MSG_MAP(Batch_analysis)
	ON_WM_CLOSE()
	ON_CBN_SELCHANGE(IDC_EXPERIMENT_MODE, OnSelchangeExperimentMode)
	ON_BN_CLICKED(IDC_LIST_FILE_BROWSE, OnListFileBrowse)
	ON_BN_CLICKED(IDC_OUTPUT_FILE_BROWSE, OnOutputFileBrowse)
	ON_BN_CLICKED(IDC_ANALYSE, OnAnalyse)
	ON_CBN_SELCHANGE(IDC_ANALYSIS_OPTIONS, OnSelchangeAnalysisOptions)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Batch_analysis message handlers

BOOL Batch_analysis::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Variables

	int i;
	CClientDC* p_display_dc;

	// Initialise window size and position

	SetWindowPos(NULL,600,30,400,425,SWP_SHOWWINDOW);

	// Create display dc

	CClientDC dc(this);
	p_display_dc =& dc;

	// Set controls

	// Experiment mode control

	for (i=0;i<=(no_of_experiment_modes-1);i++)
	{
		experiment_mode_control.AddString(experiment_mode_string[i]);
	}

	experiment_mode_control.SetCurSel(0);
	current_experiment_mode=experiment_mode_control.GetCurSel();

	// Analysis options

	analysis_options_control.AddString(analysis_option_string[ISOMETRIC_TENSION]);
	analysis_options_control.AddString(analysis_option_string[KTR_RATE]);

	no_of_analysis_options=2;
	analysis_options_control.SetCurSel(ISOMETRIC_TENSION);
	current_analysis_option=analysis_options_control.GetCurSel();

	// Function list control

	function_list_control.ResetContent();

	function_list_control.AddString(linear_regression_string);
	function_list_control.AddString(robust_linear_string);
	function_list_control.AddString(single_exponential_string);
	function_list_control.AddString(double_exponential_string);

	function_list_control.SetCurSel(2);

	GetDlgItem(IDC_FIT_FUNCTION)->EnableWindow(FALSE);

	// Spin control - first attach spin control to stretch number, then disable

	spin_stretch_number_control.SetBuddy(GetDlgItem(IDC_STRETCH_NUMBER));
	spin_stretch_number_control.SetRange(1,3);

	GetDlgItem(IDC_STRETCH_NUMBER)->EnableWindow(FALSE);

	// Disable other controls irrelevant to isometric tension (default selection)

	GetDlgItem(IDC_FIT_POINTS)->EnableWindow(FALSE);
	GetDlgItem(IDC_KTR_SKIP_POINTS)->EnableWindow(FALSE);
	GetDlgItem(IDC_FV_PRE_KTR_POINTS)->EnableWindow(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void Batch_analysis::OnClose() 
{
	// Signal to parent

	((CSLControlDlg*)m_pParent)->Batch_analysis_Window_Done();

	// Destroy window

	DestroyWindow();
}

void Batch_analysis::OnSelchangeExperimentMode() 
{
	// Code changes analysis options depending on the selected experiment mode

	// Variables

	int i;																// counter

	int experiment_mode=experiment_mode_control.GetCurSel();

	// Code

	UpdateData(TRUE);

	// Reset analysis_options control

	for (i=(no_of_analysis_options-1);i>=0;i--)
	{
		analysis_options_control.DeleteString(i);
	}
	
	// Add new options as appropriate

	switch (experiment_mode)
	{
		case SIMPLE_KTR:
		{
			analysis_options_control.AddString(analysis_option_string[ISOMETRIC_TENSION]);
			analysis_options_control.AddString(analysis_option_string[KTR_RATE]);
			
			no_of_analysis_options=2;
		}
		break;

		case STRETCH_LENGTH_CONTROL:
		{
			analysis_options_control.AddString(analysis_option_string[ISOMETRIC_TENSION]);
			analysis_options_control.AddString(analysis_option_string[KTR_RATE]);
			analysis_options_control.AddString(analysis_option_string[STIFFNESS]);
			analysis_options_control.AddString(analysis_option_string[RELATIVE_PRE_STRETCH_TENSION]);
			analysis_options_control.AddString(analysis_option_string[PEAK_TENSION]);
			
			no_of_analysis_options=5;
		}
		break;

		case ISOTONIC_HOLD:
		{
			analysis_options_control.AddString(analysis_option_string[ISOMETRIC_TENSION]);
			analysis_options_control.AddString(analysis_option_string[KTR_RATE]);
			analysis_options_control.AddString(analysis_option_string[SHORTENING_VELOCITY]);
			
			no_of_analysis_options=3;
		}
		break;

		case STRETCH_TENSION_CONTROL:
		{
			analysis_options_control.AddString(analysis_option_string[ISOMETRIC_TENSION]);
			analysis_options_control.AddString(analysis_option_string[KTR_RATE]);
			analysis_options_control.AddString(analysis_option_string[STIFFNESS]);
			analysis_options_control.AddString(analysis_option_string[RELATIVE_PRE_STRETCH_TENSION]);
			
			no_of_analysis_options=4;
		}
		break;

		default:
		{
			analysis_options_control.AddString(analysis_option_string[ISOMETRIC_TENSION]);
						
			no_of_analysis_options=1;
		}
		break;
	}

	analysis_options_control.SetCurSel(0);
	current_analysis_option=ISOMETRIC_TENSION;

	// Update screen

	UpdateData(FALSE);
}

void Batch_analysis::OnSelchangeAnalysisOptions() 
{
	// Code sets current_analysis_options and enables/disables appropriate controls

	// Variables

	int i;																	// counter
	
	int index;																// to do with extracting
	CString selection_string;												// current control selection

	// Find current_analysis option

	index=analysis_options_control.GetCurSel();
	analysis_options_control.GetLBText(index,selection_string);

	for (i=ISOMETRIC_TENSION;i<=PEAK_TENSION;i++)
	{
		if (selection_string==analysis_option_string[i])
			current_analysis_option=i;
	}

	// Then switch on current option

	switch (current_analysis_option)
	{
		// Enable/disable controls and update current values as appropriate

		case ISOMETRIC_TENSION:
		{
			GetDlgItem(IDC_KTR_SKIP_POINTS)->EnableWindow(FALSE);
			GetDlgItem(IDC_FIT_POINTS)->EnableWindow(FALSE);
			GetDlgItem(IDC_FIT_FUNCTION)->EnableWindow(FALSE);
			GetDlgItem(IDC_STRETCH_NUMBER)->EnableWindow(FALSE);
			GetDlgItem(IDC_FV_PRE_KTR_POINTS)->EnableWindow(FALSE);
		}
		break;

		case KTR_RATE:
		{
			GetDlgItem(IDC_KTR_SKIP_POINTS)->EnableWindow(TRUE);
			GetDlgItem(IDC_FIT_POINTS)->EnableWindow(TRUE);
			GetDlgItem(IDC_FIT_FUNCTION)->EnableWindow(TRUE);
			GetDlgItem(IDC_STRETCH_NUMBER)->EnableWindow(FALSE);
			GetDlgItem(IDC_FV_PRE_KTR_POINTS)->EnableWindow(FALSE);

			if (function_list_control.GetCurSel()!=DOUBLE_EXPONENTIAL)
				function_list_control.SetCurSel(SINGLE_EXPONENTIAL);

			fit_points=4000;
		}
		break;

		case STIFFNESS:
		{
			GetDlgItem(IDC_KTR_SKIP_POINTS)->EnableWindow(FALSE);
			GetDlgItem(IDC_FIT_POINTS)->EnableWindow(TRUE);
			GetDlgItem(IDC_FIT_FUNCTION)->EnableWindow(FALSE);
			GetDlgItem(IDC_STRETCH_NUMBER)->EnableWindow(TRUE);
			GetDlgItem(IDC_FV_PRE_KTR_POINTS)->EnableWindow(FALSE);

			fit_points=50;
		}
		break;

		case SHORTENING_VELOCITY:
		{
			GetDlgItem(IDC_KTR_SKIP_POINTS)->EnableWindow(FALSE);
			GetDlgItem(IDC_FIT_POINTS)->EnableWindow(TRUE);
			GetDlgItem(IDC_FIT_FUNCTION)->EnableWindow(TRUE);
			GetDlgItem(IDC_STRETCH_NUMBER)->EnableWindow(FALSE);
			GetDlgItem(IDC_FV_PRE_KTR_POINTS)->EnableWindow(TRUE);

			function_list_control.SetCurSel(LINEAR_REGRESSION);

			fit_points=100;
		}
		break;

		case RELATIVE_PRE_STRETCH_TENSION:
		{
			GetDlgItem(IDC_KTR_SKIP_POINTS)->EnableWindow(FALSE);
			GetDlgItem(IDC_FIT_POINTS)->EnableWindow(FALSE);
			GetDlgItem(IDC_FIT_FUNCTION)->EnableWindow(FALSE);
			GetDlgItem(IDC_STRETCH_NUMBER)->EnableWindow(TRUE);
			GetDlgItem(IDC_FV_PRE_KTR_POINTS)->EnableWindow(FALSE);
		}
		break;

		case PEAK_TENSION:
		{
			GetDlgItem(IDC_KTR_SKIP_POINTS)->EnableWindow(FALSE);
			GetDlgItem(IDC_FIT_POINTS)->EnableWindow(FALSE);
			GetDlgItem(IDC_FIT_FUNCTION)->EnableWindow(FALSE);
			GetDlgItem(IDC_STRETCH_NUMBER)->EnableWindow(TRUE);
			GetDlgItem(IDC_FV_PRE_KTR_POINTS)->EnableWindow(FALSE);
		}
		break;

		default:
		{

		}
		break;
	}

	UpdateData(FALSE);
}

BOOL Batch_analysis::PreTranslateMessage(MSG* pMsg) 
{
	// Traps Enter and Escape key presses to prevent dialog box closing

	if (pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN)
	{
		return TRUE;
	}

	if (pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE)
	{
		return TRUE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void Batch_analysis::OnListFileBrowse() 
{
	// Allows user to update output file from dialog box

	// Initialises file open dialog

	CFileDialog fileDlg(TRUE,NULL,"*.txt",OFN_HIDEREADONLY,"Text files (*.txt)|*.txt||",this);

	// Initialisation

	fileDlg.m_ofn.lpstrTitle="Select data list file";

	// Call Dialog

	if (fileDlg.DoModal()==IDOK)
	{
		list_file_string=fileDlg.GetPathName();
	}

	UpdateData(FALSE);
}

void Batch_analysis::OnOutputFileBrowse() 
{
	// Allows user to update output file from dialog box

	// Initialises file open dialog

	CFileDialog fileDlg(TRUE,NULL,"*.txt",OFN_HIDEREADONLY,"Text files (*.txt)|*.txt||",this);

	// Initialisation

	fileDlg.m_ofn.lpstrTitle="Select analysis output file";

	// Call Dialog

	if (fileDlg.DoModal()==IDOK)
	{
		list_file_string=fileDlg.GetPathName();
	}

	UpdateData(FALSE);
}


void Batch_analysis::OnAnalyse() 
{
	// Code runs appropriate analysis option

	// Variables

	int i,j;															// counters

	int no_of_files;													// no of data files to analyse

	float ramp_multiplier;												// 1.0 if record is ramps,
																		// 2.0 if record is triangles

	int ktr_function;													// integer defining ktr fit function

	char data_file_string[100];

	char display_string[300];
	char temp_string[300];

	// Code

	// Update from screen

	UpdateData(TRUE);

	// Read and check list and output files

	FILE* list_file=fopen(list_file_string,"r");

	if (list_file==NULL)
	{
		sprintf(display_string,"List file\n%s\ncould not be opened",list_file_string);
		MessageBox(display_string,"Batch_analysis file handling error",MB_ICONWARNING);
		return;
	}

	FILE* output_file=fopen(output_file_string,"w");

	if (output_file==NULL)
	{
		sprintf(display_string,"Output file\n%s\ncould not be created",output_file_string);
		MessageBox(display_string,"Batch_analysis file handling error",MB_ICONWARNING);
		return;
	}

	// Initialise output file

	// Read list file

	fscanf(list_file,"%i",&no_of_files);

	// Run through files

	for (i=1;i<=no_of_files;i++)
	{
		// Read file name

		fscanf(list_file,"%s",&data_file_string);

		// Try to open file and store record parameters

		if (p_batch_record->read_data_from_file(data_file_string)==FALSE)
		{
			// If unsuccessful

			sprintf(display_string,"Data file\n%s\ncould not be opened",p_batch_record->file_name_string);
			MessageBox(display_string,"Batch_analysis file handling error",MB_ICONWARNING);
			return;
		}
		else
		{
			// Data file was opened successfully - now create a display for it

			p_batch_display = new Analysis_display(m_pParent,p_batch_record);

			// Create window

			p_batch_display->Create();

			// Disable WM_SYSMENU and File Exit to prevent user from manually closing window

			p_batch_display->ModifyStyle(WS_SYSMENU,0);

			CMenu* p_menu = p_batch_display->GetMenu();
			p_menu->EnableMenuItem(ID_FILE_EXIT,MF_GRAYED);
	
			// Set ramp_multiplier

			if (p_batch_record->record_ramp_mode==0)	// triangles
			{
				ramp_multiplier=2.0;
			}
			else										// ramps
			{
				ramp_multiplier=1.0;
			}

			// Transform to calibrated values

			p_batch_record->transform_to_calibrated_values();

			// Update screen display

			p_batch_display->p_FOR_plot->y_axis_title="For (N m^-2)";
			p_batch_display->p_SL_plot->y_axis_title="SL (m)";
			p_batch_display->p_FL_plot->y_axis_title="FL (m)";
	
			p_batch_display->p_FOR_SL_plot->y_axis_title="For (N m^-2)";
			p_batch_display->p_FOR_FL_plot->y_axis_title="For (N m^-2)";

			p_batch_display->p_FOR_SL_plot->x_axis_title="SL (m)";
			p_batch_display->p_FOR_FL_plot->x_axis_title="FL (m)";

			// Perform desired analysis

			switch (current_analysis_option)
			{
				case ISOMETRIC_TENSION:
				{
					analysis_parameters.no_of_parameters=1;

					if (i==1)
					{
						fprintf(output_file,"Isometric tension\n");				// Output file header
					}
					isometric_tension_calculation(ramp_multiplier);
				}
				break;

				case KTR_RATE:
				{
					ktr_function=function_list_control.GetCurSel();

					if (i==1)
					{
						// Output appropriate file header for fit function

						switch (ktr_function)
						{
							case ROBUST_LINEAR:
							{
								analysis_parameters.no_of_parameters=2;

								fprintf(output_file,"ktr rate - robust_linear fit\n");
								fprintf(output_file,"Offset Gradient\n");
							}
							break;

							case LINEAR_REGRESSION:
							{
								analysis_parameters.no_of_parameters=2;

								fprintf(output_file,"ktr rate - linear_regression\n");
								fprintf(output_file,"Offset Gradient\n");
							}
							break;

							case SINGLE_EXPONENTIAL:
							{
								analysis_parameters.no_of_parameters=2;

								fprintf(output_file,"ktr rate - single exponential\n");
								fprintf(output_file,"Amplitude Rate\n");
							}
							break;

							case DOUBLE_EXPONENTIAL:
							{
								analysis_parameters.no_of_parameters=4;

								fprintf(output_file,"ktr rate - double exponential\n");
								fprintf(output_file,"Amplitude Rate Amplitude Rate\n");
							}
							break;
						}						
					}

					ktr_calculation(ramp_multiplier,ktr_function);
				}
				break;

				case STIFFNESS:
				{
					analysis_parameters.no_of_parameters=2;

					if (i==1)													// Output file feader
					{
						fprintf(output_file,"Stiffness\n");
						fprintf(output_file,"SL FL\n");
					}

					stiffness_calculations(ramp_multiplier);
				}
				break;

				case SHORTENING_VELOCITY:
				{
					analysis_parameters.no_of_parameters=3;

					if (i==1)													// Output file feader
					{
						fprintf(output_file,"Force-velocity\n");
						fprintf(output_file,"SL_(lo/s) FL_(lo/s) Mean_rel_force)\n");
					}

					force_velocity_calculations();
				}
				break;

				case RELATIVE_PRE_STRETCH_TENSION:
				{
					analysis_parameters.no_of_parameters=1;

					if (i==1)													// Output file feader
					{
						fprintf(output_file,"Relative_Pre_Stretch_Tension\n");
					}

					relative_pre_stretch_tension_calculation(ramp_multiplier);
				}
				break;

				case PEAK_TENSION:
				{
					analysis_parameters.no_of_parameters=1;
					
					if (i==1)
					{
						fprintf(output_file,"Peak tension\n");
					}

					peak_tension_calculations(ramp_multiplier);
				}
				break;

				default:
				{
					MessageBox("No analysis option selected");
				}
				break;
			}

			// Output analysis results to file

			for (j=0;j<=(analysis_parameters.no_of_parameters-1);j++)
			{
				fprintf(output_file,"%g ",analysis_parameters.parameter[j]);
			}
			fprintf(output_file,"\n");

			// User display

			sprintf(display_string,"File %i of %i\nAnalysis parameters\n",i,no_of_files);
			for (j=0;j<=(analysis_parameters.no_of_parameters-1);j++)
			{
				sprintf(temp_string,"Parameter[%i]: %g\n",j,analysis_parameters.parameter[j]);
				strcat(display_string,temp_string);
			}

			if (i<no_of_files)
			{
				strcat(display_string,"Continue? (No aborts current batch-analysis)");

				// Query to abort
				
				if (MessageBox(display_string,"Batch analysis",MB_YESNO+MB_ICONQUESTION)==IDNO)		
					i=no_of_files;		// aborts loop
			}
			else
			{
				MessageBox(display_string);
			}
				
			// Close display

			p_batch_display->remote_shut_down();
		}
	}

	// Tidy up

	fclose(list_file);
	fclose(output_file);

	return;	
}

void Batch_analysis::isometric_tension_calculation(float ramp_multiplier)
{
	// Sets analysis_parameters.parameter[0] to isometric tension

	// Variables

	int pos1,pos2;													// cursor positions

	// Code

	// Set cursors to the beginning and the middle of the ktr step

	pos1=1;

	pos2=(int)((p_batch_record->record_ktr_initiation_time_ms+
					(0.5*p_batch_record->record_ktr_duration_ms))*
				(p_batch_record->record_sampling_rate/1000.0));

	p_batch_display->remote_set_cursors(pos1,pos2);
	
	// Set parameter[0] to isomteric tension

	analysis_parameters.parameter[0]=
		(float)(p_batch_record->data[FORCE][pos1]-p_batch_record->data[FORCE][pos2]);
}

void Batch_analysis::ktr_calculation(float ramp_multiplier,int ktr_function)
{
	// Sets analysis_parameters.parameters to ktr amplitudes and rates

	// Variables

	int i;																	// counter

	int pos1,pos2;															// cursor positions

	// Code

	// Enlarge trace from the beginning of the ktr step to the end of the trace

	pos1=(int)((p_batch_record->record_ktr_initiation_time_ms)*
		(p_batch_record->record_sampling_rate/1000.0));
	
	pos2=p_batch_record->no_of_points;

	p_batch_display->remote_set_cursors(pos1,pos2);
	p_batch_display->remote_expand_trace();

	// Set parameters to beginning and end of fit region

	pos1=pos1+(int)(p_batch_record->record_ktr_duration_ms*p_batch_record->record_sampling_rate/1000.0)+
			ktr_skip_points;

	pos2=pos1+fit_points-1;

	// Error checking for pos2 - set at end of record if necessary

	if (pos2>p_batch_record->no_of_points)
	{
		pos2=p_batch_record->no_of_points;
	}

	p_batch_display->remote_set_cursors(pos1,pos2);

	// Execute the fit and set the parameters

	p_batch_display->p_fit_control->x_filter_control.SetCheck(0);
	p_batch_display->p_fit_control->y_filter_control.SetCheck(0);
	p_batch_display->p_fit_control->plot_list_control.SetCurSel(0);

	// Switch depending on ktr_function

	switch (ktr_function)
	{
		case ROBUST_LINEAR:
		{
			p_batch_display->p_fit_control->function_list_control.SetCurSel(ROBUST_LINEAR);
		}
		break;

		case LINEAR_REGRESSION:
		{
			p_batch_display->p_fit_control->function_list_control.SetCurSel(LINEAR_REGRESSION);
		}
		break;

		case SINGLE_EXPONENTIAL:
		{
			p_batch_display->p_fit_control->function_list_control.SetCurSel(SINGLE_EXPONENTIAL);
		}
		break;

		case DOUBLE_EXPONENTIAL:
		{
			p_batch_display->p_fit_control->function_list_control.SetCurSel(DOUBLE_EXPONENTIAL);
		}
		break;
	}

	// Fit

	p_batch_display->p_fit_control->bypass_to_fit();

	// Copy fit parameters to analysis_parameters structure

	for (i=0;i<=analysis_parameters.no_of_parameters;i++)
	{
		analysis_parameters.parameter[i]=p_batch_display->p_window_fit_parameters->parameters[i];
	}
}

void Batch_analysis::stiffness_calculations(float ramp_multiplier)
{
	// Fits robust_linear lines to selected regions of ramps/triangles

	// Variables

	int pos1,pos2;

	float FL_result,SL_result;

	// Code

	// Enlarge trace to the ktr step

	pos1=1;

	pos2=(int)((p_batch_record->record_ktr_initiation_time_ms+
					(0.5*p_batch_record->record_ktr_duration_ms))*
				(p_batch_record->record_sampling_rate/1000.0));

	p_batch_display->remote_set_cursors(pos1,pos2);
	p_batch_display->remote_expand_trace();

	// Move cursors to beginning and end of selected length change

	switch (stretch_number)
	{
		case (1):
		{
			pos1=(int)((p_batch_record->record_pre_triangle_ms)*
				(p_batch_record->record_sampling_rate/1000.0));
		}
		break;

		case (2):
		{
			pos1=(int)((p_batch_record->record_pre_triangle_ms+
				(fabs(p_batch_record->record_relative_first_tri_size)*
					p_batch_record->record_triangle_halftime_ms*ramp_multiplier)+
				p_batch_record->record_inter_triangle_interval_ms)*
				(p_batch_record->record_sampling_rate/1000.0));
		}
		break;

		case (3):
		{
			pos1=(int)((p_batch_record->record_pre_triangle_ms+
				(fabs(p_batch_record->record_relative_first_tri_size)*
					p_batch_record->record_triangle_halftime_ms*ramp_multiplier)+
				(2.0*p_batch_record->record_inter_triangle_interval_ms)+
				(p_batch_record->record_triangle_halftime_ms*ramp_multiplier))*
				(p_batch_record->record_sampling_rate/1000.0));
		}
		break;
	}

	pos2=pos1+fit_points-1;

	p_batch_display->remote_set_cursors(pos1,pos2);

	// FL

	p_batch_display->p_fit_control->x_filter_control.SetCheck(BST_CHECKED);
	p_batch_display->p_fit_control->y_filter_control.SetCheck(BST_CHECKED);
	p_batch_display->p_fit_control->plot_list_control.SetCurSel(4);
	p_batch_display->p_fit_control->function_list_control.SetCurSel(ROBUST_LINEAR);
	p_batch_display->p_fit_control->bypass_to_fit();

	// Convert stiffness to Young's Modulus (N m^-2)

	FL_result=(p_batch_display->p_window_fit_parameters->parameters[1]*
	p_batch_record->record_fl*(float)1.0e-6);

	// SL

	p_batch_display->p_fit_control->x_filter_control.SetCheck(BST_CHECKED);
	p_batch_display->p_fit_control->y_filter_control.SetCheck(BST_CHECKED);
	p_batch_display->p_fit_control->plot_list_control.SetCurSel(3);
	p_batch_display->p_fit_control->function_list_control.SetCurSel(ROBUST_LINEAR);
	p_batch_display->p_fit_control->bypass_to_fit();

	// Convert stiffness to Young's Modulus (N m^-2)

	SL_result=(p_batch_display->p_window_fit_parameters->parameters[1]*
		p_batch_record->record_sl*(float)1.0e-9);

	analysis_parameters.parameter[0]=SL_result;
	analysis_parameters.parameter[1]=FL_result;
}

void Batch_analysis::force_velocity_calculations(void)
{
	// Fits robust_linear lines to selected regions of ramps/triangles

	// Variables

	int pos1,pos2;

	float min_tension;															// mean tension reading at
																				// ktr step
	float hold_tension;															// mean tension during hold
	float isometric_tension;													// steady isometric tension
	
	float rel_tension;															// mean tension expressed
																				// relative to isometric tension

	float FL_result,SL_result;

	// Code

	// Calculate and draw isometric_tension

	pos2=(int)((p_batch_record->record_pre_servo_ms)*(p_batch_record->record_sampling_rate/1000.0))-1;
	pos1=pos2-50;

	isometric_tension=p_batch_record->extract_mean_value(FORCE,pos1,pos2);

	p_batch_display->set_FORCE_mean_values_line_segment(0,TRUE,
		p_batch_record->data[TIME][pos1],isometric_tension,
		p_batch_record->data[TIME][pos2],isometric_tension,
		MAGENTA);

	// Calculate and draw min tension

	pos1=(int)((p_batch_record->record_ktr_initiation_time_ms)*(p_batch_record->record_sampling_rate/1000.0))+7;
	pos2=pos1+(int)((p_batch_record->record_ktr_duration_ms)*(p_batch_record->record_sampling_rate/1000.0))-7;

	min_tension=p_batch_record->extract_mean_value(FORCE,pos1,pos2);

	p_batch_display->set_FORCE_mean_values_line_segment(1,TRUE,
		p_batch_record->data[TIME][pos1],min_tension,
		p_batch_record->data[TIME][pos2],min_tension,
		YELLOW);

	// Enlarge trace to just past the ktr step

	pos1=1;

	pos2=(int)((p_batch_record->record_servo_ms+p_batch_record->record_pre_servo_ms+
					(0.5*p_batch_record->record_ktr_duration_ms))*
				(p_batch_record->record_sampling_rate/1000.0))+100;

	p_batch_display->remote_set_cursors(pos1,pos2);
	p_batch_display->remote_expand_trace();

	// Move cursors to beginning and end of selected length change

	pos2=(int)((p_batch_record->record_servo_ms+p_batch_record->record_pre_servo_ms)*
			(p_batch_record->record_sampling_rate/1000.0))-fv_last_pre_ktr_points;

	pos1=(pos2-fit_points+1);

	// Error checking if cursor is moved to isometric phase

	if (pos1<(int)((p_batch_record->record_pre_servo_ms)*(p_batch_record->record_sampling_rate/1000.0)))
		pos1=(int)((p_batch_record->record_pre_servo_ms)*(p_batch_record->record_sampling_rate/1000.0))+10;

	p_batch_display->remote_set_cursors(pos1,pos2);

	// SL

	p_batch_display->p_fit_control->x_filter_control.SetCheck(BST_CHECKED);
	p_batch_display->p_fit_control->y_filter_control.SetCheck(BST_CHECKED);
	p_batch_display->p_fit_control->plot_list_control.SetCurSel(1);
	p_batch_display->p_fit_control->function_list_control.SetCurSel(LINEAR_REGRESSION);
	p_batch_display->p_fit_control->bypass_to_fit();

	// Convert SL shortening velocity to muscle lengths per second

	SL_result=(p_batch_display->p_window_fit_parameters->parameters[1]/
		(p_batch_record->record_sl*(float)1.0e-9));

	// FL

	p_batch_display->p_fit_control->x_filter_control.SetCheck(BST_CHECKED);
	p_batch_display->p_fit_control->y_filter_control.SetCheck(BST_CHECKED);
	p_batch_display->p_fit_control->plot_list_control.SetCurSel(2);
	p_batch_display->p_fit_control->function_list_control.SetCurSel(LINEAR_REGRESSION);
	p_batch_display->p_fit_control->bypass_to_fit();

	// Convert FL shortening velocity

	FL_result=(p_batch_display->p_window_fit_parameters->parameters[1]/
		(p_batch_record->record_fl*(float)1.0e-6));
	
	// Extract and draw hold_tension

	hold_tension=p_batch_record->extract_mean_value(FORCE,pos1,pos2);

	p_batch_display->set_FORCE_mean_values_line_segment(2,TRUE,
		p_batch_record->data[TIME][pos1],hold_tension,
		p_batch_record->data[TIME][pos2],hold_tension,
		WHITE);

	// Calculate rel_tension

	rel_tension=(hold_tension-min_tension)/(isometric_tension-min_tension);

	// Update analysis_parameters structure

	analysis_parameters.parameter[0]=SL_result;
	analysis_parameters.parameter[1]=FL_result;
	analysis_parameters.parameter[2]=rel_tension;
}

void Batch_analysis::relative_pre_stretch_tension_calculation(float ramp_multiplier)
{
	// Extracts relative tension at the beginning of the nth stretch 

	// Variables

	int pos1,pos2;																// cursor positions

	float min_tension;															// mean tension reading at
																				// ktr step
	float pre_stretch_tension;													// pre_stretch tension
	float isometric_tension;													// steady isometric tension
	
	float rel_tension;															// pre_stretch tension expressed
																				// relative to isometric tension

	// Code

	// Calculate and draw isometric_tension

	pos2=(int)((p_batch_record->record_pre_triangle_ms)*(p_batch_record->record_sampling_rate/1000.0))-1;
	pos1=pos2-50;
	if (pos1<1) pos1=1;

	isometric_tension=p_batch_record->extract_mean_value(FORCE,pos1,pos2);

	p_batch_display->set_FORCE_mean_values_line_segment(0,TRUE,
		p_batch_record->data[TIME][pos1],isometric_tension,
		p_batch_record->data[TIME][pos2],isometric_tension,
		MAGENTA);

	// Calculate and draw min tension

	pos1=(int)((p_batch_record->record_ktr_initiation_time_ms)*(p_batch_record->record_sampling_rate/1000.0))+7;
	pos2=pos1+(int)((p_batch_record->record_ktr_duration_ms)*(p_batch_record->record_sampling_rate/1000.0))-7;

	min_tension=p_batch_record->extract_mean_value(FORCE,pos1,pos2);

	p_batch_display->set_FORCE_mean_values_line_segment(1,TRUE,
		p_batch_record->data[TIME][pos1],min_tension,
		p_batch_record->data[TIME][pos2],min_tension,
		YELLOW);

	// Enlarge trace to just past the ktr step

	pos1=1;

	pos2=(int)((p_batch_record->record_ktr_initiation_time_ms+p_batch_record->record_ktr_duration_ms)*
				(p_batch_record->record_sampling_rate/1000.0))+100;

	p_batch_display->remote_set_cursors(pos1,pos2);
	p_batch_display->remote_expand_trace();

	// Move cursor 1 to beginning of the appropriate stretch

	switch (stretch_number)
	{
		case (1):
		{
			pos1=(int)((p_batch_record->record_pre_triangle_ms)*
				(p_batch_record->record_sampling_rate/1000.0));
		}
		break;

		case (2):
		{
			pos1=(int)((p_batch_record->record_pre_triangle_ms+
				(fabs(p_batch_record->record_relative_first_tri_size)*
					p_batch_record->record_triangle_halftime_ms*ramp_multiplier)+
				p_batch_record->record_inter_triangle_interval_ms)*
				(p_batch_record->record_sampling_rate/1000.0));
		}
		break;

		case (3):
		{
			pos1=(int)((p_batch_record->record_pre_triangle_ms+
				(fabs(p_batch_record->record_relative_first_tri_size)*
					p_batch_record->record_triangle_halftime_ms*ramp_multiplier)+
				(2.0*p_batch_record->record_inter_triangle_interval_ms)+
				(p_batch_record->record_triangle_halftime_ms*ramp_multiplier))*
				(p_batch_record->record_sampling_rate/1000.0));
		}
		break;
	}

	// Move cursor 2 to the ktr step

	pos2=(int)((p_batch_record->record_ktr_initiation_time_ms)*
				(p_batch_record->record_sampling_rate/1000.0));

	// Expand between cursors

	p_batch_display->remote_set_cursors(pos1,pos2);

	// Extract and draw hold_tension

	pre_stretch_tension=p_batch_record->data[FORCE][pos1];

	// Calculate rel_tension

	rel_tension=(pre_stretch_tension-min_tension)/(isometric_tension-min_tension);

	// Update analysis_parameters structure

	analysis_parameters.parameter[0]=rel_tension;
}

void Batch_analysis::peak_tension_calculations(float ramp_multiplier)
{
	// Extracts tension at the end of the nth stretch

	// Variables

	int pos1,pos2;										// cursor positions

	float isometric_tension;							// steady isometric tension
	
	float peak_tension;									// difference between isometric tension
														// and tension at end of nth stretch

	// Code

	// Calculate and draw isometric_tension

	pos2=(int)((p_batch_record->record_pre_triangle_ms)*(p_batch_record->record_sampling_rate/1000.0))-1;
	pos1=pos2-50;
	if (pos1<1) pos1=1;

	isometric_tension=p_batch_record->extract_mean_value(FORCE,pos1,pos2);

	p_batch_display->set_FORCE_mean_values_line_segment(0,TRUE,
		p_batch_record->data[TIME][pos1],isometric_tension,
		p_batch_record->data[TIME][pos2],isometric_tension,
		MAGENTA);

	// Enlarge trace to just past the nth stretch

	pos1=1;

	switch (stretch_number)
	{
		case (1):
		{
			pos2=(int)((p_batch_record->record_pre_triangle_ms+
				(fabs(p_batch_record->record_relative_first_tri_size)*
					p_batch_record->record_triangle_halftime_ms*ramp_multiplier))*
				(p_batch_record->record_sampling_rate/1000.0));
		}
		break;

		case (2):
		{
			pos2=(int)((p_batch_record->record_pre_triangle_ms+
				(fabs(p_batch_record->record_relative_first_tri_size)*
					p_batch_record->record_triangle_halftime_ms*ramp_multiplier)+
				(p_batch_record->record_inter_triangle_interval_ms)+
					(p_batch_record->record_triangle_halftime_ms*ramp_multiplier))*
				(p_batch_record->record_sampling_rate/1000.0));
		}
		break;

		case (3):
		{
			pos2=(int)((p_batch_record->record_pre_triangle_ms+
				(fabs(p_batch_record->record_relative_first_tri_size)*
					p_batch_record->record_triangle_halftime_ms*ramp_multiplier)+
				(2.0*p_batch_record->record_inter_triangle_interval_ms)+
					(2.0*p_batch_record->record_triangle_halftime_ms*ramp_multiplier))*
				(p_batch_record->record_sampling_rate/1000.0));
		}
		break;
	}

	pos2=pos2+(int)((p_batch_record->record_triangle_halftime_ms)*
			(p_batch_record->record_sampling_rate/1000.0));

	p_batch_display->remote_set_cursors(pos1,pos2);
	p_batch_display->remote_expand_trace();

	// Move cursor 2 to the peak of the appropriate stretch

	pos2=pos2-(int)((p_batch_record->record_triangle_halftime_ms)*
			(p_batch_record->record_sampling_rate/1000.0));
	
	p_batch_display->remote_set_cursors(pos1,pos2);

	// Extract peak_tension

	peak_tension=p_batch_record->data[FORCE][pos2]-isometric_tension;

	// Update analysis_parameters structure

	analysis_parameters.parameter[0]=peak_tension;
}
