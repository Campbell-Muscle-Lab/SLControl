#pragma once


// Update_check dialog

class Update_check : public CDialog
{
	DECLARE_DYNAMIC(Update_check)

public:
	Update_check(CWnd* pParent = NULL);   // standard constructor
	virtual ~Update_check();

	BOOL Update_check::Create();

	CWnd* m_pParent;							// Pointer to parent window
	int m_nID;

	void remote_shut_down(void);						// called by parent to shut down window
	void display_text(void);


// Dialog Data
	enum { IDD = IDD_CHECKING_FOR_UPDATES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();


	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
};
