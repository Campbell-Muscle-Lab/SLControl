// LC_ktr_step_page.cpp : implementation file
//

#include "stdafx.h"
#include "SLControl.h"
#include "LC_ktr_step_page.h"
#include ".\lc_ktr_step_page.h"
#include "Length_control_dialog.h"


// LC_ktr_step_page dialog

IMPLEMENT_DYNAMIC(LC_ktr_step_page, LC_generic_page)

LC_ktr_step_page::LC_ktr_step_page() : LC_generic_page(LC_ktr_step_page::IDD)
{

}

LC_ktr_step_page::LC_ktr_step_page(Length_control_sheet* set_p_lc_sheet)
	:LC_generic_page(LC_ktr_step_page::IDD, set_p_lc_sheet)
{
	int i;

	lc_ktr_step_page_mode=0;

	ktr_step_page_size_microns=(float)-20.0;
	ktr_step_page_duration_ms=(float)20.0;
	ktr_step_page_pre_ms=(float)100.0;

	lc_ktr_step_page_mode_string="Single trial";
	lc_ktr_step_file_string="c:\\temp\\lc_ktr_step_file_string.txt";

	// Initialise arrays

	max_no_of_ktr_step_trials=LC_MAX_TRIALS;
  
	lc_ktr_step_rel_size_array = new float [max_no_of_ktr_step_trials];
	lc_ktr_step_rel_duration_array = new float [max_no_of_ktr_step_trials];
	lc_ktr_step_pre_array = new float [max_no_of_ktr_step_trials];

	no_of_ktr_step_trials=1;

	for (i=0;i<max_no_of_ktr_step_trials;i++)
	{
		lc_ktr_step_rel_size_array[i]=(float)1.0;
		lc_ktr_step_rel_duration_array[i]=(float)1.0;
		lc_ktr_step_pre_array[i]=ktr_step_page_pre_ms;
	}
}

LC_ktr_step_page::~LC_ktr_step_page()
{

	delete [] lc_ktr_step_rel_size_array;
	delete [] lc_ktr_step_rel_duration_array;
	delete [] lc_ktr_step_pre_array;

}

void LC_ktr_step_page::PostNcDestroy()
{
	delete this;
}

void LC_ktr_step_page::DoDataExchange(CDataExchange* pDX)
{
	LC_generic_page::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_KTR_STEP_PAGE_SIZE, KTR_STEP_PAGE_SIZE);
	DDX_Text(pDX, IDC_KTR_STEP_PAGE_SIZE,ktr_step_page_size_microns);

	DDX_Control(pDX, IDC_KTR_STEP_PAGE_DURATION, KTR_STEP_PAGE_DURATION);
	DDX_Text(pDX, IDC_KTR_STEP_PAGE_DURATION,ktr_step_page_duration_ms);

	DDX_Control(pDX, IDC_KTR_STEP_PAGE_PRE, KTR_STEP_PAGE_PRE);
	DDX_Text(pDX, IDC_KTR_STEP_PAGE_PRE, ktr_step_page_pre_ms);

	DDX_Control(pDX, IDC_LC_KTR_STEP_PAGE_MODE, LC_KTR_STEP_PAGE_MODE);
	DDX_Text(pDX, IDC_LC_KTR_STEP_PAGE_MODE, lc_ktr_step_page_mode_string);

	DDX_Control(pDX, IDC_LC_KTR_STEP_FILE, LC_KTR_STEP_FILE);
	DDX_Text(pDX, IDC_LC_KTR_STEP_FILE,lc_ktr_step_file_string);
}

BEGIN_MESSAGE_MAP(LC_ktr_step_page, LC_generic_page)
	ON_CBN_SELCHANGE(IDC_LC_KTR_STEP_PAGE_MODE, OnCbnSelchangeLcKtrStepPageMode)
	ON_BN_CLICKED(IDC_LC_KTR_STEP_PAGE_BROWSE, OnBnClickedLcKtrStepPageBrowse)
	ON_EN_CHANGE(IDC_KTR_STEP_PAGE_SIZE, OnEnChangeKtrStepPageSize)
	ON_EN_CHANGE(IDC_KTR_STEP_PAGE_DURATION, OnEnChangeKtrStepPageDuration)
	ON_EN_CHANGE(IDC_KTR_STEP_PAGE_PRE, OnEnChangeKtrStepPagePre)
	ON_EN_CHANGE(IDC_LC_KTR_STEP_FILE, OnEnChangeLcKtrStepFile)
END_MESSAGE_MAP()

BOOL LC_ktr_step_page::OnInitDialog()
{
	LC_generic_page::OnInitDialog();

	// Enable/disable appropriate controls

	GetDlgItem(IDC_LC_KTR_STEP_FILE)->EnableWindow(FALSE);
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
}

void LC_ktr_step_page::update_lc_ktr_step_page_dialog()
{
	// Coordinates lc_ktr_step_page window

	int i;

	// Branch depending on whether or not we're in batch mode

	if (lc_ktr_step_page_mode==0)
	{
		// Single trial

		GetDlgItem(IDC_KTR_STEP_PAGE_PRE)->EnableWindow(TRUE);
		GetDlgItem(IDC_LC_KTR_STEP_FILE)->EnableWindow(FALSE);

		for (i=0;i<max_no_of_ktr_step_trials;i++)
		{
			lc_ktr_step_rel_size_array[i]=(float)1.0;
			lc_ktr_step_rel_duration_array[i]=(float)1.0;
			lc_ktr_step_pre_array[i]=ktr_step_page_pre_ms;
		}

		no_of_ktr_step_trials=1;
	}
	else
	{
		// Batch mode

		GetDlgItem(IDC_KTR_STEP_PAGE_PRE)->EnableWindow(FALSE);
		GetDlgItem(IDC_LC_KTR_STEP_FILE)->EnableWindow(TRUE);

		load_ktr_step_page_file();
	}

	p_lc_sheet->p_lc_dialog->check_trial_status();
}

// LC_ktr_step_page message handlers

void LC_ktr_step_page::OnEnChangeKtrStepPageSize()
{
	UpdateData(TRUE);

	update_lc_ktr_step_page_dialog();
}

void LC_ktr_step_page::OnEnChangeKtrStepPageDuration()
{
	UpdateData(TRUE);

	update_lc_ktr_step_page_dialog();
}

void LC_ktr_step_page::OnEnChangeKtrStepPagePre()
{
	UpdateData(TRUE);

	update_lc_ktr_step_page_dialog();
}

void LC_ktr_step_page::OnCbnSelchangeLcKtrStepPageMode()
{
	UpdateData(TRUE);
	lc_ktr_step_page_mode=LC_KTR_STEP_PAGE_MODE.GetCurSel();
	
	load_ktr_step_page_file();

	update_lc_ktr_step_page_dialog();
}

void LC_ktr_step_page::OnBnClickedLcKtrStepPageBrowse()
{
	// Allows user to update ktr batch file from dialog box
	// Initialises file open dialog

	UpdateData(TRUE);

	CFileDialog fileDlg(TRUE,NULL,"*.txt",OFN_HIDEREADONLY,"Text files (*.txt)|*.txt||",this);

	// Initialisation

	fileDlg.m_ofn.lpstrTitle="Select ktr batch file";

	// Call Dialog

	if (fileDlg.DoModal()==IDOK)
	{
		lc_ktr_step_file_string=fileDlg.GetPathName();
	}

	UpdateData(FALSE);

	update_lc_ktr_step_page_dialog();
}

void LC_ktr_step_page::OnEnChangeLcKtrStepFile()
{
	update_lc_ktr_step_page_dialog();
}

void LC_ktr_step_page::load_ktr_step_page_file(void)
{
	// Variables

	int i;
	char display_string[200];

	// Update data

	UpdateData(TRUE);

	// Loads ktr_step_page file

	FILE *ktr_step_page_file=fopen(lc_ktr_step_file_string,"r");

	if (ktr_step_page_file==NULL)
	{
		// File Error
		MessageBeep(MB_ICONEXCLAMATION);
		sprintf(display_string,"Error: The ktr_step_page file could not be opened");
		p_lc_sheet->p_lc_dialog->display_status(TRUE,display_string);
		sprintf(display_string,"File_string: %s",lc_ktr_step_file_string);
		p_lc_sheet->p_lc_dialog->display_status(TRUE,display_string);
	}
	else
	{
		// Load file

		fscanf(ktr_step_page_file,"%i",&no_of_ktr_step_trials);

		for (i=0;i<no_of_ktr_step_trials;i++)
		{
			fscanf(ktr_step_page_file,"%f %f %f",
				&lc_ktr_step_rel_size_array[i],&lc_ktr_step_rel_duration_array[i],
				&lc_ktr_step_pre_array[i]);
		}

		fclose(ktr_step_page_file);
	}

}