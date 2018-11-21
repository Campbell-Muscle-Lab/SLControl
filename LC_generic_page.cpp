// LC_generic_page.cpp : implementation file
//

#include "stdafx.h"
#include "SLControl.h"
#include "Length_control_sheet.h"
#include "LC_generic_page.h"
#include ".\lc_generic_page.h"


// LC_generic_page dialog

IMPLEMENT_DYNAMIC(LC_generic_page, CPropertyPage)

LC_generic_page::LC_generic_page(UINT id)
	: CPropertyPage(id)
{
}

LC_generic_page::LC_generic_page(UINT id, Length_control_sheet* set_p_lc_sheet)
	: CPropertyPage(id)
{
	p_lc_sheet=set_p_lc_sheet;
}

LC_generic_page::~LC_generic_page()
{
}


void LC_generic_page::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(LC_generic_page, CPropertyPage)
END_MESSAGE_MAP()


// LC_generic_page message handlers


void LC_generic_page::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class

//	CPropertyPage::PostNcDestroy();
	AfxMessageBox("LC_generic::PostNcDestroy");
}
