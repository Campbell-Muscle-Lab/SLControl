// LC_dout_tab.cpp : implementation file
//

#include "stdafx.h"
#include "SLControl.h"
#include "LC_dout_page.h"
#include "LC_dout_tab.h"
#include ".\lc_dout_tab.h"


// LC_dout_tab dialog

IMPLEMENT_DYNAMIC(LC_dout_tab, CDialog)
LC_dout_tab::LC_dout_tab(CWnd* pParent /*=NULL*/,int set_tab_number)
	: CDialog(LC_dout_tab::IDD, pParent)
	
{
	tab_number=set_tab_number;
	channel_number=tab_number;

	lc_dout_tab_freeform_filestring="c:\\temp\\freeform_file.txt";
}

LC_dout_tab::~LC_dout_tab()
{

}

void LC_dout_tab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_FREE_FORM_FILE, lc_dout_tab_freeform_filestring);
}

BEGIN_MESSAGE_MAP(LC_dout_tab, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BROWSE, OnBnClickedBrowse)
END_MESSAGE_MAP()

// LC_dout_tab message handlers

void LC_dout_tab::OnPaint()
{
	CPaintDC dc(this);
	display_channel_number();
}

void LC_dout_tab::display_channel_number(void)
{
	// Display channel number in top right of tab dialog

	// Create display dc

	CClientDC dc(this);
	
	// Create font

	dc.SetBkMode(TRANSPARENT);
	CFont display_font;
	display_font.CreateFont(16,0,0,0,FW_BOLD /*400*/,FALSE,FALSE,0,ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_SWISS,"Arial");
	CFont* pOldFont = dc.SelectObject(&display_font);

	// Check window dimensions

	CRect current_window;
	GetWindowRect(current_window);
	current_window.NormalizeRect();

	char display_string[20];
	sprintf(display_string,"DO Channel: %i",channel_number);
	dc.TextOut((int)(0.7*current_window.Width()),(int)(0.04*current_window.Height()),
		display_string);
}

void LC_dout_tab::OnBnClickedBrowse()
{
	// Allows user to browse for freeform file
	// Allows user to update lc_pre_ramp_file_string from dialog box
	// Initialises file open dialog

	UpdateData(TRUE);

	CFileDialog fileDlg(TRUE,NULL,"*.txt",OFN_HIDEREADONLY,"Text files (*.txt)|*.txt||",this);

	// Initialisation

	fileDlg.m_ofn.lpstrTitle="Select freeform file";

	// Call Dialog

	if (fileDlg.DoModal()==IDOK)
	{
		lc_dout_tab_freeform_filestring=fileDlg.GetPathName();
	}

	UpdateData(FALSE);
}

