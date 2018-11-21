// Plot.h: interface for the Plot class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLOT_H__B0B81441_235E_45EC_BD5E_024FA67406B7__INCLUDED_)
#define AFX_PLOT_H__B0B81441_235E_45EC_BD5E_024FA67406B7__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////
// Ken's code starts here //
////////////////////////////

#include <afxwin.h>

#include "Data_record.h"

////////////////////////////
//  Ken's code ends here  //
////////////////////////////

class Fit_parameters;	// forward declaration

class Plot  
{
public:
	Plot();											// default constructor
	
	virtual ~Plot();								// destructor

	Data_record* p_record;
	int x_channel, y_channel;

	int cursor_point[2];
	int old_cursor_point[2];
	int active_cursor,inactive_cursor;
	int active_colour,inactive_colour;

	int x_origin,y_origin,x_length,y_length;		// origin and axes lengths in pixels

	float x_scale,y_scale;
	float x_min,y_min;
	float x_max,y_max;

	int y_axis_label_separation;

	int first_drawn_point;
	int last_drawn_point;

	int auto_scale;
	int cursor_display;
	int text_display;

	int window_width;
	int window_height;

	int max_drawn_points;							// max number of data points drawn to trace

	CString x_axis_title;
	CString y_axis_title;

	CString plot_title;

	// Structures

	struct line_segment								// line segment
	{
		int status;									// 1 if active, 0 otherwise
		float x_start,y_start;						// x and y coordinates
		float x_stop,y_stop;
		
		int colour;									// colour
	};

	line_segment mean_value_line_segments[MAX_LINE_SEGMENTS];
													// an array of line segments

	// Functions

	void initialise_plot(Data_record* set_p_record,
		int set_x_channel, int set_y_channel,
		int set_auto_scale, int set_cursor_display, int set_text_display,
		int set_window_width,int set_window_height,
		float f_x_origin,float f_y_origin, float f_x_length, float f_y_length,
		CString set_x_axis_title, CString set_y_axis_title,
		CString set_plot_title);

	void set_plot_dimensions(int set_window_width,int set_window_height,
		float f_x_origin, float f_y_origin, float f_x_length, float f_y_length);

	COLORREF return_colour(int colour);

	void display_text(CClientDC* p_dc);
	void draw_plot(CClientDC* p_dc, int start_int, int stop_int, int AUTO_SCALING_ON=1);
	void draw_trace(CClientDC* p_dc, int start_int, int stop_int, int trace_colour);
	void draw_line(CClientDC* p_dc,float x_start,float y_start,float x_end,float y_end,int colour);
	void draw_fit(CClientDC* p_dc,Fit_parameters* p_fit_parameters, int colour);
	void draw_line_segment(CClientDC* p_dc, line_segment* p_segment);

	void set_scaling_factors(int start_int, int stop_int);
	void draw_cursor(CClientDC* p_dc,int cursor,int colour);
	void draw_cursor_labels(CClientDC* p_dc);

	void respond_to_key_press(CClientDC* p_display_dc,int key_code,int control_key);
	void update_plot_cursors(int increment,int swap_status);

	void print_plot(CMetaFileDC* p_dc, int start_int, int stop_int);
	void print_trace(CMetaFileDC* p_dc, int start_int, int stop_int, int trace_colour);
	void print_cursor(CMetaFileDC* p_dc, int cursor, int colour);
	void print_cursor_labels(CMetaFileDC* p_dc);
	void print_fit(CMetaFileDC* p_dc,Fit_parameters* p_fit_parameters, int colour);
};

#endif // !defined(AFX_PLOT_H__B0B81441_235E_45EC_BD5E_024FA67406B7__INCLUDED_)
