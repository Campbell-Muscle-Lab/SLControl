// data_record.h: interface for the data_record class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATA_RECORD_H__31B8EB60_9A3E_4E26_B3D1_58E62AFB1306__INCLUDED_)
#define AFX_DATA_RECORD_H__31B8EB60_9A3E_4E26_B3D1_58E62AFB1306__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////
// Ken's code starts here //
////////////////////////////

#include "global_definitions.h"

////////////////////////////
//  Ken's code ends here  //
////////////////////////////

class Data_record  
{
public:
	Data_record(void);										// default constructor
	Data_record(int set_no_of_points);
	virtual ~Data_record();

	// Data

	float ** data;											// pointer to 2D data array

	int no_of_points;
	int record_no_of_input_channels;

	char version_string[100];								// string holding SLControl version number
															// used to record the data file

	char file_name_string[_MAX_PATH];
	int year,month,day;
	int minute,hour,second;

	float record_force_calibration;
	float record_fl_command_calibration;
	float record_fl_response_calibration;
	float record_fl_polarity;
	float record_sl_volts_to_fl_command_volts_calibration;
	
	float record_fl;
	float record_sl;
	float record_area;

	char record_experiment_mode_string[101]; 
	float record_sampling_rate;
	float record_proportional_gain;
	float record_integral_gain;

	float record_ktr_initiation_time_ms;
	float record_ktr_duration_ms;
	float record_ktr_step_size_microns;

	float record_pre_ktr_ms;
	int record_no_of_triangles;
	float record_triangle_size_microns;
	float record_triangle_halftime_ms;
	float record_inter_triangle_interval_ms;
	float record_relative_first_tri_size;
	float record_pre_triangle_ms;
	int record_ramp_mode;

	float record_pre_servo_ms;
	float record_servo_ms;
	float record_isotonic_hold_volts;

	float record_triangle_size_volts;
	float record_post_triangle_ms;
	float record_inter_pull_interval_ms;
	int record_pre_tension_servo_points;
	int record_ramp_increment_mode;

	// Cyclical or Chirp specific parameters

	float record_pre_length_ms;
	float record_length_duration_ms;
	float record_length_size_microns;
	float record_length_period_ms;
	float record_length_phase_deg;
	float record_length_offset_microns;

	float default_float_value;
	int default_integer_value;
	char default_string[100];

	// Functions

	int read_data_from_file(CString data_file_string);						// controls reading data file

	void read_data_record_from_file(FILE* file_pointer);					// reads data arrays

	int read_SL_SREC_parameters(FILE* file_pointer);

	int read_Tension_control_parameters(FILE* file_pointer);

	int read_Cyclical_parameters(FILE* file_pointer);

	int read_Chirp_parameters(FILE* file_pointer);

	int read_Triggers_parameters(FILE* file_pointer);

	int read_Length_control_parameters(FILE* file_pointer);

	float extract_mean_value(int channel,int start_int,int stop_int);

	void transform_to_calibrated_values(void);

	void reset_force_to_active_cursor(int active_cursor_point);

	int return_no_of_points();

	float extract_float_data(FILE* data_file,char tag_string[]);

	int extract_integer_data(FILE* file_pointer,char tag_string[]);

	void extract_string_data(FILE* file_pointer,char tag_string[],char * p_string);

	void extract_record_time(FILE* file_pointer);

	int output_header_to_file(CString data_file_string,FILE* output_file);
};

#endif // !defined(AFX_DATA_RECORD_H__31B8EB60_9A3E_4E26_B3D1_58E62AFB1306__INCLUDED_)
