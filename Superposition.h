#if !defined(AFX_SUPERPOSITION_H__3767F79C_5C87_4A75_B5D8_6FD4B916F61D__INCLUDED_)
#define AFX_SUPERPOSITION_H__3767F79C_5C87_4A75_B5D8_6FD4B916F61D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Superposition.h : header file
//

////////////////////////////
// Ken's code starts here //
////////////////////////////

#include "Data_record.h"
#include "Plot.h"
#include "Global_definitions.h"
#include "Superposition_control.h"

////////////////////////////
//  Ken's code ends here  //
////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Superposition dialog

class Superposition : public CDialog
{
// Construction
public:
	Superposition(CWnd* pParent = NULL,CString* set_p_file_strings=NULL,int set_no_of_traces=0);
												// constructor

	~Superposition(void);						// destructor

	BOOL Create();	// create window

	CWnd* m_pParent;							// pointer to parent window
	int m_nID;									// dialog ID number

	Data_record superposition_record;			// default data record
	Data_record* p_data_record;					// pointer to associated data record

	int no_of_traces;							// no of traces to plot

	CString * p_file_strings;					// pointer to an array of file strings
	CString file_strings[MAX_SUPERPOSED_FILES];
												// an array of the file strings

	Superposition_control* p_superposition_control;
												// pointer to daughter Superposition_control window

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

	Plot* pointers_array[MAX_NO_OF_PLOTS];

	// First plot cursor values

	float first_record_active_cursor_values[NO_OF_CHANNELS];

	// Screen DC

	CClientDC* p_display_dc;

	// Screen controls

	int multiple_plots_active;									// 1 if multiple plots have been drawn
																// 0 otherwise

	// Functions

	void Superposition_control_Window_Done(void);				// called by daughter window when it closes

	void clear_window(void);									// clears screen

	void plot_first_trace(void);								// plots the graph axes and the first record
	void plot_single_trace(int colour);							// plots a single trace in the specified colour		
	void plot_remaining_traces(int increment_colour);			// plots remaining traces
																// increments colour if increment_colour>0

	void display_file_name(int increment,int colour);			// displays filename in appropriate colour


	void shuffle_trace_order(void);								// reverses the order of file_strings array

	void remote_shut_down(void);								// called by parent to shut down window

	COLORREF return_colour(int colour);							// returns RGB colour

// Dialog Data
	//{{AFX_DATA(Superposition)
	enum { IDD = IDD_SUPERPOSITION_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Superposition)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Superposition)
	virtual BOOL OnInitDialog();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnClose();
	afx_msg void OnPaint();
	afx_msg void OnFileCopytoclipboard();
	afx_msg void OnFileExit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUPERPOSITION_H__3767F79C_5C87_4A75_B5D8_6FD4B916F61D__INCLUDED_)
