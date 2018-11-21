#pragma once


// LC_dout_tab dialog

class LC_dout_tab : public CDialog
{
	DECLARE_DYNAMIC(LC_dout_tab)

public:
	LC_dout_tab(CWnd* pParent = NULL, int set_tab_number=0);
	virtual ~LC_dout_tab();

	int tab_number;
	int channel_number;

	LC_dout_page* p_lc_dout_page;

	void display_channel_number(void);

// Dialog Data
	enum { IDD = IDD_LC_DOUT_TAB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	CString lc_dout_tab_freeform_filestring;
	afx_msg void OnBnClickedBrowse();
protected:
	
};
