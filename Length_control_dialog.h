#pragma once

// Length_control_dialog dialog

#include "Experiment_display.h"
#include "Calibration.h"
#include "Length_control_sheet.h"
#include "LC_ramp_page.h"
//#include "LC_ktr_page.h"
#include "LC_ktr_step_page.h"
#include "LC_triggered_page.h"
#include "LC_dout_page.h"
#include "LC_freeform_page.h"
#include "Data_record.h"
#include "Plot.h"
#include "LC_summary_page.h"

#include "Abort_multiple_trials.h"

#include "afxcmn.h"
#include "afxwin.h"

UINT length_control_dialog_delay_as_thread(LPVOID set_pointer);
UINT length_control_dialog_run_trial_thread(LPVOID set_pointer);
UINT length_control_dialog_run_freeform_trial_thread(LPVOID set_pointer);

class Length_control_dialog : public CDialog
{
	DECLARE_DYNAMIC(Length_control_dialog)

public:
	Length_control_dialog(CWnd* pParent = NULL);   // standard constructor

	//virtual ~Length_control_dialog();

	Length_control_dialog::~Length_control_dialog();
	BOOL Length_control_dialog::Create();

	CWnd* m_pParent;									// Pointer to parent window
	int m_nID;

	// Pointers to associated windows

	Calibration* p_Calibration;
	Experiment_display* p_Experiment_display;

	// Data records and pointers to them

	Data_record Length_control_record;
	Data_record* p_Length_control_record;

	Data_record Preview_record;							// Preview record
	Data_record* p_Preview_record;

	// Preview Plot

	Plot Preview_plot;
	Plot* p_Preview_plot;

	// Pointer to Length_control_sheet

	int no_of_tabs;
	Length_control_sheet* p_lc_sheet;
	LC_ramp_page* p_lc_ramp_page;
	LC_triggered_page* p_lc_triggered_page;
	LC_dout_page* p_lc_dout_page;
	LC_freeform_page* p_lc_freeform_page;
	LC_ktr_step_page* p_lc_ktr_step_page;
	LC_summary_page* p_lc_summary_page;

	// File strings

	CString DAPL_source_file_string;

	CString * lc_freeform_file_string;
	int * lc_freeform_total_sample_points;
	int * lc_freeform_no_of_channels;
	float * lc_freeform_time_increment;

	// Status strings and counter for user display

	CString status_top_string;
	CString status_middle_string;
	CString status_bottom_string;
	int status_counter;

	// LC variables

	//int lc_ktr_size;

	int trials_left;											// number of trials left to perform
	int no_of_trials;

	int lc_trials;

	int ktr_initiation_points;

	clock_t start_time,stop_time;

	// Abort Thread

	CWinThread* p_delay_thread;
	CWinThread* p_run_trial_thread;
	int thread_status;											// used to communicate betweeen worker_threads
																// >0 means run or is running an experiment
																// <0 means delay between experiments

	CWinThread* p_thread;
	Abort_multiple_trials* p_amt;

	int trial_status;
	int previous_trial_status;

	int abort_signal;
	int close_abort_window;

	float lc_pre_ramp_ms;
	float lc_risetime_ms;

	float lc_pre_ktr_ms;
	float lc_ktr_duration_ms;

	float sampling_rate;
	float time_increment;
	float duration_seconds;

	int lc_no_of_input_channels;

	int lc_dialog_max_trials;
	float* lc_dialog_pre_ms_array;
	float* lc_dialog_rel_size_array;
	float* lc_dialog_rel_time_array;
	float* lc_dialog_delay_array;
	float* lc_dialog_ktr_rel_size_array;
	float* lc_dialog_ktr_rel_duration_array;
	float* lc_dialog_ktr_pre_array;

	int max_triggered_points;
	int triggered_points;
	float * p_triggered_FL;

	float ktr_initiation_time_s;

	// File Strings

	CString output_data_file_string;

	// Cursor

	HCURSOR hcurWait;
	HCURSOR hcurSave;
		
	// Functions

	int check_trial_status(void);

	int run_single_trial(void);
	int run_freeform_trial(void);
	void write_data_to_file(void);

	void update_preview_record(void);
	void update_preview_plot(void);

	int create_DAPL_source_file(void);

	void display_status(int update_mode,CString new_string);	// updates user display


	void remote_shut_down(void);								// called by parent to close window

	// MessageHandlers

// Dialog Data
	enum { IDD = IDD_LENGTH_CONTROL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);			// DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void PostNcDestroy();
public:
	afx_msg void OnClose();
	// Length control data directory string

	CComboBox SAMPLING_RATE;
	CString sampling_rate_string;
	
	int data_points;
	
	CString duration_string;

	CString LC_data_path_string;
	CEdit DATA_BASE;
	CString data_base_string;
	CEdit DATA_INDEX;
	int data_index;
	float inter_trial_interval_s;

	afx_msg void OnBnClickedPathBrowse();
	afx_msg void OnBnClickedRunExperiment();
	afx_msg void OnBnClickedTest();
protected:
public:	
	CButton ABORT_BUTTON;
	afx_msg void OnEnChangeDataPoints();
	afx_msg void OnCbnSelchangeSamplingRate();
	afx_msg void OnPaint();
	float lc_dialog_SL_volts_to_FL_microns_calibration;
	float lc_dialog_SL_volts_to_FL_command_volts;
};
