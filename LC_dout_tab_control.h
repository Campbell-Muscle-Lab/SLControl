#pragma once

// LC_dout_tab_control

class LC_dout_page;
class LC_dout_tab;

class LC_dout_tab_control : public CTabCtrl
{
	DECLARE_DYNAMIC(LC_dout_tab_control)

public:
	LC_dout_tab_control();
	virtual ~LC_dout_tab_control();

	LC_dout_page* p_lc_dout_page;
	
	int no_of_tabs;
	int current_page;
	LC_dout_tab* p_lc_dout_tab[17];


	void Init(void);
	void SetRectangle(void);
	void OnLButtonDown(UINT nFlags, CPoint point);

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
};


