#pragma once

class LC_dout_page;

// LC_dout_sheet

class LC_dout_sheet : public CPropertySheet
{
	DECLARE_DYNAMIC(LC_dout_sheet)

public:
	LC_dout_sheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	LC_dout_sheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

	LC_dout_sheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage = 0,LC_dout_page* set_p_lc_dout_page=NULL);
	//LC_dout_sheet(LPCTSTR pszCaption, CPropertyPage* pParentWnd, UINT iSelectPage = 0,LC_dout_page* set_p_lc_dout_page=NULL);
		
	virtual ~LC_dout_sheet();

	LC_dout_page* p_lc_dout_page;

protected:
	DECLARE_MESSAGE_MAP()
};


