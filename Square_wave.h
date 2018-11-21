#if !defined(AFX_SQUARE_WAVE_H__D5C025BF_59BB_41E3_8B01_C8CF4C1C8973__INCLUDED_)
#define AFX_SQUARE_WAVE_H__D5C025BF_59BB_41E3_8B01_C8CF4C1C8973__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Square_wave.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Square_wave dialog

class Square_wave : public CDialog
{
// Construction
public:
	Square_wave(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Square_wave)
	enum { IDD = IDD_SQUARE_WAVE_DIALOG };
	float	fl_step_duration_ms;
	float	fl_step_size_microns;
	//}}AFX_DATA

	// Variables

	CString Square_wave_path_string;					// string holding associated directory path
	CString start_file_string;							// string for DAPL start_file
	CString stop_file_string;							// string for DAPL stop_file

	float time_increment_ms;							// time in ms between consecutive data points

	int square_wave_active;								// 0 if square wave is not running
														// 1 otherwise
	
	// Functions

	int write_start_file(void);							// writes start_file
	int write_stop_file(void);							// write stop_file

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Square_wave)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Square_wave)
	virtual BOOL OnInitDialog();
	afx_msg void OnStart();
	afx_msg void OnStop();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SQUARE_WAVE_H__D5C025BF_59BB_41E3_8B01_C8CF4C1C8973__INCLUDED_)
