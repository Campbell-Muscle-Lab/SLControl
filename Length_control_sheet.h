#pragma once

class Length_control_dialog;

// Length_control_sheet

class Length_control_sheet : public CPropertySheet
{
	DECLARE_DYNAMIC(Length_control_sheet)

public:
	Length_control_sheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	Length_control_sheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0,
		Length_control_dialog* set_p_lc_dialog=NULL);
	virtual ~Length_control_sheet();

	void highlight_tab(int tab_number, BOOL status);

	Length_control_dialog* p_lc_dialog;

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
};


