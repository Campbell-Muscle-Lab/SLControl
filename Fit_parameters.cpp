// Fit_parameters.cpp: implementation of the Fit_parameters class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "slcontrol.h"
#include "Fit_parameters.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

////////////////////////////
// Ken's code starts here //
////////////////////////////

#include <math.h>
#include "NR.h"
#include <float.h>

////////////////////////////
//  Ken's code ends here  //
////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Fit_parameters::Fit_parameters(Plot* set_p_plot,int set_no_of_fit_parameters, int set_function_type,
	int set_first_point, int set_last_point)
{
	// Constructor

	p_plot=set_p_plot;

	no_of_fit_parameters = set_no_of_fit_parameters;
	function_type = set_function_type;

	first_point = set_first_point;
	last_point = set_last_point;

	no_of_fit_points=(last_point-first_point+1);

	parameters[0]=x_offset;
	parameters[1]=y_offset;
	parameters[2]=(float)first_point;
	parameters[3]=(float)last_point;

	x_rogue_points=0;
	y_rogue_points=0;

	r_squared=(float)99.9;
}

void Fit_parameters::set_fit_parameters(Plot* set_p_plot,int set_no_of_fit_parameters, int set_function_type,
	int set_first_point, int set_last_point)
{
	p_plot = set_p_plot;

	no_of_fit_parameters=set_no_of_fit_parameters;
	function_type=set_function_type;

	first_point=set_first_point;
	last_point=set_last_point;

	no_of_fit_points=(last_point-first_point+1);
}
	

Fit_parameters::~Fit_parameters()
{
	// Destructor
}

void Fit_parameters::fill_fit_array(int x_filter, int y_filter)
{
	// Fill fit array with selected data and zero first point with offsets

	// Variables

	int i;

	// Code

	for (i=1;i<=no_of_fit_points;i++)
	{
		x_data[i]=(p_plot->p_record)->data[p_plot->x_channel][first_point+i-1];
		y_data[i]=(p_plot->p_record)->data[p_plot->y_channel][first_point+i-1];
	}

	// Extract offsets

	x_offset=x_data[1];
	y_offset=y_data[1];

	// Zero arrays

	for (i=1;i<=no_of_fit_points;i++)
	{
		x_data[i]=x_data[i]-x_offset;
		y_data[i]=y_data[i]-y_offset;
	}

	// Removes rogue points

	x_rogue_points=0;
	y_rogue_points=0;

	if (x_filter)
	{
		x_rogue_points=remove_rogue_points(x_data);
	}

	if (y_filter)
	{
		y_rogue_points=remove_rogue_points(y_data);
	}
}

int Fit_parameters::remove_rogue_points(float* array)
{
	// Code scans data arrays for rogue points and resets any such points to their local values

	// Variables

	int i,j;													// counters
	int counter=0;

	int half_width=20 ;

	float threshold=4.0;

	float holder;
	
	float* running_average;
	float* running_sd;

	running_average=vector(1,MAX_DATA_POINTS);
	running_sd=vector(1,MAX_DATA_POINTS);

	// Code

	// Calculate running_average

	for (i=1+half_width;i<=(no_of_fit_points-half_width);i++)
	{
		holder=0;

		for (j=(i-half_width);j<=(i+half_width);j++)
		{
			holder=(holder+array[j]);
		}

		running_average[i]=holder/(((float)2.0*(float)half_width)+(float)1.0);
	}

	// Calculate running_sd

	for (i=1+half_width;i<=(no_of_fit_points-half_width);i++)
	{
		holder=0;

		for (j=(i-half_width);j<=(i+half_width);j++)
		{
			holder=(holder+(float)pow(array[j]-running_average[j],(float)2.0));
		}

		running_sd[i]=(float)sqrt(holder/((float)2.0*half_width));
	}

	// Reset array

	for (i=1+half_width;i<=(no_of_fit_points-half_width);i++)
	{
		if (fabs(array[i]-running_average[i])>(threshold*running_sd[i]))
		{
			array[i]=running_average[i];
			counter++;
		}
	}

	// Tidy  up

	free_vector(running_average,1,MAX_DATA_POINTS);
	free_vector(running_sd,1,MAX_DATA_POINTS);

	// Return no of points removed from fit

	return counter;
}

void Fit_parameters::mean_value(void)
{
	// Fits data to a constant, fills y_fit[] and sets parameters[0]

	// Variables

	int i;

	// Variables used in calculation

	float sum_y;
	float mean_y;

	// Code

	sum_y=0;

	for (i=1;i<=no_of_fit_points;i++)
	{
		sum_y=sum_y+y_data[i];
	}

	mean_y=sum_y/(float)no_of_fit_points;

	// Calculate y_fit

	for (i=1;i<=no_of_fit_points;i++)
	{
		y_fit[i]=mean_y;
	}

	// Set parameters

	parameters[0]=mean_y+y_offset;
}

void Fit_parameters::linear_regression(void)
{
	// Fits data to straight line, fills y_fit[] and sets parameters[0], parameters[1] and r_squared

	// Variables

	int i;

	// Variables used in calculation

	float sum_x=0.0;
	float sum_y=0.0;	
	float sum_xsq=0.0;
	float sum_xy=0.0;

	float grad;
	float intercept;

	// Code

	for (i=1;i<=no_of_fit_points;i++)
	{
		sum_x = sum_x+x_data[i];
		sum_y = sum_y+y_data[i];
		sum_xsq = sum_xsq+(x_data[i]*x_data[i]);
		sum_xy = sum_xy+(x_data[i]*y_data[i]);
	}

	grad=((sum_xy-(sum_x*sum_y/(float)no_of_fit_points))/
					(sum_xsq-(sum_x*sum_x/(float)no_of_fit_points)));

	intercept=(((sum_x*sum_xy)-(sum_y*sum_xsq))/
					((sum_x*sum_x)-((float)no_of_fit_points*sum_xsq)));

	// Calculating y_fit

	for (i=1;i<=no_of_fit_points;i++)
	{
		y_fit[i]=intercept+(grad*x_data[i]);
	}

	// Set parameters

	parameters[0]=intercept;
	parameters[1]=grad;
}

void Fit_parameters::robust_linear(void)
{
	// Fits data to straight line, fills y_fit[] and sets parameters[0], parameters[1] and r_squared

	// Variables

	int i;

	// Variables used in calculation

	float intercept;
	float grad;
	float absdev;
	
	// Code

	medfit(x_data,y_data,no_of_fit_points,&intercept,&grad,&absdev);

	// Calculating y_fit

	for (i=1;i<=no_of_fit_points;i++)
	{
		y_fit[i]=intercept+(grad*x_data[i]);
	}

	// Set parameters

	parameters[0]=intercept;
	parameters[1]=grad;
}

int Fit_parameters::single_exponential(void)
{
	int i,k,itst;
	int *ia;

	float *sig,**covar,**alpha;
	float alamda,chisq,ochisq;

	static float a[10];

	ia=ivector(1,no_of_fit_parameters);
	sig=vector(1,no_of_fit_points);
	covar=matrix(1,no_of_fit_parameters,1,no_of_fit_parameters);
	alpha=matrix(1,no_of_fit_parameters,1,no_of_fit_parameters);

	// Fill data arrays

	for (i=1;i<=no_of_fit_points;i++)
	{
		sig[i]=(float)1.0e-9;
	}

	// Fit for all parameters

	for (i=1;i<=no_of_fit_parameters;i++)
	{
		ia[i]=1;
	}

	// Set initial guesses

	// Amplitude
	
	a[1]=y_data[no_of_fit_points]-y_data[1];

	// Rate

	i=1;
	while ((0.5*fabs(a[1])>(fabs(y_data[i]-y_data[1])))&&
		   (i<no_of_fit_points))
	{
		i++;
	}

	a[2]=(float)1.0/x_data[i];

	// Initialise curve-fit

	alamda=-1;
	mrqmin(x_data,y_data,sig,no_of_fit_points,a,ia,no_of_fit_parameters,covar,alpha,&chisq,fexpfit,&alamda);

	k=1;
	itst=0;

	// Fit

	for (;;)
	{
		if ((_isnan(a[1]))||(_isnan(a[2])))
		{
			for (i=1;i<=no_of_fit_points;i++)
			{
				y_fit[i]=y_data[i];
			}

			return 0;	// fit has failed
		}
		
		k++;
		ochisq=chisq;
		mrqmin(x_data,y_data,sig,no_of_fit_points,a,ia,no_of_fit_parameters,covar,alpha,&chisq,fexpfit,&alamda);

		if (chisq>ochisq) itst=0;
		else if (fabs(ochisq-chisq)<0.1) itst++;

		if (itst < 1000) continue;

		alamda=0.0;

		mrqmin(x_data,y_data,sig,no_of_fit_points,a,ia,no_of_fit_parameters,covar,alpha,&chisq,fexpfit,&alamda);

		break;
	}

	// Update parameters

	for (i=0;i<=(no_of_fit_parameters-1);i++)
	{
		parameters[i]=a[i+1];
	}

	// Update y_fit

	for (i=1;i<=no_of_fit_points;i++)
	{
		y_fit[i]=a[1]*((float)1.0-(float)exp(-a[2]*x_data[i]));
	}

	// Free memory

	free_matrix(alpha,1,no_of_fit_parameters,1,no_of_fit_parameters);
	free_matrix(covar,1,no_of_fit_parameters,1,no_of_fit_parameters);
	free_vector(sig,1,no_of_fit_points);
	free_ivector(ia,1,no_of_fit_parameters);

	return(1);															// successful completion
}

int Fit_parameters::double_exponential(void)
{
	int i,k,itst;
	int *ia;

	float *sig,**covar,**alpha;
	float alamda,chisq,ochisq;

	static float a[10];

	ia=ivector(1,no_of_fit_parameters);
	sig=vector(1,no_of_fit_points);
	covar=matrix(1,no_of_fit_parameters,1,no_of_fit_parameters);
	alpha=matrix(1,no_of_fit_parameters,1,no_of_fit_parameters);

	// Fill data arrays

	for (i=1;i<=no_of_fit_points;i++)
	{
		sig[i]=(float)1.0e-9;
	}

	// Fit for all parameters

	for (i=1;i<=no_of_fit_parameters;i++)
	{
		ia[i]=1;
	}

	// Set initial guesses

	// Amplitude
	
	a[1]=(float)0.8*(y_data[no_of_fit_points]-y_data[1]);
	a[3]=(float)0.2*a[1];

	// Rate

	i=1;
	while ((0.5*fabs(a[1])>(fabs(y_data[i]-y_data[1])))&&
		   (i<no_of_fit_points))
	{
		i++;
	}

	a[2]=(float)0.2/x_data[i];
	a[4]=(float)5.0*a[2];

	// Initialise curve-fit

	alamda=-1;
	mrqmin(x_data,y_data,sig,no_of_fit_points,a,ia,no_of_fit_parameters,covar,alpha,&chisq,fdoubleexpfit,&alamda);

	k=1;
	itst=0;

	// Fit

	for (;;)
	{
		if ((_isnan(a[1]))||(_isnan(a[2]))||(_isnan(a[3]))||(_isnan(a[4])))
		{
			for (i=1;i<=no_of_fit_points;i++)
			{
				y_fit[i]=y_data[i];
			}

			return 0;	// fit has failed
		}
		
		k++;
		ochisq=chisq;
		mrqmin(x_data,y_data,sig,no_of_fit_points,a,ia,no_of_fit_parameters,covar,alpha,&chisq,fdoubleexpfit,&alamda);

		if (chisq>ochisq) itst=0;
		else if (fabs(ochisq-chisq)<0.1) itst++;

		if (itst < 1000) continue;

		alamda=0.0;

		mrqmin(x_data,y_data,sig,no_of_fit_points,a,ia,no_of_fit_parameters,covar,alpha,&chisq,fdoubleexpfit,&alamda);

		break;
	}

	// Update parameters

	for (i=0;i<=(no_of_fit_parameters-1);i++)
	{
		parameters[i]=a[i+1];
	}

	// Update y_fit

	for (i=1;i<=no_of_fit_points;i++)
	{
		y_fit[i]=(a[1]*((float)(1.0-exp(-a[2]*x_data[i]))))+
			(a[3]*((float)(1.0-exp(-a[4]*x_data[i]))));
	}

	// Free memory

	free_matrix(alpha,1,no_of_fit_parameters,1,no_of_fit_parameters);
	free_matrix(covar,1,no_of_fit_parameters,1,no_of_fit_parameters);
	free_vector(sig,1,no_of_fit_points);
	free_ivector(ia,1,no_of_fit_parameters);

	return(1);															// successful completion
}

void Fit_parameters::calculate_r_squared(void)
{
	// Code calculates r_squared

	// Variables

	int i;													// counter

	float sum_squares_residuals;
	float sum_squares_mean;
	float sum_y;
	float y_mean;
	
	// Calculating sum_squares,x_mean and y_mean

	sum_squares_residuals=0.0;
	sum_y=0.0;

	for (i=1;i<=no_of_fit_points;i++)
	{
		sum_squares_residuals=sum_squares_residuals+(float)pow(((float)y_data[i]-y_fit[i]),(float)2.0);
		sum_y=sum_y+y_data[i];
	}

	y_mean=(sum_y/(float)no_of_fit_points);

	// Sum squares mean

	sum_squares_mean=0.0;

	for (i=1;i<=no_of_fit_points;i++)
	{
		sum_squares_mean=sum_squares_mean+(float)pow((float)(y_data[i]-y_mean),(float)2.0);
	}

	// Calculating r_squared

	r_squared=(float)1.0-(sum_squares_residuals/sum_squares_mean);
}

int Fit_parameters::two_lines(void)
{
	// Fits data to two straight lines, fills y_fit[] and sets parameters[0], parameters[1]

	// Variables

	int i;
	int mid_point;
	int best_position;

	float cum_error;
	float best_error;

	float grad1;
	float grad2;
	float intercept1;
	float intercept2;

	// Variables used in calculation

	for (mid_point=2;mid_point<=(no_of_fit_points-1);mid_point++)
	{
		cum_error=single_line(&x_data[0],&y_data[0],mid_point,0)+
			single_line(&x_data[mid_point-1],&y_data[mid_point-1],(no_of_fit_points-mid_point+1),2);

		if (mid_point==2)
		{
			best_error=cum_error;
			best_position=mid_point;

			intercept1=parameters[0];
			grad1=parameters[1];
			intercept2=parameters[2];
			grad2=parameters[3];
		}
		
		if (cum_error<best_error)
		{
			best_error=cum_error;
			best_position=mid_point;

			intercept1=parameters[0];
			grad1=parameters[1];
			intercept2=parameters[2];
			grad2=parameters[3];
		}
	}

	// Calculating y_fit

	for (i=1;i<=best_position;i++)
	{
		y_fit[i]=intercept1+(grad1*x_data[i]);
	}

	for (i=best_position;i<=no_of_fit_points;i++)
	{
		y_fit[i]=intercept2+(grad2*x_data[i]);
	}

	// Update parameters

	parameters[0]=(float)best_position;

	parameters[1]=(p_plot->p_record)->data[TIME][best_position] + x_offset - 
		(float)((p_plot->p_record)->record_ktr_initiation_time_ms/1000.0);

	// Tidy up

	// Signal completion

	return 1;
}

float Fit_parameters::single_line(float* x_data, float* y_data, int no_of_points,int offset)
{
	// Fits data to straight line and returns the chi_squared error

	// Variables

	int i;

	// Variables used in calculation

	float sum_x=0.0;
	float sum_y=0.0;	
	float sum_xsq=0.0;
	float sum_xy=0.0;

	float grad;
	float intercept;

	float error;

	// Code

	for (i=1;i<=no_of_points;i++)
	{
		sum_x = sum_x+x_data[i];
		sum_y = sum_y+y_data[i];
		sum_xsq = sum_xsq+(x_data[i]*x_data[i]);
		sum_xy = sum_xy+(x_data[i]*y_data[i]);
	}

	grad=((sum_xy-(sum_x*sum_y/(float)no_of_points))/
					(sum_xsq-(sum_x*sum_x/(float)no_of_points)));

	intercept=(((sum_x*sum_xy)-(sum_y*sum_xsq))/
					((sum_x*sum_x)-((float)no_of_points*sum_xsq)));

	// Calculating y_fit

	for (i=1;i<=no_of_points;i++)
	{
		y_fit[i]=intercept+(grad*x_data[i]);
	}

	// Set parameters

	parameters[0+offset]=intercept;
	parameters[1+offset]=grad;

	// Calculate error

	error=0;
	
	for (i=1;i<=no_of_points;i++)
	{
		error=error+(float)pow((y_fit[i]-y_data[i]),(float)2.0);
	}

	return error;
}
