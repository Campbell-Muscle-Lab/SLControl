#pragma once

#include "Abort_multiple_trials_dialog.h"
//#include "SL_SREC.h"
//#include "Length_Control_Dialog.h"

class Length_control_dialog;


// Abort_multiple_trials

class Abort_multiple_trials : public CWinThread
{
	DECLARE_DYNCREATE(Abort_multiple_trials)

public:
	Abort_multiple_trials(Length_control_dialog* set_p_lc_dialog);

/*
public:
	Fit_control(CWnd* pParent = NULL,
		Analysis_display* set_p_parent_analysis_display = NULL,
		Fit_parameters* set_p_analysis_fit_parameters = NULL,
		int set_no_of_plots=1, Plot* set_p_plot[] = NULL);
*/

	Abort_multiple_trials_dialog* p_amtd;

	void self_close();

	//SL_SREC* p_SL_SREC;

	Length_control_dialog* p_lc_dialog;



	

protected:
	Abort_multiple_trials();           // protected constructor used by dynamic creation
	virtual ~Abort_multiple_trials();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnIdle(LONG lCount);
};


