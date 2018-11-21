// LC_triggered_page.cpp : implementation file
//

#include "stdafx.h"
#include "SLControl.h"
#include "LC_triggered_page.h"
#include ".\lc_triggered_page.h"

#include "Length_Control_Dialog.h"


// LC_triggered_page dialog

IMPLEMENT_DYNAMIC(LC_triggered_page, LC_generic_page)

LC_triggered_page::LC_triggered_page()
	: LC_generic_page(LC_triggered_page::IDD)
	, lc_analog_threshold(0)
{
}

LC_triggered_page::LC_triggered_page(Length_control_sheet* set_p_lc_sheet)
	: LC_generic_page(LC_triggered_page::IDD, set_p_lc_sheet)
{
	// Initialise variables

	triggered_capture=0;

	lc_no_of_input_channels_string="4";
	lc_no_of_input_channels=4;

	lc_analog_trigger_channel_string="3";
	lc_analog_trigger_channel=3;

	lc_analog_threshold=3.0;
}

LC_triggered_page::~LC_triggered_page()
{
	
}


void LC_triggered_page::PostNcDestroy()
{
	delete this;
}


BOOL LC_triggered_page::OnInitDialog()
{
	LC_generic_page::OnInitDialog();

	// Disable appropriate controls

	GetDlgItem(IDC_ANALOG_TRIGGER_CHANNEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_ANALOG_THRESHOLD)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void LC_triggered_page::DoDataExchange(CDataExchange* pDX)
{
	LC_generic_page::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TRIGGERED_CAPTURE, TRIGGERED_CAPTURE);
	DDX_Check(pDX, IDC_TRIGGERED_CAPTURE, triggered_capture);

	DDX_Control(pDX, IDC_NO_OF_CHANNELS, NO_OF_INPUT_CHANNELS);
	DDX_Text(pDX, IDC_NO_OF_CHANNELS, lc_no_of_input_channels_string);

	DDX_Control(pDX, IDC_ANALOG_TRIGGER_CHANNEL, ANALOG_TRIGGER_CHANNEL);
	DDX_Text(pDX, IDC_ANALOG_TRIGGER_CHANNEL, lc_analog_trigger_channel_string);

	DDX_Control(pDX, IDC_ANALOG_THRESHOLD, ANALOG_THRESHOLD);
	DDX_Text(pDX, IDC_ANALOG_THRESHOLD,lc_analog_threshold);
}



BEGIN_MESSAGE_MAP(LC_triggered_page, LC_generic_page)
	ON_BN_CLICKED(IDC_TRIGGERED_CAPTURE, OnBnClickedTriggeredCapture)
	ON_CBN_SELCHANGE(IDC_ANALOG_TRIGGER_CHANNEL, OnCbnSelchangeAnalogTriggerChannel)
	ON_CBN_SELCHANGE(IDC_NO_OF_CHANNELS, OnCbnSelchangeNoOfChannels)
	ON_EN_CHANGE(IDC_ANALOG_THRESHOLD, OnEnChangeAnalogThreshold)
END_MESSAGE_MAP()


// LC_triggered_page message handlers

void LC_triggered_page::OnBnClickedTriggeredCapture()
{
	UpdateData(TRUE);

	if (triggered_capture)
	{
		p_lc_sheet->highlight_tab(tab_number,TRUE);
		GetDlgItem(IDC_ANALOG_TRIGGER_CHANNEL)->EnableWindow(TRUE);
		GetDlgItem(IDC_ANALOG_THRESHOLD)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_ANALOG_TRIGGER_CHANNEL)->EnableWindow(FALSE);
		GetDlgItem(IDC_ANALOG_THRESHOLD)->EnableWindow(FALSE);

		if (lc_no_of_input_channels==4)
			p_lc_sheet->highlight_tab(tab_number,FALSE);
	}

	p_lc_sheet->p_lc_dialog->update_preview_record();
}

void LC_triggered_page::OnCbnSelchangeAnalogTriggerChannel()
{
	// Variables

	char* p_string;
	int temp;
	CString value_string;

	// Code

//	UpdateData(FALSE);

	temp=ANALOG_TRIGGER_CHANNEL.GetCurSel();
	ANALOG_TRIGGER_CHANNEL.GetLBText(temp,value_string);
	lc_analog_trigger_channel=strtol(value_string,&p_string,10);
}

void LC_triggered_page::OnCbnSelchangeNoOfChannels()
{
	// Variables

	int i;

	char* p_string;
	int temp;
	CString value_string;

	char channel_string[5];
	CString temp_string;
	CString original_analog_trigger_channel_string;
	int original_analog_trigger_channel;

	char display_string[200];

	// Code

	UpdateData(TRUE);

	temp=NO_OF_INPUT_CHANNELS.GetCurSel();
	NO_OF_INPUT_CHANNELS.GetLBText(temp,value_string);
	lc_no_of_input_channels=strtol(value_string,&p_string,10);

	// Update parent dialog values

	p_lc_sheet->p_lc_dialog->lc_no_of_input_channels=lc_no_of_input_channels;

	// Highlight tab if appropriate

	if (lc_no_of_input_channels>4)
		p_lc_sheet->highlight_tab(tab_number,TRUE);
	else
		if (!triggered_capture)
			p_lc_sheet->highlight_tab(tab_number,FALSE);

	// First save original analog trigger channel, then repopulate list box
	// Update the trigger channel if it is no longer valid

    original_analog_trigger_channel=strtol(lc_analog_trigger_channel_string,&p_string,10);

	ANALOG_TRIGGER_CHANNEL.ResetContent();
	for (i=0;i<=(lc_no_of_input_channels-1);i++)
	{
		sprintf(channel_string,"%i",i);
		temp_string=channel_string;
		ANALOG_TRIGGER_CHANNEL.AddString(temp_string);
	}

	if (original_analog_trigger_channel<=lc_no_of_input_channels-1)
	{
		lc_analog_trigger_channel=original_analog_trigger_channel;
		lc_analog_trigger_channel_string=original_analog_trigger_channel_string;
	}
	else
	{
		lc_analog_trigger_channel=lc_no_of_input_channels-1;
		sprintf(channel_string,"%i",lc_no_of_input_channels-1);
		lc_analog_trigger_channel_string=channel_string;

		// Warn user
		MessageBeep(MB_ICONEXCLAMATION);
		sprintf(display_string,"Warning: Analog trigger channel has been changed");
		p_lc_sheet->p_lc_dialog->display_status(TRUE,display_string);
	}
	ANALOG_TRIGGER_CHANNEL.SetCurSel(lc_analog_trigger_channel);

	// Update parent dialog

	p_lc_sheet->p_lc_dialog->lc_no_of_input_channels=lc_no_of_input_channels;
}

void LC_triggered_page::OnEnChangeAnalogThreshold()
{
	UpdateData(TRUE);
}
