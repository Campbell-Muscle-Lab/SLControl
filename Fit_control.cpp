// Fit_control.cpp : implementation file
//

#include "stdafx.h"
#include "slcontrol.h"
#include "Fit_control.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////
// Ken's code starts here //
////////////////////////////

#include "Analysis_display.h"
#include "global_definitions.h"
#include "Fit_parameters.h"
#include "Plot.h"
#include "NR.h"

////////////////////////////
//  Ken's code ends here  //
////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Fit_control dialog


Fit_control::Fit_control(CWnd* pParent,
		Analysis_display* set_p_parent_analysis_display,
		Fit_parameters* set_p_analysis_fit_parameters,
		int set_no_of_plots,Plot* set_p_plot[])
	: CDialog(Fit_control::IDD, pParent)
{
	m_pParent=pParent;
	m_nID=Fit_control::IDD;

	p_parent_analysis_display = set_p_parent_analysis_display;
	p_analysis_fit_parameters = set_p_analysis_fit_parameters;

	no_of_plots=set_no_of_plots;

	for (int i=0;i<=(no_of_plots-1);i++)
	{
		p_plot[i]=set_p_plot[i];
	}

	// Relative window size and position

	window_x_pos=(float)0.6;
	window_y_pos=(float)0.75;
	window_x_size=(float)0.38;
	window_y_size=(float)0.25;

	// Combo box strings

	mean_value_string="Mean Value";
	linear_regression_string="Linear Regression";
	robust_linear_string="Robust Linear";
	single_exponential_string="Single Exponential";
	double_exponential_string="Double Exponential";
	two_lines_string="Two Linear Regressions";

	mean_value_function_string="p0";
	linear_regression_function_string="p0 + p1.x";
	robust_linear_function_string="p0 + p1.x";
	single_exponential_function_string="p0.(1 - exp(-p1.x))";
	double_exponential_function_string="p0.(1 - exp(-p1.x)) + p2.(1 - exp(-p3.x))";
	two_lines_function_string="p0 = mid point, p1 = (s) from ktr init";

	//{{AFX_DATA_INIT(Fit_control)
	fit_function_string = _T("");
	p0_string = _T("");
	p1_string = _T("");
	p2_string = _T("");
	p3_string = _T("");
	r_squared_string = _T("");
	x_spike_filter = FALSE;
	x_rogue_points = 0;
	y_spike_filter = FALSE;
	y_rogue_points = 0;
	//}}AFX_DATA_INIT

	// Set string default values

	default_string="*-----*";
	p0_string=default_string;
	p1_string=default_string;
	p2_string=default_string;
	p3_string=default_string;
	r_squared_string=default_string;

	x_rogue_points=0;
	y_rogue_points=0;
}

Fit_control::~Fit_control(void)
{	
	DestroyWindow();
}

void Fit_control::PostNcDestroy() 
{
	delete this;
}

BOOL Fit_control::Create()
{
	return CDialog::Create(m_nID, m_pParent);
}

void Fit_control::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Fit_control)
	DDX_Control(pDX, IDC_Y_FILTER, y_filter_control);
	DDX_Control(pDX, IDC_X_FILTER, x_filter_control);
	DDX_Control(pDX, IDC_PLOT_LIST, plot_list_control);
	DDX_Control(pDX, IDC_FUNCTION_LIST, function_list_control);
	DDX_Text(pDX, IDC_FIT_FUNCTION, fit_function_string);
	DDX_Text(pDX, IDC_p0, p0_string);
	DDX_Text(pDX, IDC_p1, p1_string);
	DDX_Text(pDX, IDC_p2, p2_string);
	DDX_Text(pDX, IDC_p3, p3_string);
	DDX_Text(pDX, IDC_R_SQUARED, r_squared_string);
	DDX_Check(pDX, IDC_X_FILTER, x_spike_filter);
	DDX_Text(pDX, IDC_X_ROGUE_POINTS, x_rogue_points);
	DDX_Check(pDX, IDC_Y_FILTER, y_spike_filter);
	DDX_Text(pDX, IDC_Y_ROGUE_POINTS, y_rogue_points);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Fit_control, CDialog)
	//{{AFX_MSG_MAP(Fit_control)
	ON_CBN_SELCHANGE(IDC_FUNCTION_LIST, OnSelchangeFunctionList)
	ON_BN_CLICKED(IDC_FIT_BUTTON, OnFitButton)
	ON_CBN_SELCHANGE(IDC_PLOT_LIST, OnSelchangePlotList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Fit_control message handlers

BOOL Fit_control::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	// Variables

	int i;

	int width=1022;																	// screen width and height
	int height=740;																	// and pixels
	
	// Initialise window size and position

	SetWindowPos(NULL,(int)(window_x_pos*width),(int)(window_y_pos*height),
		(int)(window_x_size*width),(int)(window_y_size*height),SWP_SHOWWINDOW);

	// Initialise combo boxes

	// Function list control

	function_list_control.ResetContent();

	function_list_control.AddString(mean_value_string);
	function_list_control.AddString(linear_regression_string);
	function_list_control.AddString(robust_linear_string);
	function_list_control.AddString(single_exponential_string);
	function_list_control.AddString(double_exponential_string);
	function_list_control.AddString(two_lines_string);

	function_list_control.SetCurSel(SINGLE_EXPONENTIAL);
		
	no_of_parameters=2;

	fit_function_string=single_exponential_function_string;
	GetDlgItem(IDC_FIT_FUNCTION)->EnableWindow(FALSE);

	GetDlgItem(IDC_p0)->EnableWindow(FALSE);
	GetDlgItem(IDC_p1)->EnableWindow(FALSE);
	GetDlgItem(IDC_p2)->EnableWindow(FALSE);
	GetDlgItem(IDC_p3)->EnableWindow(FALSE);

	// Plot list control

	plot_list_control.ResetContent();

	for (i=0;i<=(no_of_plots-1);i++)
	{
		plot_list_control.AddString(p_plot[i]->plot_title);
	}

	plot_list_control.SetCurSel(0);
	current_plot=plot_list_control.GetCurSel();

	// Disable r_squared control

	GetDlgItem(IDC_R_SQUARED)->EnableWindow(FALSE);

	// Disable rogue point controls

	GetDlgItem(IDC_X_ROGUE_POINTS)->EnableWindow(FALSE);
	GetDlgItem(IDC_Y_ROGUE_POINTS)->EnableWindow(FALSE);

	// Update screen

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void Fit_control::close_fit_control_window(void)
{
	// Code is called by parent class to shut down the window

	// Signal to parent that window is closing

	((Analysis_display*)m_pParent)->Fit_control_Window_Done();

	DestroyWindow();
}

void Fit_control::OnSelchangeFunctionList() 
{
	// TODO: Add your control notification handler code here

	// Variables

	int function=function_list_control.GetCurSel();

	// Code

	UpdateData(TRUE);

	switch (function)
	{
		case MEAN_VALUE:
		{
			fit_function_string=mean_value_string;
		}
		break;

		case LINEAR_REGRESSION:
		{
			fit_function_string=linear_regression_function_string;
		}
		break;

		case ROBUST_LINEAR:
		{
			fit_function_string=robust_linear_function_string;
		}
		break;

		case SINGLE_EXPONENTIAL:
		{
			fit_function_string=single_exponential_function_string;
		}
		break;

		case DOUBLE_EXPONENTIAL:
		{
			fit_function_string=double_exponential_function_string;
		}
		break;

		case TWO_LINES:
		{
			fit_function_string=two_lines_function_string;
		}
		break;

		default:
		{
		}
		break;
	}

	// Set string default values

	p0_string=default_string;
	p1_string=default_string;
	p2_string=default_string;
	p3_string=default_string;
	r_squared_string=default_string;

	// Update Screen

	UpdateData(FALSE);

	// Update active fit for parent window

	p_parent_analysis_display->fit_active=FALSE;
	p_parent_analysis_display->update_plots();
}

void Fit_control::OnFitButton() 
{
	// Variables

	int first_point,last_point;										// first and last points for curve-fit
	int temp_swap;

	int plot_selection=plot_list_control.GetCurSel();				// plot_selection
	int function_selection=function_list_control.GetCurSel();		// function selection

	int fit_successful=1;											// if fit is successful = 1
																	// else = 0

	char temp_string[100];											// for parameter output

	// Code

	// Update parameters from screen

	UpdateData(TRUE);

	// Set cursor

	BeginWaitCursor();

	// Set first and last_point

	first_point=p_plot[plot_selection]->cursor_point[p_plot[plot_selection]->active_cursor];

	last_point=	p_plot[plot_selection]->cursor_point[p_plot[plot_selection]->inactive_cursor];
		
	// Ensure first_point is < last_point

	if (first_point>last_point)
	{
		// Swap

		temp_swap=first_point;
		first_point=last_point;
		last_point=temp_swap;
	}

	if (first_point==last_point)									// checks for points being equal
	{
		if (last_point==(p_plot[plot_selection]->p_record)->no_of_points)
		{
			first_point--;
		}
		else
		{
			last_point++;
		}
	}

	// Reset fit parameters

	p_analysis_fit_parameters->set_fit_parameters(p_plot[plot_selection],no_of_parameters,function_selection,
					first_point,last_point);

	p_analysis_fit_parameters->fill_fit_array(x_spike_filter,y_spike_filter);

	switch (function_selection)
	{
		case MEAN_VALUE:
		{
			p_analysis_fit_parameters->no_of_fit_parameters=1;

			p_analysis_fit_parameters->mean_value();
			
			sprintf(temp_string,"%6.4e",p_analysis_fit_parameters->parameters[0]);
			p0_string=temp_string;

			p1_string=default_string;
			p2_string=default_string;
			p3_string=default_string;
		}
		break;

		case LINEAR_REGRESSION:
		{
			p_analysis_fit_parameters->no_of_fit_parameters=2;

			p_analysis_fit_parameters->linear_regression();
			
			sprintf(temp_string,"%6.4e",p_analysis_fit_parameters->parameters[0]);
			p0_string=temp_string;

			sprintf(temp_string,"%6.4e",p_analysis_fit_parameters->parameters[1]);
			p1_string=temp_string;

			p2_string=default_string;
			p3_string=default_string;
		}
		break;

		case ROBUST_LINEAR:
		{
			p_analysis_fit_parameters->no_of_fit_parameters=2;

			p_analysis_fit_parameters->robust_linear();
			
			sprintf(temp_string,"%6.4e",p_analysis_fit_parameters->parameters[0]);
			p0_string=temp_string;

			sprintf(temp_string,"%6.4e",p_analysis_fit_parameters->parameters[1]);
			p1_string=temp_string;

			p2_string=default_string;
			p3_string=default_string;
		}
		break;

		case SINGLE_EXPONENTIAL:
		{
			p_analysis_fit_parameters->no_of_fit_parameters=2;

			if (p_analysis_fit_parameters->single_exponential())
			{
				sprintf(temp_string,"%6.4e",p_analysis_fit_parameters->parameters[0]);
				p0_string=temp_string;
				sprintf(temp_string,"%6.4e",p_analysis_fit_parameters->parameters[1]);
				p1_string=temp_string;

				p2_string=default_string;
				p3_string=default_string;
			}
			else
			{
				// Fit failed

				p0_string="Fit failed";
				p1_string="Fit failed";

				p2_string=default_string;
				p3_string=default_string;

				fit_successful=0;
			}
		}
		break;

		case DOUBLE_EXPONENTIAL:
		{

			p_analysis_fit_parameters->no_of_fit_parameters=4;

			if (p_analysis_fit_parameters->double_exponential())
			{
				sprintf(temp_string,"%6.4e",p_analysis_fit_parameters->parameters[0]);
				p0_string=temp_string;
				sprintf(temp_string,"%6.4e",p_analysis_fit_parameters->parameters[1]);
				p1_string=temp_string;
				sprintf(temp_string,"%6.4e",p_analysis_fit_parameters->parameters[2]);
				p2_string=temp_string;
				sprintf(temp_string,"%6.4e",p_analysis_fit_parameters->parameters[3]);
				p3_string=temp_string;
			}
			else
			{
				// Fit failed

				p0_string="Fit failed";
				p1_string="Fit failed";
				p2_string="Fit failed";
				p3_string="Fit failed";

				fit_successful=0;
			}
		}
		break;

		case TWO_LINES:
		{
			p_analysis_fit_parameters->no_of_fit_parameters=2;

			if (p_analysis_fit_parameters->two_lines())
			{
				sprintf(temp_string,"%6.4e",p_analysis_fit_parameters->parameters[0]);
				p0_string=temp_string;
				sprintf(temp_string,"%6.4e",
					p_analysis_fit_parameters->parameters[1]);
				p1_string=temp_string;

				p2_string=default_string;
				p3_string=default_string;
			}
			else
			{
				// Fit failed

				p0_string="Fit failed";
				p1_string="Fit failed";
				p2_string="Fit failed";
				p3_string="Fit failed";

				fit_successful=0;
			}
		}
		break;

		default:
		{
			MessageBox("Invalid function selected in Fit Control");
		}
		break;
	}

	// Calculate fit if successful

	if (fit_successful)
	{
		p_analysis_fit_parameters->calculate_r_squared();
		sprintf(temp_string,"%.4f",p_analysis_fit_parameters->r_squared);
		r_squared_string=temp_string;
	}
	else
	{
		sprintf(temp_string,"Failed");
		r_squared_string=temp_string;
	}

	// Draw Fit
	
	p_parent_analysis_display->draw_fit(p_analysis_fit_parameters,MAGENTA);

	// End wait cursor

	EndWaitCursor();

	// Update screen

	x_rogue_points=p_analysis_fit_parameters->x_rogue_points;
	y_rogue_points=p_analysis_fit_parameters->y_rogue_points;

	UpdateData(FALSE);

	// Warn of rogue points

	if ((x_rogue_points>0)||(y_rogue_points>0))
	{
		MessageBeep(0xFFFFFFFF);
	}
}

void Fit_control::OnSelchangePlotList() 
{
	// Update active fit and set string default values if selection has changed

	if (plot_list_control.GetCurSel()!=current_plot)
	{
		p_parent_analysis_display->fit_active=FALSE;
		p_parent_analysis_display->update_plots();

		p0_string=default_string;
		p1_string=default_string;
		p2_string=default_string;
		p3_string=default_string;
		r_squared_string=default_string;
	}

	// Update screen

	UpdateData(FALSE);

	// Update current_plot

	current_plot=plot_list_control.GetCurSel();
}

void Fit_control::bypass_to_fit()
{
	OnFitButton();
}

BOOL Fit_control::PreTranslateMessage(MSG* pMsg) 
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
