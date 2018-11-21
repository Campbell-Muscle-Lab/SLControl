// Analysis_display.cpp : implementation file
//

#include "stdafx.h"
#include "slcontrol.h"
#include "Analysis_display.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////
// Ken's code starts here //
////////////////////////////

#include "SLControlDlg.h"
#include "Fit_parameters.h"

#include "global_functions.h"

extern int FL_OR_SL_CHANNEL_MODE;

////////////////////////////
//  Ken's code ends here  //
////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Analysis_display dialog


Analysis_display::Analysis_display(CWnd* pParent,Data_record* set_p_data_record)
	: CDialog(Analysis_display::IDD, pParent)
{
	m_pParent = pParent;										// set pointer to parent window
	m_nID = Analysis_display::IDD;

	p_data_record = set_p_data_record;							// set pointer to associated data record

	p_window_fit_parameters = new Fit_parameters;
	window_fit_colour=0;
	fit_active=FALSE;

	// Window size and dimensions

	window_x_pos=(float)0.01;
	window_y_pos=(float)0.12;
	window_x_size=(float)0.85;
	window_y_size=(float)0.7;

	// Plots and associated pointers

	if (FL_OR_SL_CHANNEL_MODE==SL_CHANNEL_MODE)
	{
		no_of_plots=5;
	
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

		// Text anchor

		x_text_anchor=(float)0.55;										// text anchors relative to window size
		y_text_anchor=(float)0.65;												
	}
	else
	{
		no_of_plots=2;

		p_FOR_plot    =& FOR_plot;
		p_FL_plot     =& FL_plot;

		pointers_array[0]=p_FOR_plot;
		pointers_array[1]=p_FL_plot;

		// Text anchor

		x_text_anchor=(float)0.1;										// text anchors relative to window size
		y_text_anchor=(float)0.65;												

	}

	//{{AFX_DATA_INIT(Analysis_display)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

Analysis_display::~Analysis_display(void)
{
	// Destructor

	delete p_window_fit_parameters;

	delete p_fit_control;
}

void Analysis_display::Fit_control_Window_Done(void)
{
	p_fit_control = NULL;
}

void Analysis_display::PostNcDestroy() 
{
	delete this;
}

BOOL Analysis_display::Create()
{
	return CDialog::Create(m_nID, m_pParent);
}

void Analysis_display::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Analysis_display)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Analysis_display, CDialog)
	//{{AFX_MSG_MAP(Analysis_display)
	ON_WM_CLOSE()
	ON_WM_KEYDOWN()
	ON_WM_PAINT()
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_COMMAND(ID_TRANSFORM_CHANGETOCALIBRATEDVALUES, OnTransformChangetocalibratedvalues)
	ON_COMMAND(ID_TRANSFORM_CHANGETORAWVOLTAGES, OnTransformChangetorawvoltages)
	ON_COMMAND(ID_FILE_DUMPTRACEVALUESTOFILE, OnFileDumptracevaluestofile)
	ON_COMMAND(ID_OFFSET_RESCALEFORCETOACTIVECURSOR, OnOffsetRescaleforcetoactivecursor)
	ON_COMMAND(ID_PEAK_POSITIONS, OnPeakPositions)
	ON_COMMAND(ID_PEAK_RESETFILE, OnPeakResetfile)
	ON_COMMAND(ID_PEAK_ADDPOINT, OnPeakAddpoint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Analysis_display message handlers

BOOL Analysis_display::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Variables

	int i;
	CClientDC* p_display_dc;

	// Initialise window size and position

	int width,height;

	// Set window size and position
	
	CRect work_area;
	CRect* p_work_area =& work_area;

	SystemParametersInfo(SPI_GETWORKAREA,0,p_work_area,0);

	SetWindowPos(NULL,NULL,0,work_area.Width(),work_area.Height(),SWP_SHOWWINDOW);

	width=work_area.Width();													// screen width and height in pixels
	height=work_area.Height();														

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
		p_FOR_plot->initialise_plot(p_data_record,TIME,FORCE,AUTO_SCALE_ON,
			CURSOR_DISPLAY_ON,TEXT_DISPLAY_ON,
			current_window.Width(),current_window.Height(),(float)0.07,(float)0.25,(float)0.31,(float)0.2,
			"Time (s)","For (V)","For v Time");
	
		p_SL_plot->initialise_plot(p_data_record,TIME,SL,AUTO_SCALE_ON,
			CURSOR_DISPLAY_ON,TEXT_DISPLAY_ON,
			current_window.Width(),current_window.Height(),(float)0.07,(float)0.55,(float)0.31,(float)0.2,
			"Time (s)","SL (V)","SL v Time");

		p_FL_plot->initialise_plot(p_data_record,TIME,FL,AUTO_SCALE_ON,
			CURSOR_DISPLAY_ON,TEXT_DISPLAY_ON,
			current_window.Width(),current_window.Height(),(float)0.07,(float)0.85,(float)0.31,(float)0.2,
			"Time (s)","FL (V)","FL v Time");

		p_FOR_SL_plot->initialise_plot(p_data_record,SL,FORCE,AUTO_SCALE_ON,
			CURSOR_DISPLAY_ON,TEXT_DISPLAY_ON,
			current_window.Width(),current_window.Height(),(float)0.6,(float)0.25,(float)0.20,(float)0.2,
			"SL (V)","For (V)","For v SL");

		p_FOR_FL_plot->initialise_plot(p_data_record,FL,FORCE,AUTO_SCALE_ON,
			CURSOR_DISPLAY_ON,TEXT_DISPLAY_ON,
			current_window.Width(),current_window.Height(),(float)0.6,(float)0.55,(float)0.20,(float)0.2,
			"FL (V)","For (V)","For v FL");
	}
	else
	{
		p_FOR_plot->initialise_plot(p_data_record,TIME,FORCE,AUTO_SCALE_ON,
			CURSOR_DISPLAY_ON,TEXT_DISPLAY_ON,
			current_window.Width(),current_window.Height(),(float)0.1,(float)0.25,(float)0.6,(float)0.2,
			"Time (s)","For (V)","For v Time");
	
		p_FL_plot->initialise_plot(p_data_record,TIME,FL,AUTO_SCALE_ON,
			CURSOR_DISPLAY_ON,TEXT_DISPLAY_ON,
			current_window.Width(),current_window.Height(),(float)0.1,(float)0.55,(float)0.6,(float)0.2,
			"Time (s)","FL (V)","FL v Time");
	}

	// Draw default plots

	for (i=0;i<=(no_of_plots-1);i++)
	{
		pointers_array[i]->cursor_point[pointers_array[i]->active_cursor]=1;

		pointers_array[i]->cursor_point[pointers_array[i]->inactive_cursor]=
			(int)((p_data_record->record_ktr_initiation_time_ms+
				p_data_record->record_ktr_duration_ms*0.5)
				*p_data_record->record_sampling_rate/1000.0);
		
		pointers_array[i]->draw_plot(p_display_dc,1,p_data_record->no_of_points);
	}

	// Disable Change to Raw Voltages Menu Option

	CMenu* p_menu = GetMenu();
	p_menu->EnableMenuItem(ID_TRANSFORM_CHANGETORAWVOLTAGES,MF_GRAYED);

	// Create Fit_control window

	p_fit_control = new Fit_control(this,this,p_window_fit_parameters,no_of_plots,pointers_array);
	p_fit_control->Create();
	p_fit_control->SetActiveWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void Analysis_display::OnClose() 
{
	// Close derived windows

	p_fit_control->close_fit_control_window();

	// Signal to parent

	((CSLControlDlg*)m_pParent)->Analysis_display_Window_Done();

	// Destroy window

	DestroyWindow();
}

void Analysis_display::remote_shut_down(void)
{
	// Called by parent to shut down dialog
	// Parent cannot access OnClose etc.

	OnClose();
}

void Analysis_display::remote_set_cursors(int active_cursor_position, int inactive_cursor_position)
{
	for (int i=0;i<=(no_of_plots-1);i++)
	{
		pointers_array[i]->cursor_point[pointers_array[i]->active_cursor]=active_cursor_position;
		pointers_array[i]->cursor_point[pointers_array[i]->inactive_cursor]=inactive_cursor_position;
	}

	update_plots();
}

void Analysis_display::remote_expand_trace(void)
{
	// Expands trace by remote

	OnKeyDown(69,1,18);
}

void Analysis_display::update_plots(void)
{
	// Variables

	CClientDC* p_display_dc;

	// Create display dc

	CClientDC dc(this);
	p_display_dc =& dc;

	// Clear window

	clear_window();

	// Update plots

	for (int i=0;i<=(no_of_plots-1);i++)
	{
		pointers_array[i]->draw_plot(p_display_dc,
			pointers_array[i]->first_drawn_point,
			pointers_array[i]->last_drawn_point);
	}

	// Check for active fit

	if (fit_active==TRUE)
		draw_fit(p_window_fit_parameters,window_fit_colour);

	// Record parameters

	display_record_parameters();
}

void Analysis_display::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// Handles user input

	// Variables

	int i;
	int key_code=nChar;
	int control_key;

	CClientDC* p_display_dc;

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

	char display_string[200];
	sprintf(display_string,"key code: %i",key_code);
	//MessageBox(display_string);

	if (key_code==77)
	{
		OnPeakAddpoint();
	}

	// Draw parameters

	display_record_parameters();


	// Check for active fit

	if (fit_active==TRUE)
		draw_fit(p_window_fit_parameters,window_fit_colour);

	//////////////////////////
	// Ken's code ends here //
	//////////////////////////

	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void Analysis_display::clear_window(void)
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

void Analysis_display::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// Code redraws screen

	update_plots();
}

void Analysis_display::draw_fit(Fit_parameters* p_fit_parameters, int colour)
{
	// Draws fit to selected region

	CClientDC dc_window(this);
	CClientDC* p_display_dc =& dc_window;

	// Redraw original plot and overdraw fit

	(p_fit_parameters->p_plot)->draw_plot(p_display_dc,
		(p_fit_parameters->p_plot->first_drawn_point),
		(p_fit_parameters->p_plot->last_drawn_point));
	(p_fit_parameters->p_plot)->draw_fit(p_display_dc,p_fit_parameters,colour);

	// Updates status indicators

	p_window_fit_parameters=p_fit_parameters;
	window_fit_colour=colour;
	fit_active=TRUE;
}

void Analysis_display::display_record_parameters(void)
{
	// Function draws record parameters to screen

	// Variables

	int x_pos;
	int y_pos;

	char display_string[200];

	// Screen DC

	CClientDC dc_window(this);
	CClientDC* p_display_dc =& dc_window;

	// Set text positions

	CRect current_window;
	GetWindowRect(current_window);
	current_window.NormalizeRect();

	x_pos=(int)(x_text_anchor*current_window.Width());
	y_pos=(int)(y_text_anchor*current_window.Height());

	// Create font

	p_display_dc->SetBkMode(TRANSPARENT);
	CFont display_font;
	display_font.CreateFont(12,6,0,0,400,FALSE,FALSE,0,ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_SWISS,"Arial");
	CFont* pOldFont = p_display_dc->SelectObject(&display_font);

	p_display_dc->SetTextColor(RGB(128,0,128));

	// Labels

	// Filename

	sprintf(display_string,"%s",p_data_record->file_name_string);
	p_display_dc->TextOut(x_pos,y_pos,display_string);
	y_pos=(y_pos+15);

	// Record Time

	sprintf(display_string,"Record date and time");
	p_display_dc->TextOut(x_pos,y_pos,display_string);
	y_pos=(y_pos+10);

	sprintf(display_string,"DD/MM/YY: %02i/%02i/%i",
		p_data_record->day,
		p_data_record->month,
		p_data_record->year);
	p_display_dc->TextOut(x_pos,y_pos,display_string);
	y_pos=(y_pos+10);

	sprintf(display_string,"Time: %02i:%02i:%02i",
		p_data_record->hour,
		p_data_record->minute,
		p_data_record->second);
	p_display_dc->TextOut(x_pos,y_pos,display_string);
	y_pos=(y_pos+15);

	// Muscle Dimensions

	sprintf(display_string,"Muscle Dimensions");
	p_display_dc->TextOut(x_pos,y_pos,display_string);
	y_pos=(y_pos+10);

	sprintf(display_string,"FL (um): %5.2f",p_data_record->record_fl);
	p_display_dc->TextOut(x_pos,y_pos,display_string);
	y_pos=(y_pos+10);

	sprintf(display_string,"SL (nm): %5.2f",p_data_record->record_sl);
	p_display_dc->TextOut(x_pos,y_pos,display_string);
	y_pos=(y_pos+10);

	sprintf(display_string,"Area (m^2): %.3e",p_data_record->record_area);
	p_display_dc->TextOut(x_pos,y_pos,display_string);
	y_pos=(y_pos+15);

	// Calibrations

	x_pos=(int)((x_text_anchor+0.15)*current_window.Width());
	y_pos=(int)(y_text_anchor*current_window.Height())+15;

	sprintf(display_string,"Calibrations");
	p_display_dc->TextOut(x_pos,y_pos,display_string);
	y_pos=(y_pos+10);

	sprintf(display_string,"FORCE_CALIBRATION (N/V): %6.4g",p_data_record->record_force_calibration);
	p_display_dc->TextOut(x_pos,y_pos,display_string);
	y_pos=(y_pos+10);

	sprintf(display_string,"FL_COMMAND_CALIBRATION (um/V): %6.4g",p_data_record->record_fl_command_calibration);
	p_display_dc->TextOut(x_pos,y_pos,display_string);
	y_pos=(y_pos+10);

	sprintf(display_string,"FL_RESPONSE_CALIBRATION (um/V): %6.4g",p_data_record->record_fl_response_calibration);
	p_display_dc->TextOut(x_pos,y_pos,display_string);
	y_pos=(y_pos+10);

	sprintf(display_string,"FL_POLARITY: %.1f",p_data_record->record_fl_polarity);
	p_display_dc->TextOut(x_pos,y_pos,display_string);
	y_pos=(y_pos+10);

	sprintf(display_string,"SL_V_TO_FL_V_CALIBRATION: %6.4g",
		p_data_record->record_sl_volts_to_fl_command_volts_calibration);
	p_display_dc->TextOut(x_pos,y_pos,display_string);
	
	// Return font

	p_display_dc->SelectObject(pOldFont);
}

void Analysis_display::OnFilePrint() 
{
	// Prints screen to a metafile and copies the metafile to the Clipboard
	// Code calls uses print_functions defined in Plot class and
	// more print_functions defined in this Analysis_display class

	// Variables

	char cursor_path_string[_MAX_PATH];				// cursor path

	// Rectangle used to define size of metafile (arbitrary values found by trial and error)

	CRect print_rectangle(1,1,27000,15000);
	
	CMetaFileDC * m_pMetaDC = new CMetaFileDC();
	m_pMetaDC->CreateEnhanced(GetDC(),NULL,print_rectangle,"SLControl");

	// Switch cursor to animated cursor
	
	::set_executable_directory_string(cursor_path_string);
	strcat(cursor_path_string,"\\print.ani");

	HCURSOR hcurWait = (HCURSOR)LoadImage(AfxGetApp()->m_hInstance,
		cursor_path_string,IMAGE_CURSOR,0,0,
		LR_DEFAULTSIZE|LR_DEFAULTCOLOR|LR_LOADFROMFILE);
	const HCURSOR hcurSave=SetCursor(hcurWait);

	SetCursor(hcurWait);
	
	//draw meta file

	// Traces

	for (int i=0;i<=(no_of_plots-1);i++)
	{
		pointers_array[i]->print_plot(m_pMetaDC,
			pointers_array[i]->first_drawn_point,
			pointers_array[i]->last_drawn_point);
	}

	// Check for active fit and draw if necessary
	
	if (fit_active==TRUE)
		(p_window_fit_parameters->p_plot)->print_fit(m_pMetaDC,p_window_fit_parameters,window_fit_colour);

	// Parameters

	print_record_parameters(m_pMetaDC);

	//close meta file dc and prepare for clipboard;
	HENHMETAFILE hMF = m_pMetaDC->CloseEnhanced();

	//copy to clipboard
	OpenClipboard();
	EmptyClipboard();
	::SetClipboardData(CF_ENHMETAFILE,hMF);
	CloseClipboard();
	//DeleteMetaFile(hMF);
	delete m_pMetaDC;

	// End Cursor wait

	SetCursor(hcurSave);
}

void Analysis_display::print_record_parameters(CMetaFileDC *p_display_dc)
{
	// Function prints record parameters to a metafile

	// Variables

	int x_pos;
	int y_pos;

	char display_string[200];

	// Set text positions

	CRect current_window;
	GetWindowRect(current_window);
	current_window.NormalizeRect();

	x_pos=(int)(x_text_anchor*current_window.Width());
	y_pos=(int)(y_text_anchor*current_window.Height());

	// Create font

	p_display_dc->SetBkMode(TRANSPARENT);
	CFont display_font;
	display_font.CreateFont(12,6,0,0,400,FALSE,FALSE,0,ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_SWISS,"Arial");
	CFont* pOldFont = p_display_dc->SelectObject(&display_font);

	p_display_dc->SetTextColor(RGB(128,0,128));

	// Labels

	// Filename

	sprintf(display_string,"%s",p_data_record->file_name_string);
	p_display_dc->TextOut(x_pos,y_pos,display_string);
	y_pos=(y_pos+15);

	// Record Time

	sprintf(display_string,"Record date and time");
	p_display_dc->TextOut(x_pos,y_pos,display_string);
	y_pos=(y_pos+11);

	sprintf(display_string,"DD/MM/YY: %02i/%02i/%i",
		p_data_record->day,
		p_data_record->month,
		p_data_record->year);
	p_display_dc->TextOut(x_pos,y_pos,display_string);
	y_pos=(y_pos+11);

	sprintf(display_string,"Time: %02i:%02i:%02i",
		p_data_record->hour,
		p_data_record->minute,
		p_data_record->second);
	p_display_dc->TextOut(x_pos,y_pos,display_string);
	y_pos=(y_pos+15);

	// Muscle Dimensions

	sprintf(display_string,"Muscle Dimensions");
	p_display_dc->TextOut(x_pos,y_pos,display_string);
	y_pos=(y_pos+11);

	sprintf(display_string,"FL (um): %5.2f",p_data_record->record_fl);
	p_display_dc->TextOut(x_pos,y_pos,display_string);
	y_pos=(y_pos+11);

	sprintf(display_string,"SL (nm): %5.2f",p_data_record->record_sl);
	p_display_dc->TextOut(x_pos,y_pos,display_string);
	y_pos=(y_pos+11);

	sprintf(display_string,"Area (m^2): %.3e",p_data_record->record_area);
	p_display_dc->TextOut(x_pos,y_pos,display_string);
	y_pos=(y_pos+15);

	// Calibrations

	x_pos=(int)((x_text_anchor+0.17)*current_window.Width());
	y_pos=(int)(y_text_anchor*current_window.Height())+15;;

	sprintf(display_string,"Calibrations");
	p_display_dc->TextOut(x_pos,y_pos,display_string);
	y_pos=(y_pos+11);

	sprintf(display_string,"FORCE_CALIBRATION (N/V): %6.4g",p_data_record->record_force_calibration);
	p_display_dc->TextOut(x_pos,y_pos,display_string);
	y_pos=(y_pos+11);

	sprintf(display_string,"FL_COMMAND_CALIBRATION (um/V): %6.4g",p_data_record->record_fl_command_calibration);
	p_display_dc->TextOut(x_pos,y_pos,display_string);
	y_pos=(y_pos+11);

	sprintf(display_string,"FL_RESPONSE_CALIBRATION (um/V): %6.4g",p_data_record->record_fl_response_calibration);
	p_display_dc->TextOut(x_pos,y_pos,display_string);
	y_pos=(y_pos+11);

	sprintf(display_string,"SL_V_TO_FL_V_CALIBRATION: %6.4g",
		p_data_record->record_sl_volts_to_fl_command_volts_calibration);
	p_display_dc->TextOut(x_pos,y_pos,display_string);
	
	// Return font

	p_display_dc->SelectObject(pOldFont);
}

void Analysis_display::OnFileExit() 
{
	OnClose();
}

BOOL Analysis_display::PreTranslateMessage(MSG* pMsg) 
{
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

void Analysis_display::OnTransformChangetocalibratedvalues() 
{
	// Code transforms to SI units - fibre length (m), sarcomere length (m), force (N/m^2)

	p_data_record->transform_to_calibrated_values();

	// Update menu options

	CMenu* p_menu = GetMenu();
	p_menu->EnableMenuItem(ID_TRANSFORM_CHANGETOCALIBRATEDVALUES,MF_GRAYED);
	p_menu->EnableMenuItem(ID_TRANSFORM_CHANGETORAWVOLTAGES,MF_ENABLED);

	// Update plot titles

	p_FOR_plot->y_axis_title="For (N m^-2)";
	p_SL_plot->y_axis_title="SL (m)";
	p_FL_plot->y_axis_title="FL (m)";
	
	p_FOR_SL_plot->y_axis_title="For (N m^-2)";
	p_FOR_SL_plot->x_axis_title="SL (m)";

	p_FOR_FL_plot->y_axis_title="For (N m^-2)";
	p_FOR_FL_plot->x_axis_title="FL (m)";

	// Update screen

	update_plots();
}

void Analysis_display::OnTransformChangetorawvoltages() 
{
	// Re-reads data record from file

	p_data_record->read_data_from_file(p_data_record->file_name_string);

	// Update menu options

	CMenu* p_menu = GetMenu();
	p_menu->EnableMenuItem(ID_TRANSFORM_CHANGETOCALIBRATEDVALUES,MF_ENABLED);
	p_menu->EnableMenuItem(ID_TRANSFORM_CHANGETORAWVOLTAGES,MF_GRAYED);

	// Update plot titles

	p_FOR_plot->y_axis_title="For (V)";
	p_SL_plot->y_axis_title="SL (V)";
	p_FL_plot->y_axis_title="FL (V)";
	
	p_FOR_SL_plot->y_axis_title="For (V)";
	p_FOR_FL_plot->y_axis_title="For (V)";

	// Update screen

	update_plots();
}

void Analysis_display::OnFileDumptracevaluestofile() 
{
	// Code saves trace values to user-selected file

	// Variables

	int i;
	char display_string[200];

	CString file_string;

	// Create a SaveAs Dialog

	CFileDialog fileDlg(FALSE,NULL,"*.txt",OFN_OVERWRITEPROMPT,"Text files (*.txt)|*.txt||",this);
	fileDlg.m_ofn.lpstrTitle="Select file for trace values";

	// Display dialog

	if (fileDlg.DoModal()==IDOK)
	{
		file_string=fileDlg.GetPathName();

		FILE* trace_file=fopen(file_string,"w");

		if (trace_file==NULL)
		{
			// File handling error

			sprintf(display_string,"File\n%s\ncould not be opened\nFile output failed",file_string);
			MessageBox(display_string,"Analysis_display::OnFileDumptracevaluestofile",MB_ICONWARNING);
		}
		else
		{
			for (i=1;i<=p_data_record->no_of_points;i++)
			{	
				fprintf(trace_file,"%g\t%g\t%g\t%g\t%g\n",
					p_data_record->data[TIME][i],
					p_data_record->data[FORCE][i],
					p_data_record->data[SL][i],
					p_data_record->data[FL][i],
					p_data_record->data[INTENSITY][i]);
			}

			fclose(trace_file);

			sprintf(display_string,"Trace values successfuly written to\n%s",file_string);
			MessageBox(display_string,"Analysis_display");
		}
	}
}

void Analysis_display::OnOffsetRescaleforcetoactivecursor() 
{
	// Code resets force relative to active cursor

	p_data_record->reset_force_to_active_cursor(p_FOR_plot->cursor_point[p_FOR_plot->active_cursor]);

	// Update screen

	update_plots();
}

void Analysis_display::set_FORCE_mean_values_line_segment(int number,int status,
	float x_start,float y_start,float x_stop,float y_stop,int colour)
{
	// Code sets line segment for the FORCE plot

	p_FOR_plot->mean_value_line_segments[number].status=status;

	p_FOR_plot->mean_value_line_segments[number].x_start=x_start;
	p_FOR_plot->mean_value_line_segments[number].y_start=y_start;
	p_FOR_plot->mean_value_line_segments[number].x_stop=x_stop;
	p_FOR_plot->mean_value_line_segments[number].y_stop=y_stop;

	p_FOR_plot->mean_value_line_segments[number].colour=colour;
}

void Analysis_display::remote_draw_force_line(float x_start,float y_start,float x_stop,float y_stop,int colour)
{
	// Code draws line between (x_start,y_start) and (x_stop,y_stop) in colour

	// Variables

	CClientDC* p_display_dc;

	// Create display dc

	CClientDC dc(this);
	p_display_dc =& dc;

	// Code

	p_FOR_plot->draw_line(p_display_dc,x_start,y_start,x_stop,y_stop,colour);
}

void Analysis_display::OnPeakPositions() 
{
	// Look for peaks

	// Variables

	int i,j;
	int holder=1;

	float last_min;

	float threshold=(float)0.05;
	int group=25;
	int result=0;

	int maximum_position[100000];


FILE* output_file=fopen("d:\\temp\\peak.txt","w");
maximum_position[0]=0;

	CClientDC* p_display_dc;

	// Create display dc

	CClientDC dc(this);
	p_display_dc =& dc;


	// Code

	last_min=p_data_record->data[FL][1];

	for (i=10;i<=p_data_record->no_of_points-10;i++)
	{
		result=1;

		for (j=i-group;j<=i+group;j++)
		{
			if (p_data_record->data[FL][i]>=p_data_record->data[FL][j])
			{
				result=result*1;
			}
			else
			{
				result=result*0;
			}
		}

		if ((p_data_record->data[FL][i]>threshold)&&(result==1))
		{
			// It is a maximum

			maximum_position[holder]=i;
		
			p_FL_plot->draw_line(p_display_dc,
				p_data_record->data[TIME][i],
				(float)0.95*p_data_record->data[FL][i],
				p_data_record->data[TIME][i],
				(float)1.05*p_data_record->data[FL][i],
				GREEN);

fprintf(output_file,"%i %g %g %g\n",holder,
	p_data_record->data[TIME][i],
	p_data_record->data[TIME][maximum_position[holder]]-
		p_data_record->data[TIME][maximum_position[holder-1]],
	1.0/(p_data_record->data[TIME][maximum_position[holder]]-
		p_data_record->data[TIME][maximum_position[holder-1]]));

			holder++;

			// Mark it

			i=i+60;
		}
	}

fclose(output_file);
}


void Analysis_display::OnPeakResetfile() 
{
	// TODO: Add your command handler code here
	
	FILE *output_file=fopen("c:\\temp\\output.txt","w");
	fclose(output_file);
}



void Analysis_display::OnPeakAddpoint() 
{
	// TODO: Add your command handler code here

	FILE *output_file=fopen("c:\\temp\\output.txt","a");

	fprintf(output_file,"Time: %g For: %g SL: %g FL: %g\n",
		p_data_record->data[TIME][pointers_array[1]->cursor_point[pointers_array[1]->active_cursor]],
		p_data_record->data[FORCE][pointers_array[1]->cursor_point[pointers_array[1]->active_cursor]],
		p_data_record->data[SL][pointers_array[1]->cursor_point[pointers_array[1]->active_cursor]],
		p_data_record->data[FL][pointers_array[1]->cursor_point[pointers_array[1]->active_cursor]]);

	char display_string[200];
	sprintf(display_string,"Time: %g For: %g SL: %g FL: %g",
		p_data_record->data[TIME][pointers_array[1]->cursor_point[pointers_array[1]->active_cursor]],
		p_data_record->data[FORCE][pointers_array[1]->cursor_point[pointers_array[1]->active_cursor]],
		p_data_record->data[SL][pointers_array[1]->cursor_point[pointers_array[1]->active_cursor]],
		p_data_record->data[FL][pointers_array[1]->cursor_point[pointers_array[1]->active_cursor]]);
	MessageBox(display_string);


	fclose(output_file);

}
