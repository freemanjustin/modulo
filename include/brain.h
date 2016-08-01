/* brainex.h - include with brainex .c source. */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <math.h>
#include <time.h>
#include <signal.h>
#include <ctype.h>

#include <SDL2/SDL.h>
#include <OpenGL/gl.h>	// Header File For The OpenGL32 Library
#include <OpenGL/glu.h>	// Header File For The GLu32 Library

#include "nrutil.h"

#define F_TRUE  1
#define F_FALSE 0

#define NOTHING -1
#define NET   0
#define TPAT  1
#define RPAT  2
#define NPAT  3
#define ORDER 4

#define SYNCHRONOUS     1
#define NON_SYNCHRONOUS 2
#define SEQUENTIAL      3

#define MEMORY 0
#define DREAMING     0
#define NOT_DREAMING 1
#define RANDOM_TEST  2

// globals

int	window;	// stores the window id of the X11 window
int	textWindow; // for some informative text messages
SDL_Window *screen;

float	red[256];
float	green[256];
float	blue[256];

//struct WEIGHT_MATRIX
typedef struct	{	int	**weight;	//2d array for the weights
			} WEIGHT_MATRIX ;

typedef struct	{	int *states;	//1d array for the states
			} STATE_VECTOR;

// struct NEURONE_TYPE
typedef struct { 	int id_num;  	// The neurone identity number
					int *states;  	// Activations (vector)
					int nw;  		// The number of weights
					WEIGHT_MATRIX **w;  	// Pointer to nw weights (matrix)
					int largest;  		// used in the update method
					int	tag; 		// used with the random pattern generation
					int	l_squ;
					int l_copy_squ;
	       } NEURONE_TYPE  ;	// change to NODE_TYPE

// struct MATCH_TYPE
typedef struct {	int pos ;  // Incremented for a positive match
                  	int inv ;  // Incremented for a negative match
                  	int pos_con ; // Positive convergence count
                  	int inv_con ; // Inverse convergence count
               } MATCH_TYPE ;

// struct RUN_FILE_TYPE
typedef struct {	char file[41]     ; // The file name
                 	int  flag         ; // Flag indicating file use status
                	FILE *stream      ; // The io stream of the file
               } RUN_FILE_TYPE ;


// Set up a type that holds the run time parameters
typedef struct {
			int rows ;  	// Number of rows
			int cols ;  	// Number of cols
			int states ; 	// Number of states per module
			int actives ; 	// Number of active modules per memory
			int converge_cycles ; // # of cycles till no change
			int num_pats       ;  // Number of pats to learn

			int rtype_flag     ; // The type of run being done, this
				      		 // may be one of
				      		 // DREAMING, NOT_DREAMING or
				      		 // RANDOM_TEST
       		int number_of_neurones ;   // Will be made equal to row*cols
       		int number_of_patterns ;
       		int noise_level ;

			RUN_FILE_TYPE 	input_weight, output_weight,
							pattern, report,
							weight      // Disk or memory weight control
							;
			float match_perc     ; // % to be matched for acceptance
			int brain_iterations ; // Maximum cycle count
			int update_cycles  ; // Number of update cycles
       } RUN_TIME_TYPE   ;

// Define a type that holds graphics information
typedef struct
{
	int ncolour ; /* Number of available colours. */
	int xmax    ; /* Maximum number of pixels in x direction. */
	int ymax    ; /* Maximum number of pixels in y direction. */
	int xs      ; /* xs, xe, ys and ye define the range over */
	int xe      ; /* which the neural display will be made.  */
	int ys      ;
	int ye      ;
	int xw      ; /* Box width in pixels. */
	int yh      ; /* Box height in pixels. */

	int rows    ; /* Number of rows in neural pattern. */
	int cols    ; /* Number of cols in neural pattern. */
	int numtextrows  ; /* Number of text rows. */
} GRAPHICS_TYPE ;

//global graphics type
GRAPHICS_TYPE *g ;

char *lr_pack( char *c )   ;
char *uppercase( char *c ) ;
char *fix_string( char *c ) ;

int gprint( int row, int col, char *str )  ;

int fill_neural_array( NEURONE_TYPE *neurone, char *string, int index ) ;

int update_neural_weights( RUN_TIME_TYPE *p, NEURONE_TYPE *neurone ) ;

void sync_update_npat( RUN_TIME_TYPE *p, NEURONE_TYPE *neurone ) ;

void display_neurone_block( RUN_TIME_TYPE *p, NEURONE_TYPE *neurone, int i, char *sbuf ) ;

void match_initialise( int n, MATCH_TYPE *match ) ;

void match_patterns( NEURONE_TYPE *neurone, FILE *pattern_stream,
		     int number_of_patterns, MATCH_TYPE *match,
		     RUN_TIME_TYPE *p    ) ;

void report_header( FILE *report_stream, RUN_TIME_TYPE *p ) ;

void report_patterns( FILE *report_stream, FILE *pattern_stream ) ;

int get_number_of_patterns( FILE *pattern_stream ) ;

void report_matches( FILE *report_stream, int rt, int n, MATCH_TYPE *match ) ;

void write_pattern_file( FILE *pattern_stream, int n, NEURONE_TYPE *neurone, RUN_TIME_TYPE *p );

void sran4( unsigned rseed ) ;
float ran4( void ) ;

int	square_states(int* states, RUN_TIME_TYPE *p);

int read_parameter_file( char *parameter_filename,
	RUN_TIME_TYPE **p,  NEURONE_TYPE **neurone,
	MATCH_TYPE **match, int *batch_flag ) ;

void drawARect(double x1, double x2, double y1, double y2);

void graphicsPrep();

void clearWindow();


void sort2(unsigned long n, float arr[], float brr[]);

void InitGL(int Width, int Height);
