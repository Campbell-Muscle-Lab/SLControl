// Fit_parameters.h: interface for the Fit_parameters class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIT_PARAMETERS_H__6198DD5F_29DB_452B_8FD4_C003E29DD81E__INCLUDED_)
#define AFX_FIT_PARAMETERS_H__6198DD5F_29DB_452B_8FD4_C003E29DD81E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////
// Ken's code starts here //
////////////////////////////

#include "Data_record.h"
#include "Plot.h"

////////////////////////////
//  Ken's code ends here  //
////////////////////////////

class Fit_parameters  
{
public:
	Fit_parameters(Plot* set_p_plot=NULL,int set_no_of_fit_parameters=0, int set_function_type=0,
		int set_first_point=1, int set_last_point=1);

	virtual ~Fit_parameters();

	Plot* p_plot;

	Data_record* p_data_record;
	int x_channel;
	int y_channel;

	int no_of_fit_parameters;										// no of fit parameters
	int function_type;												// Linear Regression (0),
																	// Single Exponential (1) or
																	// Double Exponential (2)

	int first_point;												// first and last points for fitting
	int last_point;

	int no_of_fit_points;											// no of data points in fitting routine

	int x_rogue_points;												// number of rogue points in x and y
	int y_rogue_points;												// channels

	float parameters[10];											// fit parameters
	float x_offset;													// x offset
	float y_offset;													// y offset

	float r_squared;												// fit coefficient

	float x_data[MAX_DATA_POINTS];									// x data
	float y_data[MAX_DATA_POINTS];									// y data

	float y_fit[MAX_DATA_POINTS];									// y fit

	// Functions

	void set_fit_parameters(Plot* set_p_plot,int set_no_of_fit_parameters, int set_function_type,
		int set_first_point, int set_last_point);

	void fill_fit_array(int x_filter,int y_filter);					// fills fit arrays, with or without
																	// spike filtering for each channel
																	// depending on the parameter values
						
	int remove_rogue_points(float* array);							// used to remove rogue 'spike'
																	// data points during curve-fitting
																	// returns the number of points removed
	void mean_value(void);
	void linear_regression(void);
	void robust_linear(void);
	int single_exponential(void);
	int double_exponential(void);
	int two_lines(void);
	float single_line(float* x_data, float* y_data, int no_of_points, int offset);

	void calculate_r_squared(void);
};

#endif // !defined(AFX_FIT_PARAMETERS_H__6198DD5F_29DB_452B_8FD4_C003E29DD81E__INCLUDED_)
