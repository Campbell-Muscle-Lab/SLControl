#pragma once

#include "Length_control_sheet.h"


// LC_generic_page dialog

class LC_generic_page : public CPropertyPage
{
	DECLARE_DYNAMIC(LC_generic_page)

	int m_nID;

	int tab_number;
	Length_control_sheet* p_lc_sheet;


public:
	LC_generic_page() {ASSERT(FALSE);} // never create
	LC_generic_page(UINT id);
	LC_generic_page(UINT id, Length_control_sheet* set_p_lc_sheet);
	
	virtual ~LC_generic_page();

// Dialog Data
	enum { IDD = IDD_LC_GENERIC_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
};
