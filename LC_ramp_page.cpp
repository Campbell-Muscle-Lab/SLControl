// LC_ramp_page.cpp : implementation file
//

#include "stdafx.h"
#include "SLControl.h"
#include "LC_ramp_page.h"
#include ".\lc_ramp_page.h"
#include "Length_control_dialog.h"

// LC_ramp_page dialog

IMPLEMENT_DYNAMIC(LC_ramp_page, LC_generic_page)

LC_ramp_page::LC_ramp_page() : LC_generic_page(LC_ramp_page::IDD)
{
}

LC_ramp_page::LC_ramp_page(Length_control_sheet* set_p_lc_sheet)
	: LC_generic_page(LC_ramp_page::IDD, set_p_lc_sheet)
{
	// Variables

	int i;

	// Initialise variables

	lc_stretch_size_microns=(float)20.0;

	lc_no_of_stretches_string="0";
	lc_no_of_stretches=0;

	lc_ramp_mode=0;

	lc_hold_mode=0;
	lc_hold_period_ms=(float)50.0;

	lc_inter_stretch_delay_ms=(float)100.0;

	lc_rise_mode=0;
	lc_rise_mode_string="Single trial";
    lc_risetime_ms=(float)400.0;
	lc_risetime_file_string="c:\\temp\\lc_rise_time_string.txt";

	lc_pre_mode=0;
	lc_pre_mode_string="Single value (ms)";
	lc_pre_ramp_ms=150;
	lc_pre_ramp_file_string="c:\\temp\\lc_pre_ramp_string.txt";

	// Initialise arrays

	max_no_of_lc_ramp_trials=LC_MAX_TRIALS;

	lc_ramp_pre_ms_array = new float [max_no_of_lc_ramp_trials];
	lc_ramp_rel_size_array = new float [max_no_of_lc_ramp_trials];
	lc_ramp_rel_time_array = new float [max_no_of_lc_ramp_trials];
	lc_ramp_delay_array = new float [max_no_of_lc_ramp_trials];

	no_of_lc_ramp_trials=1;
	for (i=0;i<max_no_of_lc_ramp_trials;i++)
	{
		lc_ramp_pre_ms_array[i]=lc_pre_ramp_ms;
		lc_ramp_rel_size_array[i]=(float)1.0;
		lc_ramp_rel_time_array[i]=(float)1.0;
		lc_ramp_delay_array[i]=lc_inter_stretch_delay_ms;
	}
}

LC_ramp_page::~LC_ramp_page()
{
	// Destructor

	delete [] lc_ramp_pre_ms_array;
	delete [] lc_ramp_rel_size_array;
	delete [] lc_ramp_rel_time_array;
	delete [] lc_ramp_delay_array;
}

void LC_ramp_page::PostNcDestroy()
{
	delete this;
}

void LC_ramp_page::DoDataExchange(CDataExchange* pDX)
{
	LC_generic_page::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_STRETCH_SIZE, STRETCH_SIZE);
	DDX_Text(pDX, IDC_STRETCH_SIZE, lc_stretch_size_microns);

	DDX_Control(pDX, IDC_RAMP, RAMP);
	DDX_Check(pDX, IDC_RAMP, lc_ramp_mode);

	DDX_Control(pDX, IDC_HOLD, HOLD);
	DDX_Check(pDX, IDC_HOLD, lc_hold_mode);

	DDX_Control(pDX, IDC_HOLD_PERIOD, HOLD_PERIOD);
	DDX_Text(pDX, IDC_HOLD_PERIOD, lc_hold_period_ms);

	DDX_Control(pDX, IDC_NO_OF_STRETCHES, NO_OF_STRETCHES);
	DDX_CBString(pDX, IDC_NO_OF_STRETCHES, lc_no_of_stretches_string);

	DDX_Text(pDX, IDC_INTER_STRETCH_DELAY, lc_inter_stretch_delay_ms);

	DDX_Control(pDX, IDC_RISE_MODE, LC_RISE_MODE);
	DDX_CBString(pDX, IDC_RISE_MODE, lc_rise_mode_string);
	DDX_Text(pDX, IDC_RISETIME, lc_risetime_ms);
	DDX_Text(pDX, IDC_RISE_FILE, lc_risetime_file_string);

	DDX_Control(pDX, IDC_PRE_MODE, LC_PRE_MODE);
	DDX_CBString(pDX, IDC_PRE_MODE, lc_pre_mode_string);
	DDX_Text(pDX, IDC_PRE_VALUE,lc_pre_ramp_ms);
	DDX_Text(pDX, IDC_PRE_FILE,lc_pre_ramp_file_string);
	DDX_Control(pDX, IDC_INTER_STRETCH_DELAY, INTER_STRETCH_DELAY);

}


BEGIN_MESSAGE_MAP(LC_ramp_page, LC_generic_page)
	ON_CBN_SELCHANGE(IDC_PRE_MODE, OnCbnSelchangePreMode)
	ON_CBN_SELCHANGE(IDC_NO_OF_STRETCHES, OnCbnSelchangeNoOfStretches)
	ON_BN_CLICKED(IDC_RISE_BROWSE, OnBnClickedRiseBrowse)
	ON_BN_CLICKED(IDC_PRE_BROWSE, OnBnClickedPreBrowse)
	ON_CBN_SELCHANGE(IDC_RISE_MODE, OnCbnSelchangeRiseMode)
	ON_EN_CHANGE(IDC_STRETCH_SIZE, OnEnChangeStretchSize)
	ON_BN_CLICKED(IDC_RAMP, OnBnClickedRamp)
	ON_EN_CHANGE(IDC_INTER_STRETCH_DELAY, OnEnChangeInterStretchDelay)
	ON_EN_CHANGE(IDC_RISETIME, OnEnChangeRisetime)
	ON_EN_CHANGE(IDC_PRE_VALUE, OnEnChangePreValue)
	ON_BN_CLICKED(IDC_HOLD, OnBnClickedHold)
	ON_EN_CHANGE(IDC_HOLD_PERIOD, OnEnChangeHoldPeriod)
	ON_WM_PAINT()
	ON_EN_CHANGE(IDC_RISE_FILE, OnEnChangeRiseFile)
	ON_EN_CHANGE(IDC_PRE_FILE, OnEnChangePreFile)
END_MESSAGE_MAP()

BOOL LC_ramp_page::OnInitDialog()
{
	LC_generic_page::OnInitDialog();

	UpdateData(TRUE);

	// Disable appropriate controls

	GetDlgItem(IDC_STRETCH_SIZE)->EnableWindow(FALSE);
	GetDlgItem(IDC_RAMP)->EnableWindow(FALSE);
	GetDlgItem(IDC_HOLD)->EnableWindow(FALSE);
	GetDlgItem(IDC_HOLD_PERIOD)->EnableWindow(FALSE);
	GetDlgItem(IDC_INTER_STRETCH_DELAY)->EnableWindow(FALSE);

	GetDlgItem(IDC_RISE_MODE)->EnableWindow(FALSE);
	GetDlgItem(IDC_RISETIME)->EnableWindow(FALSE);
	GetDlgItem(IDC_RISE_FILE)->EnableWindow(FALSE);
	GetDlgItem(IDC_RISE_BROWSE)->EnableWindow(FALSE);

	GetDlgItem(IDC_PRE_MODE)->EnableWindow(FALSE);
	GetDlgItem(IDC_PRE_VALUE)->EnableWindow(FALSE);
	GetDlgItem(IDC_PRE_FILE)->EnableWindow(FALSE);
	GetDlgItem(IDC_PRE_BROWSE)->EnableWindow(FALSE);

//display_batch_values();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void LC_ramp_page::update_lc_ramp_dialog()
{
//display_batch_values();

	// Variables

	int i;

	// Coordinates lc_ramp_window

	// Switch based on the number of stretches

	switch (lc_no_of_stretches)
	{
		case 0:
		{
			// Untag page
			p_lc_sheet->highlight_tab(tab_number,FALSE);

			// Disable all controls other than no of stretches

			GetDlgItem(IDC_STRETCH_SIZE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RISETIME)->EnableWindow(FALSE);
			GetDlgItem(IDC_RAMP)->EnableWindow(FALSE);
			GetDlgItem(IDC_HOLD)->EnableWindow(FALSE);
			GetDlgItem(IDC_HOLD_PERIOD)->EnableWindow(FALSE);
			GetDlgItem(IDC_INTER_STRETCH_DELAY)->EnableWindow(FALSE);

			GetDlgItem(IDC_RISE_MODE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RISE_FILE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RISE_BROWSE)->EnableWindow(FALSE);

			GetDlgItem(IDC_PRE_MODE)->EnableWindow(FALSE);
			GetDlgItem(IDC_PRE_VALUE)->EnableWindow(FALSE);
			GetDlgItem(IDC_PRE_FILE)->EnableWindow(FALSE);
			GetDlgItem(IDC_PRE_BROWSE)->EnableWindow(FALSE);
		}
		break;

		default:
		{
			// Tag page
			p_lc_sheet->highlight_tab(tab_number,TRUE);

			// Enable/Disable delay control depending on number of stretches
			if (lc_no_of_stretches==1)
			{
				// Single stretch
				GetDlgItem(IDC_INTER_STRETCH_DELAY)->EnableWindow(FALSE);
			}
			else
			{
				// Multiple stretches
				GetDlgItem(IDC_INTER_STRETCH_DELAY)->EnableWindow(TRUE);
			}

			GetDlgItem(IDC_STRETCH_SIZE)->EnableWindow(TRUE);
			GetDlgItem(IDC_RAMP)->EnableWindow(TRUE);

			if (lc_ramp_mode)
			{
				GetDlgItem(IDC_HOLD)->EnableWindow(TRUE);
				
				if (lc_hold_mode)
					GetDlgItem(IDC_HOLD_PERIOD)->EnableWindow(TRUE);
				else
					GetDlgItem(IDC_HOLD_PERIOD)->EnableWindow(FALSE);
			}
			else
			{
				GetDlgItem(IDC_HOLD)->EnableWindow(FALSE);
				GetDlgItem(IDC_HOLD_PERIOD)->EnableWindow(FALSE);
			}

			GetDlgItem(IDC_RISETIME)->EnableWindow(TRUE);

			// Pre mode

			GetDlgItem(IDC_PRE_MODE)->EnableWindow(TRUE);
			if (lc_pre_mode==0)
			{
				GetDlgItem(IDC_PRE_VALUE)->EnableWindow(TRUE);
				GetDlgItem(IDC_PRE_FILE)->EnableWindow(FALSE);
				GetDlgItem(IDC_PRE_BROWSE)->EnableWindow(FALSE);

				lc_pre_ramp_trials=0;
			}
			else
			{
				GetDlgItem(IDC_PRE_VALUE)->EnableWindow(FALSE);
				GetDlgItem(IDC_PRE_FILE)->EnableWindow(TRUE);
				GetDlgItem(IDC_PRE_BROWSE)->EnableWindow(TRUE);

				load_pre_ramp_file();
			}

			// Rise mode

			GetDlgItem(IDC_RISE_MODE)->EnableWindow(TRUE);
			if (lc_rise_mode==0)
			{
				GetDlgItem(IDC_RISE_FILE)->EnableWindow(FALSE);
				GetDlgItem(IDC_RISE_BROWSE)->EnableWindow(FALSE);

				lc_rise_ramp_trials=0;
			}
			else
			{
				GetDlgItem(IDC_RISE_FILE)->EnableWindow(TRUE);
				GetDlgItem(IDC_RISE_BROWSE)->EnableWindow(TRUE);

				load_rise_file();
			}
		}
		break;
	}

	// Cope with different batch modes

	if ((lc_pre_ramp_trials>0)&&(lc_rise_ramp_trials>0))
	{
		no_of_lc_ramp_trials=IMAX(lc_pre_ramp_trials,lc_rise_ramp_trials);
	}

	if ((lc_pre_ramp_trials>0)&&(lc_rise_ramp_trials==0))
	{
		no_of_lc_ramp_trials=lc_pre_ramp_trials;
		for (i=0;i<=no_of_lc_ramp_trials;i++)
		{
			lc_ramp_rel_size_array[i]=1.0;
			lc_ramp_rel_time_array[i]=1.0;
			lc_ramp_delay_array[i]=lc_inter_stretch_delay_ms;
		}
	}
	
	if ((lc_pre_ramp_trials==0)&&(lc_rise_ramp_trials>0))
	{
		no_of_lc_ramp_trials=lc_rise_ramp_trials;
		for (i=0;i<no_of_lc_ramp_trials;i++)
			lc_ramp_pre_ms_array[i]=lc_pre_ramp_ms;
	}
	
	if ((lc_pre_ramp_trials==0)&&(lc_rise_ramp_trials==0))
	{
		// Default from screen

		no_of_lc_ramp_trials=1;
		lc_ramp_pre_ms_array[0]=lc_pre_ramp_ms;
		lc_ramp_rel_size_array[0]=1.0;
		lc_ramp_rel_time_array[0]=1.0;
		lc_ramp_delay_array[0]=lc_inter_stretch_delay_ms;
	}

	// Update displays and parent dialog

	display_batch_values();
/*
	if (no_of_lc_ramp_trials>1)
		p_lc_sheet->p_lc_dialog->GetDlgItem(IDC_INTER_TRIAL_INTERVAL)->EnableWindow(TRUE);
	else
		p_lc_sheet->p_lc_dialog->GetDlgItem(IDC_INTER_TRIAL_INTERVAL)->EnableWindow(FALSE);
*/

	p_lc_sheet->p_lc_dialog->check_trial_status();
}

// LC_ramp_page message handlers

void LC_ramp_page::OnCbnSelchangeNoOfStretches()
{
	// Variables

	char* p_string;
	int temp;
	CString value_string;

	// Code

	UpdateData(TRUE);

	temp=NO_OF_STRETCHES.GetCurSel();
	NO_OF_STRETCHES.GetLBText(temp,value_string);

	lc_no_of_stretches=strtol(value_string,&p_string,10);

	// Update dialog

	update_lc_ramp_dialog();
}

void LC_ramp_page::OnBnClickedRamp()
{
	UpdateData(TRUE);

	update_lc_ramp_dialog();
}

void LC_ramp_page::OnBnClickedHold()
{
	UpdateData(TRUE);

	update_lc_ramp_dialog();
}

void LC_ramp_page::OnEnChangeHoldPeriod()
{
	UpdateData(TRUE);

	update_lc_ramp_dialog();
}

void LC_ramp_page::OnEnChangeStretchSize()
{
	UpdateData(TRUE);

	update_lc_ramp_dialog();
}

void LC_ramp_page::OnEnChangeRisetime()
{
	UpdateData(TRUE);

	update_lc_ramp_dialog();
}

void LC_ramp_page::OnEnChangeInterStretchDelay()
{
	UpdateData(TRUE);

	update_lc_ramp_dialog();
}

void LC_ramp_page::OnCbnSelchangeRiseMode()
{
	// Read data

	UpdateData(TRUE);
	lc_rise_mode=LC_RISE_MODE.GetCurSel();

	update_lc_ramp_dialog();
}

void LC_ramp_page::OnCbnSelchangePreMode()
{
	// Read data

	UpdateData(TRUE);
	lc_pre_mode=LC_PRE_MODE.GetCurSel();

	update_lc_ramp_dialog();
}

void LC_ramp_page::OnEnChangePreValue()
{
	UpdateData(TRUE);

	int i;
	for (i=0;i<max_no_of_lc_ramp_trials;i++)
		lc_ramp_pre_ms_array[i]=lc_pre_ramp_ms;

	update_lc_ramp_dialog();
}

void LC_ramp_page::OnBnClickedRiseBrowse()
{
	// Allows user to update lc_rise_file_string from dialog box
	// Initialises file open dialog

	// Read in data

	UpdateData(TRUE);

	CFileDialog fileDlg(TRUE,NULL,"*.txt",OFN_HIDEREADONLY,"Text files (*.txt)|*.txt||",this);

	// Initialisation

	fileDlg.m_ofn.lpstrTitle="Select rise_time file";

	// Call Dialog

	if (fileDlg.DoModal()==IDOK)
	{
		lc_risetime_file_string=fileDlg.GetPathName();
	}

	// Update screen

	UpdateData(FALSE);

	update_lc_ramp_dialog();
}

void LC_ramp_page::OnEnChangeRiseFile()
{
	UpdateData(TRUE);

	update_lc_ramp_dialog();
}

void LC_ramp_page::load_rise_file(void)
{
	// Variables

	int i;
	char display_string[200];
	
	// Update data

	UpdateData(TRUE);

	// Loads pre_ramp file

    FILE *rise_file=fopen(lc_risetime_file_string,"r");

	if (rise_file==NULL)
	{
		// File Error
		MessageBeep(MB_ICONEXCLAMATION);
		sprintf(display_string,"Error: The Rise file could not be opened");
		p_lc_sheet->p_lc_dialog->display_status(TRUE,display_string);
		sprintf(display_string,"File_string: %s",lc_risetime_file_string);
		p_lc_sheet->p_lc_dialog->display_status(TRUE,display_string);
	}
	else
	{
		// Load file
		fscanf(rise_file,"%i",&lc_rise_ramp_trials);
		for (i=0;i<lc_rise_ramp_trials;i++)
			fscanf(rise_file,"%f %f %f",
				&lc_ramp_rel_size_array[i],&lc_ramp_rel_time_array[i],&lc_ramp_delay_array[i]);
		fclose(rise_file);
	}
}

void LC_ramp_page::OnBnClickedPreBrowse()
{
	// Allows user to update lc_pre_ramp_file_string from dialog box
	// Initialises file open dialog

	UpdateData(TRUE);

	CFileDialog fileDlg(TRUE,NULL,"*.txt",OFN_HIDEREADONLY,"Text files (*.txt)|*.txt||",this);

	// Initialisation

	fileDlg.m_ofn.lpstrTitle="Select pre_ramp_delay file";

	// Call Dialog

	if (fileDlg.DoModal()==IDOK)
	{
		lc_pre_ramp_file_string=fileDlg.GetPathName();
	}

	UpdateData(FALSE);

	update_lc_ramp_dialog();
}

void LC_ramp_page::OnEnChangePreFile()
{
	update_lc_ramp_dialog();
}

void LC_ramp_page::load_pre_ramp_file(void)
{
	// Variables

	int i;
	char display_string[200];

	// Update data

	UpdateData(TRUE);

	// Loads pre_ramp file

	FILE *pre_ramp_file=fopen(lc_pre_ramp_file_string,"r");

	if (pre_ramp_file==NULL)
	{
		// File Error
		MessageBeep(MB_ICONEXCLAMATION);
		sprintf(display_string,"Error: The Pre-ramp file could not be opened");
		p_lc_sheet->p_lc_dialog->display_status(TRUE,display_string);
		sprintf(display_string,"File_string: %s",lc_pre_ramp_file_string);
		p_lc_sheet->p_lc_dialog->display_status(TRUE,display_string);
	}
	else
	{
		// Load file
		fscanf(pre_ramp_file,"%i",&lc_pre_ramp_trials);
		for (i=0;i<lc_pre_ramp_trials;i++)
			fscanf(pre_ramp_file,"%f",&lc_ramp_pre_ms_array[i]);
		fclose(pre_ramp_file);
	}
}

void LC_ramp_page::display_batch_values(void)
{
	// Variables

	int i;
	char display_string[200];

	float x_anchor=(float)0.63;
	float y_anchor=(float)0.28;
	float x_right=(float)0.98;
	float y_bottom=(float)0.98;

	float y_spacing=(float)0.07;

    // Code

	// Create display dc

	CClientDC dc(this);
	
	// Create font

	dc.SetBkMode(TRANSPARENT);
	CFont display_font;
	display_font.CreateFont(12,0,0,0,400,FALSE,FALSE,0,ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_SWISS,"Arial");
	CFont* pOldFont = dc.SelectObject(&display_font);

	// Check window dimensions

	CRect current_window;
	GetWindowRect(current_window);
	current_window.NormalizeRect();

	// Draws over status display area in grey

	CPen* pOriginalPen;
	CPen BackgroundPen;
	BackgroundPen.CreatePen(PS_SOLID,1,GetSysColor(COLOR_HIGHLIGHT));
	pOriginalPen = dc.SelectObject(&BackgroundPen);

    CBrush* pOriginalBrush;
	CBrush BackgroundBrush;
	BackgroundBrush.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	pOriginalBrush = dc.SelectObject(&BackgroundBrush);

    dc.Rectangle((int)(x_anchor*current_window.Width()),(int)(y_anchor*current_window.Height()),
		(int)(x_right*current_window.Width()),(int)(y_bottom*current_window.Height()));
	
	// Now displays text

	dc.SelectObject(pOriginalPen);

	dc.TextOut(int(x_anchor*current_window.Width()),int(y_anchor*current_window.Height()),
		" N  Pre   Rel Size/Time Delay");
	
	for (i=0;i<no_of_lc_ramp_trials;i++)
	{
		sprintf(display_string,"% 2i) %5.0f %6.2f %6.2f %5.0f",
			(i+1),lc_ramp_pre_ms_array[i],lc_ramp_rel_size_array[i],
				lc_ramp_rel_time_array[i],lc_ramp_delay_array[i]);
		dc.TextOut(int(x_anchor*current_window.Width()),
			int((y_anchor+((i+1)*y_spacing))*current_window.Height()),display_string);
	}
}

void LC_ramp_page::OnPaint()
{
	CPaintDC dc(this);

	display_batch_values();
}