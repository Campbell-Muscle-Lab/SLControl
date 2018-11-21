#if !defined(AFX_FIT_CONTROL_H__E3CDFA29_2E76_46D0_BD18_6793AD7F930A__INCLUDED_)
#define AFX_FIT_CONTROL_H__E3CDFA29_2E76_46D0_BD18_6793AD7F930A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Fit_control.h : header file
//

////////////////////////////
// Ken's code starts here //
////////////////////////////

#include "global_definitions.h"
#include "Fit_parameters.h"

////////////////////////////
//  Ken's code ends here  //
////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Fit_control dialog

class Analysis_display;		// forward declaration
class Plot;					// forward declaration
class Fit_parameters;

class Fit_control : public CDialog
{
// Construction
public:
	Fit_control(CWnd* pParent = NULL,
		Analysis_display* set_p_parent_analysis_display = NULL,
		Fit_parameters* set_p_analysis_fit_parameters = NULL,
		int set_no_of_plots=1, Plot* set_p_plot[] = NULL);

	Fit_control::~Fit_control(void);

	BOOL Fit_control::Create();

	CWnd* m_pParent;
	int m_nID;


// Dialog Data
	//{{AFX_DATA(Fit_control)
	enum { IDD = IDD_FIT_CONTROL };
	CButton	y_filter_control;
	CButton	x_filter_control;
	CComboBox	plot_list_control;
	CComboBox	function_list_control;
	CString	fit_function_string;
	CString	p0_string;
	CString	p1_string;
	CString	p2_string;
	CString	p3_string;
	CString	r_squared_string;
	BOOL	x_spike_filter;
	int		x_rogue_points;
	BOOL	y_spike_filter;
	int		y_rogue_points;
	//}}AFX_DATA

	Analysis_display* p_parent_analysis_display;
	Fit_parameters* p_analysis_fit_parameters;
	
	int no_of_plots;
	Plot* p_plot[MAX_NO_OF_PLOTS];
		
	// Relative window size and position

	float window_x_pos;
	float window_y_pos;
	float window_x_size;
	float window_y_size;

	// Control strings

	CString mean_value_string;
	CString linear_regression_string;
	CString robust_linear_string;
	CString single_exponential_string;
	CString double_exponential_string;
	CString two_lines_string;

	CString mean_value_function_string;
	CString linear_regression_function_string;
	CString robust_linear_function_string;
	CString single_exponential_function_string;
	CString double_exponential_function_string;
	CString two_lines_function_string;

	CString default_string;

	// Last selected plot

	int current_plot;

	// Fit variables

	int no_of_parameters;
	float r_squared;

	// Functions

	void close_fit_control_window(void);

	void bypass_to_fit(void);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Fit_control)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Fit_control)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeFunctionList();
	afx_msg void OnFitButton();
	afx_msg void OnSelchangePlotList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIT_CONTROL_H__E3CDFA29_2E76_46D0_BD18_6793AD7F930A__INCLUDED_)
