#pragma once

// Stimulus_dialog dialog

#include "Experiment_display.h"
#include "Calibration.h"

// Stimulus_dialog dialog

class Stimulus_dialog : public CDialog
{
	DECLARE_DYNAMIC(Stimulus_dialog)

public:
	Stimulus_dialog(CWnd* pParent = NULL);   // standard constructor
	//virtual ~Stimulus_dialog();

	Stimulus_dialog::~Stimulus_dialog();
	BOOL Stimulus_dialog::Create();

	CWnd* m_pParent;									// Pointer to parent window
	int m_nID;

	// Pointers to associated windows

	Calibration* p_Calibration;
	Experiment_display* p_Experiment_display;

	// Data record and a pointer to it

	Data_record Stimulus_record;
	Data_record* p_Stimulus_record;

	// Strings

	CString DAPL_source_file_string;

	// Status strings and counter for user display

	CString status_top_string;
	CString status_middle_string;
	CString status_bottom_string;
	int status_counter;

	// Functions

	void remote_shut_down(void);						// called by parent to close window

	void display_status(int update_mode,CString new_string);

// Dialog Data
	enum { IDD = IDD_STIMULUS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
protected:
	virtual void PostNcDestroy();
public:
//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedRun();
};
