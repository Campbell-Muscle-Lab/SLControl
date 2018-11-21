// Superposition.cpp : implementation file
//

#include "stdafx.h"
#include "slcontrol.h"
#include "Superposition.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Superposition dialog

////////////////////////////
// Ken's code starts here //
////////////////////////////

#include "SLControlDlg.h"
#include "global_functions.h"

////////////////////////////
//  Ken's code ends here  //
////////////////////////////

Superposition::Superposition(CWnd* pParent /*=NULL*/,CString* set_p_file_strings,int set_no_of_traces)
	: CDialog(Superposition::IDD, pParent)
{
	m_pParent = pParent;										// set pointer to parent window
	m_nID = Superposition::IDD;

	p_file_strings=set_p_file_strings;							// point to an array of file strings

	no_of_traces=set_no_of_traces;								// sets the no_of_traces		

	// Set pointer to data record

	p_data_record =& superposition_record;

	// Window size and dimensions

	window_x_pos=(float)0.03;
	window_y_pos=(float)0.1;
	window_x_size=(float)0.9;
	window_y_size=(float)0.9;

	// Text anchor

	x_text_anchor=(float)0.55;									// text anchors relative to window size
	y_text_anchor=(float)0.65;												

	// Plots and associated pointers

	no_of_plots=5;
	
	p_FOR_plot =& FOR_plot;
	p_SL_plot  =& SL_plot;
	p_FL_plot  =& FL_plot;
	p_FOR_SL_plot =& FOR_SL_plot;
	p_FOR_FL_plot =& FOR_FL_plot;

	pointers_array[0]=p_FOR_plot;
	pointers_array[1]=p_SL_plot;
	pointers_array[2]=p_FL_plot;
	pointers_array[3]=p_FOR_SL_plot;
	pointers_array[4]=p_FOR_FL_plot;

	// Set file strings

	for (int i=0;i<=(no_of_traces-1);i++)
	{
		file_strings[i]=p_file_strings[i];
	}

	// Screen controls

	multiple_plots_active=0;

	//{{AFX_DATA_INIT(Superposition)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

Superposition::~Superposition(void)
{
	delete p_superposition_control;
}

void Superposition::Superposition_control_Window_Done(void)
{
	p_superposition_control = NULL;
}

void Superposition::PostNcDestroy() 
{
	delete this;
}

BOOL Superposition::Create()
{
	return CDialog::Create(m_nID, m_pParent);
}

void Superposition::remote_shut_down(void)
{
	// Called by parent to shut down window

	OnClose();
}

void Superposition::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Superposition)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Superposition, CDialog)
	//{{AFX_MSG_MAP(Superposition)
	ON_WM_KEYDOWN()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_COMMAND(ID_FILE_COPYTOCLIPBOARD, OnFileCopytoclipboard)
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Superposition message handlers

BOOL Superposition::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	// Initialise window size and position

	// Variables

	int i;

	int width;																// screen width and height
	int height;																// in pixels

	// Set window size and position
	
	CRect work_area;
	CRect* p_work_area =& work_area;

	SystemParametersInfo(SPI_GETWORKAREA,0,p_work_area,0);

	SetWindowPos(NULL,NULL,0,work_area.Width(),work_area.Height(),SWP_SHOWWINDOW);

	width=work_area.Width();													// screen width and height in pixels
	height=work_area.Height();														

	SetWindowPos(NULL,(int)(window_x_pos*width),(int)(window_y_pos*height),
		(int)(window_x_size*width),(int)(window_y_size*height),SWP_SHOWWINDOW);

	// Code

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

	// Initialise plots

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
		CURSOR_DISPLAY_ON,TEXT_DISPLAY_OFF,
		current_window.Width(),current_window.Height(),(float)0.6,(float)0.25,(float)0.2,(float)0.2,
		"SL (V)","For (V)","For v SL");

	p_FOR_FL_plot->initialise_plot(p_data_record,FL,FORCE,AUTO_SCALE_ON,
		CURSOR_DISPLAY_ON,TEXT_DISPLAY_OFF,
		current_window.Width(),current_window.Height(),(float)0.6,(float)0.55,(float)0.2,(float)0.2,
		"FL (V)","For (V)","For v FL");

	// Re-reads data record from file

	p_data_record->read_data_from_file(p_file_strings[0]);

	// Initialise default plots

	for (i=0;i<=(no_of_plots-1);i++)
	{
		pointers_array[i]->cursor_point[pointers_array[i]->active_cursor]=1;

		pointers_array[i]->cursor_point[pointers_array[i]->inactive_cursor]=
			(int)((p_data_record->record_ktr_initiation_time_ms+
				p_data_record->record_ktr_duration_ms*0.5)
				*p_data_record->record_sampling_rate/1000.0);
		
		pointers_array[i]->first_drawn_point=1;
		pointers_array[i]->last_drawn_point=p_data_record->no_of_points;
	}

	// Create Superposition_control window

	p_superposition_control = new Superposition_control(this,this);
	p_superposition_control->Create();
	p_superposition_control->SetActiveWindow();

	// And draw

	plot_first_trace();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void Superposition::OnClose() 
{	
	// Close derived windows

	p_superposition_control->close_superposition_control_window();

	// Signal to parent

	((CSLControlDlg*)m_pParent)->Superposition_Window_Done();

	// Destroy window

	DestroyWindow();
}


void Superposition::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
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

	// Reacts to user input

	// Extract Ctrl Key status if multiple plots are not active, otherwise ignore

	if (!multiple_plots_active)
	{
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

		display_file_name(0,DARK_GREEN);

	}

	//////////////////////////
	// Ken's code ends here //
	//////////////////////////
	
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void Superposition::clear_window(void)
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

void Superposition::plot_first_trace(void)
{
	// Plots the first trace

	// Code

	// Create display dc

	CClientDC dc(this);
	CClientDC* p_dc =& dc;

	// Re-reads data record from file

	p_data_record->read_data_from_file(file_strings[0]);

	// Adjusts for calibrated values as appropriate

	if (p_superposition_control->plot_calibrated_values)
	{
		p_data_record->transform_to_calibrated_values();

		// Update plot titles

		p_FOR_plot->y_axis_title="For (N m^-2)";
		p_SL_plot->y_axis_title="SL (m)";
		p_FL_plot->y_axis_title="FL (m)";
	
		p_FOR_SL_plot->y_axis_title="For (N m^-2)";
		p_FOR_SL_plot->x_axis_title="SL (m)";

		p_FOR_FL_plot->y_axis_title="For (N m^-2)";
		p_FOR_FL_plot->x_axis_title="FL (m)";
	}
	else
	{
		// Update plot titles

		p_FOR_plot->y_axis_title="For (V)";
		p_SL_plot->y_axis_title="SL (V)";
		p_FL_plot->y_axis_title="FL (V)";
	
		p_FOR_SL_plot->y_axis_title="For (V)";
		p_FOR_SL_plot->x_axis_title="SL (V)";

		p_FOR_FL_plot->y_axis_title="For (V)";
		p_FOR_FL_plot->x_axis_title="FL (V)";
	}


	// Draw the plots

	for (int j=0;j<=(no_of_plots-1);j++)
	{
		pointers_array[j]->draw_plot(p_dc,
			pointers_array[j]->first_drawn_point,
			pointers_array[j]->last_drawn_point);
	}

	// Record active cursor y values

	first_record_active_cursor_values[FORCE]=p_data_record->
		data[FORCE][pointers_array[0]->cursor_point[pointers_array[0]->active_cursor]];

	first_record_active_cursor_values[SL]=p_data_record->
		data[SL][pointers_array[0]->cursor_point[pointers_array[0]->active_cursor]];

	first_record_active_cursor_values[FL]=p_data_record->
		data[FL][pointers_array[0]->cursor_point[pointers_array[0]->active_cursor]];

	first_record_active_cursor_values[INTENSITY]=p_data_record->
		data[INTENSITY][pointers_array[0]->cursor_point[pointers_array[0]->active_cursor]];

	// Display file name

	display_file_name(0,DARK_GREEN);
}

void Superposition::display_file_name(int increment,int colour)
{
	// Displays file name in appropriate colour
	
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
	y_pos=(int)(y_text_anchor*current_window.Height())+(increment*15);

	// Create font

	p_display_dc->SetBkMode(TRANSPARENT);
	CFont display_font;
	display_font.CreateFont(12,6,0,0,400,FALSE,FALSE,0,ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_SWISS,"Arial");
	CFont* pOldFont = p_display_dc->SelectObject(&display_font);

	p_display_dc->SetTextColor(return_colour(colour));

	// Labels

	// Filename

	sprintf(display_string,"%s",p_data_record->file_name_string);
	p_display_dc->TextOut(x_pos,y_pos,display_string);
}


void Superposition::plot_single_trace(int colour)
{
	// Plots the first trace

	// Code

	// Create display dc

	CClientDC dc(this);
	CClientDC* p_dc =& dc;

	// Draw the plots

	for (int j=0;j<=(no_of_plots-1);j++)
	{
		pointers_array[j]->draw_trace(p_dc,
			pointers_array[j]->first_drawn_point,
			pointers_array[j]->last_drawn_point,
			colour);
	}
}

void Superposition::plot_remaining_traces(int increment_colour) 
{
	// Superposes selected traces

	// Variables

	int i,j,k;
	int colour;
	float shift;

	// Code

	// Create display dc

	CClientDC dc(this);
	CClientDC* p_dc =& dc;
	
	// Set colour

	if (increment_colour)
	{
		colour=RED;
	}
	else
	{
		colour=WHITE;
	}

	// Loop through traces

	for (i=2;i<=(no_of_traces);i++)
	{
		// Re-reads data record from file

		p_data_record->read_data_from_file(file_strings[i-1]);

		if (p_superposition_control->plot_calibrated_values)
		{
			p_data_record->transform_to_calibrated_values();
		}

		if (p_superposition_control->align_traces_to_cursor)
		{
			for (j=FORCE;j<=INTENSITY;j++)
			{
				shift=p_data_record->data[j][pointers_array[0]->cursor_point[pointers_array[0]->active_cursor]]-
					first_record_active_cursor_values[j];

				for (k=1;k<=p_data_record->no_of_points;k++)
				{
					p_data_record->data[j][k]=p_data_record->data[j][k]-shift;
				}
			}

			MessageBox("Aligned traces");
		}

		// Draw plots and file_name

		plot_single_trace(colour);

		display_file_name((i-1),colour);
		
		// Update colour if necessary

		if (increment_colour)
		{
			colour++;
		
			if (colour>WHITE) colour=RED;
		}
	}

	// Update controls

	multiple_plots_active=1;
}

void Superposition::shuffle_trace_order(void)
{
	// Code reverses file_strings order

	// Variables

	int i;																		// counters

	CString temp_strings[MAX_SUPERPOSED_FILES];									// temp strings

	// Code

	// Copy existing file_strings to temp_strings

	for (i=0;i<=(no_of_traces-1);i++)
	{
		temp_strings[i]=file_strings[i];
	}

	// Shuffle

	for (i=0;i<=(no_of_traces-2);i++)
	{
		file_strings[i]=temp_strings[i+1];
	}

	file_strings[no_of_traces-1]=temp_strings[0];
}

void Superposition::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	plot_first_trace();

	if (multiple_plots_active==1)
	{
		plot_remaining_traces(1);
	}
	
	// Do not call CDialog::OnPaint() for painting messages
}

void Superposition::OnFileCopytoclipboard() 
{
	// TODO: Add your command handler code here
	
	// Prints screen to a metafile and copies the metafile to the Clipboard
	// Code calls uses print_functions defined in Plot class

	// Variables

	int colour=RED;
	char cursor_path_string[200];


	// Rectangle used to define size of metafile (arbitrary values found by trial and error)

	CRect print_rectangle(1,1,32000,21000);
	
	CMetaFileDC * m_pMetaDC = new CMetaFileDC();
	m_pMetaDC->CreateEnhanced(GetDC(),NULL,print_rectangle,"SLControl");
	
	//draw meta file

	// Switch cursor to animated cursor
	
	::set_executable_directory_string(cursor_path_string);
	strcat(cursor_path_string,"\\print.ani");

	HCURSOR hcurWait = (HCURSOR)LoadImage(AfxGetApp()->m_hInstance,
		cursor_path_string,IMAGE_CURSOR,0,0,
		LR_DEFAULTSIZE|LR_DEFAULTCOLOR|LR_LOADFROMFILE);
	const HCURSOR hcurSave=SetCursor(hcurWait);

	SetCursor(hcurWait);

	// Traces

	// Reads first data record from file

	p_data_record->read_data_from_file(file_strings[0]);

	// Draw the plots

	for (int j=0;j<=(no_of_plots-1);j++)
	{
		pointers_array[j]->print_plot(m_pMetaDC,
			pointers_array[j]->first_drawn_point,
			pointers_array[j]->last_drawn_point);
	}

	// Loop through remaining traces

	for (int i=2;i<=(no_of_traces);i++)
	{
		// Re-reads data record from file

		p_data_record->read_data_from_file(file_strings[i-1]);

		// Draw plots

		for (int j=0;j<=(no_of_plots-1);j++)
		{
			pointers_array[j]->print_trace(m_pMetaDC,
				pointers_array[j]->first_drawn_point,
				pointers_array[j]->last_drawn_point,
				colour);
		}
		
		// Update colour

		colour++;
		
		if (colour>MAGENTA) colour=RED;
	}

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

void Superposition::OnFileExit() 
{
	OnClose();
}

COLORREF Superposition::return_colour(int colour)
{
	COLORREF desired_colour;

	switch (colour)
	{
		case BLACK:
		{
			desired_colour=RGB(0,0,0);
		}
		break;

		case RED:
		{
			desired_colour=RGB(255,0,0);
		}
		break;

		case GREEN:
		{
			desired_colour=RGB(0,255,0);
		}
		break;

		case BLUE:
		{
			desired_colour=RGB(0,0,255);
		}
		break;

		case GREY:
		{
			desired_colour=RGB(192,192,192);
		}
		break;

		case YELLOW:
		{	
			desired_colour=RGB(255,255,0);
		}
		break;

		case CYAN:
		{
			desired_colour=RGB(0,255,255);
		}
		break;

		case MAGENTA:
		{
			desired_colour=RGB(255,0,255);
		}
		break;

		case WHITE:
		{
			desired_colour=RGB(255,255,255);
		}
		break;

		case DARK_GREEN:
		{
			desired_colour=RGB(0,207,0);
		}
		break;
		
		default:
		{
			desired_colour=RGB(0,0,0);
		}
	}

	return desired_colour;
}
