#pragma once

#include "LC_generic_page.h"
#include "afxwin.h"

#include "Length_control_sheet.h"

// LC_ktr_step_page dialog

class LC_ktr_step_page : public LC_generic_page
{
	DECLARE_DYNAMIC(LC_ktr_step_page)

public:
	LC_ktr_step_page();
	LC_ktr_step_page(Length_control_sheet* set_p_lc_sheet);
	virtual ~LC_ktr_step_page();

	int lc_ktr_step_page_mode;

	int max_no_of_ktr_step_trials;
	int no_of_ktr_step_trials;

	float *lc_ktr_step_rel_size_array;
	float *lc_ktr_step_rel_duration_array;
	float *lc_ktr_step_pre_array;

	void update_lc_ktr_step_page_dialog(void);

	void load_ktr_step_page_file(void);

// Dialog Data
	enum { IDD = IDD_LC_KTR_STEP_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
public:
	afx_msg void OnCbnSelchangeLcKtrStepPageMode();

	CComboBox LC_KTR_STEP_PAGE_MODE;
	CString lc_ktr_step_page_mode_string;

	CEdit LC_KTR_STEP_FILE;
	CString lc_ktr_step_file_string;
	
	CEdit KTR_STEP_PAGE_SIZE;
	float ktr_step_page_size_microns;
	CEdit KTR_STEP_PAGE_DURATION;
    float ktr_step_page_duration_ms;
	CEdit KTR_STEP_PAGE_PRE;
	float ktr_step_page_pre_ms;

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedLcKtrStepPageBrowse();
	afx_msg void OnEnChangeKtrStepPageSize();
	afx_msg void OnEnChangeKtrStepPageDuration();
	afx_msg void OnEnChangeKtrStepPagePre();
	afx_msg void OnEnChangeLcKtrStepFile();
};
