// Plot.cpp: implementation of the Plot class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "slcontrol.h"
#include "Plot.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

////////////////////////////
// Ken's code starts here //
////////////////////////////

#include "global_definitions.h"
#include "Fit_parameters.h"
#include <math.h>
#include <afxctl.h>

////////////////////////////
//  Ken's code ends here  //
////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Plot::Plot()
{
	// Default constructor

	// Variables

	int i;

	// Code

	max_drawn_points=2000;

	active_cursor=0;
	inactive_cursor=1;

	active_colour=RED;
	inactive_colour=BLUE;

	y_axis_label_separation=55;

	// Default line_segments

	for (i=0;i<=(MAX_LINE_SEGMENTS-1);i++)
	{
		mean_value_line_segments[i].status=0;					// all are inactive by default
	}
}

Plot::~Plot()
{
	// Destructor
}

void Plot::initialise_plot(Data_record* set_p_record,
		int set_x_channel, int set_y_channel,
		int set_auto_scale, int set_cursor_display, int set_text_display,
		int set_window_width,int set_window_height,
		float f_x_origin,float f_y_origin, float f_x_length, float f_y_length,
		CString set_x_axis_title, CString set_y_axis_title,
		CString set_plot_title)
{
	// Set plot parameters
	// Called by Cal_display::OnInitDialog
	// because Cal_display window dimensions are needed for these parameters

	p_record = set_p_record;
	
	x_channel = set_x_channel;
	y_channel = set_y_channel;

	auto_scale=set_auto_scale;
	cursor_display=set_cursor_display;
	text_display=set_text_display;

	window_width=set_window_width;
	window_height=set_window_height;

	x_origin=(int)(f_x_origin*window_width);
	y_origin=(int)(f_y_origin*window_height);

	x_length=(int)(f_x_length*window_width);
	y_length=(int)(f_y_length*window_height);

	x_axis_title=set_x_axis_title;
	y_axis_title=set_y_axis_title;

	plot_title=set_plot_title;

	old_cursor_point[active_cursor]=cursor_point[active_cursor]=(int)(0.33*p_record->no_of_points);
	old_cursor_point[inactive_cursor]=cursor_point[inactive_cursor]=(int)(0.66*p_record->no_of_points);

	first_drawn_point=1;
	last_drawn_point=p_record->no_of_points;
}

void Plot::draw_plot(CClientDC* p_dc, int start_int, int stop_int, int AUTO_SCALING_ON)
{
	// Code first rights over old plot in grey,
	// then draws new axes, adds scales and then
	// draws new trace between start_int and stop_int

	// Variables

	int i;																// counter

	char display_string[100];

	// Code

	// Draws axes

	CPen* pOriginalPen;	
	CPen AxesPen;
	AxesPen.CreatePen(PS_SOLID,1,return_colour(BLACK));
	pOriginalPen = p_dc->SelectObject(&AxesPen);

	p_dc->MoveTo(x_origin,(y_origin-y_length));
	p_dc->LineTo(x_origin,y_origin);
	p_dc->LineTo((x_origin+x_length),y_origin);

	p_dc->SelectObject(pOriginalPen);

	// Set scaling factors

	if (AUTO_SCALING_ON)
		set_scaling_factors(start_int,stop_int);

	// Display text

	// Background Pen

	CPen BackgroundPen;
	BackgroundPen.CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNFACE));
	pOriginalPen = p_dc->SelectObject(&BackgroundPen);

	// Background Brush

	CBrush* pOriginalBrush;
	CBrush BackgroundBrush;
	BackgroundBrush.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	pOriginalBrush = p_dc->SelectObject(&BackgroundBrush);

	// Create font

	p_dc->SetBkMode(TRANSPARENT);
	CFont display_font;
	display_font.CreateFont(12,0,0,0,400,FALSE,FALSE,0,ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_SWISS,"Arial");
	CFont* pOldFont = p_dc->SelectObject(&display_font);

	// Overdraw old plot

	p_dc->Rectangle((x_origin+1),(y_origin-y_length),(x_origin+x_length),(y_origin-1));

	// Titles
	
	p_dc->Rectangle((x_origin+(int)(0.39*x_length)),(y_origin+1),
		(x_origin+(int)(0.61*x_length)),(y_origin+15));
	p_dc->TextOut((x_origin+(int)(0.4*x_length)),(y_origin+5),x_axis_title);

	p_dc->Rectangle((x_origin-y_axis_label_separation),(y_origin-(int)(0.6*y_length)),
		(x_origin-1),(y_origin-(int)(0.4*y_length)));
	p_dc->TextOut((x_origin-y_axis_label_separation),(y_origin-(int)(0.55*y_length)),y_axis_title);

	// Labels

	if ((fabs(x_min)>99.0)||(fabs(x_min)<0.1))
	{
		sprintf(display_string,"%4.3e",x_min);
	}
	else
	{
		sprintf(display_string,"%4.3f",x_min);
	}
	p_dc->Rectangle(x_origin,(y_origin+1),
		(x_origin+(int)(0.2*x_length)),(y_origin+15));
	p_dc->TextOut(x_origin,(y_origin+5),display_string);

	
	if ((fabs(x_max)>99.0)||(fabs(x_max)<0.1))
	{
		sprintf(display_string,"%4.3e",x_max);
	}
	else
	{
		sprintf(display_string,"%4.3f",x_max);
	}
	p_dc->Rectangle((x_origin+x_length-30),(y_origin+1),
		(x_origin+x_length),(y_origin+15));
	p_dc->TextOut((x_origin+x_length-30),(y_origin+5),display_string);

	if ((fabs(y_min)>99.0)||(fabs(y_min)<0.1)) 
	{
		sprintf(display_string,"%4.3e",y_min);
	}
	else
	{
		sprintf(display_string,"%4.3f",y_min);
	}
	p_dc->Rectangle((x_origin-y_axis_label_separation),(y_origin-15),
		(x_origin-1),y_origin);
	p_dc->TextOut((x_origin-y_axis_label_separation),(y_origin-10),display_string);

	if ((fabs(y_max)>99.0)||(fabs(y_max)<0.1)) 
	{
		sprintf(display_string,"%4.3e",y_max);
	}
	else
	{
		sprintf(display_string,"%4.3f",y_max);
	}
	p_dc->Rectangle((x_origin-y_axis_label_separation),(y_origin-y_length),
		(x_origin-1),(y_origin-y_length+15));
	p_dc->TextOut((x_origin-y_axis_label_separation),(y_origin-y_length),display_string);

	// Return brush and pen

	p_dc->SelectObject(pOriginalPen);
	p_dc->SelectObject(pOriginalBrush);

	// Select old font again

	p_dc->SelectObject(pOldFont);

	// Draws trace

	draw_trace(p_dc,start_int,stop_int,DARK_GREEN);

	// Draws cursor

	if (cursor_display==CURSOR_DISPLAY_ON)
	{
		draw_cursor(p_dc,active_cursor,active_colour);
		draw_cursor(p_dc,inactive_cursor,inactive_colour);
	}
	
	if (text_display==TEXT_DISPLAY_ON)
	{
		draw_cursor_labels(p_dc);
	}

	// Draws line segments if active

	for (i=0;i<=(MAX_LINE_SEGMENTS-1);i++)
	{
		if (mean_value_line_segments[i].status==TRUE)
		{
			draw_line_segment(p_dc,&mean_value_line_segments[i]);
		}
	}
}

void Plot::draw_line_segment(CClientDC* p_dc, line_segment* p_segment)
{
	// Code draws specified line segment

	// Select Pen

	CPen* pOriginalPen;
	CPen TracePen;
	TracePen.CreatePen(PS_SOLID,1,return_colour(p_segment->colour));
	pOriginalPen = p_dc->SelectObject(&TracePen);

	// Move to start of segment

	p_dc->MoveTo((x_origin+(int)((p_segment->x_start-x_min)*x_scale)),
		(y_origin-(int)((p_segment->y_start-y_min)*y_scale)));

	// Draw to end of segment

	p_dc->LineTo((x_origin+(int)((p_segment->x_stop-x_min)*x_scale)),
		(y_origin-(int)((p_segment->y_stop-y_min)*y_scale)));

	// Return the pen

	p_dc->SelectObject(pOriginalPen);
}

void Plot::draw_trace(CClientDC* p_dc, int start_int, int stop_int, int trace_colour)
{
	// Code draws trace in specified colour

	// Variables

	int i;
	int step;

	// Code

	// Select Pen

	CPen* pOriginalPen;
	CPen TracePen;
	TracePen.CreatePen(PS_SOLID,1,return_colour(trace_colour));
	pOriginalPen = p_dc->SelectObject(&TracePen);

	// Move to start of trace

	p_dc->MoveTo((x_origin+(int)((p_record->data[x_channel][start_int]-x_min)*x_scale)),
		(y_origin-(int)((p_record->data[y_channel][start_int]-y_min)*y_scale)));

	// Draw remainder

	// First check to see whether (stop_int-start_int) exceeds max_drawn_points
	// If so, increase step from 1 until (stop_int-start_int)/step is less than max_drawn_points
	
	step=1;

	if ((stop_int-start_int)>max_drawn_points)
	{
		do
		{
			step++;
		}
		while (((float)(stop_int-start_int)/(float)step)>max_drawn_points);
	}

	// Draw trace
		
	for (i=start_int+1;i<=stop_int;i=i+step)
	{
		p_dc->LineTo((x_origin+(int)((p_record->data[x_channel][i]-x_min)*x_scale)),
			(y_origin-(int)((p_record->data[y_channel][i]-y_min)*y_scale)));
	}

	// Warn by screen prompt if step>1

	if (step>1)
	{
		// Create font

		p_dc->SetBkMode(TRANSPARENT);
		CFont display_font;
		display_font.CreateFont(12,0,0,0,400,FALSE,FALSE,0,ANSI_CHARSET,
			OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
			DEFAULT_PITCH|FF_SWISS,"Arial");
		CFont* pOldFont = p_dc->SelectObject(&display_font);

		// Add warning to plot

		p_dc->TextOut((x_origin+x_length-55),(y_origin-12),"Speed mode");	

		// Select old font again

		p_dc->SelectObject(pOldFont);
	}

	// Return the pen

	p_dc->SelectObject(pOriginalPen);
}

void Plot::draw_line(CClientDC* p_dc,float x_start,float y_start,float x_stop,float y_stop,int colour)
{
	// Draws line in colour between (x_start,y_start) and (x_end,y_end)

	// Code

	// Select Pen

	CPen* pOriginalPen;
	CPen TracePen;
	TracePen.CreatePen(PS_SOLID,1,return_colour(colour));
	pOriginalPen = p_dc->SelectObject(&TracePen);

	// Move to first point

	p_dc->MoveTo((x_origin+(int)((x_start-x_min)*x_scale)),
		(y_origin-(int)((y_start-y_min)*y_scale)));

	// Draw line

	p_dc->LineTo((x_origin+(int)((x_stop-x_min)*x_scale)),
		(y_origin-(int)((y_stop-y_min)*y_scale)));

	// Return the pen

	p_dc->SelectObject(pOriginalPen);
}

void Plot::draw_cursor(CClientDC* p_dc,int cursor,int colour)
{
	// Draws selected cursor

	// Variables

	int size=5;																// size of cross-hair (in pixels)

	// Code

	// Create Cursor Pen
	
	CPen* pOriginalPen;
	CPen CursorPen;
	CursorPen.CreatePen(PS_SOLID,1,return_colour(colour));
	pOriginalPen = p_dc->SelectObject(&CursorPen);

	// Draw Cross-hair

	p_dc->MoveTo((x_origin+(int)((p_record->data[x_channel][cursor_point[cursor]]-x_min)*x_scale)-size),
		(y_origin-(int)((p_record->data[y_channel][cursor_point[cursor]]-y_min)*y_scale)));
	p_dc->LineTo((x_origin+(int)((p_record->data[x_channel][cursor_point[cursor]]-x_min)*x_scale)+size),
		(y_origin-(int)((p_record->data[y_channel][cursor_point[cursor]]-y_min)*y_scale)));

	p_dc->MoveTo((x_origin+(int)((p_record->data[x_channel][cursor_point[cursor]]-x_min)*x_scale)),
		(y_origin-(int)((p_record->data[y_channel][cursor_point[cursor]]-y_min)*y_scale)-size));
	p_dc->LineTo((x_origin+(int)((p_record->data[x_channel][cursor_point[cursor]]-x_min)*x_scale)),
		(y_origin-(int)((p_record->data[y_channel][cursor_point[cursor]]-y_min)*y_scale)+size));

	// Return original pen

	p_dc->SelectObject(pOriginalPen);

	// Set old_cursor_point

	old_cursor_point[cursor]=cursor_point[cursor];
}

void Plot::draw_cursor_labels(CClientDC* p_dc)
{
	// Code draws cursor labels

	// Variables

	int x_increment=35;
	int y_increment=10;
	int x_text_anchor=(x_origin+x_length+15);
	int y_text_anchor=(y_origin-(int)(0.5*y_length)-(int)(4.5*y_increment));
	
	int int_value;														// current output
	float float_value;													// current output

	char display_string[100];

	// Create font

	p_dc->SetBkMode(TRANSPARENT);
	CFont display_font;
	display_font.CreateFont(12,0,0,0,400,FALSE,FALSE,0,ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_SWISS,"Arial");
	CFont* pOldFont = p_dc->SelectObject(&display_font);

	// Labels

	sprintf(display_string,"X");
	p_dc->TextOut((x_text_anchor+x_increment),y_text_anchor,display_string);

	// Left cursor

	int_value=IMIN(cursor_point[active_cursor],cursor_point[inactive_cursor]);
	sprintf(display_string,"[%i]",int_value);
	p_dc->TextOut(x_text_anchor,(y_text_anchor+y_increment),display_string);

	float_value=p_record->data[x_channel][int_value];
	if ((fabs(float_value)>99.0)||(fabs(float_value)<0.1))
	{
		sprintf(display_string,"%4.3e",float_value);
	}
	else
	{
		sprintf(display_string,"%5.4f",float_value);
	}
	p_dc->TextOut((x_text_anchor+x_increment),(y_text_anchor+y_increment),display_string);

	// Right cursor

	int_value=IMAX(cursor_point[active_cursor],cursor_point[inactive_cursor]);
	sprintf(display_string,"[%i]",int_value);
	p_dc->TextOut(x_text_anchor,(y_text_anchor+(2*y_increment)),display_string);

	float_value=p_record->data[x_channel][int_value];
	if ((fabs(float_value)>99.0)||(fabs(float_value)<0.1))
	{
		sprintf(display_string,"%4.3e",float_value);
	}
	else
	{
		sprintf(display_string,"%5.4f",float_value);
	}
	p_dc->TextOut((x_text_anchor+x_increment),(y_text_anchor+(2*y_increment)),display_string);

	// Difference

	int_value=IMAX(cursor_point[active_cursor],cursor_point[inactive_cursor])-
		IMIN(cursor_point[active_cursor],cursor_point[inactive_cursor]);
	sprintf(display_string,"Diff");
	p_dc->TextOut(x_text_anchor,(y_text_anchor+(3*y_increment)),display_string);

	float_value=p_record->data[x_channel][IMAX(cursor_point[active_cursor],cursor_point[inactive_cursor])]-
		p_record->data[x_channel][IMIN(cursor_point[active_cursor],cursor_point[inactive_cursor])];
	if ((fabs(float_value)>99.0)||(fabs(float_value)<0.1))
	{
		sprintf(display_string,"%5.4e",float_value);
	}
	else
	{
		sprintf(display_string,"%5.4f",float_value);
	}
	p_dc->TextOut((x_text_anchor+x_increment),(y_text_anchor+(3*y_increment)),display_string);

	// Labels

	sprintf(display_string,"Y");
	p_dc->TextOut((x_text_anchor+x_increment),(y_text_anchor+(5*y_increment)),display_string);

	// Left cursor

	int_value=IMIN(cursor_point[active_cursor],cursor_point[inactive_cursor]);
	sprintf(display_string,"[%i]",int_value);
	p_dc->TextOut(x_text_anchor,(y_text_anchor+(6*y_increment)),display_string);

	float_value=p_record->data[y_channel][int_value];
	if ((fabs(float_value)>99.0)||(fabs(float_value)<0.1))
	{
		sprintf(display_string,"%4.3e",float_value);
	}
	else
	{
		sprintf(display_string,"%4.3f",float_value);
	}
	p_dc->TextOut((x_text_anchor+x_increment),(y_text_anchor+(6*y_increment)),display_string);

	// Right cursor

	int_value=IMAX(cursor_point[active_cursor],cursor_point[inactive_cursor]);
	sprintf(display_string,"[%i]",int_value);
	p_dc->TextOut(x_text_anchor,(y_text_anchor+(7*y_increment)),display_string);

	float_value=p_record->data[y_channel][int_value];
	if ((fabs(float_value)>99.0)||(fabs(float_value)<0.1))
	{
		sprintf(display_string,"%4.3e",float_value);
	}
	else
	{
		sprintf(display_string,"%4.3f",float_value);
	}
	p_dc->TextOut((x_text_anchor+x_increment),(y_text_anchor+(7*y_increment)),display_string);

	// Difference

	int_value=IMAX(cursor_point[active_cursor],cursor_point[inactive_cursor])-
		IMIN(cursor_point[active_cursor],cursor_point[inactive_cursor]);
	sprintf(display_string,"Diff");
	p_dc->TextOut(x_text_anchor,(y_text_anchor+(8*y_increment)),display_string);

	float_value=p_record->data[y_channel][IMAX(cursor_point[active_cursor],cursor_point[inactive_cursor])]-
		p_record->data[y_channel][IMIN(cursor_point[active_cursor],cursor_point[inactive_cursor])];
	if ((fabs(float_value)>99.0)||(fabs(float_value)<0.1))
	{
		sprintf(display_string,"%4.3e",float_value);
	}
	else
	{
		sprintf(display_string,"%4.3f",float_value);
	}
	p_dc->TextOut((x_text_anchor+x_increment),(y_text_anchor+(8*y_increment)),display_string);

	// Select old font again

	p_dc->SelectObject(pOldFont);
}

void Plot::set_scaling_factors(int start_int, int stop_int)
{
	// Variables

	int i;

	float x,y;
	float temp_x_min,temp_x_max;
	float temp_y_min,temp_y_max;

	// Code

	for (i=start_int;i<=stop_int;i++)
	{
		x=p_record->data[x_channel][i];
		y=p_record->data[y_channel][i];

		if (i==start_int)
		{
			// Set initial values

			temp_x_min=temp_x_max=x;
			temp_y_min=temp_y_max=y;
		}

		if (x<temp_x_min)
		{
			temp_x_min=x;
		}

		if (x>temp_x_max)
		{
			temp_x_max=x;
		}

		if (y<temp_y_min)
		{
			temp_y_min=y;
		}

		if (y>temp_y_max)
		{
			temp_y_max=y;
		}
	}

	// Set scales

	x_min=temp_x_min;
	x_max=temp_x_max;

	if (x_min==x_max)
	{
		x_min=temp_x_min-(float)1.0;
		x_max=temp_x_max+(float)1.0;
	}

	x_scale=(x_length/(temp_x_max-temp_x_min));

	if (auto_scale==AUTO_SCALE_OFF)
	{
		// Auto-scaling is off

		y_scale=(float)(y_length/ADC_FULL_SCALE);
		y_min=(float)ADC_MIN;
		y_max=(float)ADC_MAX;
	}
	else
	{
		// Auto-scaling is on

		if (temp_y_max==temp_y_min)
		{
			y_min=temp_y_min-(float)0.01;
			y_max=temp_y_max+(float)0.01;
		}
		else
		{
			y_min=temp_y_min;
			y_max=temp_y_max;
		}

		y_scale=(y_length/(y_max-y_min));
	}
}

void Plot::respond_to_key_press(CClientDC* p_display_dc,int key_code,int control_key)
{
	// Code processes key process from display window

	// Variables

	int increment;

	// Code

	// Switch tree

	switch (key_code)
	{
		case 65:														// a or A pressed
		{
			if (control_key==1)											// Ctr is pressed, move rapidly
			{
				increment=(int)(-0.04*(last_drawn_point-first_drawn_point));
																		// Corrects for increment >-1

				if (increment>-1)
					increment=-1;

				update_plot_cursors(increment,NO_SWAP);					// move cursor rapidly
			}
			else
			{
				update_plot_cursors(-1,NO_SWAP);						// move one place
			}
		}
		break;

		case 68:														// d or D pressed
		{
			if (control_key==1)											// Ctr is pressed, move rapidly
			{
				increment=(int)(0.04*(last_drawn_point-first_drawn_point));

				if (increment<1)
					increment=1;

				update_plot_cursors(increment,NO_SWAP);
			}
			else
			{
				update_plot_cursors(+1,NO_SWAP);
			}
		}
		break;

		case 83:														// s or S pressed
		{
			update_plot_cursors(0,SWAP_CURSORS);
		}
		break;

		case 69:														// e or E pressed
		{
			// Expand between cursors

			first_drawn_point=IMIN(cursor_point[active_cursor],cursor_point[inactive_cursor]);
			last_drawn_point=IMAX(cursor_point[active_cursor],cursor_point[inactive_cursor]);
	
			// Correct for case where cursors are superimposed

			if ((first_drawn_point==last_drawn_point)&&
				(first_drawn_point==p_record->no_of_points))
			{
				first_drawn_point--;
			}
			else
			{
				if (first_drawn_point==1)
				{
					last_drawn_point++;
				}
			}
		}
		break;

		case 87:													// w or W pressed
		{
			first_drawn_point=1;
			last_drawn_point=p_record->no_of_points;
		}
		break;

		default:
		{
			;
		}
		break;
	}

	draw_plot(p_display_dc,first_drawn_point,last_drawn_point);
}

void Plot::update_plot_cursors(int increment,int swap_status)
{
	// Variables

	int temp;

	// Code

	if (increment!=0)													// Move cursors
	{
		cursor_point[active_cursor]=cursor_point[active_cursor]+increment;

		// Check for out of range

		if (cursor_point[active_cursor]<1)
		{
			cursor_point[active_cursor]=1;
		}
		
		if (cursor_point[active_cursor]>p_record->no_of_points)
		{
			cursor_point[active_cursor]=p_record->no_of_points;
		}

		// Check for re-scaling

		if (cursor_point[active_cursor]<first_drawn_point)
		{
			first_drawn_point=cursor_point[active_cursor];
		}

		if (cursor_point[active_cursor]>last_drawn_point)
		{
			last_drawn_point=cursor_point[active_cursor];
		}

	}

	if (swap_status==SWAP_CURSORS)
	{
		temp=active_cursor;
		active_cursor=inactive_cursor;
		inactive_cursor=temp;
	}
}


void Plot::draw_fit(CClientDC* p_dc,Fit_parameters* p_fit_parameters, int colour)
{
	// Draws fit

	// Variables

	int i;

	int first_drawn_fit_point;
	int last_drawn_fit_point;

	// Code

	// Sets first_drawn_fit_point

	if ((p_fit_parameters->first_point)>first_drawn_point)
		first_drawn_fit_point=1;
	else
		first_drawn_fit_point=first_drawn_point-(p_fit_parameters->first_point)+1;

	// Sets last_drawn_fit_point

	if (p_fit_parameters->last_point>last_drawn_point)
	{
		last_drawn_fit_point=(last_drawn_point-p_fit_parameters->first_point)+1;
	}
	else
	{
		last_drawn_fit_point=(p_fit_parameters->last_point-p_fit_parameters->first_point)+1;
	}
	
	// Draws original trace in yellow

	CPen* pOriginalPen;
	CPen TracePen;
	TracePen.CreatePen(PS_SOLID,1,RGB(255,255,0));
	pOriginalPen = p_dc->SelectObject(&TracePen);
	
	p_dc->MoveTo((x_origin+(int)((p_fit_parameters->x_offset+p_fit_parameters->x_data[first_drawn_fit_point]-x_min)*x_scale)),
		(y_origin-(int)((p_fit_parameters->y_offset+p_fit_parameters->y_data[first_drawn_fit_point]-y_min)*y_scale)));
	
	for (i=first_drawn_fit_point+1;i<=last_drawn_fit_point;i++)
	{
		p_dc->LineTo((x_origin+(int)((p_fit_parameters->x_offset+p_fit_parameters->x_data[i]-x_min)*x_scale)),
			(y_origin-(int)((p_fit_parameters->y_offset+p_fit_parameters->y_data[i]-y_min)*y_scale)));
	}

	p_dc->SelectObject(pOriginalPen);

	// Draw fit in selected color

	CPen FitPen;

	switch (colour)
	{
		case CYAN:
		{
			FitPen.CreatePen(PS_SOLID,1,RGB(0,255,255));
		}
		break;

		case MAGENTA:
		{
			FitPen.CreatePen(PS_SOLID,1,RGB(255,0,255));
		}
		break;

		default:	// black
		{
			FitPen.CreatePen(PS_SOLID,1,RGB(0,0,0));
		}
		break;
	}

	pOriginalPen = p_dc->SelectObject(&FitPen);

	p_dc->MoveTo((x_origin+(int)((p_fit_parameters->x_offset+p_fit_parameters->x_data[first_drawn_fit_point]-x_min)*x_scale)),
		(y_origin-(int)((p_fit_parameters->y_offset+p_fit_parameters->y_fit[first_drawn_fit_point]-y_min)*y_scale)));
	
	for (i=first_drawn_fit_point+1;i<=last_drawn_fit_point;i++)
	{
		p_dc->LineTo((x_origin+(int)((p_fit_parameters->x_offset+p_fit_parameters->x_data[i]-x_min)*x_scale)),
			(y_origin-(int)((p_fit_parameters->y_offset+p_fit_parameters->y_fit[i]-y_min)*y_scale)));
	}

	p_dc->SelectObject(pOriginalPen);

	// Redraw cursors

	draw_cursor(p_dc,active_cursor,active_colour);
	draw_cursor(p_dc,inactive_cursor,inactive_colour);
}

void Plot::print_plot(CMetaFileDC* p_dc, int start_int, int stop_int)
{
	// Code prints plot to metafile - based on draw_plot with slight differences
	// e.g. traces are drawn in grey, background is white

	// Variables

	char display_string[100];

	// Code

	// Draws axes

	CPen* pOriginalPen;	
	CPen AxesPen;
	AxesPen.CreatePen(PS_SOLID,1,RGB(0,0,0));
	pOriginalPen = p_dc->SelectObject(&AxesPen);

	p_dc->MoveTo(x_origin,(y_origin-y_length));
	p_dc->LineTo(x_origin,y_origin);
	p_dc->LineTo((x_origin+x_length),y_origin);

	p_dc->SelectObject(pOriginalPen);

	// Set scaling factors

	set_scaling_factors(start_int,stop_int);

	// Display text

	// Background Pen

	CPen BackgroundPen;
	BackgroundPen.CreatePen(PS_SOLID,1,GetSysColor(RGB(255,255,255)));
	pOriginalPen = p_dc->SelectObject(&BackgroundPen);

	// Background Brush

	CBrush* pOriginalBrush;
	CBrush BackgroundBrush;
	BackgroundBrush.CreateSolidBrush(RGB(255,255,255));
	pOriginalBrush = p_dc->SelectObject(&BackgroundBrush);

	// Create font

	p_dc->SetBkMode(TRANSPARENT);
	CFont display_font;
	display_font.CreateFont(12,0,0,0,400,FALSE,FALSE,0,ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_SWISS,"Arial");
	CFont* pOldFont = p_dc->SelectObject(&display_font);

	// Titles
	
	p_dc->TextOut((x_origin+(int)(0.4*x_length)),(y_origin+5),x_axis_title);

	p_dc->TextOut((x_origin-55),(y_origin-(int)(0.55*y_length)),y_axis_title);

	// Labels
	
	if ((fabs(x_min)>99.0)||(fabs(x_min)<0.1))
	{
		sprintf(display_string,"%4.3e",x_min);
	}
	else
	{
		sprintf(display_string,"%4.3f",x_min);
	}
	p_dc->TextOut(x_origin,(y_origin+5),display_string);

	
	if ((fabs(x_max)>99.0)||(fabs(x_max)<0.1))
	{
		sprintf(display_string,"%4.3e",x_max);
	}
	else
	{
		sprintf(display_string,"%4.3f",x_max);
	}
	p_dc->TextOut((x_origin+x_length-30),(y_origin+5),display_string);

	if ((fabs(y_min)>99.0)||(fabs(y_min)<0.1)) 
	{
		sprintf(display_string,"%4.3e",y_min);
	}
	else
	{
		sprintf(display_string,"%4.3f",y_min);
	}
	p_dc->TextOut((x_origin-55),(y_origin-10),display_string);

	if ((fabs(y_max)>99.0)||(fabs(y_max)<0.1)) 
	{
		sprintf(display_string,"%4.3e",y_max);
	}
	else
	{
		sprintf(display_string,"%4.3f",y_max);
	}
	p_dc->TextOut((x_origin-55),(y_origin-y_length),display_string);
	
	// Return brush and pen

	p_dc->SelectObject(pOriginalPen);
	p_dc->SelectObject(pOriginalBrush);

	// Select old font again

	p_dc->SelectObject(pOldFont);

	// Draws trace

	print_trace(p_dc,start_int,stop_int,BLACK);

	// Draws cursor

	if (cursor_display==CURSOR_DISPLAY_ON)
	{
		print_cursor(p_dc,active_cursor,active_colour);
		print_cursor(p_dc,inactive_cursor,inactive_colour);
	}

	if (text_display==TEXT_DISPLAY_ON)
	{
		print_cursor_labels(p_dc);
	}
}

void Plot::print_trace(CMetaFileDC* p_dc, int start_int, int stop_int, int trace_colour)
{
	// Code draws trace in specified colour

	// Variables

	int i;

	int step;																// used to skip data points
																			// in long traces

	// Code

	// Select Pen

	CPen* pOriginalPen;
	CPen TracePen;
	TracePen.CreatePen(PS_SOLID,1,return_colour(trace_colour));
	pOriginalPen = p_dc->SelectObject(&TracePen);

	// Move to start of trace

	p_dc->MoveTo((x_origin+(int)((p_record->data[x_channel][start_int]-x_min)*x_scale)),
		(y_origin-(int)((p_record->data[y_channel][start_int]-y_min)*y_scale)));

	// Draw remainder

	// First check to see whether (stop_int-start_int) exceeds max_drawn_points
	// If so, increase step from 1 until (stop_int-start_int)/step is less than max_drawn_points
	
	step=1;

	if ((stop_int-start_int)>max_drawn_points)
	{
		do
		{
			step++;
		}
		while (((float)(stop_int-start_int)/(float)step)>max_drawn_points);
	}

	// Draw trace
		
	for (i=start_int+1;i<=stop_int;i=i+step)
	{
		p_dc->LineTo((x_origin+(int)((p_record->data[x_channel][i]-x_min)*x_scale)),
			(y_origin-(int)((p_record->data[y_channel][i]-y_min)*y_scale)));
	}

	// Warn by screen prompt if step>1

	if (step>1)
	{
		// Create font

		p_dc->SetBkMode(TRANSPARENT);
		CFont display_font;
		display_font.CreateFont(12,0,0,0,400,FALSE,FALSE,0,ANSI_CHARSET,
			OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
			DEFAULT_PITCH|FF_SWISS,"Arial");
		CFont* pOldFont = p_dc->SelectObject(&display_font);

		// Add warning to plot

		p_dc->TextOut((x_origin+x_length-55),(y_origin-12),"Speed mode");	

		// Select old font again

		p_dc->SelectObject(pOldFont);
	}

	// Return the pen

	p_dc->SelectObject(pOriginalPen);
}

void Plot::print_cursor(CMetaFileDC* p_dc,int cursor,int colour)
{
	// Prints selected cursor to the metafile

	// Variables

	int size=5;																// size of cross-hair (in pixels)

	// Code

	// Create Cursor Pen
	
	CPen* pOriginalPen;
	CPen CursorPen;

	switch (colour)
	{
		case RED:															// Red Pen
		{
			CursorPen.CreatePen(PS_SOLID,1,RGB(255,0,0));
		}
		break;

		case GREEN:
		{
			CursorPen.CreatePen(PS_SOLID,1,RGB(0,255,0));
		}
		break;

		case BLUE:
		{
			CursorPen.CreatePen(PS_SOLID,1,RGB(0,0,255));
		}
		break;

		case GREY:
		{
			CursorPen.CreatePen(PS_SOLID,1,RGB(192,192,192));
		}
		break;
	}

	pOriginalPen = p_dc->SelectObject(&CursorPen);

	// Draw Cross-hair

	p_dc->MoveTo((x_origin+(int)((p_record->data[x_channel][cursor_point[cursor]]-x_min)*x_scale)-size),
		(y_origin-(int)((p_record->data[y_channel][cursor_point[cursor]]-y_min)*y_scale)));
	p_dc->LineTo((x_origin+(int)((p_record->data[x_channel][cursor_point[cursor]]-x_min)*x_scale)+size),
		(y_origin-(int)((p_record->data[y_channel][cursor_point[cursor]]-y_min)*y_scale)));

	p_dc->MoveTo((x_origin+(int)((p_record->data[x_channel][cursor_point[cursor]]-x_min)*x_scale)),
		(y_origin-(int)((p_record->data[y_channel][cursor_point[cursor]]-y_min)*y_scale)-size));
	p_dc->LineTo((x_origin+(int)((p_record->data[x_channel][cursor_point[cursor]]-x_min)*x_scale)),
		(y_origin-(int)((p_record->data[y_channel][cursor_point[cursor]]-y_min)*y_scale)+size));

	// Return original pen

	p_dc->SelectObject(pOriginalPen);

	// Set old_cursor_point

	old_cursor_point[cursor]=cursor_point[cursor];
}

void Plot::print_cursor_labels(CMetaFileDC* p_dc)
{
	// Code prints cursor labels to metafile

	// Variables

	int x_text_anchor=(x_origin+x_length+15);
	int y_text_anchor(y_origin-(int)(1.0*y_length));
	int x_increment=35;
	int y_increment=10;

	int int_value;														// current output
	float float_value;													// current output

	char display_string[100];

	// Create font

	p_dc->SetBkMode(TRANSPARENT);
	CFont display_font;
	display_font.CreateFont(12,0,0,0,400,FALSE,FALSE,0,ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_SWISS,"Arial");
	CFont* pOldFont = p_dc->SelectObject(&display_font);

	// Labels

	sprintf(display_string,"X");
	p_dc->TextOut((x_text_anchor+x_increment),y_text_anchor,display_string);

	// Left cursor

	int_value=IMIN(cursor_point[active_cursor],cursor_point[inactive_cursor]);
	sprintf(display_string,"[%i]",int_value);
	p_dc->TextOut(x_text_anchor,(y_text_anchor+y_increment),display_string);

	float_value=p_record->data[x_channel][int_value];
	if ((fabs(float_value)>99.0)||(fabs(float_value)<0.1))
	{
		sprintf(display_string,"%4.3e",float_value);
	}
	else
	{
		sprintf(display_string,"%5.4f",float_value);
	}
	p_dc->TextOut((x_text_anchor+x_increment),(y_text_anchor+y_increment),display_string);

	// Right cursor

	int_value=IMAX(cursor_point[active_cursor],cursor_point[inactive_cursor]);
	sprintf(display_string,"[%i]",int_value);
	p_dc->TextOut(x_text_anchor,(y_text_anchor+(2*y_increment)),display_string);

	float_value=p_record->data[x_channel][int_value];
	if ((fabs(float_value)>99.0)||(fabs(float_value)<0.1))
	{
		sprintf(display_string,"%4.3e",float_value);
	}
	else
	{
		sprintf(display_string,"%5.4f",float_value);
	}
	p_dc->TextOut((x_text_anchor+x_increment),(y_text_anchor+(2*y_increment)),display_string);

	// Difference

	int_value=IMAX(cursor_point[active_cursor],cursor_point[inactive_cursor])-
		IMIN(cursor_point[active_cursor],cursor_point[inactive_cursor]);
	sprintf(display_string,"Diff");
	p_dc->TextOut(x_text_anchor,(y_text_anchor+(3*y_increment)),display_string);

	float_value=p_record->data[x_channel][IMAX(cursor_point[active_cursor],cursor_point[inactive_cursor])]-
		p_record->data[x_channel][IMIN(cursor_point[active_cursor],cursor_point[inactive_cursor])];
	if ((fabs(float_value)>99.0)||(fabs(float_value)<0.1))
	{
		sprintf(display_string,"%4.3e",float_value);
	}
	else
	{
		sprintf(display_string,"%5.4f",float_value);
	}
	p_dc->TextOut((x_text_anchor+x_increment),(y_text_anchor+(3*y_increment)),display_string);

	// Labels

	sprintf(display_string,"Y");
	p_dc->TextOut((x_text_anchor+x_increment),(y_text_anchor+(5*y_increment)),display_string);

	// Left cursor

	int_value=IMIN(cursor_point[active_cursor],cursor_point[inactive_cursor]);
	sprintf(display_string,"[%i]",int_value);
	p_dc->TextOut(x_text_anchor,(y_text_anchor+(6*y_increment)),display_string);

	float_value=p_record->data[y_channel][int_value];
	if ((fabs(float_value)>99.0)||(fabs(float_value)<0.1))
	{
		sprintf(display_string,"%4.3e",float_value);
	}
	else
	{
		sprintf(display_string,"%4.3f",float_value);
	}
	p_dc->TextOut((x_text_anchor+x_increment),(y_text_anchor+(6*y_increment)),display_string);

	// Right cursor

	int_value=IMAX(cursor_point[active_cursor],cursor_point[inactive_cursor]);
	sprintf(display_string,"[%i]",int_value);
	p_dc->TextOut(x_text_anchor,(y_text_anchor+(7*y_increment)),display_string);

	float_value=p_record->data[y_channel][int_value];
	if ((fabs(float_value)>99.0)||(fabs(float_value)<0.1))
	{
		sprintf(display_string,"%4.3e",float_value);
	}
	else
	{
		sprintf(display_string,"%4.3f",float_value);
	}
	p_dc->TextOut((x_text_anchor+x_increment),(y_text_anchor+(7*y_increment)),display_string);

	// Difference

	int_value=IMAX(cursor_point[active_cursor],cursor_point[inactive_cursor])-
		IMIN(cursor_point[active_cursor],cursor_point[inactive_cursor]);
	sprintf(display_string,"Diff");
	p_dc->TextOut(x_text_anchor,(y_text_anchor+(8*y_increment)),display_string);

	float_value=p_record->data[y_channel][IMAX(cursor_point[active_cursor],cursor_point[inactive_cursor])]-
		p_record->data[y_channel][IMIN(cursor_point[active_cursor],cursor_point[inactive_cursor])];
	if ((fabs(float_value)>99.0)||(fabs(float_value)<0.1))
	{
		sprintf(display_string,"%4.3e",float_value);
	}
	else
	{
		sprintf(display_string,"%4.3f",float_value);
	}
	p_dc->TextOut((x_text_anchor+x_increment),(y_text_anchor+(8*y_increment)),display_string);

	// Select old font again

	p_dc->SelectObject(pOldFont);
}

void Plot::print_fit(CMetaFileDC* p_dc,Fit_parameters* p_fit_parameters, int colour)
{
	// Draws fit line to metafile - based on draw_fit with slightly differences
	// also adds text to the relevant plot with parameter values

	// Variables

	int i;

	int first_drawn_fit_point;
	int last_drawn_fit_point;

	int x_pos;
	int y_pos;

	char display_string[200];
	char temp_string[200];

	// Code

	// Sets first_drawn_fit_point

	if ((p_fit_parameters->first_point)>first_drawn_point)
		first_drawn_fit_point=1;
	else
		first_drawn_fit_point=first_drawn_point-(p_fit_parameters->first_point)+1;

	// Sets last_drawn_fit_point

	if (p_fit_parameters->last_point>last_drawn_point)
	{
		last_drawn_fit_point=(last_drawn_point-p_fit_parameters->first_point)+1;
	}
	else
	{
		last_drawn_fit_point=(p_fit_parameters->last_point-p_fit_parameters->first_point)+1;
	}

	// Draws original trace in yellow

	CPen* pOriginalPen;
	CPen TracePen;
	TracePen.CreatePen(PS_SOLID,1,RGB(255,255,0));
	pOriginalPen = p_dc->SelectObject(&TracePen);
	
	p_dc->MoveTo((x_origin+(int)((p_fit_parameters->x_offset+p_fit_parameters->x_data[first_drawn_fit_point]-x_min)*x_scale)),
		(y_origin-(int)((p_fit_parameters->y_offset+p_fit_parameters->y_data[first_drawn_fit_point]-y_min)*y_scale)));
	
	for (i=first_drawn_fit_point+1;i<=last_drawn_fit_point;i++)
	{
		p_dc->LineTo((x_origin+(int)((p_fit_parameters->x_offset+p_fit_parameters->x_data[i]-x_min)*x_scale)),
			(y_origin-(int)((p_fit_parameters->y_offset+p_fit_parameters->y_data[i]-y_min)*y_scale)));
	}

	p_dc->SelectObject(pOriginalPen);

	// Draw fit in selected color

	CPen FitPen;
	FitPen.CreatePen(PS_SOLID,1,return_colour(colour));
	pOriginalPen = p_dc->SelectObject(&FitPen);

	p_dc->MoveTo((x_origin+(int)((p_fit_parameters->x_offset+p_fit_parameters->x_data[first_drawn_fit_point]-x_min)*x_scale)),
		(y_origin-(int)((p_fit_parameters->y_offset+p_fit_parameters->y_fit[first_drawn_fit_point]-y_min)*y_scale)));
	
	for (i=first_drawn_fit_point+1;i<=last_drawn_fit_point;i++)
	{
		p_dc->LineTo((x_origin+(int)((p_fit_parameters->x_offset+p_fit_parameters->x_data[i]-x_min)*x_scale)),
			(y_origin-(int)((p_fit_parameters->y_offset+p_fit_parameters->y_fit[i]-y_min)*y_scale)));
	}

	p_dc->SelectObject(pOriginalPen);

	// Redraw cursors

	print_cursor(p_dc,active_cursor,active_colour);
	print_cursor(p_dc,inactive_cursor,inactive_colour);

	// Draw text

	// Create Font

	p_dc->SetBkMode(TRANSPARENT);
	CFont display_font;
	display_font.CreateFont(8,0,0,0,400,FALSE,FALSE,0,ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_SWISS,"Arial");
	CFont* pOldFont = p_dc->SelectObject(&display_font);

	// Text positions

	x_pos=x_origin+5;
	y_pos=y_origin-y_length;

	// Function type

	if (p_fit_parameters->function_type=LINEAR_REGRESSION)
		sprintf(display_string,"Linear Regression");
	if (p_fit_parameters->function_type=SINGLE_EXPONENTIAL)
		sprintf(display_string,"Single Exponential");
	if (p_fit_parameters->function_type=DOUBLE_EXPONENTIAL)
		sprintf(display_string,"Double Exponential");

	p_dc->TextOut(x_pos,y_pos,display_string);
	y_pos=y_pos+10;

	// Parameters
	
	sprintf(display_string,"p[0]: %f  ",p_fit_parameters->parameters[0]);
	for (i=1;i<=p_fit_parameters->no_of_fit_parameters-1;i++)
	{
		sprintf(temp_string,"p[%i]: %f  ",i,p_fit_parameters->parameters[i]);
		strcat(display_string,temp_string);
	}
	p_dc->TextOut(x_pos,y_pos,display_string);
	y_pos=y_pos+10;

	// R_squared

	sprintf(display_string,"R_squared: %f",p_fit_parameters->r_squared);
	p_dc->TextOut(x_pos,y_pos,display_string);

	// Select old font again

	p_dc->SelectObject(pOldFont);
}

COLORREF Plot::return_colour(int colour)
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
