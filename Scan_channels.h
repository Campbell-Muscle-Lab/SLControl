#if !defined(AFX_SCAN_CHANNELS_H__AE037539_7B8F_450A_B13A_819BDC3E9182__INCLUDED_)
#define AFX_SCAN_CHANNELS_H__AE037539_7B8F_450A_B13A_819BDC3E9182__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Scan_channels.h : header file
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
// Scan_channels dialog

class Scan_channels : public CDialog
{
// Construction
public:
	Scan_channels(CWnd* pParent = NULL);   // standard constructor

	Scan_channels::~Scan_channels(void);

	BOOL Scan_channels::Create();

// Dialog Data
	//{{AFX_DATA(Scan_channels)
	enum { IDD = IDD_Scan_channels_dialog };
	float	force_value_volts;
	float	fl_value_volts;
	float	sl_int_value_volts;
	float	sl_value_volts;
	float	time_seconds;
	//}}AFX_DATA

	// Variables

	CWnd* m_pParent;							// pointer to parent window
	int m_nID;									// dialog ID number

	// Window size and position

	float window_x_pos;
	float window_y_pos;
	float window_x_size;
	float window_y_size;

	// Device context

	CClientDC* p_display_dc;

	// Data record

	Data_record Scan_channels_record;
	Data_record* p_Scan_channels_record;

	// File handling Variables

	CString Scan_channels_path_string;					// string holding associated directory path
	CString dapl_source_file_string;					// string for DAPL source_file

	// Scan status

	int scanning_status;								// TRUE if object is scanning data
														// FALSE if not

	int record_point;									// index of current data point

	// Plots
	
	int no_of_plots;

	Plot FOR_plot;
	Plot* p_FOR_plot;

	Plot SL_plot;
	Plot* p_SL_plot;
	
	Plot FL_plot;				
	Plot* p_FL_plot;
	
	Plot SL_INT_plot;
	Plot* p_SL_INT_plot;

	Plot* pointers_array[7];

	
	// Functions

	void single_scan(void);								// runs single scan

	void clear_window(void);							// clears window

	void remote_shut_down(void);						// called by parent to shut down window

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Scan_channels)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Scan_channels)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCAN_CHANNELS_H__AE037539_7B8F_450A_B13A_819BDC3E9182__INCLUDED_)
