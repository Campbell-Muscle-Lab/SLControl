#if !defined(AFX_EXPERIMENT_DISPLAY_H__D18D0475_6FBD_4D20_8810_BD7C806BE993__INCLUDED_)
#define AFX_EXPERIMENT_DISPLAY_H__D18D0475_6FBD_4D20_8810_BD7C806BE993__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Experiment_display.h : header file
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
// Experiment_display dialog

class Experiment_display : public CDialog
{
// Construction
public:
	Experiment_display::Experiment_display(CWnd* pParent,
		Data_record* set_p_Experiment_display_record);

	Experiment_display::~Experiment_display(void);
	BOOL Experiment_display::Create(void);

// Dialog Data
	//{{AFX_DATA(Experiment_display)
	enum { IDD = IDD_Experiment_display };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	CWnd* m_pParent;
	int m_nID;

	// Data record

	Data_record Experiment_display_record;
	Data_record* p_Experiment_display_record;

	// Window size and position

	float window_x_pos;
	float window_y_pos;
	float window_x_size;
	float window_y_size;

	// Plots and pointers to them

	int no_of_plots;

	Plot FOR_plot;
	Plot* p_FOR_plot;

	Plot SL_plot;
	Plot* p_SL_plot;
	
	Plot FL_plot;				
	Plot* p_FL_plot;
	
	Plot FOR_SL_plot;
	Plot* p_FOR_SL_plot;
	
	Plot FOR_FL_plot;
	Plot* p_FOR_FL_plot;

	Plot SL_FL_plot;
	Plot* p_SL_FL_plot;

	Plot* pointers_array[7];

	// Device context

	CClientDC* p_display_dc;

	// Functions

	void clear_window(void);

	void close_experiment_display_window(void);

	void update_plots();

	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Experiment_display)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Experiment_display)
	virtual BOOL OnInitDialog();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPERIMENT_DISPLAY_H__D18D0475_6FBD_4D20_8810_BD7C806BE993__INCLUDED_)
