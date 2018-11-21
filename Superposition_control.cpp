// Superposition_control.cpp : implementation file
//

#include "stdafx.h"
#include "slcontrol.h"
#include "Superposition_control.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Superposition_control dialog

#include "Superposition.h"
#include "global_functions.h"
#include "NR.h"
#include ".\superposition_control.h"


Superposition_control::Superposition_control(CWnd* pParent /*=NULL*/,
	Superposition* set_p_superposition_display)
	: CDialog(Superposition_control::IDD, pParent)
	, align_traces(false)
	, align_traces_to_cursor(FALSE)
{
	m_pParent=pParent;
	m_nID=Superposition_control::IDD;

	// Set pointer to the parent

	p_superposition_display = set_p_superposition_display;

	// Window size and dimensions

	window_x_pos=(float)0.7;
	window_y_pos=(float)0.7;
	window_x_size=(float)0.28;
	window_y_size=(float)0.32;

	// Default parameters

	average_file_string="c:\\temp\\average.slc";


	//{{AFX_DATA_INIT(Superposition_control)
	plot_calibrated_values = FALSE;
	//}}AFX_DATA_INIT
}

Superposition_control::~Superposition_control(void)
{	
	DestroyWindow();
}

void Superposition_control::PostNcDestroy() 
{
	delete this;
}

BOOL Superposition_control::Create()
{
	return CDialog::Create(m_nID, m_pParent);
}


void Superposition_control::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Superposition_control)
	DDX_Text(pDX, IDC_AVERAGE_FILE_STRING, average_file_string);
	DDX_Check(pDX, IDC_CALIBRATED_VALUES, plot_calibrated_values);
	//}}AFX_DATA_MAP

	DDX_Check(pDX, IDC_ALIGN, align_traces_to_cursor);
}


BEGIN_MESSAGE_MAP(Superposition_control, CDialog)
	//{{AFX_MSG_MAP(Superposition_control)
	ON_BN_CLICKED(IDC_PLOT_OTHER_TRACES, OnPlotOtherTraces)
	ON_BN_CLICKED(IDC_ADJUST_FIRST_TRACE, OnAdjustFirstTrace)
	ON_BN_CLICKED(IDC_AVERAGE, OnAverage)
	ON_BN_CLICKED(IDC_CALIBRATED_VALUES, OnCalibratedValues)
	ON_BN_CLICKED(IDC_SHUFFLE_ORDER, OnShuffleOrder)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_ALIGN, OnAligntraces)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Superposition_control message handlers

BOOL Superposition_control::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Variables

	int width=1022;															// screen width and height
	int height=740;															// in pixels

	// Code

	SetWindowPos(NULL,(int)(window_x_pos*width),(int)(window_y_pos*height),
		(int)(window_x_size*width),(int)(window_y_size*height),SWP_SHOWWINDOW);

	// Disable other_trace button if there is only one trace

	if (p_superposition_display->no_of_traces==1)
	{
		GetDlgItem(IDC_PLOT_OTHER_TRACES)->EnableWindow(FALSE);
	}

	// Disable adjust_first trace button since only one trace will be displayed

	GetDlgItem(IDC_ADJUST_FIRST_TRACE)->EnableWindow(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void Superposition_control::close_superposition_control_window(void)
{
	// Code is called by parent class to shut down the window

	// Signal to parent that window is closing

	((Superposition*)m_pParent)->Superposition_control_Window_Done();

	DestroyWindow();
}

void Superposition_control::OnPlotOtherTraces() 
{
	// Plot other traces

	p_superposition_display->plot_remaining_traces(1);

	// Enable/disable buttons

	GetDlgItem(IDC_ADJUST_FIRST_TRACE)->EnableWindow(TRUE);
	GetDlgItem(IDC_ADJUST_FIRST_TRACE)->SetFocus();
	GetDlgItem(IDC_PLOT_OTHER_TRACES)->EnableWindow(FALSE);
}

void Superposition_control::OnAdjustFirstTrace() 
{
	// Resets parent window to default view of the first trace

	p_superposition_display->clear_window();

	p_superposition_display->plot_first_trace();

	p_superposition_display->multiple_plots_active=0;

	// Enable/disable buttons

	if (p_superposition_display->no_of_traces>1)
	{
		GetDlgItem(IDC_PLOT_OTHER_TRACES)->EnableWindow(TRUE);
	}

	GetDlgItem(IDC_PLOT_OTHER_TRACES)->SetFocus();
	GetDlgItem(IDC_ADJUST_FIRST_TRACE)->EnableWindow(FALSE);
}

void Superposition_control::OnCalibratedValues() 
{
	// Forces a screen redraw

	UpdateData(TRUE);

	p_superposition_display->clear_window();

	p_superposition_display->plot_first_trace();

	if (p_superposition_display->multiple_plots_active==1)
	{
		p_superposition_display->plot_remaining_traces(1);
	}
}

void Superposition_control::OnAligntraces()
{
	// Forces a screen redraw

	UpdateData(TRUE);

	p_superposition_display->clear_window();

	p_superposition_display->plot_first_trace();

	if (p_superposition_display->multiple_plots_active==1)
	{
		p_superposition_display->plot_remaining_traces(1);
	}
}

void Superposition_control::OnShuffleOrder() 
{
	// Reverses trace order and redraws screen

	UpdateData(TRUE);

	p_superposition_display->shuffle_trace_order();

	// Redraw screen

	p_superposition_display->clear_window();

	p_superposition_display->plot_first_trace();

	if (p_superposition_display->multiple_plots_active==1)
	{
		p_superposition_display->plot_remaining_traces(1);
	}
}


void Superposition_control::OnAverage() 
{
	// Plots all data files and writes the average to file

	// Variables

	int i,j,k;

	float** processing_array;

	char display_string[300];
	char cursor_path_string[300];

	// Code

	UpdateData(TRUE);

	// File handling

	FILE* average_file=fopen(average_file_string,"w");

	if (average_file==NULL)
	{
		sprintf(display_string,"Output file\n%s\ncould not be opened",average_file_string);
		MessageBox(display_string);
		return;
	}

	// Output header

	fprintf(average_file,"_AVERAGED_FILE_\n");
	fprintf(average_file,"%i records\n",p_superposition_display->no_of_traces);

	for (i=0;i<=(p_superposition_display->no_of_traces-1);i++)
	{
		fprintf(average_file,"File %i:\t%s\n",
			i,p_superposition_display->file_strings[i]);
	}

	fprintf(average_file,"\n");

	// Clear screen, open and draw the first trace

	p_superposition_display->clear_window();
	p_superposition_display->plot_first_trace();

	// Output the first trace header

	p_superposition_display->p_data_record->output_header_to_file(
		p_superposition_display->p_data_record->file_name_string,
		average_file);

	// Switch cursor to animated cursor
	
	::set_executable_directory_string(cursor_path_string);
	strcat(cursor_path_string,"\\calculator.ani");

	HCURSOR hcurWait = (HCURSOR)LoadImage(AfxGetApp()->m_hInstance,
		cursor_path_string,IMAGE_CURSOR,0,0,
		LR_DEFAULTSIZE|LR_DEFAULTCOLOR|LR_LOADFROMFILE);
	const HCURSOR hcurSave=SetCursor(hcurWait);

	SetCursor(hcurWait);

	// Average files

	processing_array=matrix(0,5,0,MAX_DATA_POINTS+1);

	// Load first record

	for (i=TIME;i<=INTENSITY;i++)
	{
		for (j=1;j<=p_superposition_display->p_data_record->no_of_points;j++)
		{
			processing_array[i][j]=p_superposition_display->p_data_record->data[i][j];
		}
	}

	// Add subsequent records

	for (k=1;k<=(p_superposition_display->no_of_traces-1);k++)
	{
		p_superposition_display->p_data_record->read_data_from_file(
			p_superposition_display->file_strings[k]);

		for (i=TIME;i<=INTENSITY;i++)
		{
			for (j=1;j<=p_superposition_display->p_data_record->no_of_points;j++)
			{
				processing_array[i][j]=processing_array[i][j]+
					p_superposition_display->p_data_record->data[i][j];
			}
		}
	}

	p_superposition_display->plot_remaining_traces(0);

	// Calculate average

	for (i=TIME;i<=INTENSITY;i++)
	{
		for (j=1;j<=p_superposition_display->p_data_record->no_of_points;j++)
		{
			processing_array[i][j]=processing_array[i][j]/
				(float)(p_superposition_display->no_of_traces);
		}
	}
	
	// Output average to file

	for (i=1;i<=p_superposition_display->p_data_record->no_of_points;i++)
	{
		fprintf(average_file,"%g %g %g %g %g\n",
			processing_array[TIME][i],processing_array[FORCE][i],
			processing_array[SL][i],processing_array[FL][i],
			processing_array[INTENSITY][i]);
	}

	// Update current record with average and plot it

	for (i=TIME;i<=INTENSITY;i++)
	{
		for (j=1;j<=p_superposition_display->p_data_record->no_of_points;j++)
		{
			p_superposition_display->p_data_record->data[i][j]=
				processing_array[i][j];
		}
	}

	p_superposition_display->plot_single_trace(BLACK);

	// End Cursor wait

	SetCursor(hcurSave);

	// Tidy up

	fclose(average_file);

	free_matrix(processing_array,0,5,0,MAX_DATA_POINTS );

	return;
}

BOOL Superposition_control::PreTranslateMessage(MSG* pMsg) 
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
