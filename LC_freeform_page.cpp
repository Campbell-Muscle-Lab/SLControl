// LC_freeform_page.cpp : implementation file
//

#include "stdafx.h"
#include "SLControl.h"
#include "LC_freeform_page.h"
#include ".\lc_freeform_page.h"

#include "Length_control_sheet.h"
#include "Length_Control_Dialog.h"
//class Length_control_sheet;


// LC_freeform_page dialog

IMPLEMENT_DYNAMIC(LC_freeform_page, LC_generic_page)
LC_freeform_page::LC_freeform_page()	
	: LC_generic_page(LC_freeform_page::IDD)
//	, lc_freeform_listfile_string(0)
{
	// Constructor
}

LC_freeform_page::LC_freeform_page(Length_control_sheet* set_p_lc_sheet)	
	: LC_generic_page(LC_freeform_page::IDD, set_p_lc_sheet)
{
	// Initialise variables

	p_lc_sheet=set_p_lc_sheet;

	lc_freeform_mode_status=0;
	lc_freeform_listfile_string="c:\\temp\\freeform_file.txt";
}


LC_freeform_page::~LC_freeform_page()
{
}

void LC_freeform_page::PostNcDestroy()
{
	delete this;
}

void LC_freeform_page::DoDataExchange(CDataExchange* pDX)
{
	LC_generic_page::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_FREEFORM_MODE, lc_freeform_mode_status);

	DDX_Text(pDX, IDC_FREEFORM_LISTFILE, lc_freeform_listfile_string);
}

BOOL LC_freeform_page::OnInitDialog()
{
	LC_generic_page::OnInitDialog();

	// TODO:  Add extra initialization here

	//lc_freeform_listfile_string="c:\\temp\\freeform_file.txt";

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


BEGIN_MESSAGE_MAP(LC_freeform_page, LC_generic_page)
	ON_BN_CLICKED(IDC_FREEFORM_MODE, OnBnClickedFreeformMode)
	ON_BN_CLICKED(IDC_BROWSE, OnBnClickedBrowse)
END_MESSAGE_MAP()


// LC_freeform_page message handlers

void LC_freeform_page::OnBnClickedFreeformMode()
{
	// Variables

	char display_string[200];

	// Update Data
	
	UpdateData(TRUE);

	if (lc_freeform_mode_status)
	{
		p_lc_sheet->highlight_tab(tab_number,TRUE);
		sprintf(display_string,"Warning - Freeform mode takes precedence");
		p_lc_sheet->p_lc_dialog->display_status(TRUE,display_string);
		sprintf(display_string,"Warning - over all other settings");
		p_lc_sheet->p_lc_dialog->display_status(TRUE,display_string);
		MessageBeep(MB_ICONEXCLAMATION);

		upload_freeform_dap_files();
	}
	else
	{
		p_lc_sheet->highlight_tab(tab_number,FALSE);
	}
}

void LC_freeform_page::OnBnClickedBrowse()
{
	// Allows user to update lc_freeform_file_string from dialog box
	// Initialises file open dialog

	UpdateData(TRUE);

	CFileDialog fileDlg(TRUE,NULL,"*.txt",OFN_HIDEREADONLY,"Text files (*.txt)|*.txt||",this);

	// Initialisation

	fileDlg.m_ofn.lpstrTitle="Select freeform file";

	// Call Dialog

	if (fileDlg.DoModal()==IDOK)
	{
		lc_freeform_listfile_string=fileDlg.GetPathName();
	}

	UpdateData(FALSE);
}

void LC_freeform_page::upload_freeform_dap_files()
{
	// Function opens lc_freeform_list_file and
	// stores dap file strings in p_lc_dialog->lc_freeform_file_string

	int file_counter;
	int temp_int;
	float temp_float;

	char display_string[200];
	char temp_string[_MAX_PATH];

	FILE *freeform_list_file=fopen(lc_freeform_listfile_string,"r");

	if (freeform_list_file==NULL)
	{
		// File error
		MessageBeep(MB_ICONEXCLAMATION);
		sprintf(display_string,"Error: freeform_list_file could not be opened");
		p_lc_sheet->p_lc_dialog->display_status(TRUE,display_string);
		sprintf(display_string,"File_string: %s",lc_freeform_listfile_string);
		p_lc_sheet->p_lc_dialog->display_status(TRUE,display_string);
	}
	else
	{
		// Scan through list file

		// First read number of trials

		fscanf(freeform_list_file,"%i",&p_lc_sheet->p_lc_dialog->no_of_trials);
		sprintf(display_string,"Attempting to read %i trials",p_lc_sheet->p_lc_dialog->no_of_trials);
		p_lc_sheet->p_lc_dialog->display_status(TRUE,display_string);
        
		if (p_lc_sheet->p_lc_dialog->no_of_trials>p_lc_sheet->p_lc_dialog->lc_dialog_max_trials)
		{
			MessageBeep(MB_ICONEXCLAMATION);
			sprintf(display_string,"Maximum trials (currently %i) exceeded",
				p_lc_sheet->p_lc_dialog->lc_dialog_max_trials);
			p_lc_sheet->p_lc_dialog->display_status(TRUE,display_string);
			sprintf(display_string,"Please change the Freeform list file and try again");
			p_lc_sheet->p_lc_dialog->display_status(TRUE,display_string);

			lc_freeform_mode_status=FALSE;
			UpdateData(FALSE);
		}
		else
		{
			for (file_counter=0;file_counter<p_lc_sheet->p_lc_dialog->no_of_trials;file_counter++)
			{
				fscanf(freeform_list_file,"%i",&temp_int);
				p_lc_sheet->p_lc_dialog->lc_freeform_no_of_channels[file_counter]=temp_int;
				fscanf(freeform_list_file,"%f",&temp_float);
				p_lc_sheet->p_lc_dialog->lc_freeform_time_increment[file_counter]=temp_float;
				fscanf(freeform_list_file,"%i",&temp_int);
				p_lc_sheet->p_lc_dialog->lc_freeform_total_sample_points[file_counter]=temp_int;
				fscanf(freeform_list_file,"%s",temp_string);
				p_lc_sheet->p_lc_dialog->lc_freeform_file_string[file_counter]=temp_string;
			}

			sprintf(display_string,"%i files read, last file: %s",
				p_lc_sheet->p_lc_dialog->no_of_trials,
				p_lc_sheet->p_lc_dialog->lc_freeform_file_string[p_lc_sheet->p_lc_dialog->no_of_trials-1]);
			p_lc_sheet->p_lc_dialog->display_status(TRUE,display_string);
			sprintf(display_string,"Channels: %i, Time inc: %g  Total points: %i",
				p_lc_sheet->p_lc_dialog->lc_freeform_no_of_channels[p_lc_sheet->p_lc_dialog->no_of_trials-1],
				p_lc_sheet->p_lc_dialog->lc_freeform_time_increment[p_lc_sheet->p_lc_dialog->no_of_trials-1],
				p_lc_sheet->p_lc_dialog->
					lc_freeform_total_sample_points[p_lc_sheet->p_lc_dialog->no_of_trials-1]);
			p_lc_sheet->p_lc_dialog->display_status(TRUE,display_string);
		}
	}

	fclose(freeform_list_file);
}
