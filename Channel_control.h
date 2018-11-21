#pragma once

#include "Channel_tab.h"


// Channel_control

class Triggers; // forward declaration

class Channel_control : public CTabCtrl
{
	DECLARE_DYNAMIC(Channel_control)

	Channel_tab* tab_pages[17];

	Triggers* p_triggers;

	int current_page;
	int no_of_tab_pages;

	void update_tab_calculations();

public:
	Channel_control();
	virtual ~Channel_control();

	void Init();
	void SetRectangle();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


