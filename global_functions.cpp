// global_functions.cpp

#include "stdafx.h"
#include "global_functions.h"
#include "global_definitions.h"
#include <math.h>
#include <string.h>

// Variables

float FL_POLARITY;													// +1 means fiber length increases with increasing
																	// DAPL integers
																	// -1 means fiber length decreases with increasing
																	// DAPL integers

float FL_RESPONSE_CALIBRATION;										// change in fiber length (microns)
																	// (motor position) for a +1V
																	// change in FL_Response voltage

float FL_COMMAND_CALIBRATION;										// change in fiber length (microns)
																	// (motor position) for a +1V
																	// change in FL_Command voltage

float FORCE_CALIBRATION;											// increase in force (N) for a +1V
																	// increase in Force voltage

char CALIBRATION_DATE_STRING[100];									// date of calibration
																	// (string)

int FL_OR_SL_CHANNEL_MODE=1;										// defines whether only FL and For
																	// or instead FL, SL and For channels
																	// are displayed

char SLCONTROL_VERSION_STRING[100]="4.6.1";							// software version string

int AUTOMATIC_UPDATE_CHECK=1;										// Automatic update check

int NO_OF_COMMAND_LINE_DATA_FILES;									// the number of data files
																	// specified in the command line

CString COMMAND_LINE_DATA_FILE_STRING;								// data file string specified in
																	// command line


// Functions

int read_apparatus_calibration_parameters(void)
{
	// Reads calibration parameters from file "apparatus_calibration_parameters.txt"
	// which must be in the same directory as the executable

	// Variables

	char temp_string[100];
	int check=1;

	char path_string[_MAX_PATH+20];

	// Code

	set_executable_directory_string(path_string);
	strcat(path_string,"\\apparatus_calibration_parameters.txt");

	FILE *calibration_file=fopen(path_string,"r");

	if (calibration_file==NULL)
	{
		return 0;
	}
	else
	{
		// FL_POLARITY

		while ((strcmp("_FL_POLARITY_",temp_string)!=0)&&(!feof(calibration_file)))
		{
			fscanf(calibration_file,"%s",&temp_string);
		}

		if (!feof(calibration_file))
		{
			fscanf(calibration_file,"%f",&FL_POLARITY);
			check=(check*1);
		}
		else
		{
			check=0;
		}

		rewind(calibration_file);

		// FL_RESPONSE_CALIBRATION

		while ((strcmp("_FL_RESPONSE_CALIBRATION_",temp_string)!=0)&&(!feof(calibration_file)))
		{
			fscanf(calibration_file,"%s",&temp_string);
		}

		if (!feof(calibration_file))
		{
			fscanf(calibration_file,"%f",&FL_RESPONSE_CALIBRATION);
			check=(check*1);
		}
		else
		{
			check=0;
		}

		rewind(calibration_file);

		// FL_COMMAND_CALIBRATION

		while ((strcmp("_FL_COMMAND_CALIBRATION_",temp_string)!=0)&&(!feof(calibration_file)))
		{
			fscanf(calibration_file,"%s",&temp_string);
		}

		if (!feof(calibration_file))
		{
			fscanf(calibration_file,"%f",&FL_COMMAND_CALIBRATION);
			check=(check*1);
		}
		else
		{
			check=0;
		}

		rewind(calibration_file);

		// FORCE_CALIBRATION

		while ((strcmp("_FORCE_CALIBRATION_",temp_string)!=0)&&(!feof(calibration_file)))
		{
			fscanf(calibration_file,"%s",&temp_string);
		}

		if (!feof(calibration_file))
		{
			fscanf(calibration_file,"%f",&FORCE_CALIBRATION);
			check=(check*1);
		}
		else
		{
			check=0;
		}

		// CALIBRATION_DATE

		while ((strcmp("_CALIBRATION_DATE_",temp_string)!=0)&&(!feof(calibration_file)))
		{
			fscanf(calibration_file,"%s",&temp_string);
		}

		if (!feof(calibration_file))
		{
			fscanf(calibration_file,"%s",&CALIBRATION_DATE_STRING);
			check=(check*1);
		}
		else
		{
			check=0;
		}

		// FL_OR_SL_CHANNEL_MODE

		while ((strcmp("_FL_OR_SL_CHANNEL_MODE_",temp_string)!=0)&&(!feof(calibration_file)))
		{
			fscanf(calibration_file,"%s",&temp_string);
		}

		if (!feof(calibration_file))
		{
			fscanf(calibration_file,"%i",&FL_OR_SL_CHANNEL_MODE);
			check=(check*1);
		}
		else
		{
			check=0;
		}

		// Automatic update check

		rewind(calibration_file);

		while ((strcmp("_AUTOMATIC_UPDATE_CHECK_",temp_string)!=0)&&(!feof(calibration_file)))
		{
			fscanf(calibration_file,"%s",&temp_string);
		}

		if (!feof(calibration_file))
		{
			fscanf(calibration_file,"%i",&AUTOMATIC_UPDATE_CHECK);
		}
	}

	fclose(calibration_file);

	return check;
}

float convert_FL_COMMAND_microns_to_volts(float microns)
{
	// Takes a floating value corresponding to an increase in FL microns
	// and converts it to the appropriate FL_COMMAND voltage

	return(FL_POLARITY*microns/FL_COMMAND_CALIBRATION);
}

float convert_FL_RESPONSE_volts_to_microns(float volts)
{
	// Takes a floating value corresponding to a voltage
	// and converts it to a displacement in microns

	return(FL_POLARITY*volts*FL_RESPONSE_CALIBRATION);
}


int convert_volts_to_DAPL_units(float volts)
{
	// Takes a floating value of volts and converts it to DAPL units
	// i.e. +5V=32767, -5V=-32768

	int DAPL_units;
	
	// Convert to DAPL units based on +- 5V scale

	DAPL_units=(int)(volts*6553.4);

	// Prevent clipping
	
	if (DAPL_units>32767) DAPL_units=32767;
	if (DAPL_units<-32767) DAPL_units=-32767;

	// Return integer value

	return(DAPL_units);
}

float convert_DAPL_units_to_volts(int units)
{
	// Takes a DAPL unit value i.e. +5V=32767, -5V=-32768
	// and converts it to volts

	float volts;

	// Convert to volts based on a +- 5V scale

	volts=(units*(float)(5.0/32768.0));

	// Return value

	return(volts);
}

void generate_waveform(int function, float* array, int start_int, int stop_int, float period, float amplitude)
{
	// Code fills array from start_int to stop_int with a waveform of type function,
	// repeating every period array points and with set amplitude

	// Variables

	int i;																// counter
		
	float phase;

	// Code

	switch (function)
	{
		case SINE_WAVE:
		{
			for (i=start_int;i<=stop_int;i++)
			{
				array[i]=amplitude*(float)sin((float)(i-start_int)*(float)2.0*PI/(period-(float)1.0));
			}
		}
		break;

		case COS_WAVE:
		{
			for (i=start_int;i<=stop_int;i++)
			{
				array[i]=amplitude*(float)cos((float)(i-start_int)*(float)2.0*PI/(period-(float)1.0));
			}
		}
		break;

		case TRIANGLE:
		{
			for (i=start_int;i<=stop_int;i++)
			{

				phase=((float)fmod((float)(i-start_int),(period-(float)1.0))/(period-(float)1.0));

				if (phase<=0.25)
				{
					array[i]=amplitude*phase*(float)4.0;
				}
				
				if ((phase>0.25)&&(phase<0.75))
				{
					array[i]=amplitude-((float)4.0*(phase-(float)0.25)*amplitude);
				}

				if (phase>=0.75)
				{
					array[i]=-amplitude+((float)4.0*(phase-(float)0.75)*amplitude);
				}
			}
		}
		break;

		case SQUARE_WAVE:
		{
			for (i=start_int;i<=stop_int;i++)
			{
				phase=((float)fmod((float)(i-start_int),period)/period);

				if (phase<=0.5)
				{
					array[i]=amplitude;
				}
				else
				{
					array[i]=-amplitude;
				}
			}
		}
		break;

		case CONSTANT:
		{
			for (i=start_int;i<=stop_int;i++)
			{
				array[i]=amplitude;
			}
		}
		break;
	}
}

void write_pipe_to_file(FILE* file_pointer, char* pipe_name, float * array, int data_points)
{
	// Code writes array to file

	// Variables

	int i,j;																	// counters

	int fill_width=20;															// number of values sent to
																				// DAPL in each fill line

	// Code
		
	for (i=1;i<=data_points;i=(i+fill_width))
	{
		fprintf(file_pointer,"fill %s ",pipe_name);

		for (j=0;j<=(fill_width-1);j++)
		{
			// Last check that data point is in range
			// Corrects specifically for pfl or ptarget being out of range
			// in a ramp/increment scenario, a rare problem in versions prior to 3.2.4

			if (array[i+j]>32767) array[i+j]=32767;
			if (array[i+j]<-32767) array[i+j]=-32767;

			// Write array value to file
	
			fprintf(file_pointer,"%.0f ",array[i+j]);

			if ((i+j)==data_points)												// break out of loop
			{
				i=data_points;
				j=fill_width;
			}
		}

		fprintf(file_pointer,"\n");
	}

	fprintf(file_pointer,"\n");
}

void write_pipe_segment_to_file(FILE* file_pointer, char* pipe_name, float * array, int start_point, int stop_point)
{
	// Code writes array from start_point to stop_point to file

	// Variables

	int i;																		// counters
	
	// Code

	fprintf(file_pointer,"fill %s ",pipe_name);

	for (i=start_point;i<=stop_point;i++)
	{
		fprintf(file_pointer,"%.0f ",array[i]);
	}

	fprintf(file_pointer,"\n");
}

void set_executable_directory_string(char* module_string)
{
	// Function returns string containing the exectuable directory path

	// Variables

	char executable_string[200];

	// Code

	GetModuleFileName(NULL,executable_string,200);

	CString path_string=executable_string;

	sprintf(module_string,path_string.Left(path_string.ReverseFind('\\')));
}

int nearest_integer(float float_value)
{
	// Returns the integer nearest to float_value

	if (fmod(float_value,1)<0.5)
		return (int)floor(float_value);
	else
		return (int)ceil(float_value);
}

int test_file_exists(CString file_name_string)
{
	// Returns 1 if the file exists, zero otherwise

	FILE *test_file=fopen(file_name_string,"r");
	if (test_file==NULL)
		return 0;
	else
	{
		fclose(test_file);
		return 1;
	}
}

	
