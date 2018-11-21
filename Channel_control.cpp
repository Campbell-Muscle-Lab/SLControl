// Channel_control.cpp : implementation file
//

#include "stdafx.h"
#include "SLControl.h"
#include "Channel_control.h"

#include "Channel_tab.h"
#include "Triggers.h"


// Channel_control

IMPLEMENT_DYNAMIC(Channel_control, CTabCtrl)
Channel_control::Channel_control()
{
	int i;

	no_of_tab_pages=16;

	for (i=0;i<no_of_tab_pages;i++)
	{
		tab_pages[i]=new Channel_tab(this,i+1);
	}
}

Channel_control::~Channel_control()
{
	// Delete tabs

	int i;

	for (i=0;i<no_of_tab_pages;i++)
		delete tab_pages[i];
}


BEGIN_MESSAGE_MAP(Channel_control, CTabCtrl)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// Channel_control message handlers

void Channel_control::Init(void)
{
	int i;
	
	// Initialise values

	for (i=0;i<no_of_tab_pages;i++)
	{
		tab_pages[i]->p_triggers=p_triggers;
		tab_pages[i]->p_channel_control=this;
		
		tab_pages[i]->channel_active=FALSE;
	}

	current_page=0;

	for (i=0;i<no_of_tab_pages;i++)
		tab_pages[i]->Create(IDD_CHANNEL_TAB,this);

	tab_pages[0]->ShowWindow(SW_SHOW);

	for (i=1;i<no_of_tab_pages;i++)
		tab_pages[i]->ShowWindow(SW_HIDE);

	SetRectangle();
}

void Channel_control::SetRectangle(void)
{
	CRect tabRect, itemRect;
	int nX, nY, nXc, nYc;

	GetClientRect(&tabRect);
	GetItemRect(0, &itemRect);

	nX=itemRect.left;
	nY=itemRect.bottom+1;
	nXc=tabRect.right-itemRect.left-1;
	nYc=tabRect.bottom-nY-1;

	tab_pages[0]->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
	for(int nCount=1; nCount < no_of_tab_pages; nCount++){
		tab_pages[nCount]->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_HIDEWINDOW);
	}
}

void Channel_control::OnLButtonDown(UINT nFlags, CPoint point)
{
	// Makes window active when clicked by mouse

	CTabCtrl::OnLButtonDown(nFlags, point);

	if(current_page != GetCurFocus()){
		tab_pages[current_page]->ShowWindow(SW_HIDE);
		current_page=GetCurFocus();
		tab_pages[current_page]->ShowWindow(SW_SHOW);
		tab_pages[current_page]->SetFocus();
	}

	UpdateData(TRUE);
}

void Channel_control::update_tab_calculations(void)
{
	// Update trigger_arrays

	for (int i=0;i<no_of_tab_pages;i++)
	{
		tab_pages[i]->update_trigger_array();
	}
}

