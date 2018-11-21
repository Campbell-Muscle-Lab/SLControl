#if !defined(AFX_FILE_HEADER_H__AA9A3AC7_D5DB_4EAB_A595_B6AAB8B38456__INCLUDED_)
#define AFX_FILE_HEADER_H__AA9A3AC7_D5DB_4EAB_A595_B6AAB8B38456__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// File_header.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// File_header dialog

class File_header : public CDialog
{
// Construction
public:
	File_header(CWnd* pParent = NULL, CString set_data_file_string="");	// constructor
	
	~File_header(void);													// destructor

	BOOL Create();	// create window

	CWnd* m_pParent;							// pointer to parent window
	int m_nID;									// dialog ID number

	// Variables

	CString data_file_string;					// filename of the data file

	// Window position and dimensions

	float window_x_pos;
	float window_y_pos;
	float window_x_size;
	float window_y_size;

	// List box size and dimensions

	float list_x_pos;
	float list_y_pos;
	float list_x_size;
	float list_y_size;

	// Max line length

	int max_line_length;

	// Functions

	void remote_shut_down(void);

	int read_header(void);

// Dialog Data
	//{{AFX_DATA(File_header)
	enum { IDD = IDD_FILE_HEADER };
	CListBox	file_header_list_box;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(File_header)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();

	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(File_header)
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILE_HEADER_H__AA9A3AC7_D5DB_4EAB_A595_B6AAB8B38456__INCLUDED_)
