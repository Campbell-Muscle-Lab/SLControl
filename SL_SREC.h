#if !defined(AFX_SL_SREC_H__BB278114_C7A5_4B17_87D0_2672EE100D7F__INCLUDED_)
#define AFX_SL_SREC_H__BB278114_C7A5_4B17_87D0_2672EE100D7F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SL_SREC.h : header file
//

////////////////////////////
// Ken's code starts here //
////////////////////////////

#include "Calibration.h"
#include "Experiment_display.h"

//#include "SLControlDlg.h"


////////////////////////////
//  Ken's code ends here  //
////////////////////////////

// Thread functions

UINT run_tri_experiment_as_thread(LPVOID p_SL_SREC);
UINT run_experiment_as_thread(LPVOID p_SL_SREC);
UINT run_step_experiment_as_thread(LPVOID p_SL_SREC);
UINT delay_as_thread(LPVOID p_SL_SREC);


/////////////////////////////////////////////////////////////////////////////
// SL_SREC dialog

class SL_SREC : public CDialog
{
// Construction
public:
	SL_SREC::SL_SREC(CWnd* pParent /*= NULL*/);
	SL_SREC::~SL_SREC();
	BOOL SL_SREC::Create();

	CWnd* m_pParent;									// Pointer to parent window
	int m_nID;

	float window_width,window_height;					// dimensions in pixels of
														// SL_SREC window


	SL_SREC* p_SL_SREC;									// Points to SL_SREC object

	// Pointers to associated windows

	Calibration* p_Calibration;
	Experiment_display* p_Experiment_display;

	// Data record and pointer to it

	Data_record SL_SREC_record;
	Data_record* p_SL_SREC_record;

	// File string for data file

	CString output_data_file_string;
	
	// File strings for dapl_source_file components

	CString SL_SREC_dir_path;							// Directory path

	CString header_file_string;
	CString fill_pipes_file_string;
	CString create_pipes_file_string;
	CString control_file_string;
	CString io_procedures_file_string;
	CString DAPL_source_file_string;

	// Status strings and counter for user display

	CString status_top_string;
	CString status_middle_string;
	CString status_bottom_string;
	int status_counter;

	// Strings for batch files

	CString single_trial_string;
	CString delay_trials_string;
	CString size_trials_string;
	CString velocity_trials_string;
	CString all_trials_string;
	CString step_trials_string;


	// Variables

	float post_ktr_ms;												// post_ktr_ms

	float post_ktr_final_fl_ms;										// length of final segment of fl control
								
	float duration_seconds;											// record duration (s)

	float sampling_rate;											// single channel sampling rate (Hz)

	float time_increment;											// time increment (s) between
																	// consecutive data points on an
																	// individual channel

	int no_of_triangles;											// no of triangles (0-3)

	int clamp_mode;													// SL clamp mode
																	// (0 = prevailing postion)
																	// (1 = zero volts)

	int post_ktr_servo_mode;										// post KTR SL control mode
																	// (0 = FL control)
																	// (1 = SL control)

	int pre_ktr_servo_mode;											// pre KTR SL control mode
																	// (0 = FL control)
																	// (1 = SL control)

	int SL_calibration;												// 0 if not yet calibrated
																	// 1 otherwise

	int ramp_mode;													// 0 if triangles (default)
																	// 1 if ramp option selected

	int ramp_increment_mode;										// 0 if ramps reset to zero
																	// 1 if ramps build on each other

	int experiment_mode;											// current experiment mode
																	// SL_SREC_SINGLE_TRIAL,
																	// SL_SREC_DELAY_TRIALS,
																	// SL_SREC_SIZE_TRIALS or
																	// SL_SREC_VELOCITY_TRIALS

	int parameters_valid;											// 1 if experiment parameters are valid
																	// -1 if length valid but not calibrated
																	// -2 if length invalid but calibrated

	int previous_parameters_valid;									// used as a flag to indicate whether
																	// parameters_valid has changed


	float relative_first_tri_size;									// relative size of first triangle

	float ktr_initiation_time_ms;									// time at beginning of ktr step in ms

	float first_step_ms,first_step_microns;
	float second_step_ms,second_step_microns;

	float* pipe_fl;
	short* pipe_overflow;
	int overflow_threshold;
	int overflow_points;
    
	int thread_running;
	int current_trial;
	int no_of_trials;
	int thread_problem;

	// Thread stuff

	CWinThread* m_pThread;

	// Functions
	 
	void run_single_trial(void);									// runs a single trial


	void display_status(int update_mode,CString new_string);		// user display
	void write_data_to_file(void);									// writes data to file

	void set_post_ktr_values(void);									// calculates post_ktr_ms (ms after ktr step)
																	// post_ktr_final_fl_ms (ms under fl control
																	// at the end of the trial)

	void set_no_of_triangles(void);									// sets no_of_triangles from control string

	int convert_time_ms_to_sampling_points(float time_ms);			// returns integer number of sampling points
																	// for a given ms time interval
	void set_sampling_parameters(void);								// sets time increment, sampling_rate and
																	// and duration_seconds
	float return_pre_ktr_ms(void);									// returns pre_ktr_ms

	void set_parameters_valid(void);								// sets parameters_valid to TRUE or
																	// FALSE depending on whether experiment
																	// has plausible parameters

	int create_DAPL_source_file(void);								// returns 1 if file was created successfully
																	// 0 otherwise

	int create_step_DAPL_source_file(void);

	int write_header_file(void);									// create components of DAPL source file
	int write_fill_pipes_file(void);								// all return 1 if successful
	int write_create_pipes_file(void);
	int write_control_file(void);
	int write_io_procedures_file(void);

	void remote_shut_down(void);									// called by parent to shut down window

	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////

	// Dialog Data
	//{{AFX_DATA(SL_SREC)
	enum { IDD = IDD_SL_SREC_Dialog };
	CComboBox	experiment_mode_control;
	CEdit	post_ktr_final_fl_ms_control;
	CEdit	post_ktr_ms_control;
	CEdit	duration_control;
	CComboBox	no_of_triangles_control;
	CComboBox	SAMPLING_RATE_CONTROL;
	CComboBox	SERVO_MODE_CONTROL;
	float	SL_SREC_SL_volts_to_FL_microns_calibration;
	float	ktr_step_size_microns;
	float	ktr_step_duration_ms;
	CString	servo_mode_string;
	CString	clamp_mode_string;
	CString	sampling_rate_string;
	int		data_points;
	CString	duration_string;
	float	triangle_size_microns;
	float	triangle_halftime_ms;
	int		pre_servo_points;
	float	proportional_gain;
	CString	data_path_string;
	CString	data_base_string;
	int		data_index;
	float	pre_triangle_ms;
	CString	batch_file_string;
	float	inter_trial_interval_seconds;
	float	inter_triangle_interval_ms;
	float	ktr_slcontrol_duration_ms;
	BOOL	ktr_slcontrol;
	CString	no_of_triangles_string;
	float	pre_ktr_ms;
	CString	post_ktr_ms_string;
	float	post_ktr_ms_fl;
	CString	post_ktr_final_fl_ms_string;
	float	SL_volts_to_FL_command_volts;
	CString	experiment_mode_string;
	BOOL	ramp_option;
	BOOL	ramp_increment_option;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SL_SREC)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SL_SREC)
	afx_msg void OnRunExperiment();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnSLtoFLCalibrationOverride();
	afx_msg void OnSelchangeServoMode();
	afx_msg void OnSelchangeSamplingRate();
	afx_msg void OnChangeDataPoints();
	afx_msg void OnKtrSlcontrol();
	afx_msg void OnSelchangeNoOfTriangles();
	afx_msg void OnChangeInterTriangleInterval();
	afx_msg void OnChangeKtrStepDuration();
	afx_msg void OnChangeKtrSlcontrolDurationMs();
	afx_msg void OnChangePostKtrFl();
	afx_msg void OnChangePreKtr();
	afx_msg void OnChangeTriHalftime();
	afx_msg void OnChangePreTri();
	afx_msg void OnPaint();
	afx_msg void OnSelchangeExptMode();
	afx_msg void OnRampControl();
	afx_msg void OnRampIncrementControl();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnAbort();
	afx_msg void OnBnClickedBrowsePath();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SL_SREC_H__BB278114_C7A5_4B17_87D0_2672EE100D7F__INCLUDED_)
