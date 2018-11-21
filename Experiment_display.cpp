// Experiment_display.cpp : implementation file
//

#include "stdafx.h"
#include "slcontrol.h"
#include "Experiment_display.h"
#include "SLControlDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Experiment_display dialog

extern int FL_OR_SL_CHANNEL_MODE;

Experiment_display::Experiment_display(CWnd* pParent,
	Data_record* set_p_Experiment_display_record)
	: CDialog(Experiment_display::IDD, pParent)
{
	////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	m_pParent=pParent;
	m_nID=Experiment_display::IDD;

	// Pointer for data record

	p_Experiment_display_record=set_p_Experiment_display_record;

	// Window location and dimensions

	window_x_pos=(float)0.02;
	window_y_pos=(float)0.51;
	window_x_size=(float)0.63;
	window_y_size=(float)0.49;

	// Pointers for data plots

	if (FL_OR_SL_CHANNEL_MODE==SL_CHANNEL_MODE)
	{
		no_of_plots=6;

		p_FOR_plot    =& FOR_plot;
		p_SL_plot     =& SL_plot;
		p_FL_plot     =& FL_plot;

		p_FOR_SL_plot =& FOR_SL_plot;
		p_FOR_FL_plot =& FOR_FL_plot;
		p_SL_FL_plot  =& SL_FL_plot;

		pointers_array[0]=p_FOR_plot;
		pointers_array[1]=p_SL_plot;
		pointers_array[2]=p_FL_plot;
		pointers_array[3]=p_FOR_SL_plot;
		pointers_array[4]=p_FOR_FL_plot;
		pointers_array[5]=p_SL_FL_plot;
	}
	else
	{
		no_of_plots=2;

		p_FOR_plot    =& FOR_plot;
		p_FL_plot     =& FL_plot;

		pointers_array[0]=p_FOR_plot;
		pointers_array[1]=p_FL_plot;
	}

	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////

	//{{AFX_DATA_INIT(Experiment_display)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

Experiment_display::~Experiment_display(void)
{
	// Destructor
}

BOOL Experiment_display::Create(void)
{
	return CDialog::Create(m_nID, m_pParent);
}

void Experiment_display::PostNcDestroy() 
{
	delete this;
}

void Experiment_display::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Experiment_display)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Experiment_display, CDialog)
	//{{AFX_MSG_MAP(Experiment_display)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Experiment_display message handlers

void Experiment_display::close_experiment_display_window(void)
{
	// Code is called by parent to close the window

	DestroyWindow();
}

BOOL Experiment_display::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	// Variables

	int i;

	int width=1022;																// screen width and height
	int height=740;																// in pixels


	// Set window

	SetWindowPos(NULL,(int)(window_x_pos*width),(int)(window_y_pos*height),
		(int)(window_x_size*width),(int)(window_y_size*height),SWP_SHOWWINDOW);

	// Create display dc

	CClientDC dc(this);
	p_display_dc =& dc;

	// Calculate window dimensions and pass to plot

	CRect current_window;
	GetWindowRect(current_window);
	current_window.NormalizeRect();

	if (FL_OR_SL_CHANNEL_MODE==SL_CHANNEL_MODE)
	{
		p_FOR_plot->initialise_plot(p_Experiment_display_record,TIME,FORCE,AUTO_SCALE_ON,
			CURSOR_DISPLAY_ON,TEXT_DISPLAY_ON,
			current_window.Width(),current_window.Height(),(float)0.1,(float)0.24,(float)0.3,(float)0.22,
			"Time (s)","FOR (V)","Force v Time");
	
		p_SL_plot->initialise_plot(p_Experiment_display_record,TIME,SL,AUTO_SCALE_ON,
			CURSOR_DISPLAY_ON,TEXT_DISPLAY_ON,
			current_window.Width(),current_window.Height(),(float)0.1,(float)0.56,(float)0.3,(float)0.22,
			"Time (s)","SL (V)","SL v Time");

		p_FL_plot->initialise_plot(p_Experiment_display_record,TIME,FL,AUTO_SCALE_ON,
			CURSOR_DISPLAY_ON,TEXT_DISPLAY_ON,
			current_window.Width(),current_window.Height(),(float)0.1,(float)0.87,(float)0.3,(float)0.22,
			"Time (s)","FL (V)","FL v Time");

		p_FOR_SL_plot->initialise_plot(p_Experiment_display_record,SL,FORCE,AUTO_SCALE_ON,
			CURSOR_DISPLAY_ON,TEXT_DISPLAY_ON,
			current_window.Width(),current_window.Height(),(float)0.67,(float)0.24,(float)0.15,(float)0.22,
			"SL (V)","FOR (V)","Force v SL");

		p_FOR_FL_plot->initialise_plot(p_Experiment_display_record,FL,FORCE,AUTO_SCALE_ON,
			CURSOR_DISPLAY_ON,TEXT_DISPLAY_ON,
			current_window.Width(),current_window.Height(),(float)0.67,(float)0.56,(float)0.15,(float)0.22,
			"FL (V)","FOR (V)","Force v FL");

		p_SL_FL_plot->initialise_plot(p_Experiment_display_record,FL,SL,AUTO_SCALE_ON,
			CURSOR_DISPLAY_ON,TEXT_DISPLAY_ON,
			current_window.Width(),current_window.Height(),(float)0.67,(float)0.87,(float)0.15,(float)0.22,
			"FL (V)","SL (V)","SL v FL");
	}
	else
	{
		p_FOR_plot->initialise_plot(p_Experiment_display_record,TIME,FORCE,AUTO_SCALE_ON,
			CURSOR_DISPLAY_ON,TEXT_DISPLAY_ON,
			current_window.Width(),current_window.Height(),(float)0.15,(float)0.4,(float)0.6,(float)0.3,
			"Time (s)","FOR (V)","Force v Time");
	
		p_FL_plot->initialise_plot(p_Experiment_display_record,TIME,FL,AUTO_SCALE_ON,
			CURSOR_DISPLAY_ON,TEXT_DISPLAY_ON,
			current_window.Width(),current_window.Height(),(float)0.15,(float)0.87,(float)0.6,(float)0.3,
			"Time (s)","FL (V)","FL v Time");
	}

	// Draw default plots

	for (i=0;i<=(no_of_plots-1);i++)
	{
		pointers_array[i]->draw_plot(p_display_dc,1,p_Experiment_display_record->no_of_points);
	}

	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void Experiment_display::update_plots()
{
	// Code updates plots

	// Variables

	int i;

	// Create display dc

	CClientDC dc(this);
	p_display_dc =& dc;

	// Cycle through plots

	for (i=0;i<=(no_of_plots-1);i++)
	{
		pointers_array[i]->draw_plot(p_display_dc,1,p_Experiment_display_record->no_of_points);
	}
}

void Experiment_display::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	// Handles user input

	// Variables

	int i;

	int key_code=nChar;
	int control_key;

	// Code

	// Create display dc

	CClientDC dc(this);
	p_display_dc =& dc;

	// Reacts to user input

	// Extract Ctrl Key status

	if (GetKeyState(VK_CONTROL)<0)										// Ctrl Key pressed
	{
		control_key=1;														
	}
	else																// Ctrl Key not pressed
	{
		control_key=0;
	}

	// Clear window

	clear_window();

	// Cycle through plots processing input

	for (i=0;i<=(no_of_plots-1);i++)
	{
		pointers_array[i]->respond_to_key_press(p_display_dc,key_code,control_key);
	}

	//////////////////////////
	// Ken's code ends here //
	//////////////////////////

	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void Experiment_display::clear_window(void)
{
	// Code clears window by drawing over it in background colour

	// Create display dc

	CClientDC dc(this);
	CClientDC* p_dc =& dc;
	
	// Pen

	CPen* pOriginalPen;
	CPen BackgroundPen;
	BackgroundPen.CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNFACE));
	pOriginalPen = p_dc->SelectObject(&BackgroundPen);

	// Background brush

	CBrush* pOriginalBrush;
	CBrush BackgroundBrush;
	BackgroundBrush.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	pOriginalBrush = p_dc->SelectObject(&BackgroundBrush);

	// Overdraw screen

	// Calculate window dimensions and pass to plot

	CRect current_window;
	GetWindowRect(current_window);
	current_window.NormalizeRect();

	p_dc->Rectangle(1,1,current_window.Width(),current_window.Height());

	// Return brush and pen

	p_dc->SelectObject(pOriginalBrush);
	p_dc->SelectObject(pOriginalPen);
}

void Experiment_display::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CDialog::OnPaint() for painting messages

	// Code redraws screen

	// Variables

	int i;

	// Create display_dc

	CClientDC dc_window(this);
	CClientDC* p_display_dc =& dc_window;

	// Redraw screen

	for (i=0;i<=(no_of_plots-1);i++)
	{
		pointers_array[i]->draw_plot(p_display_dc,
			pointers_array[i]->first_drawn_point,
			pointers_array[i]->last_drawn_point);
	}
}

BOOL Experiment_display::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class

	// Traps Enter and Escape key presses to prevent dialog box closing

	if (pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN)
	{
		return TRUE;
	}

	if (pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE)
	{
		return TRUE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}
