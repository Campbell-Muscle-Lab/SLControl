// LC_dout_tab_control.cpp : implementation file
//

#include "stdafx.h"
#include "SLControl.h"
#include "LC_dout_tab_control.h"
#include "LC_dout_page.h"
#include "LC_dout_tab.h"
#include ".\lc_dout_tab_control.h"

// LC_dout_tab_control

IMPLEMENT_DYNAMIC(LC_dout_tab_control, CTabCtrl)
LC_dout_tab_control::LC_dout_tab_control()
{
	// Constructor

	int i;

	no_of_tabs=16;

	for (i=0;i<no_of_tabs;i++)
	{
		p_lc_dout_tab[i]=new LC_dout_tab(this,i);
	}
}

LC_dout_tab_control::~LC_dout_tab_control()
{
	// Destructor
}

void LC_dout_tab_control::PostNcDestroy()
{
	int i;

	for (i=0;i<no_of_tabs;i++)
	{
		delete p_lc_dout_tab[i];
	}
}

BEGIN_MESSAGE_MAP(LC_dout_tab_control, CTabCtrl)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// LC_dout_tab_control message handlers

void LC_dout_tab_control::Init(void)
{
	// Initialization - must be called before parent finished InitDialog;

	// Variables

	int i;

	// Code

	for (i=0;i<no_of_tabs;i++)
	{
		p_lc_dout_tab[i]->Create(IDD_LC_DOUT_TAB,this);
		p_lc_dout_tab[i]->p_lc_dout_page=p_lc_dout_page;
	}

	p_lc_dout_tab[0]->ShowWindow(SW_SHOW);
	current_page=0;

	for (i=1;i<no_of_tabs;i++)
		p_lc_dout_tab[i]->ShowWindow(SW_HIDE);

	SetRectangle();
}


void LC_dout_tab_control::SetRectangle(void)
{
	CRect tabRect, itemRect;
	int nX, nY, nXc, nYc;

	GetClientRect(&tabRect);
	GetItemRect(0, &itemRect);

	nX=itemRect.left;
	nY=itemRect.bottom+1;
	nXc=tabRect.right-itemRect.left-1;
	nYc=tabRect.bottom-nY-1;

	p_lc_dout_tab[0]->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
	for(int nCount=1; nCount < no_of_tabs; nCount++){
		p_lc_dout_tab[nCount]->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_HIDEWINDOW);
	}
}

void LC_dout_tab_control::OnLButtonDown(UINT nFlags, CPoint point)
{
	// Makes window active when clicked by mouse

	CTabCtrl::OnLButtonDown(nFlags, point);

	if(current_page != GetCurFocus()){
		p_lc_dout_tab[current_page]->ShowWindow(SW_HIDE);
		current_page=GetCurFocus();
		p_lc_dout_tab[current_page]->ShowWindow(SW_SHOW);
		p_lc_dout_tab[current_page]->SetFocus();
	}

	UpdateData(TRUE);
}
