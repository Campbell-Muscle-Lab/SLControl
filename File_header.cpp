// File_header.cpp : implementation file
//

#include "stdafx.h"
#include "slcontrol.h"
#include "File_header.h"

#include "SLControlDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// File_header dialog


File_header::File_header(CWnd* pParent,CString set_data_file_string)
	: CDialog(File_header::IDD, pParent)
{
	m_pParent = pParent;										// set pointer to parent window
	m_nID = File_header::IDD;

	// Set data_file_string

	data_file_string=set_data_file_string;

	// Window position and dimensions

	window_x_pos=(float)0.20;
	window_y_pos=(float)0.2;
	window_x_size=(float)0.4;
	window_y_size=(float)0.5;

	// List box position and dimensions

	list_x_pos=(float)0.1;
	list_y_pos=(float)0.05;
	list_x_size=(float)0.7;
	list_y_size=(float)0.8;

	//{{AFX_DATA_INIT(File_header)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

File_header::~File_header(void)
{
	// Destructor
}

void File_header::PostNcDestroy() 
{
	delete this;
}

BOOL File_header::Create()
{
	return CDialog::Create(m_nID, m_pParent);
}

void File_header::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(File_header)
	DDX_Control(pDX, IDC_HEADER, file_header_list_box);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(File_header, CDialog)
	//{{AFX_MSG_MAP(File_header)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// File_header message handlers

void File_header::OnClose() 
{
	// Signal to parent

	((CSLControlDlg*)m_pParent)->File_header_Window_Done();

	// Destroy window

	DestroyWindow();
}

void File_header::remote_shut_down(void)
{
	// Called by parent to shut down dialog
	// Parent cannot access OnClose etc.

	OnClose();
}

BOOL File_header::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Variables

	char display_string[200];

	// Initialise window size and position

	int width,height;

	// Number of lines read in file

	int lines_read;

	// Set window size and position
	
	CRect work_area;
	CRect* p_work_area =& work_area;

	SystemParametersInfo(SPI_GETWORKAREA,0,p_work_area,0);

	SetWindowPos(NULL,NULL,0,work_area.Width(),work_area.Height(),SWP_SHOWWINDOW);

	width=work_area.Width();													// screen width and height in pixels
	height=work_area.Height();														

	SetWindowPos(NULL,(int)(window_x_pos*width),(int)(window_y_pos*height),
		(int)(window_x_size*width),(int)(window_y_size*height),SWP_SHOWWINDOW);

	// Insert text from file

	lines_read=read_header();

	// Update title bar

	sprintf(display_string,"%s (first %i lines)",data_file_string,lines_read);
	SetWindowText(display_string);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int File_header::read_header(void)
{
	// Reads file header

	// Variables

	char display_string[200];
	char temp_string[200];
	char line_string[200];
	char single_character=' ';

	int test;
	int lines_read;

	CDC* pDC = GetDC();							// used to get the width in pixels of the
	CSize szText;								// longest line
	 
	// Code

	// File handling - try to open the file

	FILE *data_file=fopen(data_file_string,"r");

	// Error checking

	if (data_file == NULL)
	{
		sprintf(display_string,"Sorry the data file you selected could not be opened\nDialog will close");
		MessageBox(display_string);
		OnClose();
	}

	// Insert header into text box one line at a time

	lines_read=0;
	max_line_length=0;

	while ((single_character!=EOF)&&(lines_read<1000))
	{
		// Start a new line

		sprintf(line_string,"");

		// Check character is not EOF or a new line and append it to line_string

		while (((single_character=fgetc(data_file))!=EOF)&&((test=single_character)!=10))
		{
			// Copy character unless it is a tab in which case space it out

			if (test==9)
				sprintf(temp_string,"   ");
			else
				sprintf(temp_string,"%c",single_character);

			strcat(line_string,temp_string);
		}

		file_header_list_box.AddString(line_string);
		
		// Extract the width in pixels of the longest line

		szText=pDC->GetTextExtent(line_string);
		if (szText.cx>max_line_length)
			max_line_length=szText.cx;

		lines_read++;
	}

	fclose(data_file);

	return lines_read;
}

void File_header::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// Check window dimensions

	// Size list box

	if( NULL != file_header_list_box.GetSafeHwnd() )
	{
		file_header_list_box.SetWindowPos(NULL,
		(int)(list_x_pos*cx),
		(int)(list_y_pos*cy),
		(int)((list_x_pos+list_x_size)*cx),
		(int)((list_y_pos+list_y_size)*cy),NULL);

		file_header_list_box.SetHorizontalExtent(max_line_length);
	}
}

void File_header::OnPaint() 
{
	// Refreshes screen when necessary after resizing the dialog

	CPaintDC dc(this); // device context for painting
	
	UpdateData(FALSE);
}
