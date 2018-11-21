// Stimulus.cpp : implementation file
//

#include "stdafx.h"
#include "SLControl.h"
#include "Stimulus.h"


// Stimulus dialog

IMPLEMENT_DYNAMIC(Stimulus, CDialog)
Stimulus::Stimulus(CWnd* pParent /*=NULL*/)
	: CDialog(Stimulus::IDD, pParent)
{
}

Stimulus::~Stimulus()
{
}

void Stimulus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Stimulus, CDialog)
END_MESSAGE_MAP()


// Stimulus message handlers
