#pragma once

// Forward declaration

class Abort_multiple_trials;

// Abort_multiple_trials_dialog dialog

class Abort_multiple_trials_dialog : public CDialog
{
	DECLARE_DYNAMIC(Abort_multiple_trials_dialog)

public:
	Abort_multiple_trials_dialog(CWnd* pParent = NULL,Abort_multiple_trials* set_p_amt = NULL);
		// standard constructor
	virtual ~Abort_multiple_trials_dialog();
	BOOL Abort_multiple_trials_dialog::Create(void);

	CWnd* m_pParent;
	int m_nID;

	Abort_multiple_trials* p_amt;

	void manual_shut_down(void);

// Dialog Data
	enum { IDD = IDD_MULTIPLE_TRIALS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void PostNcDestroy();
public:
	afx_msg void OnClose();
	afx_msg void OnBnClickedAbort();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
