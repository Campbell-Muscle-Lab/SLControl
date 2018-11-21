#if !defined(AFX_CHIRP_H__4088AEF9_8FA3_4F48_8436_F69DCF2B8A48__INCLUDED_)
#define AFX_CHIRP_H__4088AEF9_8FA3_4F48_8436_F69DCF2B8A48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Chirp.h : header file
//

#include "Calibration.h"
#include "Experiment_display.h"

/////////////////////////////////////////////////////////////////////////////
// Chirp dialog

class Chirp : public CDialog
{
// Construction
public:
	Chirp(CWnd* pParent = NULL);   // standard constructor

	Chirp::~Chirp();									// Destructor
	BOOL Chirp::Create();

	CWnd* m_pParent;
	int m_nID;

	// Pointers to associated windows

	Calibration* p_Calibration;
	Experiment_display* p_Experiment_display;

	// Data record and a pointer to it

	Data_record Chirp_record;
	Data_record* p_Chirp_record;

	// Variables

	// Sampling parameters

	float sampling_rate;											// sampling rate per channel in Hz

	float time_increment;											// time interval in seconds between
																	// consecutive data points

	float duration_seconds;											// record duration in seconds

	float ktr_initiation_time_ms;									// ktr initiation time in ms

	// Modes and calibration status

	int SL_calibration;												// 0 if not yet calibrated
																	// 1 if SL is calibrated

	int length_servo_mode;											// 0 = FL_Control
																	// 1 = SL_Control

	int overflow_status;											// 0 if all data points < 65000
																	// (all data points before procedures in
																	// source file)
																	// 1 otherwise, when data points are before
																	// and after procedures in source file

	int overflow_threshold;											// maximum number of points before
																	// overflow file is invoked

	int clamp_mode;													// SL control mode
																	// 0 = prevailing position
																	// 1 = zero volts

	int chirp_sweep_mode;											// Chirp sweep mode
																	// LOGARITHMIC_CHIRP = 0
																	// LINEAR_CHIRP = 1
																	// Defined in global_definitions.h

	int parameters_valid;											// 1 if experiment parameters are valid
																	// -1 if length valid but not calibrated
																	// -2 if length invalid but calibrated

	int previous_parameters_valid;									// used as a flag to indicate whether
																	// parameters_valid has changed

	// DAPL source file and components

	CString output_data_file_string;

	CString header_file_string;
	CString fill_pipes_file_string;
	CString create_pipes_file_string;
	CString control_file_string;
	CString io_procedures_file_string;
	CString overflow_file_string;
	CString DAPL_source_file_string;

	CString Chirp_dir_path;
	
	// DAPL source_code buffer

	char* code_buffer;
	int buffer_characters;

	// Status strings and counter for user display

	CString status_top_string;
	CString status_middle_string;
	CString status_bottom_string;
	int status_counter;

	// Pipes

	float* pipe_servo;
	float* pipe_offset;
	float* pipe_fl;
	float* pipe_target;

	// Functions

	void remote_shut_down(void);								// called by parent to shut down window

	void display_status(int update_mode,CString new_string);	// user display

	void fill_code_buffer(void);								// fills code buffer

	void create_pipes(void);

	float return_triangle(float position);						// returns a y-value for a triangle of unit
																// amplitude repeating with period 2.0*PI

	float return_square(float position);						// returns a y-value for a square wave of
																// unit amplitude repeating with a period of
																// 2.0*PI

	void set_parameters_valid(void);							// sets parameters_valid to TRUE or
																// FALSE depending on whether experiment
																// has plausible parameters

	void set_sampling_parameters(void);							// sets sampling rate, time_increment and
																// duration seconds from control

	void set_post_ktr_ms(void);									// sets post_ktr_ms

	int convert_time_ms_to_sampling_points(float time_ms);		// converts a time in ms to a number of
																// sampling points

	void write_data_to_file(void);								// writes data to file

	// DAPL source file functions

	int create_DAPL_source_file(void);							// creates DAPL source file

	int write_header_file(void);								// create components of DAPL source file
	int write_fill_pipes_file(void);							// all return 1 if successful
	int write_create_pipes_file(void);
	int write_control_file(void);
	int write_io_procedures_file(void);
	int write_overflow_file(void);

// Dialog Data
	//{{AFX_DATA(Chirp)
	enum { IDD = IDD_CHIRP };
	CComboBox	CHIRP_SWEEP_CONTROL;
	CComboBox	LENGTH_SERVO_MODE_CONTROL;
	CComboBox	CLAMP_MODE_CONTROL;
	CComboBox	LENGTH_FUNCTION_CONTROL;
	CComboBox	SAMPLING_RATE_CONTROL;
	CString	sampling_rate_string;
	int		data_points;
	CString	duration_string;
	float	post_ktr_ms;
	float	length_duration_ms;
	float	begin_period_ms;
	float	end_period_ms;
	float	mid_period_ms;
	float	pre_length_ms;
	float	length_size_microns;
	float	length_offset_microns;
	CString	length_function_string;
	CString	clamp_mode_string;
	int		pre_servo_points;
	float	proportional_gain;
	float	SL_volts_to_FL_command_volts;
	float	SL_volts_to_FL_microns_calibration;
	float	ktr_step_duration_ms;
	float	ktr_step_size_microns;
	float	pre_ktr_ms;
	CString	length_servo_mode_string;
	CString	chirp_sweep_string;
	float	chirp_mid_position;
	CString	data_base_string;
	CString	data_path_string;
	int		data_index;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Chirp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Chirp)
	afx_msg void OnRunExperiment();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnClose();
	afx_msg void OnPaint();
	afx_msg void OnSelchangeSamplingRate();
	afx_msg void OnChangeDataPoints();
	afx_msg void OnSelchangeChirpSweep();
	afx_msg void OnSLtoFLCalibration();
	afx_msg void OnSelchangeServoMode();
	afx_msg void OnChangeKtrStepDuration();
	afx_msg void OnChangePreKtr();
	afx_msg void OnChangeLengthDuration();
	afx_msg void OnChangePreLength();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHIRP_H__4088AEF9_8FA3_4F48_8436_F69DCF2B8A48__INCLUDED_)
