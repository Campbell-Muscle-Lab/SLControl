// global_definitions.h

#ifndef _GLOBAL_DEFINITIONS_H_
#define _GLOBAL_DEFINITIONS_H_

// MACROS

static int iminarg1,iminarg2;
#define IMIN(a,b) (iminarg1=(a),iminarg2=(b),(iminarg1) < (iminarg2) ?\
	(iminarg1) : (iminarg2))

static int imaxarg1,imaxarg2;
#define IMAX(a,b) (imaxarg1=(a),imaxarg2=(b),(imaxarg1) > (imaxarg2) ?\
	(imaxarg1) : (imaxarg2))

// CONSTANTS

#define NO_OF_CHANNELS 9
#define MAX_DATA_POINTS 501000//65000

#define TIME 0
#define FORCE 1
#define SL 2
#define FL 3
#define INTENSITY 4

#define AUTO_SCALE_ON 1
#define AUTO_SCALE_OFF 0

#define CURSOR_DISPLAY_ON 1
#define CURSOR_DISPLAY_OFF 0

#define TEXT_DISPLAY_ON 1
#define TEXT_DISPLAY_OFF 0

#define FL_CHANNEL_MODE 0
#define SL_CHANNEL_MODE 1

#define ADC_FULL_SCALE 10.0													// Full scale range for ADC and DAC
																			// ie 10.0 is +- 5V limits
#define ADC_MIN (-(ADC_FULL_SCALE/2.0))										// Minimum ADC value
#define ADC_MAX -ADC_MIN													// Maximum ADC value


#define FL_CONTROL 0
#define SL_CONTROL 1

#define PREVAILING_POSITION 0
#define ZERO_VOLTS 1

// SL_SREC dialog menu options

#define SL_SREC_SINGLE_TRIAL 0
#define SL_SREC_DELAY_TRIALS 1
#define SL_SREC_SIZE_TRIALS 2
#define SL_SREC_VELOCITY_TRIALS 3
#define SL_SREC_ALL_TRIALS 4
#define SL_SREC_STEP_TRIALS 5

// Tension dialog menu options

#define SINGLE_HOLD 4
#define FORCE_VELOCITY 5
#define SINGLE_PULLS 6
#define BATCH_PULLS 7
#define BATCH_VELOCITY 8

// Analysis experiment modes

#define SIMPLE_KTR 0
#define STRETCH_LENGTH_CONTROL 1
#define ISOTONIC_HOLD 2
#define STRETCH_TENSION_CONTROL 3

// Analysis options

#define ISOMETRIC_TENSION 0
#define KTR_RATE 1
#define STIFFNESS 2
#define SHORTENING_VELOCITY 3
#define RELATIVE_PRE_STRETCH_TENSION 4
#define PEAK_TENSION 5
#define TAKE_OFF 6

// Colours

#define BLACK 0
#define RED 1
#define GREEN 2
#define BLUE 3
#define GREY 4
#define YELLOW 5
#define CYAN 6
#define MAGENTA 7
#define DARK_GREEN 8
#define WHITE 9

#define NO_SWAP 0
#define SWAP_CURSORS 1

#define SL_NOT_CALIBRATED 0
#define SL_CALIBRATED 1

#define SINE_WAVE 0
#define COS_WAVE 1
#define TRIANGLE 2
#define SQUARE_WAVE 3
#define CONSTANT 4

#define MEAN_VALUE 0
#define LINEAR_REGRESSION 1
#define ROBUST_LINEAR 2
#define SINGLE_EXPONENTIAL 3
#define DOUBLE_EXPONENTIAL 4
#define TWO_LINES 5

#define MAX_NO_OF_PLOTS 6
#define MAX_SUPERPOSED_FILES 12

#define MAX_LINE_SEGMENTS 5

#define LOGARITHMIC_CHIRP 0
#define LINEAR_CHIRP 1

#define SINGLE_SHOT 0
#define TRAIN 1
#define FREEFORM 2

#define LC_MAX_TRIALS 100				// Maximum number of batch trials in a Length Control dialog

#define PI 3.14159

#define FILE_INFO_STRING_LENGTH 5000

#endif
										