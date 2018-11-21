// Length_control_sheet.cpp : implementation file
//

#include "stdafx.h"
#include "SLControl.h"
#include "Length_control_sheet.h"
#include ".\length_control_sheet.h"


// Length_control_sheet

IMPLEMENT_DYNAMIC(Length_control_sheet, CPropertySheet)
Length_control_sheet::Length_control_sheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

Length_control_sheet::Length_control_sheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage,
		Length_control_dialog* set_p_lc_dialog)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	p_lc_dialog=set_p_lc_dialog;

	EnableStackedTabs(FALSE);
}

Length_control_sheet::~Length_control_sheet()
{
}

void Length_control_sheet::PostNcDestroy()
{
	delete this;
}

BEGIN_MESSAGE_MAP(Length_control_sheet, CPropertySheet)
END_MESSAGE_MAP()


// Length_control_sheet message handlers

void Length_control_sheet::highlight_tab(int tab_number, BOOL status)
{
	CTabCtrl *p_tab_control = GetTabControl();

	p_tab_control->HighlightItem(tab_number,status);
}

