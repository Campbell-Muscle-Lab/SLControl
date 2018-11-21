// global_functions.h

int read_apparatus_calibration_parameters(void);

float convert_FL_COMMAND_microns_to_volts(float microns);

float convert_FL_RESPONSE_volts_to_microns(float volts);

int convert_volts_to_DAPL_units(float volts);

float convert_DAPL_units_to_volts(int units);

void generate_waveform(int function, float* array, int start_int, int points, float period, float amplitude);
						// Code fills array from start_int to stop_int with a waveform of type function,
						// repeating every period array points and with set amplitude
						// function (an integer) defines the waveform type
						// SINE_WAVE 0, COS_WAVE 1, TRIANGLE 2, SQUARE_WAVE 3, CONSTANT 4

void write_pipe_to_file(FILE* file_pointer, char* pipe_name, float * array, int data_points);
						// Code writes array to a file as a series of DAPL fill commands

void write_pipe_segment_to_file(FILE* file_pointer, char* pipe_name, float * array, int start_point, int stop_point);
						// Code writes array from start_point to stop_point to a file as a series of
						// DAPL fill commands

void set_executable_directory_string(char* module_string);
						// Code sets module_string to the directory path of the executable

int nearest_integer(float float_value);
						// Code returns nearest integer

int test_file_exists(CString file_name_string);
						// Code returns 1 if the file exists, zero otherwise

//UINT ThreadFunction(LPVOID pParam);
