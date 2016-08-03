#include "brain.h"

#define width 640
#define height 640

int quit = F_FALSE;
//Event handler
SDL_Event sdl_event;

int main( int argc, char *argv[] )
{
		int n, i, j, k, q, r, c, s;
		int random_val, state_val, ran_val;
		RUN_TIME_TYPE *p; // pointer to neural parameter block
		NEURONE_TYPE *neurone, *neurone_save; // pointer to the network weights, states etc
		MATCH_TYPE *match; // for pattern matching stats
		char parameter_filename[81]; // pointer to inout file
		FILE *input_stream;
		unsigned rseed;
		int batch_flag, num_rands = 0;
		//int local_var,m,e;
		int num_pats;
		int *na;
		int num_active;
		int *active_sites, *states_store;
		int changes = 0, num_noisy;
		char sbuf[81];

		// check the command line args ...
		if ( argc != 2  &&  argc != 3 )
		{
										printf( "\nFatal Error - no input file given ! Syntax is .....\n\n"
																		"brainex <-b> <processing file>\n\n"
																		"-b is an optional run control flag. The processing file name\n"
																		"must be supplied as the last entry, it is not optional!\n\n"
																		"-b will cause the program to run with no interaction from the user,\n"
																		"this is useful when runs are done from batch files.\n" );
										exit(1);
		}

		batch_flag = F_FALSE; // Initialise as Not a batch run

		// handle the command line args here:
		for ( j=1; j<argc; j++ )
		{
										// If this is the last argument then it must be the file to process
										if ( j == (argc-1) )
										{
																		if ( (input_stream = fopen( argv[j], "r" )) == NULL )
																		{
																										printf( "\nFatal Error - could not open input file : %s\n", argv[j] );
																										exit(1);
																		}
																		else // Always copy the good file name and close the file
																		{
																										strcpy( parameter_filename, argv[j] );
																										fclose( input_stream );
																		}
										}
										else if ( strcmp( argv[j], "-b" )==0  ||  strcmp( argv[j], "/b" )==0 )
										{
																		batch_flag = F_TRUE; // Set up for non-interactive running
										}

										else
										{
																		printf( "\nFatal Error - argument %s is not a valid run-time switch !\n",
																										argv[1] );
																		exit(1);
										}

		}

		// Set the random seed for any requested random pattern generation
		rseed = (unsigned)(time(NULL) % 65536L ); // Random seed from time
		sran4( rseed );

		//  If we are here then we must have found the input parameter
		//	file. We must now go ahead and read it

		// Go get the processing parameters
		if ( read_parameter_file( parameter_filename, &p, &neurone, &match, &batch_flag) != F_TRUE)
		{
										printf( "\nFatal Error - problem with parameter file %s !\n", parameter_filename);
										exit(1);
		}


		// Set value of n for later
		n = p->number_of_neurones;

		// Set up some memory that is used to store values temporarily
		if ( (neurone_save=(NEURONE_TYPE *) malloc( sizeof(NEURONE_TYPE) * n )) == NULL)
		{
										printf( "\nFatal Error - rows and columns specify a neural grid that\n"
																		"                exceeds the available system memory (2) !\n" );
										exit(1);
		}

		// If we are here then we must be ready to do some serious execution

		// Only match a pattern if this was asked for
		if ( (p->update_cycles) > 0 )
		{
										// Initialise stats counters
										if ( (p->rtype_flag) == RANDOM_TEST && p->pattern.flag == F_TRUE )
										{
																		match_initialise( p->number_of_patterns+1, match );
										}

										/* Initialize SDL for video output */
										if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
																		fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
																		exit(1);
										}

										/* Create a 640x480 OpenGL screen */
										// sdl 1.2 call
										/*
										   if ( SDL_SetVideoMode(width, height, 0, SDL_OPENGL) == NULL ) {
										   fprintf(stderr, "Unable to create OpenGL screen: %s\n", SDL_GetError());
										   SDL_Quit();
										   exit(2);
										   }
										 */

										screen = SDL_CreateWindow("modulo",
																																				SDL_WINDOWPOS_UNDEFINED,
																																				SDL_WINDOWPOS_UNDEFINED,
																																				width, height,
																																				SDL_WINDOW_OPENGL);

										// Create an OpenGL context associated with the window.
										glcontext = SDL_GL_CreateContext(screen);

										/* Set the title bar in environments that support it */
										//SDL_WM_SetCaption("modulo", NULL);

										/* Loop, drawing and checking events */
										InitGL(width, height);

										// set up graphics mode
										g = (GRAPHICS_TYPE*)malloc(sizeof(GRAPHICS_TYPE));

										g->rows = p->rows;
										g->cols = p->cols;
										g->xmax = width;
										g->ymax = height;
										g->xs = 0;
										g->xe = g->xmax;
										g->ys = 0;
										g->ye = g->ymax;
										g->xw = (g->xe - g->xs) / (g->cols);
										g->yh = (g->ye - g->ys) / (g->rows);


										// Set the default value for brain_iterations to 1
										if( (p->rtype_flag) == NOT_DREAMING)
										{
																		(p->brain_iterations)  = 1;
										}
										else if( (p->noise_level) >= 0)
										{
																		p->pattern.stream=fopen( p->pattern.file, "r+b" );
																		if ( (p->pattern.stream=fopen( p->pattern.file, "r+b" )) == NULL )
																		{
																										printf("\nFatal Error - %s illegal !!!\n", p->pattern.file );
																										return( F_FALSE );
																		}

																		num_pats = get_number_of_patterns( p->pattern.stream );

																		num_noisy = p->brain_iterations;
																		(p->brain_iterations) = num_pats * (p->brain_iterations);
																		//printf("\n\np->brain_iterations = %d\n\n",p->brain_iterations);
																		fseek( p->pattern.stream, 0L, SEEK_SET ); // Set pattern file start

																		// Get rows and columns
																		fread( &r, sizeof(int), 1, p->pattern.stream );
																		fread( &c, sizeof(int), 1, p->pattern.stream );

																		na = ( int * ) malloc( sizeof(int) * n); // Get memory
																		states_store = ( int * ) malloc( sizeof(int) * (p->states)); // Get memory
																		active_sites = (int*)malloc(sizeof(int)*(p->actives));

																		n = r * c; // This is the number of neural elements
																		s = 0;
										}


										for ( i=0; i<(p->brain_iterations); i++ )
										{
																		if((p->rtype_flag) == RANDOM_TEST && p->noise_level < 0)
																		{
																										printf("Entering random testing phase...\n");
																										for(j=0; j<n; j++) // set all module states & tags to zero
																										{
																																		for(k=0; k<(p->states); k++)
																																		{
																																										(neurone+j)->states[k] = 0;
																																										(neurone+j)->tag=0;
																																		}
																										}
																										// Fill in the neural array....
																										//printf("generating an RPAT...\n");
																										num_rands = 0;
																										do
																										{
																																		random_val = (int)ceil((n-1) * ran4());
																																		//printf("random_val = %d\n",random_val);
																																		//printf("num_rands = %d\n",num_rands);

																																		if( (neurone+random_val)->tag != 1 )
																																		{
																																										state_val = (int)ceil((p->states)*ran4());
																																										//printf("state_val = %d\n",state_val);
																																										(neurone+(int)random_val)->states[(int)state_val-1] = 1; // -1 because 0 is the first element
																																										(neurone+(int)random_val)->tag=1;
																																										num_rands++;
																																		}
																										} while(num_rands != (p->actives));

																										/*
																										   printf("\nRPAT random input follows\n");
																										   printf("\n");

																										   for(j=0;j<n;j++)
																										   {
																										   local_var = 0;
																										   for(k=0;k<(p->states);k++)
																										   {
																										   if((neurone+j)->states[k] == 0)
																										   local_var++;
																										   else
																										   {
																										   printf("%d",local_var+1);
																										   break;
																										   }
																										   }
																										   if( local_var == (p->states))
																										   printf("0");
																										   if( (j+1) % (p->cols) == 0)
																										   printf("\n");
																										   }
																										 */

																		}
																		else if((p->rtype_flag) == RANDOM_TEST && p->noise_level >= 0)
																		{
																										printf("Random Test Phase\t\tnoise level:	%d\n",p->noise_level);

																										// read in the s'th pattern from the pattern file
																										// Get in the patterns
																										if( (i) % num_noisy == 0)
																										{
																																		//printf("\n\nReading in a learnt pattern ...\n");
																																		fseek(p->pattern.stream,0L,SEEK_SET); // seems to work with this stuff here
																																		fread( &r, sizeof(int), 1, p->pattern.stream );
																																		fread( &c, sizeof(int), 1, p->pattern.stream );
																																		fseek(p->pattern.stream,(sizeof(int)*(n*s)),SEEK_CUR); // even though this should be the only line needed
																																		fread(na, sizeof(int), n, p->pattern.stream);
																																		s++;

																																		/*
																																		   printf("\n input pattern follows...\n");
																																		   for ( j=0 ; j<n ; j++ )
																																		   {
																																		    // printf the just read in pattern
																																		    printf("%d",*(na+j));
																																		    if( (j+1) % (p->cols) == 0)
																																		   printf("\n");
																																		   }
																																		 */
																										}
																										num_active = 0;
																										for ( j=0; j<n; j++ )
																										{
																																		// set states equal to zero
																																		for(k=0; k<(p->states); k++)
																																		{
																																										(neurone+j)->states[k]=0;
																																										(neurone+j)->tag=0;
																																		}
																																		// determine the current activation
																																		if(*(na+j)!=0)
																																		{
																																										(neurone+j)->states[(*(na+j))-1]=1;
																																										(neurone+j)->tag=1;
																																										active_sites[num_active] = j;
																																										num_active++;
																																										if(num_active > (p->actives))
																																										{
																																																		fprintf(stderr,"Error reading in the learnt patterns for random input\n");
																																																		exit(1);
																																										}
																																		}
																										}

																										/*
																										   printf("\ninput pattern follows\n\n");
																										   for(m=0;m<n;m++)
																										   {
																										   local_var = 0;
																										   for(e=0;e<(p->states);e++)
																										   {
																										   if( ((neurone+m)->states[e]) == 0)
																										   {
																										   ++local_var;
																										   }
																										   else
																										   {
																										   printf("%d",local_var+1);
																										   break;
																										   }
																										   }
																										   if( local_var == (p->states))
																										   printf("0");
																										   if( (m+1) % (p->cols) == 0)
																										   printf("\n");
																										   }
																										 */

																										// pick a site until chosen_sites == p->noise level
																										if ( (p->noise_level) != 0 )
																										{
																																		num_rands = 0;
																																		do
																																		{
																																										random_val = (int)ceil((n-1) * ran4()); // pick a random module
																																										changes = 0;
																																										//printf("randomly chose a module to change...%d \n",random_val);

																																										// store the state vector
																																										for(k=0; k<(p->states); k++)
																																																		states_store[k] = (neurone+(int)random_val)->states[k];

																																										// assign a larger probability to zero
																																										ran_val = (int)ceil((n-1) * ran4());
																																										if(ran_val < (n-(p->actives))) // change to a zero
																																										{
																																																		//printf("inside the change to zero bit\n");
																																																		for(k=0; k<(p->states); k++)
																																																		{
																																																										(neurone+(int)random_val)->states[k] = 0;
																																																										changes += (states_store[k] - (neurone+(int)random_val)->states[k]);
																																																		}
																																																		if(changes != 0)
																																																		{
																																																										num_rands++;
																																																										//printf("changed the site to zero ... num_rands = %d\n",num_rands);
																																																		}
																																																		//printf("\tchose to change the active module to zero...num_rands is %d here\n",num_rands);
																																										}
																																										else // change to nonzero
																																										{
																																																		//printf("inside the change to nonzero bit\n");
																																																		//original code - can cause out of bounds memory access...
																																																		//state_val = (int)ceil((p->states)*ran4());
																																																		state_val = (int)floor((p->states)*ran4());
																																																		//printf("state_val = %d, sizeof states)store  = (p-> states) which is 1 in the letter test case", state_val);
																																																		if(states_store[state_val] != 1) //change
																																																		{
																																																										// zero the state
																																																										for(k=0; k<(p->states); k++)
																																																																		(neurone+(int)random_val)->states[k] = 0;
																																																										// and change to the new state		(there is a better way to do this)
																																																										// original
																																																										//(neurone+(int)random_val)->states[state_val-1] = 1;
																																																										(neurone+(int)random_val)->states[state_val] = 1;
																																																										(neurone+(int)random_val)->tag = 1;
																																																										num_rands++;
																																																										//printf("\tchose to change the module to another state...state_val = %d\tnum_rands is %d here\n",state_val,num_rands);
																																																		}
																																										}
																																										//printf("num_rands = %d\n",num_rands);
																																		} while(num_rands != (p->noise_level));
																										}

																										// printf the noisy pattern

																										/*
																										   printf("\nnoisy pattern follows:\n\n");
																										   for(m=0;m<n;m++)
																										   {
																										   local_var = 0;
																										   for(e=0;e<(p->states);e++)
																										   {
																										   if( ((neurone+m)->states[e]) == 0)
																										   local_var+=1;
																										   else
																										   {
																										   printf("%d",local_var+1);
																										   break;
																										   }
																										   }
																										   if( local_var == (p->states))
																										   printf("0");
																										   if( (m+1) % (p->cols) == 0)
																										   printf("\n");
																										   }
																										 */

																		}
																		// Clear the display window
																		//clearWindow();
																		// opengl printf some stuff - see graphics.c

																		if ( (p->rtype_flag) == DREAMING )
																		{
																										//clearWindow();
																										sprintf(sbuf, "Iteration %d of %d, ",(i+1), p->brain_iterations);
																										//g2_string(window, 10.0,10.0, sbuf);
																		}
																		else if ( (p->rtype_flag) == RANDOM_TEST )
																		{
																										//clearWindow();
																										sprintf(sbuf, "Iteration %d of %d ",(i+1), p->brain_iterations);
																										//g2_string(window, 10.0,10.0, sbuf);
																		}

																		// Perform the update function
																		printf("doing a sync_update on net\n");
																		sync_update_npat( p, neurone ); //updates are always synchronous

																		// Generate some statistics based on the outputs and learnt patterns
																		printf("matching patterns\n");
																		if ( (p->rtype_flag) == RANDOM_TEST && p->pattern.flag == F_TRUE )
																		{
																										match_patterns( neurone, p->pattern.stream, p->number_of_patterns, match, p );
																		}
																		printf("loop complete ... brain iterations = %d\tmax iterations = %d\n",i,(p->brain_iterations));
										} // End of the brain iterations loop


										// No interaction in batch mode
										if ( batch_flag == F_FALSE )
										{
																		fprintf(stdout,">>> Brainmod is finished <<<\n");
										}
		} // End of the optional pattern matching

		// If a pattern file was in use close it now
		if ( p->pattern.flag == F_TRUE ) fclose( p->pattern.stream );


		//  If using a report file then make the final report here,
		//  Then wrap things up by closing the report file and deallocating
		//  memory
		if ( p->report.flag == F_TRUE )
		{
										report_matches( p->report.stream, (p->brain_iterations), (p->number_of_patterns + 1), match );
										free( (void *) match );
										fclose( p->report.stream);
		}

		// If we want to save the weights to a file, do it here
		if ( p->output_weight.flag == F_TRUE )
		{
										p->output_weight.stream=fopen( p->output_weight.file, "wb+" );

										// Make sure of matching dimensions
										fwrite( &(p->rows), sizeof( int ), 1, p->output_weight.stream );
										fwrite( &(p->cols), sizeof( int ), 1, p->output_weight.stream );

										for (j=0; j<n; j++)
										{
																		for(k=0; k<n; k++)
																		{
																										for(q=0; q<(p->states); q++)
																										{
																																		for(r=0; r<(p->states); r++)
																																		{
																																										fwrite(&((neurone+j)->w[k]->weight[q][r]), sizeof(int), 1, p->output_weight.stream);
																																										//printf("j = %d\tk = %d\tweight matrix: weight[%d][%d] = %d\n",j,k,q,r,(neurone+j)->w[k]->weight[q][r]);

																																		}
																										}
																		}
										}
										fclose( p->output_weight.stream );
		} // The output of the weights should now be finished

		// deallocate the weights memory
		for (j=0; j<((p->rows)*(p->cols)); j++)
										free((void *)(neurone+j)->w);
		// deallocate the states memory
		for ( j=0; j < ((p->rows)*(p->cols)); j++ )
										free((void *)(neurone+j)->states);

		free( (void *) neurone ); // Free up allocated neural memory
		free( (void *) neurone_save ); // Free up allocated neural memory
		free( (void *) p );   // Free the parameter memory
		//free(active_sites);
		//free(states_store);		//should free this memory sometime
		//free(na);

		while( !quit ) {
										//Handle events on queue
										while( SDL_PollEvent( &sdl_event ) != 0 ) {
																		if ( sdl_event.type == SDL_QUIT ) {
																										quit = F_TRUE;
																		}
																		if ( sdl_event.type == SDL_KEYDOWN ) {
																										if ( sdl_event.key.keysym.sym == SDLK_ESCAPE ) {
																																		quit = F_TRUE;
																										}
																		}
										}
		}

		SDL_Quit();

		return(0);
}
