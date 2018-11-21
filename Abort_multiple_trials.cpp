// Abort_multiple_trials.cpp : implementation file
//

#include "stdafx.h"
#include "SLControl.h"
#include "Abort_multiple_trials.h"

#include "Abort_multiple_trials_dialog.h"
#include "SL_SREC.h"
#include "Length_Control_Dialog.h"
#include ".\abort_multiple_trials.h"

// Abort_multiple_trials

IMPLEMENT_DYNCREATE(Abort_multiple_trials, CWinThread)

Abort_multiple_trials::Abort_multiple_trials()
{
}

Abort_multiple_trials::~Abort_multiple_trials()
{
	AfxMessageBox("In thread destructor");
//	p_amtd->OnClose();

//	delete p_amtd;

	//delete this;

}

BOOL Abort_multiple_trials::InitInstance()
{
	p_amtd = new Abort_multiple_trials_dialog(NULL,this);
	p_amtd->Create();

	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int Abort_multiple_trials::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here

	p_lc_dialog->MessageBox("In thread destructor");
	delete p_amtd;

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(Abort_multiple_trials, CWinThread)
END_MESSAGE_MAP()


// Abort_multiple_trials message handlers

void Abort_multiple_trials::self_close()
{
	AfxMessageBox("p_amt->self_close");

//	p_amtd->OnBnClickedAbort();

	//p_amtd->DestroyWindow();
	
	AfxEndThread(3,TRUE);

	

	//delete p_amtd;

	//AfxEndThread(3,TRUE);
}

BOOL Abort_multiple_trials::OnIdle(LONG lCount)
{
	// TODO: Add your specialized code here and/or call the base class

	CWinThread::OnIdle(lCount);

	if (lCount>=2)
	{
		if (p_lc_dialog->close_abort_window==1)
		{
			p_amtd->OnClose();
			AfxEndThread(3,TRUE);
		}
	}

	return TRUE;
}
