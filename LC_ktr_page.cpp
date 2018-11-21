// LC_ktr_page.cpp : implementation file
//

#include "stdafx.h"
#include "SLControl.h"
#include "LC_ktr_page.h"
#include ".\lc_ktr_page.h"
#include "Length_Control_Dialog.h"


// LC_ktr_page dialog

IMPLEMENT_DYNAMIC(LC_ktr_page, LC_generic_page)

LC_ktr_page::LC_ktr_page()
	: LC_generic_page(LC_ktr_page::IDD)
	, lc_ktr_duration_ms(0)
{
}

LC_ktr_page::LC_ktr_page(Length_control_sheet* set_p_lc_sheet)
	: LC_generic_page(LC_ktr_page::IDD, set_p_lc_sheet)
{
	// Initialise variables

	lc_ktr_size_microns=(float)-20.0;

	lc_ktr_duration_ms=(float)20.0;

	lc_pre_ktr_ms=(float)100.0;

	lc_batch_mode_file_string="c:\\temp\\ktr_batch_mode.txt";
}

LC_ktr_page::~LC_ktr_page()
{
}

void LC_ktr_page::PostNcDestroy()
{
	delete this;
}


void LC_ktr_page::DoDataExchange(CDataExchange* pDX)
{
	LC_generic_page::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_LC_KTR_SIZE, lc_ktr_size_microns);

	DDX_Text(pDX, IDC_LC_PRE_KTR, lc_pre_ktr_ms);

	DDX_Check(pDX, IDC_LC_KTR_BATCH_MODE, lc_ktr_batch_mode);
	DDX_Text(pDX, IDC_LC_BATCH_MODE_FILE, lc_batch_mode_file_string);
	
	DDX_Control(pDX, IDC_LC_KTR_DURATION, LC_KTR_DURATION);
	DDX_Text(pDX, IDC_LC_KTR_DURATION, lc_ktr_duration_ms);
}


BEGIN_MESSAGE_MAP(LC_ktr_page, LC_generic_page)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_LC_KTR_BATCH_MODE, OnBnClickedLcKtrBatchMode)
	ON_EN_CHANGE(IDC_LC_KTR_SIZE, OnEnChangeLcKtrSize)
	ON_EN_CHANGE(IDC_LC_KTR_DURATION, OnEnChangeLcKtrDuration)
	ON_EN_CHANGE(IDC_LC_PRE_KTR, OnEnChangeLcPreKtr)
END_MESSAGE_MAP()


// LC_ktr_page message handlers

BOOL LC_ktr_page::OnInitDialog()
{
	p_lc_sheet->highlight_tab(tab_number,TRUE);
	
	GetDlgItem(IDC_LC_BATCH_MODE_FILE)->EnableWindow(FALSE);

	UpdateData(FALSE);

	return TRUE;
}

void LC_ktr_page::OnBnClickedLcKtrBatchMode()
{
	UpdateData(TRUE);

	if (lc_ktr_batch_mode)
	{
		GetDlgItem(IDC_LC_BATCH_MODE_FILE)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_LC_BATCH_MODE_FILE)->EnableWindow(FALSE);
	}

	p_lc_sheet->p_lc_dialog->check_trial_status();
}

void LC_ktr_page::OnEnChangeLcKtrSize()
{
	UpdateData(TRUE);

	if (lc_ktr_size_microns!=0)
		p_lc_sheet->highlight_tab(tab_number,TRUE);
	else
		p_lc_sheet->highlight_tab(tab_number,FALSE);

	p_lc_sheet->p_lc_dialog->update_preview_record();
}

void LC_ktr_page::OnEnChangeLcKtrDuration()
{
	UpdateData(TRUE);

	p_lc_sheet->p_lc_dialog->update_preview_record();
}

void LC_ktr_page::OnEnChangeLcPreKtr()
{
	UpdateData(TRUE);

	p_lc_sheet->p_lc_dialog->update_preview_record();
}

void LC_ktr_page::update_ktr_parameters()
{
	p_lc_sheet->p_lc_dialog->check_trial_status();
}

