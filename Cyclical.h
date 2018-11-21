#if !defined(AFX_CYCLICAL_H__EDDC8BBE_71E6_4F08_8381_46D4277FE2B0__INCLUDED_)
#define AFX_CYCLICAL_H__EDDC8BBE_71E6_4F08_8381_46D4277FE2B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Cyclical.h : header file
//

#include "Calibration.h"
#include "Experiment_display.h"


/////////////////////////////////////////////////////////////////////////////
// Cyclical dialog

class Cyclical : public CDialog
{
// Construction
public:
	Cyclical(CWnd* pParent = NULL);   // standard constructor

	Cyclical::~Cyclical();										// Destructor
	BOOL Cyclical::Create();	

	CWnd* m_pParent;
	int m_nID;

	// Pointers to associated windows

	Calibration* p_Calibration;
	Experiment_display* p_Experiment_display;

	// Data record and pointer to it

	Data_record Cyclical_record;
	Data_record* p_Cyclical_record;

	// File strings

	// Data output

	CString output_data_file_string;

	// DAPL source file and components

	CString Cyclical_dir_path;

	CString header_file_string;
	CString fill_pipes_file_string;
	CString create_pipes_file_string;
	CString control_file_string;
	CString io_procedures_file_string;
	CString DAPL_source_file_string;

	// Sampling parameters

	float time_increment;											// time increment in s between consecutive
																	// data points

	float sampling_rate;											// sampling rate per channel in Hz

	float ktr_initiation_time_ms;									// ktr initiation time in ms


	// Modes and calibration status

	int SL_calibration;												// 0 if not yet calibrated
																	// 1 if SL is calibrated

	int post_ktr_servo_mode;										// post KTR SL control mode
																	// 0 = FL_CONTROL
																	// 1 = SL_CONTROL

	int length_function_mode;										// function id for length change
																	// 0 = sine wave
																	// 1 = cos wave
																	// 2 = triangle

	int length_servo_mode;											// Length function servo mode
																	// 0 = FL_CONTROL
																	// 1 = SL_CONTROL

	int clamp_mode;													// SL control mode
																	// 0 = prevailing position
																	// 1 = zero volts

	int parameters_valid;											// 1 if experiment parameters are valid
																	// -1 if length valid but not calibrated
																	// -2 if length invalid but calibrated
																	
	int previous_parameters_valid;									// used as a flag to indicate if experiment
																	// validity has changed

			
	// Status strings and counter for user display

	CString status_top_string;
	CString status_middle_string;
	CString status_bottom_string;
	int status_counter;

	// Functions
	
	void remote_shut_down(void);									// called by parent to shut down window

	void write_data_to_file(void);									// writes current data record to file

	void set_sampling_parameters(void);								// sets sampling_rate, time_increment
																	// and duration_seconds

	void set_post_ktr_ms(void);										// sets post_ktr_ms

	int convert_time_ms_to_sampling_points(float time_ms);			// converts a time in ms to a number
																	// of sampling points

	void set_parameters_valid(void);								// sets parameters_valid to TRUE or
																	// FALSE depending on whether experiment
																	// has plausible parameters

	void display_status(int update_mode,CString new_string);		// user display

	int create_DAPL_source_file(void);								// returns 1 if successful, 0 otherwise

	int write_header_file(void);									// create components of DAPL source file
	int write_fill_pipes_file(void);								// all return 1 if successful
	int write_create_pipes_file(void);
	int write_control_file(void);
	int write_io_procedures_file(void);

	int fast_sampling_DAPL_source_file(void);

// Dialog Data
	//{{AFX_DATA(Cyclical)
	enum { IDD = IDD_CYCLICAL };
	CComboBox	CLAMP_MODE_CONTROL;
	CComboBox	LENGTH_SERVO_MODE_CONTROL;
	CComboBox	LENGTH_FUNCTION_CONTROL;
	CComboBox	SAMPLING_RATE_CONTROL;
	CString	data_path_string;
	CString	data_base_string;
	int		data_index;
	CString	sampling_rate_string;
	int		data_points;
	float	duration_seconds;
	float	SL_volts_to_FL_command_volts;
	float	SL_volts_to_FL_microns_calibration;
	float	ktr_step_size_microns;
	float	ktr_slcontrol_duration_ms;
	float	ktr_step_duration_ms;
	BOOL	ktr_slcontrol;
	float	ktr_fl_settle_ms;
	float	pre_ktr_ms;
	float	pre_length_ms;
	float	length_size_microns;
	float	period_ms;
	float	phase_degrees;
	CString	length_function_string;
	float	proportional_gain;
	CString	length_servo_mode_string;
	float	post_ktr_ms;
	float	length_duration_ms;
	CString	clamp_mode_string;
	int		pre_servo_points;
	float	post_ktr_final_fl_ms;
	float	length_offset_microns;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Cyclical)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Cyclical)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnCancel();
	afx_msg void OnPaint();
	afx_msg void OnSelchangeSamplingRate();
	afx_msg void OnChangeDataPoints();
	afx_msg void OnSLtoFLCalibration();
	afx_msg void OnKtrSlcontrol();
	afx_msg void OnSelchangeServoMode();
	afx_msg void OnChangeLengthDuration();
	afx_msg void OnRunExperiment();
	afx_msg void OnChangeKtrFlSettle();
	afx_msg void OnChangeKtrSlcontrolDuration();
	afx_msg void OnChangeKtrStepDuration();
	afx_msg void OnChangePreKtr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CYCLICAL_H__EDDC8BBE_71E6_4F08_8381_46D4277FE2B0__INCLUDED_)
