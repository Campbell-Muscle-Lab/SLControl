#pragma once
#include "afxwin.h"

#include "global_definitions.h"


// Channel_tab dialog

class Triggers;			// forward declarations
class Channel_control;

class Channel_tab : public CDialog
{
	DECLARE_DYNAMIC(Channel_tab)

public:
	Channel_tab(CWnd* pParent = NULL, int set_channel_number=0);   // standard constructor
	virtual ~Channel_tab();

// Dialog Data
	enum { IDD = IDD_CHANNEL_TAB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	//int integer_test;
	//BOOL test_value;

	// Ken's variables

	int channel_number;

	Channel_tab* p_this_tab;

	short trigger_array[MAX_DATA_POINTS];

	int start_level;

	int free_form_points;
	int previous_free_form_points;

	// Ken's functions and pointers

	Triggers* p_triggers;
	Channel_control* p_channel_control;

	void update_trigger_array(void);
	void single_shot_calculation(void);
	void train_calculation(void);
	void free_form_calculation(void);

	void display_channel_number(void);

	afx_msg void OnBnClickedChannelActive();
	int RADIO_CONTROL;
	float channel_flip;
	afx_msg void OnEnChangeChannelSwitch();
	float channel_duration;
	afx_msg void OnEnChangeChannelDuration();
	BOOL channel_active;
	CComboBox START_LEVEL;
	CString start_level_string;
	afx_msg void OnCbnSelchangeStartLevel();
	float train_frequency;
	float train_duty_cycle;
	float train_gate_on;
	float train_gate_off;
	afx_msg void OnEnChangeGateOn();
	afx_msg void OnEnChangeGateOff();
	afx_msg void OnEnChangeFrequency();
	afx_msg void OnEnChangeDutyCycle();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedTrain();
	afx_msg void OnBnClickedOneShot();
	CString free_form_file_string;
	afx_msg void OnBnClickedFreeForm();
	afx_msg void OnBnClickedBrowse();
	afx_msg void OnPaint();
	afx_msg void OnEnChangeFreeFormFile();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
