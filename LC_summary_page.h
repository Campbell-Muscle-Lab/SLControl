#pragma once

#include "LC_generic_page.h"
#include "afxwin.h"

#include "Length_control_sheet.h"
#include "afxcmn.h"

// LC_summary_page dialog

class LC_summary_page : public LC_generic_page
{
	DECLARE_DYNAMIC(LC_summary_page)

public:
	LC_summary_page();
	LC_summary_page(Length_control_sheet* set_p_lc_sheet);
	virtual ~LC_summary_page();

	void update_summary_display();

// Dialog Data
	enum { IDD = IDD_LC_SUMMARY_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
public:
	CListCtrl SUMMARY_LIST_CONTROL;
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
};
