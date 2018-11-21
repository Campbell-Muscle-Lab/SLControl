// Cal_display.cpp : implementation file
//

#include "stdafx.h"
#include "slcontrol.h"
#include "Cal_display.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////
// Ken's code starts here //
////////////////////////////

#include "SLControlDlg.h"
#include "plot.h"

////////////////////////////
//  Ken's code ends here  //
////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Cal_display dialog


Cal_display::Cal_display(CWnd* pParent,Data_record* set_p_Cal_display_record)
	: CDialog(Cal_display::IDD, pParent)
{
	////////////////////////////
	// Ken's code starts here //
	////////////////////////////
	
	m_pParent=pParent;
	m_nID=Cal_display::IDD;

	p_Cal_display_record=set_p_Cal_display_record;

	no_of_plots=2;

	p_FL_plot =& FL_plot;
	p_SL_plot =& SL_plot;

	pointers_array[0]=p_FL_plot;
	pointers_array[1]=p_SL_plot;

	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////

	//{{AFX_DATA_INIT(Cal_display)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

Cal_display::~Cal_display()
{
	// Destructor
}

void Cal_display::PostNcDestroy() 
{
	delete this;
}

BOOL Cal_display::Create()
{
	return CDialog::Create(m_nID, m_pParent);
}

void Cal_display::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Cal_display)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Cal_display, CDialog)
	//{{AFX_MSG_MAP(Cal_display)
	ON_WM_KEYDOWN()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Cal_display message handlers

void Cal_display::close_Cal_display_window(void)
{
	// Signal to parent

	((Calibration*)m_pParent)->Cal_display_Window_Done();

	// Destroy window

	DestroyWindow();
}

BOOL Cal_display::OnInitDialog() 
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

	// Initialise window size and position

	SetWindowPos(NULL,(int)(0.68*width),(int)(0.51*height),
		(int)(0.21*width),(int)(0.49*height),SWP_SHOWWINDOW);

	// Create display dc

	CClientDC dc(this);
	p_display_dc =& dc;

	// Calculate window dimensions and pass to plot

	CRect current_window;
	GetWindowRect(current_window);
	current_window.NormalizeRect();

	p_FL_plot->initialise_plot(p_Cal_display_record,TIME,FL,AUTO_SCALE_ON,
		CURSOR_DISPLAY_OFF,TEXT_DISPLAY_OFF,
		current_window.Width(),current_window.Height(),(float)0.3,(float)0.4,(float)0.6,(float)0.3,
		"Time (s)","FL (V)","FL v Time");

	p_SL_plot->initialise_plot(p_Cal_display_record,TIME,SL,AUTO_SCALE_ON,
		CURSOR_DISPLAY_OFF,TEXT_DISPLAY_OFF,
		current_window.Width(),current_window.Height(),(float)0.3,(float)0.8,(float)0.6,(float)0.3,
		"Time (s)","SL (V)","SL v Time");

	// Default plots

	for (i=0;i<=(no_of_plots-1);i++)
	{
		pointers_array[i]->draw_plot(p_display_dc,1,p_Cal_display_record->no_of_points);
	}

	///////////////////////////
	//  Ken's code ends here //
	///////////////////////////

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void Cal_display::update_plots(void)
{
	// Code updates plots

	// Variables

	int i;

	// Code

	// Create dc and pointer to it

	CClientDC dc(this);
	CClientDC* p_display_dc =& dc;

	// Update FL and SL plots

	clear_window();

	for (i=0;i<=(no_of_plots-1);i++)
	{
		pointers_array[i]->first_drawn_point=1;
		pointers_array[i]->last_drawn_point=p_Cal_display_record->no_of_points;

		pointers_array[i]->cursor_point[pointers_array[i]->active_cursor]=
			((int)(0.33*p_Cal_display_record->no_of_points));
		pointers_array[i]->cursor_point[pointers_array[i]->inactive_cursor]=
			((int)(0.66*p_Cal_display_record->no_of_points));

		pointers_array[i]->draw_plot(p_display_dc,
			pointers_array[i]->first_drawn_point,
			pointers_array[i]->last_drawn_point);
	}
}

void Cal_display::clear_window(void)
{
	// Code clears window by drawing over it in background colour

	// Create dc and pointer to it

	CClientDC dc(this);
	CClientDC* p_display_dc =& dc;

	// Pen

	CPen* pOriginalPen;
	CPen BackgroundPen;
	BackgroundPen.CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNFACE));
	pOriginalPen = p_display_dc->SelectObject(&BackgroundPen);

	// Background brush

	CBrush* pOriginalBrush;
	CBrush BackgroundBrush;
	BackgroundBrush.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	pOriginalBrush = p_display_dc->SelectObject(&BackgroundBrush);

	// Overdraw screen

	// Calculate window dimensions and pass to plot

	CRect current_window;
	GetWindowRect(current_window);
	current_window.NormalizeRect();

	p_display_dc->Rectangle(1,1,current_window.Width(),current_window.Height());

	// Return brush and pen

	p_display_dc->SelectObject(pOriginalBrush);
	p_display_dc->SelectObject(pOriginalPen);
}

void Cal_display::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default

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
	
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void Cal_display::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CDialog::OnPaint() for painting messages

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

BOOL Cal_display::PreTranslateMessage(MSG* pMsg) 
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

