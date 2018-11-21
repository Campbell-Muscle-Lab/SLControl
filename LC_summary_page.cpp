// LC_summary_page.cpp : implementation file
//

#include "stdafx.h"
#include "SLControl.h"
#include "LC_summary_page.h"
#include ".\lc_summary_page.h"

#include "Length_control_dialog.h"

// LC_summary_page dialog

IMPLEMENT_DYNAMIC(LC_summary_page, LC_generic_page)

LC_summary_page::LC_summary_page() : LC_generic_page(LC_summary_page::IDD)
{
}

LC_summary_page::LC_summary_page(Length_control_sheet* set_p_lc_sheet)
	:LC_generic_page(LC_summary_page::IDD, set_p_lc_sheet)
{

}

LC_summary_page::~LC_summary_page()
{
}

void LC_summary_page::PostNcDestroy()
{
	delete this;
}

void LC_summary_page::DoDataExchange(CDataExchange* pDX)
{
	LC_generic_page::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SUMMARY_LIST_CONTROL, SUMMARY_LIST_CONTROL);
}


BEGIN_MESSAGE_MAP(LC_summary_page, LC_generic_page)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// LC_summary_page message handlers

BOOL LC_summary_page::OnInitDialog()
{
	LC_generic_page::OnInitDialog();

	// Initialise display
	CRect rect;
	SUMMARY_LIST_CONTROL.GetClientRect(&rect);
	int nColInterval = rect.Width()/20;
	
	SUMMARY_LIST_CONTROL.InsertColumn(0,_T("N"), LVCFMT_LEFT, nColInterval);
	SUMMARY_LIST_CONTROL.InsertColumn(1,_T("Ramps"),LVCFMT_LEFT, 2*nColInterval);
	SUMMARY_LIST_CONTROL.InsertColumn(2,_T("Pre Str (ms)"), LVCFMT_LEFT, 4*nColInterval);
	SUMMARY_LIST_CONTROL.InsertColumn(3,_T("Rel Size"), LVCFMT_LEFT, 3*nColInterval);
	SUMMARY_LIST_CONTROL.InsertColumn(4,_T("Rel Dur"), LVCFMT_LEFT, 3*nColInterval);
	SUMMARY_LIST_CONTROL.InsertColumn(5,_T("Bet Str (ms)"), LVCFMT_LEFT, 3*nColInterval);
	SUMMARY_LIST_CONTROL.InsertColumn(6,_T("K Rel Size"), LVCFMT_LEFT, 3*nColInterval);
	SUMMARY_LIST_CONTROL.InsertColumn(7,_T("K Rel Dur"), LVCFMT_LEFT, 3*nColInterval);
	SUMMARY_LIST_CONTROL.InsertColumn(8,_T("K Pre (ms)"), LVCFMT_LEFT, 3*nColInterval);

	// Insert dummy text

	int i,j;
	char display_string[100];
	CString temp_string;
	
	for (i=0;i<=2;i++)
	{
		SUMMARY_LIST_CONTROL.InsertItem(i,"Ken");
		
		for (j=1;j<=2;j++)
		{
			sprintf(display_string,"%i.%.2f",i,(float)j);
			temp_string=display_string;
			p_lc_sheet->p_lc_dialog->display_status(TRUE,display_string);
			SUMMARY_LIST_CONTROL.SetItemText(i,j,display_string);
		}
	}

//	SUMMARY_LIST_CONTROL.EnableWindow(FALSE);

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void LC_summary_page::update_summary_display(void)
{
	int i;
	char display_string[100];

	// Refresh list
	SUMMARY_LIST_CONTROL.DeleteAllItems();

	// Now rebuild list
	for (i=0;i<p_lc_sheet->p_lc_dialog->lc_trials;i++)
	{
		sprintf(display_string,"%i",i+1);
		SUMMARY_LIST_CONTROL.InsertItem(i,display_string);

		sprintf(display_string,"%i",p_lc_sheet->p_lc_dialog->p_lc_ramp_page->lc_no_of_stretches);
		SUMMARY_LIST_CONTROL.SetItemText(i,1,display_string);

		sprintf(display_string,"%.2f",p_lc_sheet->p_lc_dialog->lc_dialog_pre_ms_array[i]);
		SUMMARY_LIST_CONTROL.SetItemText(i,2,display_string);

		sprintf(display_string,"%.2f",p_lc_sheet->p_lc_dialog->lc_dialog_rel_size_array[i]);
		SUMMARY_LIST_CONTROL.SetItemText(i,3,display_string);

		sprintf(display_string,"%.2f",p_lc_sheet->p_lc_dialog->lc_dialog_rel_time_array[i]);
		SUMMARY_LIST_CONTROL.SetItemText(i,4,display_string);

		sprintf(display_string,"%.0f",p_lc_sheet->p_lc_dialog->lc_dialog_delay_array[i]);
		SUMMARY_LIST_CONTROL.SetItemText(i,5,display_string);

		sprintf(display_string,"%.2f",p_lc_sheet->p_lc_dialog->lc_dialog_ktr_rel_size_array[i]);
		SUMMARY_LIST_CONTROL.SetItemText(i,6,display_string);

		sprintf(display_string,"%.2f",p_lc_sheet->p_lc_dialog->lc_dialog_ktr_rel_duration_array[i]);
		SUMMARY_LIST_CONTROL.SetItemText(i,7,display_string);

		sprintf(display_string,"%.0f",p_lc_sheet->p_lc_dialog->lc_dialog_ktr_pre_array[i]);
		SUMMARY_LIST_CONTROL.SetItemText(i,8,display_string);
	}
}

void LC_summary_page::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	update_summary_display();
}
