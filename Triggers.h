#pragma once
#include "afxcmn.h"

#include "Channel_control.h"
#include "afxwin.h"

#include "Calibration.h"
#include "Experiment_display.h"


// Triggers dialog

class Triggers : public CDialog
{
	DECLARE_DYNAMIC(Triggers)

	BOOL Triggers::Create();
	CWnd* m_pParent;							// Pointer to parent window
	int m_nID;

	// Pointers to associated windows

	Calibration* p_Calibration;
	Experiment_display* p_Experiment_display;

	// Data record and pointer to it

	Data_record triggers_record;
	Data_record* p_triggers_record;

	// Channel control - contains objets of class Channel_tab

	Channel_control* p_channel_control;

	// Experiment parameters

	float sampling_rate;
	float time_increment;
	float duration_seconds;

	// Parameter status values

	int experiment_status;						// 0 is valid
	int previous_experiment_status;

	int SINGLE_SHOT_OVER_RUN;					// experiment status conditions
	int SINGLE_SHOT_SHORT_PERIODS;
	int TRAIN_OFF_GT_ON;
	int TRAIN_OFF_GT_DURATION;
	int TRAIN_CYCLE_TOO_HIGH;
	int TRAIN_CYCLE_TOO_LOW;
	int FREEFORM_POINTS_MISMATCH;
	int FREEFORM_FILE_PROBLEM;
	int POST_KTR_NEGATIVE;
	int TRIGGERS_INVALID;
												
	int channel_status[17];

	// Filing strings

	CString output_data_file_string;

	// DAPL source file and components

	CString Triggers_dir_path;

	CString header_file_string;
	CString fill_pipes_file_string;
	CString control_file_string;
	CString io_procedures_file_string;
	CString DAPL_source_file_string;

	// Status strings and counter for user display

	CString status_top_string;
	CString status_middle_string;
	CString status_bottom_string;
	int status_counter;

	// Functions
	
	void set_sampling_parameters(void);
	int convert_time_ms_to_sampling_points(float time_ms);

	void set_post_ktr_ms(void);

	void write_data_to_file(void);

	int create_DAPL_source_file(void);
	int write_header_file(void);
	int write_fill_pipes_file(void);
	int write_control_file(void);
	int write_io_procedures_file(void);

	void append_parameters_to_file(FILE* file);
	void load_parameters_from_file(FILE* file);

	float extract_float_value(FILE* file,char string[]);
	
	void display_status(int update_mode,CString new_string);
	int set_parameters_valid(void);

	void remote_shut_down(void);						// called by parent to shut down window

public:
	Triggers(CWnd* pParent = NULL);   // standard constructor
	virtual ~Triggers();


// Dialog Data
	enum { IDD = IDD_TRIGGERS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
public:
	
protected:
	virtual void OnCancel();
public:
	Channel_control CHANNEL_TAB_CONTROL;
	afx_msg void OnBnClickedRunExperiment();
	afx_msg void OnPaint();
	int data_points;
	CString data_path_string;
	CString data_base_string;
	int data_index;
	CComboBox SAMPLING_RATE_CONTROL;
	CString sampling_rate_string;
	
	CString duration_string;
	afx_msg void OnEnChangeDataPoints();
	afx_msg void OnCbnSelchangeSamplingRate();
	float ktr_initiation_ms;
	float post_ktr_ms;
	float ktr_step_duration_ms;
	float ktr_step_size_microns;
	afx_msg void OnEnChangeKtrInitiation();
	afx_msg void OnEnChangeKtrStepDuration();
};
