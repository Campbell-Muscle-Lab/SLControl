#if !defined(AFX_ANALYSIS_DISPLAY_H__24711E41_E00E_4013_860E_EB780555DCFD__INCLUDED_)
#define AFX_ANALYSIS_DISPLAY_H__24711E41_E00E_4013_860E_EB780555DCFD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Analysis_display.h : header file
//

////////////////////////////
// Ken's code starts here //
////////////////////////////

#include "Data_record.h"
#include "Plot.h"
#include "Fit_control.h"

////////////////////////////
//  Ken's code ends here  //
////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Analysis_display dialog

class Fit_parameters;

class Analysis_display : public CDialog
{
// Construction
public:
	Analysis_display(CWnd* pParent = NULL, Data_record* set_p_data_record = NULL);
																	// constructor
	
	~Analysis_display(void);										// destructor

	BOOL Create();	// create window

	CWnd* m_pParent;							// pointer to parent window
	int m_nID;									// dialog ID number

	Data_record* p_data_record;					// pointer to associated data record

	CString data_record_filename;				// filename of data record

	Fit_control* p_fit_control;					// pointer to daughter Fit_control window

	Fit_parameters* p_window_fit_parameters;	// pointer to Fit_parameters
	int window_fit_colour;						// last drawn colour
	int fit_active;								// indictates whether a fit has been performed
	

	// Window position and dimensions

	float window_x_pos;
	float window_y_pos;
	float window_x_size;
	float window_y_size;

	// Text anchor

	float x_text_anchor;
	float y_text_anchor;

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

	Plot* pointers_array[MAX_NO_OF_PLOTS];

	// Functions

	void Fit_control_Window_Done(void);			// called when the Fit_control dialog box closes

	void update_plots(void);					// updates plots by redrawing screen

	void clear_window(void);					// clears screen

	void draw_fit(Fit_parameters* p_fit_parameters, int colour);

	void display_record_parameters(void);

	void print_record_parameters(CMetaFileDC* p_display_dc);

	void remote_shut_down(void);				// shut down by remote parent

	void remote_set_cursors(int active_cursor_position, int inactive_cursor_position);

	void remote_expand_trace(void);

	void remote_draw_force_line(float x_start,float y_start,float x_stop,float y_stop,int colour);

	void set_FORCE_mean_values_line_segment(int number,int status,
		float x_start,float y_start,float x_stop,float y_stop,int colour);

// Dialog Data
	//{{AFX_DATA(Analysis_display)
	enum { IDD = IDD_ANALYSIS_DISPLAY };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Analysis_display)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Analysis_display)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPaint();
	afx_msg void OnFilePrint();
	afx_msg void OnFileExit();
	afx_msg void OnTransformChangetocalibratedvalues();
	afx_msg void OnTransformChangetorawvoltages();
	afx_msg void OnFileDumptracevaluestofile();
	afx_msg void OnOffsetRescaleforcetoactivecursor();
	afx_msg void OnPeakPositions();
	afx_msg void OnPeakResetfile();
	afx_msg void OnPeakAddpoint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANALYSIS_DISPLAY_H__24711E41_E00E_4013_860E_EB780555DCFD__INCLUDED_)
