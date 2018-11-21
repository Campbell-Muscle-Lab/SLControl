// Channel_tab.cpp : implementation file
//

#include "stdafx.h"
#include "SLControl.h"
#include "Channel_tab.h"

#include "Triggers.h"
#include <math.h>


// Channel_tab dialog

IMPLEMENT_DYNAMIC(Channel_tab, CDialog)
Channel_tab::Channel_tab(CWnd* pParent /*=NULL*/, int set_channel_number)
	: CDialog(Channel_tab::IDD, pParent)
{
	// Variables inherent to constructor

	// Constructor values

	channel_number=set_channel_number;
	p_this_tab=this;

	// Default values

	RADIO_CONTROL=0;
	p_triggers=NULL;
	p_channel_control=NULL;

	// Single shot values

	start_level=0;
	channel_flip=(float)10.0;
    channel_duration=(float)5.0;
	start_level_string="Low";

	// Train values

	train_frequency=100.0;
	train_duty_cycle=0.5;
	train_gate_on=(float)0.1;
	train_gate_off=(float)0.2;

	// Free_form

	free_form_file_string="c:\\temp\\freeform1.txt";
	free_form_points=0;
	previous_free_form_points=0;

	// Initialise array

	for (int i=1;i<=MAX_DATA_POINTS;i++)
		trigger_array[i]=0;
}

Channel_tab::~Channel_tab()
{
}

void Channel_tab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_ONE_SHOT, RADIO_CONTROL);
	DDX_Text(pDX, IDC_CHANNEL_SWITCH, channel_flip);
	DDX_Text(pDX, IDC_CHANNEL_DURATION, channel_duration);
	DDX_Check(pDX, IDC_CHANNEL_ACTIVE, channel_active);
	DDX_Control(pDX, IDC_START_LEVEL, START_LEVEL);
	DDX_CBString(pDX, IDC_START_LEVEL, start_level_string);
	DDX_Text(pDX, IDC_FREQUENCY, train_frequency);
	DDX_Text(pDX, IDC_DUTY_CYCLE, train_duty_cycle);
	DDX_Text(pDX, IDC_GATE_ON, train_gate_on);
	DDX_Text(pDX, IDC_GATE_OFF, train_gate_off);
	DDX_Text(pDX, IDC_FREE_FORM_FILE, free_form_file_string);
}

BOOL Channel_tab::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Enable/disable appropriate controls

	// Train

	GetDlgItem(IDC_ONE_SHOT)->EnableWindow(FALSE);
	GetDlgItem(IDC_TRAIN)->EnableWindow(FALSE);
	GetDlgItem(IDC_FREE_FORM)->EnableWindow(FALSE);

	GetDlgItem(IDC_CHANNEL_SWITCH)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHANNEL_DURATION)->EnableWindow(FALSE);
	GetDlgItem(IDC_START_LEVEL)->EnableWindow(FALSE);

	GetDlgItem(IDC_DUTY_CYCLE)->EnableWindow(FALSE);
	GetDlgItem(IDC_FREQUENCY)->EnableWindow(FALSE);
	GetDlgItem(IDC_GATE_ON)->EnableWindow(FALSE);
	GetDlgItem(IDC_GATE_OFF)->EnableWindow(FALSE);

	GetDlgItem(IDC_FREE_FORM_FILE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BROWSE)->EnableWindow(FALSE);

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_MESSAGE_MAP(Channel_tab, CDialog)
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_CHANNEL_ACTIVE, OnBnClickedChannelActive)
	ON_EN_CHANGE(IDC_CHANNEL_SWITCH, OnEnChangeChannelSwitch)
	ON_EN_CHANGE(IDC_CHANNEL_DURATION, OnEnChangeChannelDuration)
	ON_CBN_SELCHANGE(IDC_START_LEVEL, OnCbnSelchangeStartLevel)
	ON_EN_CHANGE(IDC_GATE_ON, OnEnChangeGateOn)
	ON_EN_CHANGE(IDC_GATE_OFF, OnEnChangeGateOff)
	ON_EN_CHANGE(IDC_FREQUENCY, OnEnChangeFrequency)
	ON_EN_CHANGE(IDC_DUTY_CYCLE, OnEnChangeDutyCycle)
	ON_BN_CLICKED(IDC_TRAIN, OnBnClickedTrain)
	ON_BN_CLICKED(IDC_ONE_SHOT, OnBnClickedOneShot)
	ON_BN_CLICKED(IDC_FREE_FORM, OnBnClickedFreeForm)
	ON_BN_CLICKED(IDC_BROWSE, OnBnClickedBrowse)
	ON_WM_PAINT()
	ON_EN_CHANGE(IDC_FREE_FORM_FILE, OnEnChangeFreeFormFile)
END_MESSAGE_MAP()

void Channel_tab::OnPaint()
{
	CPaintDC dc(this);
	
	display_channel_number();
}

void Channel_tab::display_channel_number(void)
{
	// Display channel number in top right of tab dialog

	// Create display dc

	CClientDC dc(this);
	
	// Create font

	dc.SetBkMode(TRANSPARENT);
	CFont display_font;
	display_font.CreateFont(16,0,0,0,FW_BOLD /*400*/,FALSE,FALSE,0,ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_SWISS,"Arial");
	CFont* pOldFont = dc.SelectObject(&display_font);

	// Check window dimensions

	CRect current_window;
	GetWindowRect(current_window);
	current_window.NormalizeRect();

	char display_string[20];
	sprintf(display_string,"Channel No: %i",channel_number);
	dc.TextOut((int)(0.7*current_window.Width()),(int)(0.04*current_window.Height()),
		display_string);
}

// Channel_tab message handlers

BOOL Channel_tab::PreTranslateMessage(MSG* pMsg)
{
	// Traps escape key presses to prevent channel tab from being wiped clean

	if (pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE)
	{
		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void Channel_tab::OnBnClickedChannelActive()
{
	UpdateData(TRUE);

	if (channel_active==TRUE)
	{
		// Make channel active

		p_channel_control->HighlightItem(channel_number-1,TRUE);

		// Update array as appropriate

		GetDlgItem(IDC_ONE_SHOT)->EnableWindow(TRUE);
		GetDlgItem(IDC_TRAIN)->EnableWindow(TRUE);
		GetDlgItem(IDC_FREE_FORM)->EnableWindow(TRUE);

		switch (RADIO_CONTROL)
		{
			case 0:
			{
				OnBnClickedOneShot();
			}
			break;

			case 1:
			{
				OnBnClickedTrain();
			}
			break;

			case 2:
			{
				OnBnClickedFreeForm();
			}
			break;

			default:
			{
			}
			break;
		}
	}
	else
	{
		// Make channel inactive

		p_channel_control->HighlightItem(channel_number-1,FALSE);

		// Reset array to zeros

		for (int i=1;i<=p_triggers->data_points;i++)
			trigger_array[i]=0;

		// Disable all controls

		GetDlgItem(IDC_ONE_SHOT)->EnableWindow(FALSE);
		GetDlgItem(IDC_TRAIN)->EnableWindow(FALSE);
		GetDlgItem(IDC_FREE_FORM)->EnableWindow(FALSE);

		GetDlgItem(IDC_CHANNEL_SWITCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHANNEL_DURATION)->EnableWindow(FALSE);
		GetDlgItem(IDC_START_LEVEL)->EnableWindow(FALSE);
	
		GetDlgItem(IDC_DUTY_CYCLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_FREQUENCY)->EnableWindow(FALSE);
		GetDlgItem(IDC_GATE_ON)->EnableWindow(FALSE);
		GetDlgItem(IDC_GATE_OFF)->EnableWindow(FALSE);

		GetDlgItem(IDC_FREE_FORM_FILE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BROWSE)->EnableWindow(FALSE);

		// Check parameters valid

		p_triggers->set_parameters_valid();
	}

	UpdateData(FALSE);
}

void Channel_tab::OnCbnSelchangeStartLevel()
{
	// Code flips trigger_array

	if (START_LEVEL.GetCurSel()==0)
		start_level=0;
	else
		start_level=1;

	single_shot_calculation();
}

void Channel_tab::OnEnChangeChannelSwitch()
{
	UpdateData(TRUE);

	if ((p_triggers->set_parameters_valid())==0)
		single_shot_calculation();
}

void Channel_tab::OnEnChangeChannelDuration()
{
	UpdateData(TRUE);

	if ((p_triggers->set_parameters_valid())==0)
		single_shot_calculation();
}

void Channel_tab::OnEnChangeGateOn()
{
	UpdateData(TRUE);

	if ((p_triggers->set_parameters_valid())==0)
		train_calculation();
}

void Channel_tab::OnEnChangeGateOff()
{
	UpdateData(TRUE);

	if ((p_triggers->set_parameters_valid())==0)
		train_calculation();
}

void Channel_tab::OnEnChangeFrequency()
{
	UpdateData(TRUE);

	if ((p_triggers->set_parameters_valid())==0)
		train_calculation();
}

void Channel_tab::OnEnChangeDutyCycle()
{
	UpdateData(TRUE);

	if ((p_triggers->set_parameters_valid())==0)
		train_calculation();
}

void Channel_tab::OnBnClickedBrowse()
{
	// Allows user to update free_form file from dialog box

	// Initialises file open dialog

	CFileDialog fileDlg(TRUE,NULL,"*.txt",OFN_HIDEREADONLY,"Text files (*.txt)|*.txt||",this);

	// Initialisation

	fileDlg.m_ofn.lpstrTitle="Select free_form file";

	// Call Dialog

	if (fileDlg.DoModal()==IDOK)
	{
		free_form_file_string=fileDlg.GetPathName();
	}

	UpdateData(FALSE);

	// Set valid

	free_form_calculation();

	p_triggers->set_parameters_valid();
}

void Channel_tab::OnEnChangeFreeFormFile()
{
	UpdateData(TRUE);

	p_triggers->channel_status[channel_number]=0;
	
	free_form_calculation();

	p_triggers->set_parameters_valid();
}


// Event handlers for Radio buttons

void Channel_tab::OnBnClickedOneShot()
{
	// Code enables/disables appropriate controls and updates trigger_array appropriately

	UpdateData(TRUE);

	// Single shot

	GetDlgItem(IDC_CHANNEL_SWITCH)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHANNEL_DURATION)->EnableWindow(TRUE);
	GetDlgItem(IDC_START_LEVEL)->EnableWindow(TRUE);
	
	// Train

	GetDlgItem(IDC_DUTY_CYCLE)->EnableWindow(FALSE);
	GetDlgItem(IDC_FREQUENCY)->EnableWindow(FALSE);
	GetDlgItem(IDC_GATE_ON)->EnableWindow(FALSE);
	GetDlgItem(IDC_GATE_OFF)->EnableWindow(FALSE);

	// Free_form

	GetDlgItem(IDC_FREE_FORM_FILE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BROWSE)->EnableWindow(FALSE);

	// Update trigger array

	if ((p_triggers->set_parameters_valid())==0)
		single_shot_calculation();
}

void Channel_tab::OnBnClickedTrain()
{
	// Code enables/disables appropriate controls and updates trigger_array appropriately

	UpdateData(TRUE);

	// Single shot

	GetDlgItem(IDC_CHANNEL_SWITCH)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHANNEL_DURATION)->EnableWindow(FALSE);
	GetDlgItem(IDC_START_LEVEL)->EnableWindow(FALSE);
	
	// Train

	GetDlgItem(IDC_DUTY_CYCLE)->EnableWindow(TRUE);
	GetDlgItem(IDC_FREQUENCY)->EnableWindow(TRUE);
	GetDlgItem(IDC_GATE_ON)->EnableWindow(TRUE);
	GetDlgItem(IDC_GATE_OFF)->EnableWindow(TRUE);

	// Free_form

	GetDlgItem(IDC_FREE_FORM_FILE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BROWSE)->EnableWindow(FALSE);

	// Update trigger array

	if ((p_triggers->set_parameters_valid())==0)
		train_calculation();
}

void Channel_tab::OnBnClickedFreeForm()
{
	// Code enables/disables appropriate controls and updates trigger_array appropriately

	UpdateData(TRUE);

	// Single shot

	GetDlgItem(IDC_CHANNEL_SWITCH)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHANNEL_DURATION)->EnableWindow(FALSE);
	GetDlgItem(IDC_START_LEVEL)->EnableWindow(FALSE);
	
	// Train

	GetDlgItem(IDC_DUTY_CYCLE)->EnableWindow(FALSE);
	GetDlgItem(IDC_FREQUENCY)->EnableWindow(FALSE);
	GetDlgItem(IDC_GATE_ON)->EnableWindow(FALSE);
	GetDlgItem(IDC_GATE_OFF)->EnableWindow(FALSE);

	// Free-form

	GetDlgItem(IDC_FREE_FORM_FILE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BROWSE)->EnableWindow(TRUE);

	// Update trigger_array

	free_form_calculation();

	p_triggers->set_parameters_valid();

}

// Internal processing functions

void Channel_tab::update_trigger_array(void)
{
	// Code updates trigger_array appropriately

	if (channel_active==TRUE)
	{
		switch (RADIO_CONTROL)
		{
			case 0:									// Single shot
			{
				if (p_triggers->set_parameters_valid()==0)
					single_shot_calculation();
			}
			break;

			case 1:									// Train calculations
			{
				if (p_triggers->set_parameters_valid()==0)
                    train_calculation();
			}
			break;

			case 2:									// Free form calculations
			{
				free_form_calculation();

				p_triggers->set_parameters_valid();
			}
			break;

			default:
			{
			}
			break;
		}
	}
	else
		p_triggers->set_parameters_valid();
}

void Channel_tab::single_shot_calculation(void)
{
	// Code fills trigger_array for single_shot

	// Variables

	int i;

	int flip_point;
	int flop_point;

	// Code

	if (fmod(((channel_flip/1000.0)/p_triggers->time_increment),1)>0.5)
		flip_point=(int)ceil(((channel_flip/1000.0)/p_triggers->time_increment))+1;
	else
		flip_point=(int)floor(((channel_flip/1000.0)/p_triggers->time_increment))+1;

	if (fmod(((channel_duration/1000.0)/p_triggers->time_increment),1)>0.5)
		flop_point=flip_point+(int)ceil(((channel_duration/1000.0)/p_triggers->time_increment));
	else
		flop_point=flip_point+(int)floor(((channel_duration/1000.0)/p_triggers->time_increment));

	// Fill array for low-start

	for (i=1;i<=p_triggers->data_points;i++)
	{
		if ((i>=flip_point)&&(i<flop_point))
			trigger_array[i]=1;
		else
			trigger_array[i]=0;
	}

	// Flip it if necessary

	if (start_level==1)
	{
		for (i=1;i<=p_triggers->data_points;i++)
		{
			if (trigger_array[i]==0)
				trigger_array[i]=1;
			else
				trigger_array[i]=0;
		}
	}
}

void Channel_tab::train_calculation(void)
{
	// Code fills trigger array for the train condition

	// Variables

	int i;

	int gate_on;
	int gate_off;

	float time_point;
	float cycle_length=(float)1.0/train_frequency;
	int cycle;

	// Code

	if (fmod((train_gate_on/p_triggers->time_increment),1)>0.5)
		gate_on=(int)ceil((train_gate_on/p_triggers->time_increment))+1;
	else
		gate_on=(int)floor((train_gate_on/p_triggers->time_increment))+1;

	if (fmod((train_gate_off/p_triggers->time_increment),1)>0.5)
		gate_off=(int)ceil((train_gate_off/p_triggers->time_increment));
	else
		gate_off=(int)floor((train_gate_off/p_triggers->time_increment));


	// Set up train

	i=1;
	
	while (i<=p_triggers->data_points)
	{
		time_point=(float)(i-1)*p_triggers->time_increment;

		cycle=(int)(time_point/cycle_length);

		if ((time_point-((float)cycle*cycle_length))<(train_duty_cycle*cycle_length))
		{
			trigger_array[i]=1;
		}
		else
			trigger_array[i]=0;

		i++;
	}

	// Now correct for gate

	for (i=1;i<gate_on;i++)
	{
		trigger_array[i]=0;
	}
	
	for (i=gate_off;i<=p_triggers->data_points;i++)
	{
		trigger_array[i]=0;
	}
}

void Channel_tab::free_form_calculation(void)
{
	// Code fills trigger_array from the appropriate file
	// and sets free_form_points - a value of -1 indicates an error

	// Variables

	int counter;
	char single_character;

	BOOL status=TRUE;

	FILE* free_form_file;

	char display_string[200];

	// Code

	// Open and check file

	if ((free_form_file=fopen(free_form_file_string,"r"))==NULL)
	{
		sprintf(display_string,"File %s could not be opened",free_form_file_string);
		p_triggers->display_status(TRUE,display_string);

		free_form_points=-1;

		return;
	}

	// Run through file, only counting 1s and 0s

	counter=1;

	while ((!feof(free_form_file))&&(counter<=MAX_DATA_POINTS))
	{
		fscanf(free_form_file,"%c",&single_character);

		if (feof(free_form_file))				// prevents re-reading last point
		{
			single_character='a';
		}

		if ((single_character=='1')||(single_character=='0')||(single_character=='\n'))
		{
			if (single_character=='1')
			{
				trigger_array[counter]=1;
				counter++;
			}

			if (single_character=='0')
			{
				trigger_array[counter]=0;
				counter++;
			}
		}
	}

	previous_free_form_points=free_form_points;
	free_form_points=counter-1;

	// Tidy up

	fclose(free_form_file);
}
		
