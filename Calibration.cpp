// Calibration.cpp : implementation file
//

#include "stdafx.h"
#include "SLControl.h"
#include "Calibration.h"
#include "SLControlDlg.h"

////////////////////////////
// Ken's code starts here //
////////////////////////////

#include "dapio32.h"
#include "global_functions.h"
#include <direct.h>

////////////////////////////
//  Ken's code ends here  //
////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Calibration dialog


Calibration::Calibration(CWnd* pParent)
	: CDialog(Calibration::IDD, pParent)
	, Calibration_file_info_string(_T(""))
{
	m_pParent = pParent;
	m_nID = Calibration::IDD;

	//{{AFX_DATA_INIT(Calibration)
	FL_Change = 0.0f;
	SL_Change = 0.0f;
	Calibration_sarcomere_length = 0.0f;
	Calibration_fibre_length = 0.0f;
	Calibration_step = 0.0f;
	Calibration_SL_volts_to_FL_microns_calibration = 0.0f;
	Calibration_area = 0.0f;
	//}}AFX_DATA_INIT

	////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	// Variables

	char display_string[_MAX_PATH];

	// Pointers to windows

	p_Calibration = this;
	m_pParent = pParent;

	// Create pointer to the Data_record

	p_Cal_record =& Cal_record;

	// Initialisation

	Calibration_SL_volts_to_FL_microns_calibration=1.0;

	Calibration_fibre_length=(float)999.0;
	Calibration_sarcomere_length=(float)999.0;
	Calibration_area=(float)999.0;
	Calibration_step=(float)10.0;

	Calibration_file_info_string="";

	// Set Calibration_dir_path

	::set_executable_directory_string(display_string);
	strcat(display_string,"\\Calibration\\");
	
	Calibration_dir_path=display_string;
	
	header_file_string=Calibration_dir_path+"header.txt";
	create_pipes_file_string=Calibration_dir_path+"create_pipes.txt";
	control_file_string=Calibration_dir_path+"control_file.txt";
	io_procedures_file_string=Calibration_dir_path+"io_procedures.txt";
	DAPL_source_file_string=Calibration_dir_path+"DAPL_source_file.txt";

	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////
}

Calibration::~Calibration(void)
{
	// Destructor

}

void Calibration::PostNcDestroy() 
{
	delete this;
}

BOOL Calibration::Create()
{
	return CDialog::Create(m_nID, m_pParent);
}


void Calibration::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Calibration)
	DDX_Text(pDX, IDC_FL_CHANGE, FL_Change);
	DDX_Text(pDX, IDC_SL_CHANGE, SL_Change);
	DDX_Text(pDX, IDC_SL, Calibration_sarcomere_length);
	DDX_Text(pDX, IDC_FL, Calibration_fibre_length);
	DDX_Text(pDX, IDC_Calibration_Step, Calibration_step);
	DDX_Text(pDX, IDC_SL_to_FL_volts_calibration, Calibration_SL_volts_to_FL_microns_calibration);
	DDX_Text(pDX, IDC_AREA, Calibration_area);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_FILE_INFO_BOX, Calibration_file_info_string);
	DDV_MaxChars(pDX, Calibration_file_info_string, 5000);
}


BEGIN_MESSAGE_MAP(Calibration, CDialog)
	//{{AFX_MSG_MAP(Calibration)
	ON_BN_CLICKED(IDC_CALIBRATE, OnCalibrate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Calibration message handlers

void Calibration::close_calibration_window(void)
{
	// Code is called by parent class to shut down the window.
	// Calibration must first close the Cal_display window which it created

	p_Cal_display->close_Cal_display_window();
	
	delete p_Cal_display;

	DestroyWindow();
}

void Calibration::Cal_display_Window_Done()
{
	// Function is called when the Cal_display window closes

	p_Cal_display = NULL;
}

BOOL Calibration::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	////////////////////////////
	// Ken's code starts here //
	////////////////////////////

	// Variables

	int width=1022;																// screen width and height
	int height=740;																// in pixels

	SetWindowPos(NULL,int(0.68*width),(int)(0.07*height),
		(int)(0.21*width),(int)(0.43*height),SWP_SHOWWINDOW);

	// Disable FL_Change and SL_Change boxes

	GetDlgItem(IDC_FL_CHANGE)->EnableWindow(FALSE);
	GetDlgItem(IDC_SL_CHANGE)->EnableWindow(FALSE);

	// Cal_display window

	p_Cal_display = new Cal_display(this,p_Cal_record);
	p_Cal_display->Create();

	////////////////////////////
	//  Ken's code ends here  //
	////////////////////////////
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void Calibration::OnCalibrate() 
{
	// Calibrates system by updating Calibration_SL_to_FL_volts_calibration

	// Variables

	const int calibration_data_points=420;

	int first_point=1;
	int last_point=calibration_data_points;

	int pre=10;										// points before plus step
	int post=10;									// points after neg step
	int duration=200;								// points during each step

	int holder;										// temp holder

	float SL_1,SL_2,FL_1,FL_2;						// mean values for
													// negative and positive steps of the
													// SL and FL data points respectively

	float time_increment=(float)0.001;				// time increment between consecutive
													// data points

	char display_string[300];						// user output

	// Update data from screen

	UpdateData(TRUE);

	// Files

	create_Calibration_source_file();

	// Code

	const int DataCount=(calibration_data_points*4);
	HDAP hdapSysGet,hdapSysPut,hdapBinGet;
	short int asiData[DataCount];
	int i;

	hdapSysGet=	DapHandleOpen("\\\\.\\Dap0\\$SysOut",DAPOPEN_READ);

	hdapSysPut=DapHandleOpen("\\\\.\\Dap0\\$SysIn",DAPOPEN_WRITE);

	hdapBinGet=DapHandleOpen("\\\\.\\Dap0\\$BinOut",DAPOPEN_READ);

	DapInputFlush(hdapSysGet);
	DapInputFlush(hdapBinGet);

	i=DapConfig(hdapSysPut,DAPL_source_file_string);

	// Error Checking

	if (i==FALSE)
	{
		sprintf(display_string,"DAPL could not handle source file\n%s\nResulting records will be invalid",DAPL_source_file_string);
		MessageBox(display_string,"SL_SREC::run_single_trial()",MB_ICONWARNING);
	}

	DapBufferGet(hdapBinGet,sizeof(asiData),asiData);

	// Copy data to Cal_record

	holder=1;

	for (i=0;i<DataCount;i=i+4)
	{
		p_Cal_record->data[TIME][holder]=((holder-1)*time_increment);
		p_Cal_record->data[FORCE][holder]=convert_DAPL_units_to_volts(asiData[i]);
		p_Cal_record->data[SL][holder]=convert_DAPL_units_to_volts(asiData[i+1]);
		p_Cal_record->data[FL][holder]=convert_DAPL_units_to_volts(asiData[i+2]);
		p_Cal_record->data[INTENSITY][holder]=convert_DAPL_units_to_volts(asiData[i+3]);

		holder++;
	}

	p_Cal_record->no_of_points=(holder-1);

	DapHandleClose(hdapSysGet);
	DapHandleClose(hdapSysPut);
	DapHandleClose(hdapBinGet);

	// Perform Calibration
	
	SL_1=p_Cal_record->extract_mean_value(SL,(pre+1),(pre+1+duration));
	SL_2=p_Cal_record->extract_mean_value(SL,(last_point-post-duration),(last_point-post));

	FL_1=p_Cal_record->extract_mean_value(FL,(pre+1),(pre+1+duration));
	FL_2=p_Cal_record->extract_mean_value(FL,(last_point-pre-duration),(last_point-post));

	SL_Change=SL_2-SL_1;
	FL_Change=::convert_FL_RESPONSE_volts_to_microns(FL_2)
					-::convert_FL_RESPONSE_volts_to_microns(FL_1);

	// Calibration calculation

	Calibration_SL_volts_to_FL_microns_calibration=SL_Change/FL_Change;

	// Update screen

	UpdateData(FALSE);

	// Update data plots

	p_Cal_display->update_plots();
}

int Calibration::create_Calibration_source_file(void)
{
	// Code creates Calibration_source_file by combining 4 sub files

	// Variables

	int i;

	int no_of_sub_files=4;

	char single_character;
	char display_string[200];
	char* p_display_string = display_string;

	// File pointers and array

	FILE* DAPL_source_file=fopen(DAPL_source_file_string,"w");

	FILE* file_pointer[5];

	// Code
		
	if (write_header_file()*write_create_pipes_file()*
			write_control_file()*write_io_procedures_file()==1)
	{
		// Sub-files written successfully - merge files

		// Check files

		if (DAPL_source_file==NULL)
		{
			sprintf(display_string,"%s\ncould not be opened",DAPL_source_file_string);
			MessageBox(display_string,"SL_SREC::create_DAPL_source_file");
			return(0);
		}

		FILE* header_file=fopen(header_file_string,"r");
		FILE* create_pipes_file=fopen(create_pipes_file_string,"r");
		FILE* control_file=fopen(control_file_string,"r");
		FILE* io_procedures_file=fopen(io_procedures_file_string,"r");

		file_pointer[0]=header_file;
		file_pointer[1]=create_pipes_file;
		file_pointer[2]=control_file;
		file_pointer[3]=io_procedures_file;

		for (i=0;i<=(no_of_sub_files-1);i++)
		{
			if (file_pointer[i]==NULL)
			{
				sprintf(display_string,"File %i could not be opened\n",i);
				MessageBox(display_string,"SL_SREC::create_DAPL_source_file");
				return(0);
			}
		}

		// Merge files

		for (i=0;i<=(no_of_sub_files-1);i++)
		{
			while (!feof(file_pointer[i]))
			{
				single_character=fgetc(file_pointer[i]);

				if (single_character!=EOF)
					fputc(single_character,DAPL_source_file);
			}
		}

		// close files

		fclose(DAPL_source_file);

		for (i=0;i<=(no_of_sub_files-1);i++)
		{
			fclose(file_pointer[i]);
		}

		// Return

		return(1);
	}
	else
	{
		sprintf(display_string,"Create_DAPL_source_file failed\n");
		MessageBox(display_string,"create_DAPL_source_file");
		return(0);
	}
}

int Calibration::write_header_file(void)
{
	// Code creates header file

	// Variables

	char display_string[200];

	FILE *header_file;

	// Code

	// File handling

	header_file=fopen(header_file_string,"w");

	if (header_file==NULL)													// Error
	{
		sprintf(display_string,"Header file\n%s\ncould not be opened\n",header_file_string);
		MessageBox(display_string,"SL_SREC::write_header_file");
		return(0);
	}
	else
	{
		fprintf(header_file,"RESET\n\n");

		fprintf(header_file,"OPTION UNDERFLOWQ=OFF\n");
		fprintf(header_file,"OPTION ERRORQ=OFF\n");
		fprintf(header_file,"OPTION SCHEDULING=FIXED\n");
		fprintf(header_file,"OPTION BUFFERING=OFF\n");
		fprintf(header_file,"OPTION QUANTUM=100\n\n");

		fprintf(header_file,";************************************************************\n\n");

		fprintf(header_file,"// Header\n\n");

		fprintf(header_file,"pipes phigh\n");
		fprintf(header_file,"pipes pplus\n");
		fprintf(header_file,"pipes pneg\n");
		fprintf(header_file,"pipes pstep\n");
		fprintf(header_file,"pipes pzero\n");
		fprintf(header_file,"pipes pzero2\n");
		fprintf(header_file,"pipes pzero3\n\n");

		fprintf(header_file,"pipes pout\n");
		fprintf(header_file,"pipes ptrig\n\n");

		fprintf(header_file,"constants pre=10\n");
		fprintf(header_file,"constants post=10\n");
		fprintf(header_file,"constants duration=200\n\n");

		fprintf(header_file,"constants trig_high=1\n");
		fprintf(header_file,"constants trig_low=419\n\n");


		fprintf(header_file,"constants step=%i\n\n",
			::convert_volts_to_DAPL_units(::convert_FL_COMMAND_microns_to_volts(Calibration_step)));

		fprintf(header_file,";************************************************************\n\n");
	}

	// Close file

	fclose(header_file);

	return(1);
}

int Calibration::write_create_pipes_file(void)
{
	// Code creates header file

	// Variables

	char display_string[200];

	FILE *create_pipes_file;

	// Code

	// File handling

	create_pipes_file=fopen(create_pipes_file_string,"w");

	if (create_pipes_file==NULL)													// Error
	{
		sprintf(display_string,"Create_pipes_file\n%s\ncould not be opened\n",create_pipes_file_string);
		MessageBox(display_string,"SL_SREC::write_create_pipes_file");
		return(0);
	}
	else
	{
		fprintf(create_pipes_file,"\npdef create_pipes\n\n");

		fprintf(create_pipes_file,"  phigh=%i\n",::convert_volts_to_DAPL_units((float)4.95));
		fprintf(create_pipes_file,"  pplus=0\n");
		fprintf(create_pipes_file,"  pneg=step\n");
		fprintf(create_pipes_file,"  pzero=0\n");
		fprintf(create_pipes_file,"  pzero2=0\n");
		fprintf(create_pipes_file,"  pzero3=0\n\n");

		fprintf(create_pipes_file,"  nmerge(pre,pzero,duration,pneg,duration,pplus,post,pzero2,pout)\n");
		fprintf(create_pipes_file,"  nmerge(trig_high,phigh,trig_low,pzero3,ptrig)\n\n");

		fprintf(create_pipes_file,"end\n\n");

		fprintf(create_pipes_file,";************************************************************\n\n");
	}

	fclose(create_pipes_file);

	return(1);
}

int Calibration::write_control_file(void)
{
	// Code creates control_file

	// Variables

	char display_string[200];

	FILE *control_file;

	// Code

	// File handling

	control_file=fopen(control_file_string,"w");

	if (control_file==NULL)												// Error
	{
		sprintf(display_string,"Control file\n%s\ncould not be opened\n",control_file_string);
		MessageBox(display_string,"SL_SREC::write_control_file");
		return(0);
	}
	else
	{
		fprintf(control_file,"\npdef control\n");
		fprintf(control_file,"  copy(ptrig,op0)\n");
		fprintf(control_file,"  copy(pout,op1)\n\n");

		fprintf(control_file,"  merge(ip0,ip1,ip2,ip3,$binout)\n");
		fprintf(control_file,"end\n\n");

		fprintf(control_file,";************************************************************\n\n");
	}
	
	fclose(control_file);

	return(1);
}

int Calibration::write_io_procedures_file(void)
{
	// Code creates io_procedures_file

	// Variables

	char display_string[200];

	FILE *io_procedures_file;

	// Code

	// File handling

	io_procedures_file=fopen(io_procedures_file_string,"w");

	if (io_procedures_file==NULL)													// Error
	{
		sprintf(display_string,"IO_Procedures file\n%s\ncould not be opened\n",io_procedures_file_string);
		MessageBox(display_string,"SL_SREC::write_io_procedures_file");
		return(0);
	}
	else
	{
		// Output Proc

		fprintf(io_procedures_file,"\nodef outputproc 2\n");
		fprintf(io_procedures_file,"  set op0 a0\n");
		fprintf(io_procedures_file,"  set op1 a1\n");
		fprintf(io_procedures_file,"  time 500\n");
		fprintf(io_procedures_file,"  count 840\n");
		fprintf(io_procedures_file,"end\n\n");

		fprintf(io_procedures_file,";************************************************************\n\n");

		// Input Proc

		fprintf(io_procedures_file,"idef inputproc 4\n");
		fprintf(io_procedures_file,"  set ip0 s0\n");
		fprintf(io_procedures_file,"  set ip1 s1\n");
		fprintf(io_procedures_file,"  set ip2 s2\n");
		fprintf(io_procedures_file,"  set ip3 s3\n");
		fprintf(io_procedures_file,"  count 1680\n");
		fprintf(io_procedures_file,"  time 250\n");
		fprintf(io_procedures_file,"end\n\n");

		fprintf(io_procedures_file,";************************************************************\n\n");

		// Start tasks

		fprintf(io_procedures_file,"start create_pipes,control,inputproc,outputproc\n");
	}

	fclose(io_procedures_file);

	return(1);
}

BOOL Calibration::PreTranslateMessage(MSG* pMsg) 
{
	// Traps Enter and Escape key presses to prevent dialog box closing

	/*if (pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN)
	{
		return TRUE;
	}*/

	if (pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE)
	{
		return TRUE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}
