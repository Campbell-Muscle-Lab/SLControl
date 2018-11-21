#include "afxwin.h"
#if !defined(AFX_SUPERPOSITION_CONTROL_H__714A4D6F_E29A_44E9_B02E_52CB29CAD380__INCLUDED_)
#define AFX_SUPERPOSITION_CONTROL_H__714A4D6F_E29A_44E9_B02E_52CB29CAD380__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Superposition_control.h : header file
//

class Superposition;								// forward declaration

/////////////////////////////////////////////////////////////////////////////
// Superposition_control dialog

class Superposition_control : public CDialog
{
// Construction
public:
	Superposition_control(CWnd* pParent = NULL,
		Superposition* set_p_superposition_display = NULL);
													// standard constructor

	Superposition_control::~Superposition_control(void);
	BOOL Superposition_control::Create();

	CWnd* m_pParent;
	int m_nID;

				
	Superposition* p_superposition_display;			// pointer to the parent window

	// Window position and dimensions

	float window_x_pos;
	float window_y_pos;
	float window_x_size;
	float window_y_size;

	// Functions

	void close_superposition_control_window(void);	// called by parent when closing

// Dialog Data
	//{{AFX_DATA(Superposition_control)
	enum { IDD = IDD_SUPERPOSITION_CONTROL };
	CString	average_file_string;
	BOOL	plot_calibrated_values;
	BOOL	align_traces;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Superposition_control)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Superposition_control)
	virtual BOOL OnInitDialog();
	afx_msg void OnPlotOtherTraces();
	afx_msg void OnAdjustFirstTrace();
	afx_msg void OnAverage();
	afx_msg void OnCalibratedValues();
	afx_msg void OnShuffleOrder();
	afx_msg void OnBnClickedAlign();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	
	
	afx_msg void OnAligntraces();
	BOOL align_traces_to_cursor;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUPERPOSITION_CONTROL_H__714A4D6F_E29A_44E9_B02E_52CB29CAD380__INCLUDED_)
