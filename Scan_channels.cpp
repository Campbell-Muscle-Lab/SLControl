// Scan_channels.cpp : implementation file
//

#include "stdafx.h"
#include "slcontrol.h"
#include "Scan_channels.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////
// Ken's code starts here //
////////////////////////////

#include "SLControlDlg.h"
#include "dapio32.h"
#include "global_functions.h"

////////////////////////////
//  Ken's code ends here  //
////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Scan_channels dialog


Scan_channels::Scan_channels(CWnd* pParent /*=NULL*/)
	: CDialog(Scan_channels::IDD, pParent)
{
	//{{AFX_DATA_INIT(Scan_channels)
	force_value_volts = 0.0f;
	fl_value_volts = 0.0f;
	sl_int_value_volts = 0.0f;
	sl_value_volts = 0.0f;
	time_seconds = 0.0f;
	//}}AFX_DATA_INIT

	// Pointer to parent window

	m_pParent=pParent;
	m_nID=Scan_channels::IDD;

	// Variables

	char temp_string[200];

	// Initialise record

	p_Scan_channels_record =& Scan_channels_record;

	// Initialise plots

	no_of_plots=4;

	p_FOR_plot    =& FOR_plot;
	p_SL_plot     =& SL_plot;
	p_FL_plot     =& FL_plot;
	p_SL_INT_plot =& SL_INT_plot;

	pointers_array[0]=p_FOR_plot;
	pointers_array[1]=p_SL_plot;
	pointers_array[2]=p_FL_plot;
	pointers_array[3]=p_SL_INT_plot;
	
	// Set file strings

	set_executable_directory_string(temp_string);
	strcat(temp_string,"\\SCAN\\");
	
	Scan_channels_path_string=temp_string;

	dapl_source_file_string=Scan_channels_path_string+"dapl_source.txt";

	// Window size and position

	window_x_pos=(float)0.2;
	window_y_pos=(float)0.2;
	window_x_size=(float)0.35;
	window_y_size=(float)0.5;
}

Scan_channels::~Scan_channels(void)
{	
	// Destructor
}

void Scan_channels::PostNcDestroy() 
{
	delete this;
}

BOOL Scan_channels::Create()
{
	return CDialog::Create(m_nID, m_pParent);
}

void Scan_channels::remote_shut_down(void)
{
	// Called by parent to shut down window

	OnClose();
}

void Scan_channels::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Scan_channels)
	DDX_Text(pDX, IDC_FORCE_VALUE, force_value_volts);
	DDX_Text(pDX, IDC_FL_VALUE, fl_value_volts);
	DDX_Text(pDX, IDC_SL_INT_VALUE, sl_int_value_volts);
	DDX_Text(pDX, IDC_SL_VALUE, sl_value_volts);
	DDX_Text(pDX, IDC_TIME, time_seconds);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Scan_channels, CDialog)
	//{{AFX_MSG_MAP(Scan_channels)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Scan_channels message handlers

BOOL Scan_channels::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	GetDlgItem(IDC_FORCE_VALUE)->EnableWindow(FALSE);
	GetDlgItem(IDC_SL_VALUE)->EnableWindow(FALSE);
	GetDlgItem(IDC_FL_VALUE)->EnableWindow(FALSE);
	GetDlgItem(IDC_SL_INT_VALUE)->EnableWindow(FALSE);
	GetDlgItem(IDC_TIME)->EnableWindow(FALSE);

	// Install a timer

	if (SetTimer(1,250,NULL)==NULL)
	{
		// Warn user if SetTimer fails

		MessageBox("Scan_Channels dialog cannot initiate timer","Scan_channels::OnInitDialog");
	}
	else
	{
		// Set Scanning variable

		scanning_status=TRUE;
		record_point=1;
	}

	// Plots

	CRect work_area;
	CRect* p_work_area =& work_area;

	SystemParametersInfo(SPI_GETWORKAREA,0,p_work_area,0);

	SetWindowPos(NULL,(int)(window_x_pos*work_area.Width()),(int)(window_y_pos*work_area.Height()),
		(int)(window_x_size*work_area.Width()),(int)(window_y_size*work_area.Height()),SWP_SHOWWINDOW);

	// Create display dc

	CClientDC dc(this);
	p_display_dc =& dc;

	// Calculate window dimensions and pass to plot

	CRect current_window;
	GetWindowRect(current_window);
	current_window.NormalizeRect();

	p_FOR_plot->initialise_plot(p_Scan_channels_record,TIME,FORCE,AUTO_SCALE_OFF,
		CURSOR_DISPLAY_ON,TEXT_DISPLAY_OFF,
		current_window.Width(),current_window.Height(),(float)0.2,(float)0.2,(float)0.5,(float)0.14,
		"Scan Points","FOR (V)","Force v Time");
	
	p_SL_plot->initialise_plot(p_Scan_channels_record,TIME,SL,AUTO_SCALE_OFF,
		CURSOR_DISPLAY_ON,TEXT_DISPLAY_OFF,
		current_window.Width(),current_window.Height(),(float)0.2,(float)0.4,(float)0.5,(float)0.14,
		"Scan Points","SL (V)","SL v Time");

	p_FL_plot->initialise_plot(p_Scan_channels_record,TIME,FL,AUTO_SCALE_OFF,
		CURSOR_DISPLAY_ON,TEXT_DISPLAY_OFF,
		current_window.Width(),current_window.Height(),(float)0.2,(float)0.6,(float)0.5,(float)0.14,
		"Scan Points","FL (V)","FL v Time");

	p_SL_INT_plot->initialise_plot(p_Scan_channels_record,TIME,INTENSITY,AUTO_SCALE_OFF,
		CURSOR_DISPLAY_ON,TEXT_DISPLAY_OFF,
		current_window.Width(),current_window.Height(),(float)0.2,(float)0.8,(float)0.5,(float)0.14,
		"Scan Points","INT (V)","SL_INT v Time");

	// Draw default plots

	for (int i=0;i<=(no_of_plots-1);i++)
	{
		pointers_array[i]->draw_plot(p_display_dc,1,p_Scan_channels_record->no_of_points);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void Scan_channels::OnClose() 
{
	// Kill timer

	if (KillTimer(1)==0)
	{
		MessageBox("Timer could not be destroyed","Scan_channels::~Scan_channels");
	}
	else
	{
//		MessageBox("Timer destroyed");
	}

	// Inform parent window

	((CSLControlDlg*)m_pParent)->Scan_channels_Window_Done();

	// Destroy Window

	DestroyWindow();
}

void Scan_channels::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here

//	MessageBox("Painting");
	
	// Do not call CDialog::OnPaint() for painting messages
}

void Scan_channels::OnTimer(UINT nIDEvent) 
{
	// Code executes on each Timing Event
	
	CDialog::OnTimer(nIDEvent);

	// Run single scan if operating

	if (scanning_status==TRUE)
	{
		single_scan();

		// Create display dc

		CClientDC dc(this);
		p_display_dc =& dc;

		// Update plots

		clear_window();

		for (int i=0;i<=(no_of_plots-1);i++)
		{
			pointers_array[i]->cursor_point[pointers_array[i]->active_cursor]=record_point;
			pointers_array[i]->cursor_point[pointers_array[i]->inactive_cursor]=record_point;
			pointers_array[i]->draw_plot(p_display_dc,1,p_Scan_channels_record->no_of_points);
		}

		// Update record point

		if (record_point<=p_Scan_channels_record->no_of_points)
		{
			record_point++;												// increment
		}
		else
		{
			record_point=1;												// return to beginning of trace
		}
	}
}

void Scan_channels::single_scan(void)
{
		// Variables

	const int DataCount=4;
	short int asiData[DataCount];

	HDAP hdapSysGet,hdapSysPut,hdapBinGet;									// DAPL communication pipes

	int i;

	char display_string[200];

	// Code - extracts current channel values

	// Start clock used to calculate how long scan takes

	clock_t start_upload;
	start_upload=clock();

	// Open and flush DAPL communication pipes

	hdapSysGet=DapHandleOpen("\\\\.\\Dap0\\$SysOut",DAPOPEN_READ);
	hdapSysPut=DapHandleOpen("\\\\.\\Dap0\\$SysIn",DAPOPEN_WRITE);
	hdapBinGet=DapHandleOpen("\\\\.\\Dap0\\$BinOut",DAPOPEN_READ);

	// Flush pipes

	DapInputFlush(hdapSysGet);
	DapInputFlush(hdapBinGet);
	DapInputFlush(hdapSysPut);

	// Upload file
		
	i=DapConfig(hdapSysPut,dapl_source_file_string);

	if (i!=1)															// DAPL file error
	{
		sprintf(display_string,"DAPL could not handle scan_file");
		DapLastErrorTextGet(display_string,200);
		MessageBox(display_string,"Scan_channels::single_scan()");
	}

	// Read data

	i=DapBufferGet(hdapBinGet,sizeof(asiData),asiData);

	if (i!=2*DataCount)
	{
		sprintf(display_string,"%i bytes read from DAPL",i);
		MessageBox(display_string,"Scan_channels::single_scan()");
	}

	// Update window

	p_Scan_channels_record->data[FORCE][record_point]=force_value_volts=convert_DAPL_units_to_volts(asiData[0]);
	p_Scan_channels_record->data[SL][record_point]=sl_value_volts=convert_DAPL_units_to_volts(asiData[1]);
	p_Scan_channels_record->data[FL][record_point]=fl_value_volts=convert_DAPL_units_to_volts(asiData[2]);
	p_Scan_channels_record->data[INTENSITY][record_point]=sl_int_value_volts=convert_DAPL_units_to_volts(asiData[1]);
	
	// Stops clock once scan is complete

	time_seconds=((float)(clock()-start_upload)/CLOCKS_PER_SEC);

	UpdateData(FALSE);

	// Close communication pipes

	DapHandleClose(hdapSysGet);
	DapHandleClose(hdapSysPut);
	DapHandleClose(hdapBinGet);
}

void Scan_channels::clear_window(void)
{
	// Code clears plots by drawing over much of the window in the background colour

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

	p_dc->Rectangle(1,1,(int)(0.73*current_window.Width()),(int)(0.85*current_window.Height()));

	// Return brush and pen

	p_dc->SelectObject(pOriginalBrush);
	p_dc->SelectObject(pOriginalPen);
}

void Scan_channels::OnCancel() 
{
	OnClose();
}

BOOL Scan_channels::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class

	// Traps Escape key presses to prevent dialog box closing

	if (pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE)
	{
		return TRUE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}
