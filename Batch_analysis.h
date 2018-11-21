#if !defined(AFX_BATCH_ANALYSIS_H__8FC87750_C1BB_499B_84B4_F962EDDC0973__INCLUDED_)
#define AFX_BATCH_ANALYSIS_H__8FC87750_C1BB_499B_84B4_F962EDDC0973__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Batch_analysis.h : header file
//

#include "Data_record.h"
#include "Analysis_display.h"

/////////////////////////////////////////////////////////////////////////////
// Batch_analysis dialog

class Batch_analysis : public CDialog
{
// Construction
public:
	Batch_analysis(CWnd* pParent = NULL);						// standard constructor
	~Batch_analysis(void);										// destructor
	BOOL Create();												// create window


	CWnd* m_pParent;											// pointer to parent window
	int m_nID;													// dialog ID number

	// Data record and analysis_display window

	Data_record batch_record;									// data record
	Data_record* p_batch_record;								// Pointer to the data record
													
	Analysis_display* p_batch_display;							// Pointer to an Analysis_display window

	// Window position and dimensions

	float window_x_pos;
	float window_y_pos;
	float window_x_size;
	float window_y_size;

	// Control variables

	int no_of_experiment_modes;									// number of possible experiment modes
	CString experiment_mode_string[10];							// strings defining possible experiment modes
	int current_experiment_mode;								// current mode

	int no_of_analysis_options;									// number of active analysis options
	CString analysis_option_string[10];							// strings defining possible options
	int current_analysis_option;								// current option

	// Function strings

	CString linear_regression_string;
	CString robust_linear_string;
	CString single_exponential_string;
	CString double_exponential_string;

	// Analysis parameters

	struct parameters_structure
	{
		int no_of_parameters;
		float parameter[5];
	};

	parameters_structure analysis_parameters;

	void remote_shut_down(void);									// called by parent to shut down window


// Dialog Data
	//{{AFX_DATA(Batch_analysis)
	enum { IDD = IDD_BATCH_ANALYSIS };
	CSpinButtonCtrl	spin_stretch_number_control;
	CComboBox	function_list_control;
	CComboBox	analysis_options_control;
	CComboBox	experiment_mode_control;
	CString	list_file_string;
	CString	output_file_string;
	int		ktr_skip_points;
	int		fit_points;
	int		stretch_number;
	int		fv_last_pre_ktr_points;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Batch_analysis)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Ken's functions

	void isometric_tension_calculation(float ramp_multiplier);

	void ktr_calculation(float ramp_multiplier, int ktr_function);

	void stiffness_calculations(float ramp_multiplier);

	void force_velocity_calculations(void);

	void relative_pre_stretch_tension_calculation(float ramp_multiplier);

	void peak_tension_calculations(float ramp_multiplier);

	// Generated message map functions
	//{{AFX_MSG(Batch_analysis)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnSelchangeExperimentMode();
	afx_msg void OnListFileBrowse();
	afx_msg void OnOutputFileBrowse();
	afx_msg void OnAnalyse();
	afx_msg void OnSelchangeAnalysisOptions();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BATCH_ANALYSIS_H__8FC87750_C1BB_499B_84B4_F962EDDC0973__INCLUDED_)
