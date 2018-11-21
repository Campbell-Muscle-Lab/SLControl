// LC_dout_page.cpp : implementation file
//

#include "stdafx.h"
#include "SLControl.h"
#include "LC_dout_page.h"
#include ".\lc_dout_page.h"

#include "Length_Control_Dialog.h"
#include "LC_dout_tab_control.h"


// LC_dout_page dialog

IMPLEMENT_DYNAMIC(LC_dout_page, LC_generic_page)

LC_dout_page::LC_dout_page()
	: LC_generic_page(LC_dout_page::IDD)
{
}

LC_dout_page::LC_dout_page(Length_control_sheet* set_p_lc_sheet)
	: LC_generic_page(LC_dout_page::IDD, set_p_lc_sheet)
{

}

LC_dout_page::~LC_dout_page()
{
}

void LC_dout_page::PostNcDestroy()
{
	delete this;
}

void LC_dout_page::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LC_DOUT_TAB_CONTROL, LC_DOUT_TAB_CONTROL);
}


BEGIN_MESSAGE_MAP(LC_dout_page, LC_generic_page)

END_MESSAGE_MAP()

BOOL LC_dout_page::OnInitDialog()
{
	LC_generic_page::OnInitDialog();

	// Initialize tab control

	p_lc_dout_tab_control =& LC_DOUT_TAB_CONTROL;
	p_lc_dout_tab_control->p_lc_dout_page=this;
	
	p_lc_dout_tab_control->InsertItem(0, _T("0"));
	p_lc_dout_tab_control->InsertItem(1, _T("1"));
	p_lc_dout_tab_control->InsertItem(2, _T("2"));
	p_lc_dout_tab_control->InsertItem(3, _T("3"));
	p_lc_dout_tab_control->InsertItem(4, _T("4"));
	p_lc_dout_tab_control->InsertItem(5, _T("5"));
	p_lc_dout_tab_control->InsertItem(6, _T("6"));
	p_lc_dout_tab_control->InsertItem(7, _T("7"));
	p_lc_dout_tab_control->InsertItem(8, _T("8"));
	p_lc_dout_tab_control->InsertItem(9, _T("9"));
	p_lc_dout_tab_control->InsertItem(10, _T("10"));
	p_lc_dout_tab_control->InsertItem(11, _T("11"));
	p_lc_dout_tab_control->InsertItem(12, _T("12"));
	p_lc_dout_tab_control->InsertItem(13, _T("13"));
	p_lc_dout_tab_control->InsertItem(14, _T("14"));
	p_lc_dout_tab_control->InsertItem(15, _T("15"));

	p_lc_dout_tab_control->Init();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


// LC_dout_page message handlers
