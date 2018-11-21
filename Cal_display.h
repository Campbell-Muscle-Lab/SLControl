#if !defined(AFX_CAL_DISPLAY_H__52152FC4_DC8B_4764_A6FC_0A7D63997470__INCLUDED_)
#define AFX_CAL_DISPLAY_H__52152FC4_DC8B_4764_A6FC_0A7D63997470__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Cal_display.h : header file
//

////////////////////////////
// Ken's code starts here //
////////////////////////////

#include "Plot.h"
#include "Data_record.h"

////////////////////////////
//  Ken's code ends here  //
////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Cal_display dialog

class Cal_display : public CDialog
{
// Construction
public:

	////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	Cal_display::Cal_display(CWnd* pParent,Data_record* set_p_Cal_display_record);
	Cal_display::~Cal_display();
	BOOL Cal_display::Create();
	
	CWnd* m_pParent;
	int m_nID;
	
	void close_Cal_display_window(void);

	int no_of_plots;

	Plot* pointers_array[5];

	Plot FL_plot;						// FL plot
	Plot* p_FL_plot;					// and a pointer to it

	Plot SL_plot;						// SL plot
	Plot* p_SL_plot;					// and a pointer to it

	Plot* p_Force_plot;

	Data_record Cal_display_record;
	Data_record* p_Cal_display_record;

	// Device context

	CClientDC* p_display_dc;

	// Functions

	void update_plots(void);
	void clear_window(void);

	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////

// Dialog Data
	//{{AFX_DATA(Cal_display)
	enum { IDD = IDD_Calibration_Display };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Cal_display)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Cal_display)
	virtual BOOL OnInitDialog();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAL_DISPLAY_H__52152FC4_DC8B_4764_A6FC_0A7D63997470__INCLUDED_)
