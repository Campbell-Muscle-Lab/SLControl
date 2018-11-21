// Channel_cont.cpp : implementation file
//

#include "stdafx.h"
#include "SLControl.h"
#include "Channel_control.h"

#include "Channel_tab.h"


// Channel_cont

IMPLEMENT_DYNAMIC(Channel_control, CTabCtrl)
Channel_control::Channel_control()
{
	no_of_tab_pages=1;

	tab_pages[0]=new Channel_tab;
}

Channel_control::~Channel_control()
{
	// Delete tabs

	int i;

	for (i=0;i<no_of_tab_pages;i++)
		delete tab_pages[i];
}


BEGIN_MESSAGE_MAP(Channel_control, CTabCtrl)
END_MESSAGE_MAP()



// Channel_control message handlers

void Channel_control::Init(void)
{
	current_page=0;

	tab_pages[0]->Create(IDD_CHANNEL_TAB,this);

	tab_pages[0]->ShowWindow(SW_SHOW);

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


