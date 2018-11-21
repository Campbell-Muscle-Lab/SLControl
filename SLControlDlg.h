// SLControlDlg.h : header file
//

////////////////////////////
// Ken's code starts here //
////////////////////////////

#include "SL_SREC.h"
#include "Tension_control_dialog.h"
#include "Length_control_dialog.h"
#include "Fit_control.h"
#include "Analysis_display.h"
#include "Square_wave.h"
#include "Scan_channels.h"
#include "Superposition.h"
#include "Batch_analysis.h"
#include "Cyclical.h"
#include "Chirp.h"
#include "File_header.h"
#include "Triggers.h"
#include "Update_check.h"

#include <wininet.h>

////////////////////////////
//  Ken's code ends here  //
////////////////////////////

#if !defined(AFX_SLCONTROLDLG_H__722C4831_520D_4E6E_BAC7_5AC95D50043D__INCLUDED_)
#define AFX_SLCONTROLDLG_H__722C4831_520D_4E6E_BAC7_5AC95D50043D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CSLControlDlg dialog

class CSLControlDlg : public CDialog
{
// Construction
public:
	CSLControlDlg(CWnd* pParent = NULL);	// standard constructor

	////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	Data_record analysis_record;			// data record for analysis window
	Data_record* p_analysis_record;			// pointer to the record


	void SL_SREC_Window_Done(void);			// called when the SL_SREC dialog box
											// is closed

	void Length_control_Window_Done();		// called when the Length_control dialog box
											// is closed

	void Tension_control_Window_Done();		// called when the Tension_control dialog box
											// is closed

	void Analysis_display_Window_Done();	// called when the Analysis_display dialog box
											// is closed

	void Scan_channels_Window_Done();		// called when the Scan_channels dialog box
											// is closed

	void Superposition_Window_Done();		// called when the Superposition dialog is closed

	void Batch_analysis_Window_Done(void);	// called when the Batch_analysis dialog is closed

	void Cyclical_Window_Done(void);		// called when the Cyclical dialog box is closed

	void Chirp_Window_Done(void);			// called when the Chirp dialog box is closed

	void Triggers_Window_Done(void);		// called when the Triggers dialog box is closed

	void File_header_Window_Done(void);		// called when the File header dialog box is closed

	void Update_check_Window_Done(void);	// called when the Update check dialog box is closed

	CSLControlDlg* p_SLControlDlg;			// pointer to master Dialog box

	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////

// Dialog Data
	//{{AFX_DATA(CSLControlDlg)
	enum { IDD = IDD_SLCONTROL_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSLControlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	SL_SREC* p_SL_SREC;													// pointer to SL_SREC window

	Tension_control_dialog* p_Tension_control_dialog;					// pointer to Tension_control window

	Length_control_dialog* p_Length_control_dialog;						// pointer to Length_control window

	Cyclical* p_Cyclical;												// pointer to Cyclical window

	Chirp* p_Chirp;														// pointer to a Chirp window

	Triggers* p_Triggers;												// pointer to a Trigger window

	Analysis_display* p_analysis_display;								// pointer to Analysis display window

	Scan_channels* p_Scan_channels_dialog;								// pointer to Scan_channels_dialog

	Superposition* p_superposition_display;								// pointer to Superposition display window

	Batch_analysis* p_batch_analysis;									// pointer to Batch_analysis dialog

	File_header* p_file_header;											// pointer to File_header dialog

	Update_check* p_update_check;										// pointer to Update_check dialog


	void read_calibration_parameters_from_file(void);					// reads calibration_parameters from
																		// file using a global function

	
	// Memory leak detection
	
	CMemoryState oldMemState, newMemState, diffMemState;
    
	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////

	// Generated message map functions
	//{{AFX_MSG(CSLControlDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnExperimentsSlsrec();
	afx_msg void OnFileExit();
	afx_msg void OnHelpAbout();
	afx_msg void OnHelpCalibrationparameters();
	afx_msg void OnAnalysisSingledatafile();
	afx_msg void OnSystemchecksSquarewavefl();
	afx_msg void OnSystemchecksReadchannels();
	afx_msg void OnHelpDocumentation();
	afx_msg void OnAnalysisSuperposition();
	afx_msg void OnExperimentsTensioncontrol();
	afx_msg void OnHelpVersioninformation();
	afx_msg void OnAnalysisBatchanalysis();
	afx_msg void OnClose();
	afx_msg void OnExperimentsCyclical();
	afx_msg void OnExperimentsChirp();
	afx_msg void OnAnalysisReaddatafileheader();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:afx_msg void OnExperimentsTriggers();
	afx_msg void OnFileSaveexperimentprotocol();
	afx_msg void OnFileLoadexperimentprotocol();
	afx_msg void OnHelpCheckforupdate();
	afx_msg void OnSystemchecksSetmemstate();
	afx_msg void OnSystemchecksCheckmemory();
	afx_msg void OnExperimentsLength();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SLCONTROLDLG_H__722C4831_520D_4E6E_BAC7_5AC95D50043D__INCLUDED_)
