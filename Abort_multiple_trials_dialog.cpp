// Abort_multiple_trials_dialog.cpp : implementation file
//

#include "stdafx.h"
#include "SLControl.h"
#include "Abort_multiple_trials_dialog.h"
#include ".\abort_multiple_trials_dialog.h"

#include "Abort_multiple_trials.h"
#include "Length_Control_Dialog.h"


// Abort_multiple_trials_dialog dialog

IMPLEMENT_DYNAMIC(Abort_multiple_trials_dialog, CDialog)
Abort_multiple_trials_dialog::Abort_multiple_trials_dialog(CWnd* pParent,Abort_multiple_trials* set_p_amt)
	: CDialog(Abort_multiple_trials_dialog::IDD, pParent)
{
	m_pParent = pParent;
	m_nID = Abort_multiple_trials_dialog::IDD;

	p_amt = set_p_amt;
}

Abort_multiple_trials_dialog::~Abort_multiple_trials_dialog()
{

}

void Abort_multiple_trials_dialog::PostNcDestroy() 
{
	MessageBox("AMTD:PNC:Destroy");
	delete this;
}

BOOL Abort_multiple_trials_dialog::Create()
{
	return CDialog::Create(m_nID, m_pParent);
}

void Abort_multiple_trials_dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Abort_multiple_trials_dialog, CDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_Abort, OnBnClickedAbort)
END_MESSAGE_MAP()


// Abort_multiple_trials_dialog message handlers

BOOL Abort_multiple_trials_dialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	int width=1022;																// screen width and height
	int height=740;																// in pixels

	//SetWindowPos(NULL,int(0.68*width),(int)(0.07*height),
	//	(int)(0.21*width),(int)(0.43*height),SWP_SHOWWINDOW);

	SetWindowPos(&wndTopMost,100,100,400,150,SWP_SHOWWINDOW);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void Abort_multiple_trials_dialog::OnClose()
{
	MessageBox("AMTD:OnClose");
	DestroyWindow();
}

void Abort_multiple_trials_dialog::manual_shut_down()
{
	MessageBox("Manual shut down");
	p_amt->p_amtd=NULL;

	MessageBox("Manual shut down 2");
	SetFocus();

	DestroyWindow();
}

void Abort_multiple_trials_dialog::OnBnClickedAbort()
{
	p_amt->p_lc_dialog->trials_left=0;

	//DestroyWindow();
}

BOOL Abort_multiple_trials_dialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	return CDialog::PreTranslateMessage(pMsg);
}
