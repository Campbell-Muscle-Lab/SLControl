#pragma once

#include "LC_generic_page.h"
#include "afxwin.h"

#include "Length_control_sheet.h"

// LC_freeform_page dialog

class LC_freeform_page : public LC_generic_page
{
	DECLARE_DYNAMIC(LC_freeform_page)

public:
	LC_freeform_page();
	LC_freeform_page(Length_control_sheet* set_p_lc_sheet);
	virtual ~LC_freeform_page();

// Dialog Data
	enum { IDD = IDD_LC_FREEFORM_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
public:
	afx_msg void OnBnClickedFreeformMode();
	CButton freeform_mode;
	BOOL lc_freeform_mode_status;
	afx_msg void OnBnClickedBrowse();
	virtual BOOL OnInitDialog();
	CString lc_freeform_listfile_string;

	void upload_freeform_dap_files(void);
};
