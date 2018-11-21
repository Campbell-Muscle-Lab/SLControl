#pragma once

#include "LC_generic_page.h"
#include "afxwin.h"

#include "Length_control_sheet.h"

// LC_ramp_page dialog

class LC_ramp_page : public LC_generic_page
{
	DECLARE_DYNAMIC(LC_ramp_page)

public:
	LC_ramp_page();
	LC_ramp_page(Length_control_sheet* set_p_lc_sheet);
	virtual ~LC_ramp_page();

	void display_batch_values();

	void load_pre_ramp_file();
	void load_rise_file();

// Dialog Data
	enum { IDD = IDD_LC_RAMP_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	
protected:
	
	virtual void PostNcDestroy();
public:

	int max_no_of_lc_ramp_trials;
	int no_of_lc_ramp_trials;

	int lc_pre_ramp_trials;
	int lc_rise_ramp_trials;

	float * lc_ramp_pre_ms_array;
	float * lc_ramp_rel_size_array;
	float * lc_ramp_rel_time_array;
	float * lc_ramp_delay_array;
	    
    CEdit STRETCH_SIZE;
	float lc_stretch_size_microns;

	CComboBox NO_OF_STRETCHES;
	CString lc_no_of_stretches_string;
	int lc_no_of_stretches;

	CButton RAMP;
	BOOL lc_ramp_mode;

	CButton HOLD;
	BOOL lc_hold_mode;

	float lc_hold_period_ms;

	CEdit INTER_STRETCH_DELAY;
	float lc_inter_stretch_delay_ms;
	
	CComboBox LC_PRE_MODE;
	int lc_pre_mode;
	CString lc_pre_mode_string;
	float lc_pre_ramp_ms;
	CString lc_pre_ramp_file_string;
	
	CComboBox LC_RISE_MODE;
	int lc_rise_mode;
	CString lc_rise_mode_string;
	float lc_risetime_ms;
	CString lc_risetime_file_string;
	
	void update_lc_ramp_dialog();
	
	virtual BOOL OnInitDialog();

//	afx_msg void OnBnClickedPreSingle();
	afx_msg void OnCbnSelchangePreMode();
//	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnCbnSelchangeNoOfStretches();

	afx_msg void OnBnClickedRiseBrowse();
	afx_msg void OnBnClickedPreBrowse();
	afx_msg void OnCbnSelchangeRiseMode();
	
	afx_msg void OnEnChangeStretchSize();
	afx_msg void OnBnClickedRamp();
	afx_msg void OnEnChangeInterStretchDelay();
	afx_msg void OnEnChangeRisetime();
	afx_msg void OnEnChangePreValue();
	afx_msg void OnBnClickedHold();
	CEdit HOLD_PERIOD;
	afx_msg void OnEnChangeHoldPeriod();
	afx_msg void OnPaint();
	afx_msg void OnEnChangeRiseFile();
	afx_msg void OnEnChangePreFile();
};
