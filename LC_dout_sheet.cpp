// LC_dout_sheet.cpp : implementation file
//

#include "stdafx.h"
#include "SLControl.h"
#include "LC_dout_sheet.h"


// LC_dout_sheet

IMPLEMENT_DYNAMIC(LC_dout_sheet, CPropertySheet)
LC_dout_sheet::LC_dout_sheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

LC_dout_sheet::LC_dout_sheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage,LC_dout_page* set_p_lc_dout_page)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
//LC_dout_sheet::LC_dout_sheet(LPCTSTR pszCaption, CPropertyPage* pParentWnd, UINT iSelectPage,LC_dout_page* set_p_lc_dout_page)
//	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	p_lc_dout_page=set_p_lc_dout_page;

	char display_string[200];
	sprintf(display_string,"p_lc_dout_page: %i",p_lc_dout_page);
	MessageBox(display_string,"LC_dout_sheet::Constructor");
}

LC_dout_sheet::~LC_dout_sheet()
{
}


BEGIN_MESSAGE_MAP(LC_dout_sheet, CPropertySheet)
END_MESSAGE_MAP()


// LC_dout_sheet message handlers
