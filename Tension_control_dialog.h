#if !defined(AFX_TENSION_CONTROL_DIALOG_H__F937FDE1_4C6E_4121_B7A8_B6ED7BF83052__INCLUDED_)
#define AFX_TENSION_CONTROL_DIALOG_H__F937FDE1_4C6E_4121_B7A8_B6ED7BF83052__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Tension_control_dialog.h : header file
//

////////////////////////////
// Ken's code starts here //
////////////////////////////

#include "Experiment_display.h"
#include "Calibration.h"

////////////////////////////
//  Ken's code ends here  //
////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Tension_control_dialog dialog

class Tension_control_dialog : public CDialog
{
// Construction
public:
	Tension_control_dialog(CWnd* pParent);   // standard constructor

	Tension_control_dialog::~Tension_control_dialog();
	BOOL Tension_control_dialog::Create();

	CWnd* m_pParent;									// Pointer to parent window
	int m_nID;


// Dialog Data
	//{{AFX_DATA(Tension_control_dialog)
	enum { IDD = IDD_TENSION_CONTROL };
	CComboBox	no_of_triangles_control;
	CComboBox	EXPERIMENT_MODE_CONTROL;
	CEdit	DURATION_CONTROL;
	CComboBox	SAMPLING_RATE_CONTROL;
	int		data_points;
	float	isotonic_level_volts;
	float	proportional_gain;
	CString	sampling_rate_string;
	CString	duration_string;
	float	ktr_step_size_microns;
	float	ktr_step_duration_ms;
	float	pre_servo_ms;
	CString	post_ktr_ms_string;
	float	servo_ms;
	int		data_index;
	CString	data_base_string;
	CString	data_path_string;
	float	SL_volts_to_FL_microns_calibration;
	float	SL_volts_to_FL_COMMAND_volts;
	CString	batch_file_string;
	float	inter_trial_interval_seconds;
	CString	experiment_mode_string;
	float	triangle_halftime_ms;
	float	triangle_size_volts;
	CString	no_of_triangles_string;
	float	post_triangle_ms;
	float	pre_triangle_ms;
	int		pre_tension_servo_points;
	float	inter_pull_interval_ms;
	BOOL	ramp_increment_option;
	BOOL	ramp_option;
	float	integral_gain;
	//}}AFX_DATA

	////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	Tension_control_dialog* p_Tension_control_dialog;		// points to Tension_control object

	// Pointers to associated windows

	Calibration* p_Calibration;
	Experiment_display* p_Experiment_display;

	// Data record and a pointer to it

	Data_record Tension_control_record;
	Data_record* p_Tension_control_record;

	// File strings

	CString output_data_file_string;						// string for output data file

	CString Tension_control_dir_path;						// DAPL source file path
	CString DAPL_source_file_string;						// DAPL source file string
	CString header_file_string;								// Source file components
	CString fill_pipes_file_string;
	CString create_pipes_file_string;
	CString control_file_string;
	CString io_procedures_file_string;

	CString force_velocity_file_string;						// Default files
	CString batch_pulls_file_string;
	CString batch_velocity_file_string;

	// Variables

	float duration_seconds;									// record duration (s)

	float sampling_rate;									// single channel sampling rate (Hz)

	float time_increment;									// time increment (s) between consecutive
															// data points on an individual channel

	float post_ktr_ms;										// time (ms) after ktr step

	int no_of_triangles;									// no of tension triangles or ramps

	float ktr_initiation_time_ms;							// time at beginning of ktr step

	// Status strings and counter for user display

	CString status_top_string;
	CString status_middle_string;
	CString status_bottom_string;
	int status_counter;

	// Modes

	int experiment_mode;									// current experimental_mode
															// SINGLE_HOLD, FORCE_VELOCITY,
															// SINGLE_PULL or BATCH_PULLS
															// (defined in Global_definitions.h

	int ramp_mode;											// 0 if triangles (default)
															// 1 if ramp option selected

	int ramp_increment_mode;								// 0 if ramps reset to zero
															// 1 if ramps build on each other

	float ramp_multiplier;									// 1 if a single ramp
															// 2 if a triangle

	// Functions

	void set_sampling_parameters(void);						// sets time increment, sampling rate and
															// duration_seconds

	int convert_time_ms_to_sampling_points(float time_ms);	// returns no of sample points in a given
															// time interval

	void set_post_ktr_values(void);							// sets post_ktr_ms

	void set_ramp_multiplier(void);							// set ramp_multiplier

	void run_single_trial(void);							// executes DAPL_source_file and thus
															// runs a single experiment

	void write_data_to_file(void);							// writes data_record to file

	int create_DAPL_source_file(void);						// returns 1 if successful, 0 otherwise
	
	int write_header_file(void);							// all functions create sub_files
	int write_fill_pipes_file(void);						// and return 1 if successful,
	int write_create_pipes_file(void);						// 0 otherwise
	int write_control_file(void);
	int write_io_procedures_file(void);

	void display_status(int update_mode,CString new_string);
															// updates status display

	void remote_shut_down(void);							// called by parent to shut down window


	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Tension_control_dialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Tension_control_dialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnRunExperiment();
	afx_msg void OnSelchangeSamplingRate();
	afx_msg void OnChangeDataPoints();
	afx_msg void OnChangeKtrStepDuration();
	afx_msg void OnChangeServo();
	afx_msg void OnChangePreServo();
	afx_msg void OnSLtoFLCalibrationOverride();
	afx_msg void OnSelchangeExptMode();
	afx_msg void OnSelchangeNoOfTriangles();
	afx_msg void OnPaint();
	afx_msg void OnRampControl();
	afx_msg void OnChangePreTri();
	afx_msg void OnChangePostTri();
	afx_msg void OnChangeInterPullInterval();
	afx_msg void OnChangeTriHalftime();
	afx_msg void OnRampIncrementControl();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TENSION_CONTROL_DIALOG_H__F937FDE1_4C6E_4121_B7A8_B6ED7BF83052__INCLUDED_)
