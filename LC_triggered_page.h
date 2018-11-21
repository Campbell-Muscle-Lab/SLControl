#pragma once

#include "LC_generic_page.h"
#include "afxwin.h"

// LC_triggered_page dialog

class LC_triggered_page : public LC_generic_page
{
	DECLARE_DYNAMIC(LC_triggered_page)

public:
	LC_triggered_page();
	LC_triggered_page(Length_control_sheet* set_p_lc_sheet);
	virtual ~LC_triggered_page();

	int lc_no_of_input_channels;
	int lc_analog_trigger_channel;
    
// Dialog Data
	enum { IDD = IDD_LC_TRIGGERED_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	
protected:
	virtual void PostNcDestroy();

public:
	CButton TRIGGERED_CAPTURE;
	BOOL triggered_capture;
	CComboBox NO_OF_INPUT_CHANNELS;
	CString lc_no_of_input_channels_string;
	CComboBox ANALOG_TRIGGER_CHANNEL;
	CString lc_analog_trigger_channel_string;
	CEdit ANALOG_THRESHOLD;
	float lc_analog_threshold;

	afx_msg void OnBnClickedTriggeredCapture();
	afx_msg void OnCbnSelchangeAnalogTriggerChannel();
	afx_msg void OnCbnSelchangeNoOfChannels();
	afx_msg void OnEnChangeAnalogThreshold();
	
	virtual BOOL OnInitDialog();
};
