#if !defined(AFX_Calibration_H__E88250E4_D963_4E50_AFEA_3FBFFAC72B4E__INCLUDED_)
#define AFX_Calibration_H__E88250E4_D963_4E50_AFEA_3FBFFAC72B4E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Calibration.h : header file
//

////////////////////////////
// Ken's code starts here //
////////////////////////////

#include "Cal_display.h"
#include "Data_record.h"
#include "Plot.h"

////////////////////////////
//  Ken's code ends here  //
////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Calibration dialog

class Calibration : public CDialog
{
// Construction
public:
	Calibration(CWnd* pParent = NULL);   // standard constructor
	Calibration::~Calibration(void);
	BOOL Calibration::Create(void);

	Calibration* p_Calibration;				// pointer to Calibration window

	CWnd* m_pParent;
	int m_nID;

	// Strings for DAPL source files

	CString Calibration_dir_path;

	CString header_file_string;
	CString fill_pipes_file_string;
	CString create_pipes_file_string;
	CString control_file_string;
	CString io_procedures_file_string;
	CString DAPL_source_file_string;

	// Pointer to display window

	Cal_display* p_Cal_display;

	// Data record and pointer to it

	Data_record Cal_record;
	Data_record* p_Cal_record;

	void close_calibration_window(void);

	void Cal_display_Window_Done(void);

	int create_Calibration_source_file(void);						// returns 1 if file was created successfully
																	// 0 otherwise

	int write_header_file(void);									// create components of DAPL source file
	int write_create_pipes_file(void);								// all return 1 if successful
	int write_control_file(void);
	int write_io_procedures_file(void);

// Dialog Data
	//{{AFX_DATA(Calibration)
	enum { IDD = IDD_Calibration_Dialog };
	float	FL_Change;
	float	SL_Change;
	float	Calibration_sarcomere_length;
	float	Calibration_fibre_length;
	float	Calibration_step;
	float	Calibration_SL_volts_to_FL_microns_calibration;
	float	Calibration_area;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Calibration)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Calibration)
	virtual BOOL OnInitDialog();
	afx_msg void OnCalibrate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CString Calibration_file_info_string;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_Calibration_H__E88250E4_D963_4E50_AFEA_3FBFFAC72B4E__INCLUDED_)
