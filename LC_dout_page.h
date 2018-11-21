#pragma once

#include "LC_generic_page.h"
#include "afxcmn.h"

#include "LC_dout_tab_control.h"

// LC_dout_page dialog

class LC_dout_page : public LC_generic_page
{
	DECLARE_DYNAMIC(LC_dout_page)

	LC_dout_tab_control* p_lc_dout_tab_control;
	LC_dout_tab_control LC_DOUT_TAB_CONTROL;

public:
	LC_dout_page();
	LC_dout_page(Length_control_sheet* set_p_lc_sheet);
	virtual ~LC_dout_page();

// Dialog Data
	enum { IDD = IDD_LC_DOUT_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
public:
	virtual BOOL OnInitDialog();
};
