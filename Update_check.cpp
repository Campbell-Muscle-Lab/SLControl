// Update_check.cpp : implementation file
//

#include "stdafx.h"
#include "SLControl.h"
#include "Update_check.h"

#include "SLControlDlg.h"


// Update_check dialog

IMPLEMENT_DYNAMIC(Update_check, CDialog)
Update_check::Update_check(CWnd* pParent /*=NULL*/)
	: CDialog(Update_check::IDD, pParent)
{
	m_pParent=pParent;
	m_nID=Update_check::IDD;
}

Update_check::~Update_check()
{
}

BOOL Update_check::Create()
{
	return CDialog::Create(m_nID, m_pParent);
}

void Update_check::PostNcDestroy() 
{
	delete this;
}


void Update_check::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Update_check, CDialog)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// Update_check message handlers

void Update_check::OnClose()
{
	// Signal to parent that this window is closing

	((CSLControlDlg*)m_pParent)->Update_check_Window_Done();

	DestroyWindow();
}

void Update_check::remote_shut_down(void)
{
	// Called by parent to shut down window

	OnClose();
}

BOOL Update_check::OnInitDialog()
{
	CDialog::OnInitDialog();

	char display_string[10];

	// Set window size and position

	int width=1022;
	int height=740;
	
	SetWindowPos(NULL,(int)(0.5*width),(int)(0.45*height),
		(int)(0.35*width),(int)(0.1*height),SWP_SHOWWINDOW);

	display_text();

	// Delay

	int i,j;

	for (i=1;i<=100;i++)
	{
		for (j=1;j<=400;j++)
			sprintf(display_string,"%i",i);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void Update_check::display_text(void)
{
	// Code

	// Create display dc

	CClientDC dc(this);
	
	// Create font

	dc.SetBkMode(TRANSPARENT);
	CFont display_font;
	display_font.CreateFont(16,0,0,0,400,FALSE,FALSE,0,ANSI_CHARSET,
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
	BackgroundPen.CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNFACE));
	pOriginalPen = dc.SelectObject(&BackgroundPen);

	CBrush* pOriginalBrush;
	CBrush BackgroundBrush;
	BackgroundBrush.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	pOriginalBrush = dc.SelectObject(&BackgroundBrush);

	dc.SelectObject(pOriginalPen);

	dc.TextOut((int)(0.1*current_window.Width()),(int)(0.1*current_window.Height()),"Checking UW-Physiology server for updates ...");
}