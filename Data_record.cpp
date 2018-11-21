// data_record.cpp: implementation of the data_record class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "slcontrol.h"
#include "data_record.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

////////////////////////////
// Ken's code starts here //
////////////////////////////

#include <math.h>
#include "string.h"
#include "global_definitions.h"
#include "global_functions.h"

////////////////////////////
//  Ken's code ends here  //
////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Data_record::Data_record(void)
{
	// Default constructor

	int i;

	no_of_points=50;

	data = new float * [NO_OF_CHANNELS];

	for (i=0;i<NO_OF_CHANNELS;i++)
		data[i] = new float[MAX_DATA_POINTS];

	float temp_array[51];

	::generate_waveform(TRIANGLE,temp_array,1,50,20,2);
	for (i=1;i<=no_of_points;i++)
	{
		data[FORCE][i]=temp_array[i];
	}

	::generate_waveform(SINE_WAVE,temp_array,1,50,20,2);
	for (i=1;i<=no_of_points;i++)
	{
		data[SL][i]=temp_array[i];
	}

	::generate_waveform(SQUARE_WAVE,temp_array,1,50,20,2);
	for (i=1;i<=no_of_points;i++)
	{
		data[FL][i]=temp_array[i];
	}

	for (i=1;i<=no_of_points;i++)
	{
		data[TIME][i]=(float)i;
		data[INTENSITY][i]=0.0;
	}

	// Record details

	default_float_value=99.0;
	default_integer_value=99;
	sprintf(default_string,"default_string");

	sprintf(file_name_string,"Default file name");

	year=99;
	month=99;
	day=99;

	minute=99;
	hour=99;
	second=99;

	record_force_calibration=default_float_value;
	record_fl_command_calibration=default_float_value;
	record_fl_response_calibration=default_float_value;
	record_fl_polarity=default_float_value;
	record_sl_volts_to_fl_command_volts_calibration=default_float_value;
	
	record_fl=default_float_value;
	record_sl=default_float_value;
	record_area=default_float_value;

	record_sampling_rate=1.0;

	sprintf(record_experiment_mode_string,"Default_string");
	record_pre_servo_ms=default_float_value;
	record_servo_ms=default_float_value;
	record_triangle_size_volts=default_float_value;
	record_post_triangle_ms=default_float_value;
	record_inter_pull_interval_ms=default_float_value;
	record_pre_tension_servo_points=99;
	record_ramp_increment_mode=99;

	record_ktr_initiation_time_ms=default_float_value;
	record_ktr_duration_ms=default_float_value;

	// Cyclical specific parameters

	record_pre_length_ms=default_float_value;
	record_length_duration_ms=default_float_value;
	record_length_size_microns=default_float_value;
	record_length_period_ms=default_float_value;
	record_length_phase_deg=default_float_value;
}

Data_record::~Data_record()
{
	int i;
	
	for (i=0;i<NO_OF_CHANNELS;i++)
		delete [] data[i];

	delete [] data;

}

float Data_record::extract_mean_value(int channel,int start_int,int stop_int)
{
	// Code calculates mean value of channel between
	// start_int and stop_int data points (inclusive)

	// Variables

	int i;											// counter

	float no_of_points=((float)stop_int-(float)start_int+(float)1.0);
													// float of number of points
													// in calculation

	float holder=0.0;								// holder used for averaging

	float mean;										// average value

	// Code

	for (i=start_int;i<=stop_int;i++)
	{
		holder=holder+data[channel][i];
	}

	mean=(holder/no_of_points);

	// Return value

	return mean;
}

int Data_record::return_no_of_points()
{
	return no_of_points;
}

void Data_record::transform_to_calibrated_values(void)
{
	// Code converts from raw voltages traces to calibrated values

	// Variables

	float initial_FL_volts=data[FL][1];								// first points
	float initial_SL_volts=data[SL][1];								// first points

	float fractional_fl_change;										// intermediate calculation variable

	// Code

	for (int i=1;i<=no_of_points;i++)
	{
		// Convert force from V to N per m^2

		data[FORCE][i]=data[FORCE][i]*record_force_calibration/record_area;

		// Convert FL from V to m, offsetting from first point

		data[FL][i]=(((data[FL][i]-initial_FL_volts)*record_fl_polarity*
				record_fl_response_calibration)+record_fl)*
			(float)1.0e-6;

		// Convert SL from V to m, offseting from first point

		fractional_fl_change=(data[SL][i]-initial_SL_volts)*record_fl_command_calibration*record_fl_polarity/
								(record_fl*record_sl_volts_to_fl_command_volts_calibration);

		data[SL][i]=(((float)1.0+fractional_fl_change)*record_sl*(float)1.0e-9);
	}
}

int Data_record::read_data_from_file(CString data_file_string)
{
	// Code reads data from desired filename
	// Returns TRUE if successful, FALSE otherwise

	// Variables

	char temp_string[50];												// temp string used for scanning
																		// data file header

	// Code

	// Copy data_file_string passed as a parameter to the function to the data_record

	sprintf(file_name_string,"%s",data_file_string);

	// File handling - try to open the file

	FILE* data_file=fopen(data_file_string,"r");

	// Error checking

	if (data_file==NULL)
	{
		return(FALSE);													// Returns FALSE if file cannot be
	}																	// opened

	// File is open - now extract information from file

	// First check there is a data tag

	rewind(data_file);

	while ((strcmp("Data",temp_string)!=0)&&(!feof(data_file)))
	{
		fscanf(data_file,"%s",&temp_string);
	}
	if (feof(data_file))
	{
		// Return with a value 2 indicating that no data tag has been found

		return 2;
	}

	// Version number
	
 	extract_string_data(data_file,"_VERSION_",version_string);

	// No of input channels

	record_no_of_input_channels=extract_integer_data(data_file,"No_of_input_channels:");
	if (record_no_of_input_channels==default_integer_value)
	{
		// Cope with default condition
		record_no_of_input_channels=4;
	}

	// Time

	extract_record_time(data_file);

	// Calibrations

	record_fl_command_calibration=extract_float_data(data_file,"FL_COMMAND_CALIBRATION:");
	record_fl_response_calibration=extract_float_data(data_file,"FL_RESPONSE_CALIBRATION:");
	record_force_calibration=extract_float_data(data_file,"FORCE_CALIBRATION:");
	record_sl_volts_to_fl_command_volts_calibration=extract_float_data(data_file,
		"SL_volts_to_FL_COMMAND_volts:");
	record_fl_polarity=extract_float_data(data_file,"FL_POLARITY:");

		// Corrects for data files prior to Ver 3.1 which did not include FL_POLARITY data

		if ((int)record_fl_polarity==(int)default_integer_value)
			record_fl_polarity=-1.0;

		// Corrects for inconsistent data files prior to Ver 3.0.1,
		// Integer comparison to prevent truncation error problems with float comparison

		if ((int)record_sl_volts_to_fl_command_volts_calibration==(int)default_float_value)
		{
			record_sl_volts_to_fl_command_volts_calibration=
				extract_float_data(data_file,"SL_volts_to_FL_command_volts:");
		}

	// Muscle Dimensions

	rewind(data_file);
	record_fl=extract_float_data(data_file,"Muscle_length_(µm):");
	record_sl=extract_float_data(data_file,"Sarcomere_length_(nm):");
	record_area=extract_float_data(data_file,"Area_(m^2):");

	// Look for tag describing the data record type

	sprintf(temp_string,"");

	rewind(data_file);

	while((strcmp("_SL_SREC_",temp_string)*
				strcmp("_TENSION_CONTROL_",temp_string)*
				strcmp("_CYCLICAL_",temp_string)*
				strcmp("_CHIRP_",temp_string)*
				strcmp("_TRIGGERS_",temp_string)*
				strcmp("_LENGTH_CONTROL_",temp_string)*
				strcmp("Data",temp_string))!=0)
	{
		fscanf(data_file,"%s",&temp_string);
	}

	// If we haven't reached "Data", deduce the file type and read appropriate parameters
	
	if (strcmp("Data",temp_string)!=0)
	{
		if (strcmp("_SL_SREC_",temp_string)==0)
		{
			// _SL_SREC_ tag found so read appropriate parameters

			read_SL_SREC_parameters(data_file);
		}
		
		if (strcmp("_TENSION_CONTROL_",temp_string)==0)
		{
			// _TENSION_CONTROL_ tag found so read associated parameters

			read_Tension_control_parameters(data_file);
		}

		if (strcmp("_CYCLICAL_",temp_string)==0)
		{
			// _CYCLICAL_ tag found so read associated parameters

			read_Cyclical_parameters(data_file);
		}

		if (strcmp("_CHIRP_",temp_string)==0)
		{
			// _CHIRP_ tag found so read associated parameters

			read_Chirp_parameters(data_file);
		}

		if (strcmp("_TRIGGERS_",temp_string)==0)
		{
			// _TRIGGERS_ tag found so read associated parameters

			read_Triggers_parameters(data_file);
		}

		if (strcmp("_LENGTH_CONTROL_",temp_string)==0)
		{
			// _LENGTH_CONTROL_ tag found so read associated parameters

			read_Length_control_parameters(data_file);
		}
	}
	else
	{
		// No experiment tag

		return FALSE;
	}

	// Read data into record and set no_of_points

	read_data_record_from_file(data_file);

	// Close file and signal successful completion

	return 1;
}

void Data_record::read_data_record_from_file(FILE* file_pointer)
{
	// Extracts data values from file and stores them in record data[] array

	// Variables

	int i,j;															// counter

	char temp_string[100];												// temp string

	// Code

	// Scan through file until "Data" is read

	rewind(file_pointer);

	while (strcmp("Data",temp_string)!=0)
	{
		fscanf(file_pointer,"%s",&temp_string);
	}
	
	// Read data into record and set no_of_points

	i=1;
	
	while (!feof(file_pointer))
	{
		for (j=TIME;j<=INTENSITY;j++)
		{
			fscanf(file_pointer,"%f",&data[j][i]);
		}
		
		i++;
	}

	no_of_points=(i-2);
}

int Data_record::read_SL_SREC_parameters(FILE* file_pointer)
{
	// Code reads SL_SREC parameters and returns TRUE if successful

	// Variables

	char temp_string[100]="";

	// Code

	// General Experiment Parameters

	extract_string_data(file_pointer,"experiment_mode:",record_experiment_mode_string);

	record_sampling_rate=extract_float_data(file_pointer,"sampling_rate_(Hz):");

		// Correct for wrongly written records prior to Ver 3.0.1

		if ((int)record_sampling_rate==(int)default_float_value)
		{
			record_sampling_rate=extract_float_data(file_pointer,"samping_rate_(Hz):");
		}

	record_proportional_gain=extract_float_data(file_pointer,"proportional_gain:");

	// ktr parameters

	record_ktr_initiation_time_ms=extract_float_data(file_pointer,"ktr_initiation_time_ms:");
	record_ktr_duration_ms=extract_float_data(file_pointer,"ktr_duration_(ms):");
	record_pre_ktr_ms=extract_float_data(file_pointer,"pre_ktr_(ms):");
	record_ktr_step_size_microns=extract_float_data(file_pointer,"ktr_step_(µm):");

	// triangle parameters

	record_no_of_triangles=extract_integer_data(file_pointer,"no_of_triangles:");
	record_triangle_size_microns=extract_float_data(file_pointer,"triangle_size_(µm):");
	record_triangle_halftime_ms=extract_float_data(file_pointer,"triangle_halftime_(ms):");
	record_inter_triangle_interval_ms=extract_float_data(file_pointer,"inter_triangle_interval_(ms):");
	record_relative_first_tri_size=extract_float_data(file_pointer,"relative_first_tri_size:");
	record_pre_triangle_ms=extract_float_data(file_pointer,"pre_triangle_(ms):");
	record_ramp_mode=extract_integer_data(file_pointer,"ramp_mode:");
	record_ramp_increment_mode=extract_integer_data(file_pointer,"ramp_increment_mode:");

	return TRUE;
}

int Data_record::read_Tension_control_parameters(FILE* file_pointer)
{
	// Code reads Tension_control parameters

	// Variables

	char temp_string[100]="";

	// Code

	// General experiment parameters

	extract_string_data(file_pointer,"experiment_mode:",record_experiment_mode_string);

		// Corrects for records written prior to Ver 3.0.1

		if (strcmp(record_experiment_mode_string,default_string)==0)
		{
			extract_string_data(file_pointer,"experiment_mode_string:",record_experiment_mode_string);
		}

	record_sampling_rate=extract_float_data(file_pointer,"sampling_rate_(Hz):");
	record_proportional_gain=extract_float_data(file_pointer,"proportional_gain:");
	record_integral_gain=extract_float_data(file_pointer,"integral_gain:");

	// ktr parameters

	record_ktr_initiation_time_ms=extract_float_data(file_pointer,"ktr_initiation_time_ms:");
	record_ktr_duration_ms=extract_float_data(file_pointer,"ktr_duration_(ms):");
	record_ktr_step_size_microns=extract_float_data(file_pointer,"ktr_step_(µm):");

	// Tension_control parameters	

	record_pre_servo_ms=extract_float_data(file_pointer,"pre_servo_(ms):");
	record_servo_ms=extract_float_data(file_pointer,"servo_(ms):");
	record_inter_pull_interval_ms=extract_float_data(file_pointer,"inter_pull_interval_(ms):");
	record_pre_tension_servo_points=extract_integer_data(file_pointer,"pre_tension_servo_points:");
	record_isotonic_hold_volts=extract_float_data(file_pointer,"isotonic_hold_(V):");
	
	// Triangle parameters

	record_no_of_triangles=extract_integer_data(file_pointer,"no_of_triangles:");
	record_triangle_size_volts=extract_float_data(file_pointer,"triangle_size_(V):");
	record_triangle_halftime_ms=extract_float_data(file_pointer,"triangle_halftime_(ms):");
	record_pre_triangle_ms=extract_float_data(file_pointer,"pre_triangle_(ms):");
	record_post_triangle_ms=extract_float_data(file_pointer,"post_triangle_(ms):");
	record_ramp_mode=extract_integer_data(file_pointer,"ramp_mode:");
	record_ramp_increment_mode=extract_integer_data(file_pointer,"ramp_increment_mode:");

	return TRUE;
}

int Data_record::read_Cyclical_parameters(FILE* file_pointer)
{
	// Code reads Tension_control parameters

	// Variables

	char temp_string[100]="";

	// Code

	// General experiment parameters

	record_sampling_rate=extract_float_data(file_pointer,"sampling_rate_(Hz):");

	// ktr parameters

	record_ktr_initiation_time_ms=extract_float_data(file_pointer,"ktr_initiation_time_(ms):");
	record_ktr_duration_ms=extract_float_data(file_pointer,"ktr_duration_(ms):");
	record_ktr_step_size_microns=extract_float_data(file_pointer,"ktr_step_(µm):");

	// Cyclical parameters

	record_pre_length_ms=extract_float_data(file_pointer,"pre_length_(ms):");
	record_length_duration_ms=extract_float_data(file_pointer,"length_duration_(ms):");
	record_length_size_microns=extract_float_data(file_pointer,"length_size_(µm):");
	record_length_period_ms=extract_float_data(file_pointer,"length_period_(ms):");
	record_length_phase_deg=extract_float_data(file_pointer,"length_phase_(deg):");
	record_length_offset_microns=extract_float_data(file_pointer,"length_offset_(µm):");

	return TRUE;
}

int Data_record::read_Chirp_parameters(FILE* file_pointer)
{
	// Code reads Tension_control parameters

	// Variables

	char temp_string[100]="";

	// Code

	// General experiment parameters

	record_sampling_rate=extract_float_data(file_pointer,"sampling_rate_(Hz):");

	// ktr parameters

	record_ktr_initiation_time_ms=extract_float_data(file_pointer,"ktr_initiation_time_(ms):");
	record_ktr_duration_ms=extract_float_data(file_pointer,"ktr_duration_(ms):");
	record_ktr_step_size_microns=extract_float_data(file_pointer,"ktr_step_(µm):");

	// Tension_control parameters

	record_pre_length_ms=extract_float_data(file_pointer,"pre_length_(ms):");
	record_length_duration_ms=extract_float_data(file_pointer,"length_duration_(ms):");
	record_length_size_microns=extract_float_data(file_pointer,"length_size_(µm):");
	record_length_period_ms=extract_float_data(file_pointer,"length_period_(ms):");
	record_length_offset_microns=extract_float_data(file_pointer,"length_offset_(µm):");

	return TRUE;
}

int Data_record::read_Triggers_parameters(FILE* file_pointer)
{
	// Code reads Triggers_parameters

	// Variables

	char temp_string[100]="";
	
	// Code

	// General experiment parameters

	record_sampling_rate=extract_float_data(file_pointer,"sampling_rate_(Hz):");

	// ktr parameters

	record_ktr_initiation_time_ms=extract_float_data(file_pointer,"ktr_initiation_time_(ms):");
	record_ktr_duration_ms=extract_float_data(file_pointer,"ktr_duration_(ms):");
	record_ktr_step_size_microns=extract_float_data(file_pointer,"ktr_step_(µm):");

	return TRUE;
}

int Data_record::read_Length_control_parameters(FILE* file_pointer)
{
	// Code reads Length_control parameters

	// Variables

	char temp_string[100]="";
	
	// Code

	// General experiment parameters

	record_sampling_rate=extract_float_data(file_pointer,"sampling_rate_(Hz):");

	// ktr parameters

	record_ktr_duration_ms=extract_float_data(file_pointer,"ktr_duration_(ms):");
	record_ktr_step_size_microns=extract_float_data(file_pointer,"ktr_step_(µm):");
	record_pre_ktr_ms=extract_float_data(file_pointer,"pre_ktr_(ms):");

	return TRUE;
}

void Data_record::reset_force_to_active_cursor(int active_cursor_point)
{
	// Code resets force relative to active cursor

	// Variables

	float offset_force=data[FORCE][active_cursor_point];

	// Code

	for (int i=1;i<=no_of_points;i++)
	{
		data[FORCE][i]=data[FORCE][i]-offset_force;
	}
}

float Data_record::extract_float_data(FILE* data_file, char tag_string[])
{
	// Code scans data file header for the float value following tag_string.  If the function fails,
	// it returns the default value of 99.0

	// Variables

	float value;

	char temp_string[100]="";

	// Code

	// Return the pointer to the beginning of the file

	rewind(data_file);

	// Scan through the header for the tag

	sprintf(temp_string,"");

	while ((strcmp(tag_string,temp_string)!=0)&&(strcmp("Data",temp_string)!=0))
	{
		fscanf(data_file,"%s",&temp_string);
	}

	if (strcmp("Data",temp_string)!=0)
	{
		// Successfully found tag, return value

		fscanf(data_file,"%f",&value);

		return value;
	}
	else
	{
		// Tag wasn't found, return default_float_value
		
		return default_float_value;
	}
}

int Data_record::extract_integer_data(FILE* file_pointer,char tag_string[])
{
	// Code returns the integer value following tag_string.  If the function fails,
	// it returns default_integer_value

	// Variables

	char temp_string[100]="";

	int return_integer;

	// Code

	// Return the file pointer to the beginning of the file and start searching

	rewind(file_pointer);

	while ((strcmp(tag_string,temp_string)!=0)&&(strcmp("Data",temp_string)!=0))
	{
		fscanf(file_pointer,"%s",&temp_string);
	}

	if (strcmp("Data",temp_string)!=0)
	{
		// Successfully found tag

		fscanf(file_pointer,"%i",&return_integer);
		return return_integer;													// return value
	}
	else
	{
		// Tag not found - return default value

		return default_integer_value;											// return default value
	}
}

void Data_record::extract_string_data(FILE* file_pointer,char tag_string[],char * p_string)
{
	// Code returns the single string following tag_string.  If the function fails,
	// it sets return_string to default_string

	// Variables

	char temp_string[100]="";

	// Code

	// Return the file pointer to the beginning of the file and start searching

	rewind(file_pointer);

	while ((strcmp(tag_string,temp_string)!=0)&&(strcmp("Data",temp_string)!=0)&&(!feof(file_pointer)))
	{
		fscanf(file_pointer,"%s",&temp_string);
	}

	// Check for eof

	if (feof(file_pointer))
	{
		sprintf(p_string,default_string);
		return;
	}

	// If not eof and temp_string~-data we have found the tag

	if (strcmp("Data",temp_string)!=0)
	{
		// Successfully found tag

		fscanf(file_pointer,"%s",&temp_string);
		sprintf(p_string,temp_string);									// set return_string
	}
	else
	{
		// Tag not found - set to default value

		sprintf(p_string,default_string);
	}
}

void Data_record::extract_record_time(FILE* file_pointer)
{
	// Code sets data_record time and data parameters

	// Variables

	char temp_string[100];

	// Code

	rewind(file_pointer);

	while ((strcmp("_RECORD_TIME_",temp_string)!=0)&&(strcmp("Data",temp_string)!=0))
	{
		fscanf(file_pointer,"%s",&temp_string);
	}
	
	if (strcmp("Data",temp_string)!=0)
	{
		// Successful

		fscanf(file_pointer,"%s",&temp_string);
		fscanf(file_pointer,"%i",&day);									// day
		fscanf(file_pointer,"%s",&temp_string);
		fscanf(file_pointer,"%i",&month);								// month
		fscanf(file_pointer,"%s",&temp_string);
		fscanf(file_pointer,"%i",&year);								// year

		fscanf(file_pointer,"%s",&temp_string);
		fscanf(file_pointer,"%i",&hour);								// hour
		fscanf(file_pointer,"%s",&temp_string);
		fscanf(file_pointer,"%i",&minute);								// minute
		fscanf(file_pointer,"%s",&temp_string);
		fscanf(file_pointer,"%i",&second);								// second
	}

	rewind(file_pointer);
}

int Data_record::output_header_to_file(CString data_file_string, FILE* output_file)
{
	// Variables

	unsigned int i;

	char temp_string[100];

	// File handling

	FILE* data_file=fopen(data_file_string,"r");

	// Error checking

	if (data_file==NULL)
	{
		return(FALSE);													// Returns FALSE if file cannot be
	}																	// opened

	// Copy file until Data is reached

	while ((strcmp("Data",temp_string)!=0)&&(!feof(data_file)))
	{
		fscanf(data_file,"%s",&temp_string);

		for (i=0;i<=strlen(temp_string)-1;i++)
		{
			fputc(temp_string[i],output_file);
		}
		fprintf(output_file,"\n");
	}

	fprintf(output_file,"\n");

	// Tidy up

	fclose(data_file);

	return 1;
}







