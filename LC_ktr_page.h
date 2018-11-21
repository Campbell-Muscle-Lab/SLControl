#pragma once

#include "LC_generic_page.h"
#include "afxwin.h"

// LC_ktr_page dialog

class LC_ktr_page : public LC_generic_page
{
	DECLARE_DYNAMIC(LC_ktr_page)

public:
	LC_ktr_page();
	LC_ktr_page(Length_control_sheet* set_p_lc_sheet);
	virtual ~LC_ktr_page();

	void update_ktr_parameters();

	// Variables

	
// Dialog Data
	enum { IDD = IDD_LC_KTR_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
		
protected:
	virtual void PostNcDestroy();
public:
	float lc_ktr_size_microns;

	CEdit LC_KTR_DURATION;
	float lc_ktr_duration_ms;

	float lc_pre_ktr_ms;

	virtual BOOL OnInitDialog();
	BOOL lc_ktr_batch_mode;
	CString lc_batch_mode_file_string;
	afx_msg void OnBnClickedLcKtrBatchMode();
	afx_msg void OnEnChangeLcKtrSize();

	afx_msg void OnEnChangeLcKtrDuration();
	afx_msg void OnEnChangeLcPreKtr();
};
