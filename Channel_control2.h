#pragma once


// Channel_control

class Channel_control : public CTabCtrl
{
	DECLARE_DYNAMIC(Channel_cont)

	CDialog *tab_pages[20];

	int current_page;
	int no_of_tab_pages;

public:
	Channel_control();
	virtual ~Channel_control();

	void Init();
	void SetRectangle();

protected:
	DECLARE_MESSAGE_MAP()
};


