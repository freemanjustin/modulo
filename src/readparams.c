#include "brain.h"

int read_parameter_file(  char *parameter_filename,
																			RUN_TIME_TYPE **pp,
																			NEURONE_TYPE **n_neurone,
																			MATCH_TYPE **match,
																			int *batch_flag )
{
	char string[241], keyword[11];
	FILE    *input_stream;
	long input_weight_offset;
	RUN_TIME_TYPE  *p;
	NEURONE_TYPE  *neurone;
	int card_type=NOTHING,
					n, j, k, r1, c1,
					tpat_count, npat_count, row_count;
	int q;
	char dflag[2],temp_str[2];
	int state_val;
	int random_val;
	int rtype_flag_temp;
	long fp;
	int num_rands = 0;
	//int				local_var;
	int r;
	int count, temp_act;


	if ( (input_stream = fopen( parameter_filename, "r" )) == NULL )
	{
									printf( "\nFatal Error - could not open input file : %s\n",parameter_filename );
									return( F_FALSE );
	}

	//  Read in the cards. The NET card is the first valid card expected.
	//  This should be followed by the TPAT cards.
	//	Then the single NPAT block should be read.

	// First allocate memory for the parameter block.
	p = (RUN_TIME_TYPE *) malloc( sizeof(RUN_TIME_TYPE) );
	*pp = p; // Set for return

	p->weight.flag = MEMORY; // use memory rather than disk
	p->number_of_patterns = 0;
	p->states = 0;
	p->actives = 0;
	p->brain_iterations = 1;
	p->noise_level = 0;
	p->match_perc       = 100.0F;
	p->report.flag = F_FALSE;

	tpat_count = 0;
	npat_count = 0;
	row_count  = 0;


	while ( fgets( string, 240, input_stream ) != NULL )
	{
									//printf("%s", string ) ;

									// Condition the input string for further processing
									uppercase(lr_pack(fix_string(string)));

									// Take care of the NET card case
									if ( strncmp( "NET", string, 3 ) == 0 )
									{
																	if ( card_type != NOTHING ) // Have already read a NET card
																	{
																									printf( "\nFatal Error - a NET card has already been read. Only\n"
																																	"one NET specification is allowed. This should be the\n"
																																	"first valid card in the file.\n");
																									return( F_FALSE );
																	}
																	else if ( sscanf( string, "%s %d %d %d %d %d %d %s %s %s %s",
																																			keyword,
																																			&(p->rows),
																																			&(p->cols),
																																			&(p->states),
																																			&(p->actives),
																																			&(p->update_cycles),
																																			&(p->num_pats),
																																			dflag,
																																			p->input_weight.file,
																																			p->output_weight.file,
																																			p->pattern.file
																																			) != 11 )
																	{
																									printf("\nFatal Error - NET card did not have enough parameters !\n");
																									return( F_FALSE );
																	}

																	// check to see if we have valid input for the int data types ...
																	if ( (p->rows) < 1 || (p->cols) < 1 || (p->num_pats) < 0 || (p->states) < 0 || (p->actives) < 0 )
																	{
																									printf("\nFatal Error - NET has illegal parameters specified !\n" );
																									return( F_FALSE );
																	}

																	// Set dflag and check input ...
																	if   ( *dflag == 'N' ) (p->rtype_flag) = NOT_DREAMING;
																	else if ( *dflag == 'R' ) (p->rtype_flag) = RANDOM_TEST;
																	else
																	{
																									printf("\nFatal Error - NET run parameters must be N or R !!" );
																									return( F_FALSE );
																	}

																	if ( *(p->input_weight.file) == '-' ) // Set up input weight file flags
																									p->input_weight.flag = F_FALSE;
																	else
																	{
																									// Make sure that the input weight file actually exists.
																									if ( (p->input_weight.stream=fopen( p->input_weight.file, "rb" )) == NULL )

																									{
																																	printf("\nFatal Error - %s was not found !!!\n", p->input_weight.file );
																																	return( F_FALSE );
																									}

																									// Make sure of matching dimensions.
																									fread( &r1, sizeof( int ), 1, p->input_weight.stream );
																									fread( &c1, sizeof( int ), 1, p->input_weight.stream );
																									input_weight_offset = ftell( p->input_weight.stream ); // Save
																									if ( r1 != (p->rows)  ||  c1 != (p->cols) )
																									{
																																	printf(  "\nFatal Error - Weight matrix mismatch...."
																																										"\n  NET image has %d rows and %d cols while"
																																										"\n  weight file %s has %d rows and %d cols."
																																										"\n  rows and columns must match !!!\n",
																																										(p->rows), (p->cols), p->input_weight.file, r1, c1);
																																	return( F_FALSE );
																									}
																									p->input_weight.flag = F_TRUE;
																	}

																	if ( *(p->output_weight.file) == '-' ) // ...output weight file flags
																									p->output_weight.flag = F_FALSE;
																	else
																									p->output_weight.flag = F_TRUE;

																	if ( (p->num_pats) == 0  && p->input_weight.flag == F_FALSE )
																	{
																									printf( "\nFatal Error - You have specified a run with no input"
																																	"\n              weight file and and no patterns to learn"
																																	"\n              from (see parameter 5 of the NET image and"
																																	"\n              your TPAT and RPAT images). Brainex needs"
																																	"\n              to get a connection weight matrix from"
																																	"\n              somewhere you ninkempoop.\n" );
																									return( F_FALSE );
																	}

																	if ( *(p->pattern.file) == '-' ) // Set up for learnt patterns
																	{
																									p->pattern.flag = F_FALSE;
																									if ( (p->rtype_flag) == RANDOM_TEST )
																									{
																																	printf( "\nFatal Error - must have pattern file "
																																									"when random testing !!\n" );
																																	return( F_FALSE );
																									}
																	}
																	else
																	{
																									// Make sure that the input pattern file actually exists. */
																									if ( (p->pattern.stream=fopen( p->pattern.file, "a+b" )) == NULL )
																									{
																																	printf("\nFatal Error - %s illegal !!!\n", p->pattern.file );
																																	return( F_FALSE );
																									}

																									fseek( p->pattern.stream, 0L, SEEK_END ); // Set to end of file
																									fp = ftell( p->pattern.stream ); // Where are we ?

																									if ( fp == 0L  &&  (p->rtype_flag) == RANDOM_TEST )
																									{
																																	printf("\nFatal Error - pattern file empty, no report possible!!\n" );
																																	return( F_FALSE );
																									}
																									else if ( fp == 0L  &&  (p->rtype_flag) != RANDOM_TEST )
																									{
																																	fwrite( &(p->rows), sizeof(int), 1, p->pattern.stream );
																																	fwrite( &(p->cols), sizeof(int), 1, p->pattern.stream );
																									}
																									else if ( fp > 0L )
																									{
																																	fseek( p->pattern.stream, 0L, SEEK_SET ); // Go to start of file
																																	// Make sure of matching dimensions
																																	fread( &r1, sizeof(int), 1, p->pattern.stream );
																																	fread( &c1, sizeof(int), 1, p->pattern.stream );
																																	if ( r1 != (p->rows)  ||  c1 != (p->cols) )
																																	{
																																									printf( "\nFatal Error - Pattern file matrix mismatch...."
																																																	"\n  NET image has %d rows and %d cols while"
																																																	"\n  learnt pattern file %s has %d rows and %d cols."
																																																	"\n  Rows and columns must match !!!\n",
																																																	(p->rows), (p->cols), p->pattern.file, r1, c1 );
																																									return( F_FALSE );
																																	}
																									}
																									p->pattern.flag = F_TRUE;
																	}

																	card_type = NET;

																	// set the number of modules in the network
																	p->number_of_neurones = (p->rows) * (p->cols);
																	n = p->number_of_neurones;

																	// If we are here then we must have found a NET card with valid parameters
																	if ( ( neurone = (NEURONE_TYPE *) malloc( sizeof(NEURONE_TYPE) * n ) ) == NULL )
																	{
																									printf( "\nFatal Error - rows and columns specify a neural grid that\n"
																																	"                exceeds the available system memory.\t:(\n" );
																									return( F_FALSE );
																	}

																	*n_neurone = neurone; // Set for return

																	//printf("\ntrying to malloc some memory for the weight file...\n");

																	// get memory for the weights
																	count = 0;
																	for ( j=0; j<n; j++ )
																	{
																									(neurone+j)->id_num = j;
																									(neurone+j)->nw     = n*n*(p->states)*(p->states);// we have n squared weights
																									// malloc the top level:
																									if ( ((neurone+j)->w = (WEIGHT_MATRIX **)malloc(sizeof(WEIGHT_MATRIX)*n)) == NULL)
																									{
																																	printf( "\n Not enough memory for the weight grid\n");
																																	return( F_FALSE );
																									}
																									else // malloc each matrix
																									{
																																	for(k=0; k<n; k++)
																																	{
																																									// malloc rows:
																																									if ( ((neurone+j)->w[k] = (WEIGHT_MATRIX *)malloc(sizeof(WEIGHT_MATRIX)*n)) == NULL)
																																									{
																																																	return( F_FALSE);
																																									}
																																									if (((neurone+j)->w[k]->weight = (int **) malloc( (p->states) * sizeof(int*))) == NULL )
																																									{
																																																	printf( "\n Not enough memory for the weight grid...\n");
																																																	return( F_FALSE );
																																									}
																																									// and finally malloc cols:
																																									for (q = 0; q<(p->states); q++)
																																									{
																																																	if (((neurone+j)->w[k]->weight[q] = malloc( (p->states) * sizeof(int))) == NULL )
																																																	{
																																																									printf( "\n Not enough memory for the weight grid...\n");
																																																									return( F_FALSE );
																																																	}
																																																	// init elements of the weight matrix to zero
																																																	else
																																																	{
																																																									for(r=0; r<(p->states); r++)
																																																									{
																																																																	(neurone+j)->w[k]->weight[q][r] = 0; //initialise to zero
																																																																	count++;
																																																																	//printf("j = %d\tk = %d\tweight matrix: weight[%d][%d] = %d\n",j,k,q,r,(neurone+j)->w[k]->weight[q][r]);
																																																									}
																																																	}
																																									}
																																	}
																									}
																	}

																	//printf("\nmalloc of weight memory done. did it work?\tcount = %d\tshould be %d\n\n",count,n*n*(p->states)*(p->states));
																	// read in the weights from file if requested to do so
																	if ( p->input_weight.flag == F_TRUE )
																	{
																									count = 0;
																									//printf("\nreading in predefined weights from file %s\n",p->input_weight.file);
																									for(j=0; j<n; j++)
																									{
																																	for(k=0; k<n; k++)
																																	{
																																									for(q=0; q< (p->states); q++)
																																									{
																																																	for(r=0; r< (p->states); r++)
																																																	{
																																																									fread(&((neurone+j)->w[k]->weight[q][r]), sizeof(int),1, p->input_weight.stream);
																																																									count++;
																																																									//printf("j = %d\tk = %d\tweight matrix: weight[%d][%d] = %d\n",j,k,q,r,(neurone+j)->w[k]->weight[q][r]);
																																																	}
																																									}
																																	}
																									}
																									// close the file
																									fclose( p->input_weight.stream );
																									//printf("read %d elements from file %s\tshould have read %d elements!\n",count,p->input_weight.file,n*n*(p->states)*(p->states));
																	}

																	// get memory for states per module
																	if (p->states == 0)
																	{
																									printf("p->states == 0. something bad is about to happen...\n");
																									return(F_FALSE);
																	}
																	else
																	{
																									for ( j=0; j<n; j++ )
																									{
																																	if ( ( (neurone+j)->states = (int *) malloc( sizeof(int)*(p->states) ) ) == NULL )
																																	{
																																									printf( "\nNot enough memory for the state vectors.\n");
																																									return( F_FALSE );
																																	}
																																	else
																																	{
																																									for ( k = 0; k < (p->states); k++ )
																																									{
																																																	(neurone+j)->states[k] = 0; // Initialise to 0
																																									}
																																	}
																									}
																	}
									} // end of the NET case

									// *****************************************************************************************

									// Take care of the TPAT/RPAT card case
									if ( strncmp( "TPAT", string, 4 ) == 0 || strncmp( "RPAT", string, 4 ) == 0     )
									{
																	// Investigate the entry status
																	if ( card_type != RPAT  &&  card_type != TPAT  &&  card_type != NET )
																	{
																									printf(  "\nFatal Error - Only a NET, TPAT or RPAT image may\n"
																																		"precede a TPAT or RPAT image. A NET card must precede\n"
																																		"all TPAT/RPAT blocks. Exactly one NET card must appear\n"
																																		"and it must be the first valid card in the parameter\n"
																																		"specification file.\n" );
																									return( F_FALSE );
																	}
																	else if ( card_type == TPAT  &&  row_count != (p->rows) )
																	{
																									printf( "\nFatal Error - TPAT Pattern block %d should have %d rows,"
																																	"\n              only %d rows were actually counted !\n",
																																	tpat_count, (p->rows), row_count );
																									return( F_FALSE );
																	}

																	if ( strncmp( "TPAT", string, 4)== 0)
																									card_type = TPAT;
																	else if ( strncmp( "RPAT", string, 4)== 0)
																									card_type = RPAT;

																	row_count = 0; // This is used to control image access

																	tpat_count++; // Use this to count both TPAT and RPAT images
																	if ( tpat_count > (p->num_pats) )
																	{
																									printf( "\nFatal Error - Too many TPAT/RPAT definitions. The number\n"
																																	"of TPAT/RPAT definitions must match the NET specification.\n" );
																									return( F_FALSE );
																	}
																	for(j=0; j<n; j++) // zero the states for TPAT reading
																	{
																									for(k=0; k<(p->states); k++)
																									{
																																	(neurone+j)->states[k] = 0; // zero the states
																									}
																									(neurone+j)->tag = 0; // and the tags
																	}
									} // end of the TPAT/RPAT case

									// Take care of the NPAT/RANDIN card case
									if ( strncmp( "NPAT", string, 4 ) == 0 || strncmp( "RANDIN", string, 6 ) == 0)
									{
																	if ( tpat_count != (p->num_pats) )
																	{
																									printf( "\nFatal Error - Not enough TPAT/RPAT sets before NPAT!!\n");
																									return( F_FALSE );
																	}
																	else if ( card_type == TPAT  &&  row_count != (p->rows) )
																	{
																									printf( "\nFatal Error - TPAT Pattern block %d should have %d rows,"
																																	"\n                only %d rows were actually counted !\n",
																																	tpat_count, (p->rows), row_count );
																									return( F_FALSE );
																	}
																	else if ( npat_count > 0 )
																	{
																									printf( "\nFatal Error - Only 1 NPAT/RANDIN block is allowed !!\n");
																									return( F_FALSE );
																	}

																	if ( strncmp( "NPAT", string, 4 ) == 0  )
																	{
																									row_count = 0; // We have to count the rows....
																	}

																	if ( strncmp( "RANDIN", string, 4 ) == 0  )
																	{
																									row_count = (p->rows); // Set up as dummy value

																									if ( sscanf(string, "%s %d %d %f %s",
																																					keyword,
																																					&(p->brain_iterations),&(p->noise_level),
																																					&(p->match_perc),
																																					p->report.file        ) != 5 )
																									{
																																	printf("\nFatal Error - RANDIN card, not enough parameters !\n");
																																	return( F_FALSE );
																									}

																									if ( (p->brain_iterations) < 1 )
																									{
																																	printf( "\nFatal Error - RANDIN parameters illegal - "
																																									"\n              Number of random cycles must be >= 1" );
																																	return( F_FALSE );
																									}

																									// Check on the report file.
																									if ( (p->report.stream=fopen( p->report.file, "a+" )) == NULL )
																									{
																																	printf("\nFatal Error - %s illegal !!!\n", p->report.file );
																																	return( F_FALSE );
																									}
																									p->report.flag = F_TRUE; // Enable

																									// Start the report
																									report_header( p->report.stream, p );

																									p->number_of_patterns = get_number_of_patterns( p->pattern.stream );

																									report_patterns( p->report.stream, p->pattern.stream );

																									// Get memory to be used in report of stats
																									*match = (MATCH_TYPE *) malloc( sizeof(MATCH_TYPE) * ( p->number_of_patterns + 1 ) );
																	}
																	card_type = NPAT;
																	npat_count = 1;
									} // end of the NPAT/RANDIN case


									if ( ( card_type == TPAT || card_type == NPAT) && ( strncmp( "0", string, 1 ) == 0 ||
																																																													strncmp( "1", string, 1 ) == 0 || strncmp( "2", string, 1 ) == 0 ||
																																																													strncmp( "3", string, 1 ) == 0 || strncmp( "4", string, 1 ) == 0 ||
																																																													strncmp( "5", string, 1 ) == 0 || strncmp( "6", string, 1 ) == 0 ||
																																																													strncmp( "7", string, 1 ) == 0 || strncmp( "8", string, 1 ) == 0 ||
																																																													strncmp( "9", string, 1 ) == 0   )  )
									{
																	row_count++;
																	if ( row_count > (p->rows) )
																	{
																									printf( "\nFatal Error - Pattern block must have only %d rows!\n",(p->rows));
																									return( F_FALSE );
																	}

																	if ( (int)strlen( string ) != (p->cols) )
																	{
																									printf( "\nFatal Error - Pattern block bad row %d\n\n"
																																	"There are an incorrect number of columns.\n"
																																	"in the pattern string.\n"
																																	"The number of columns found was : %d\n"
																																	"There should be exactly %d columns.\n",
																																	row_count, (int)strlen( string ), (p->cols) );
																									return( F_FALSE );
																	}

																	if ( (int)strspn( string, "0123456789" ) != (p->cols)  )
																	{
																									printf( "\nFatal Error - Pattern block bad at row %d\n\n"
																																	"There are illegal characters in the pattern string.\n"
																																	"Only integers are allowed in a pattern string.\n",
																																	row_count );
																									return( F_FALSE );
																	}

																	// fill in the neural array with the TPAT/NPAT values...
																	//printf("temp_act: NPAT/TPAT\n");
																	for(k=0; k<(p->cols); k++)
																	{
																									strncpy(temp_str, &string[k], sizeof(char));
																									temp_act = atoi(&temp_str[0]);
																									//printf("%d",temp_act);
																									if(temp_act != 0)
																																	(neurone+((row_count-1)*(p->cols)+k))->states[temp_act-1] = 1; // set the state
																	}
																	//printf("\n");



																	//  If the neural array has been filled with the input
																	//	activation values then we can calculate the connecting
																	//	weights (to this point).

																	if ( card_type == TPAT && row_count == (p->rows) )
																	{
																									rtype_flag_temp   = p->rtype_flag;// Preserve for restoration
																									p->rtype_flag   = NOT_DREAMING;// Set for call to update_...

																									update_neural_weights( p, neurone );

																									p->rtype_flag   = rtype_flag_temp;// Restore

																									/*
																									   printf("\nTPAT follows\n");
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

									}
									else if (  card_type != TPAT && card_type != NPAT &&
																				( strncmp( "0", string, 1 ) == 0 || strncmp( "1", string, 1 ) == 0 ||
																						strncmp( "2", string, 1 ) == 0 || strncmp( "3", string, 1 ) == 0 ||
																						strncmp( "4", string, 1 ) == 0 || strncmp( "5", string, 1 ) == 0 ||
																						strncmp( "6", string, 1 ) == 0 || strncmp( "7", string, 1 ) == 0 ||
																						strncmp( "8", string, 1 ) == 0 || strncmp( "9", string, 1 ) == 0 ))
									{
																	printf("\nFatal Error - Pattern detected without a preceding TPAT!\n" );
																	return( F_FALSE );
									}

									// Now process RPAT. This means the generation of a random pattern.
									if ( card_type == RPAT  &&  row_count == 0 )
									{
																	for(j=0; j<n; j++)
																	{
																									for(k=0; k<(p->states); k++)
																									{
																																	(neurone+j)->states[k] = 0; // zero the states
																									}
																									(neurone+j)->tag = 0; // and the tags
																	}
																	// Fill in the neural array....
																	num_rands = 0;
																	//printf("generating an RPAT...\n");
																	do
																	{
																									random_val = (int)ceil((n-1) * ran4());
																									//printf("random_val = %d\n",random_val);
																									if( (neurone+(int)random_val)->tag != 1 )
																									{
																																	state_val = (int)ceil((p->states)*ran4());
																																	//printf("\tstate_val = %d\n",state_val);
																																	(neurone+(int)random_val)->states[(int)state_val-1] = 1; // -1 because 0 is the first element
																																	(neurone+(int)random_val)->tag=1;
																																	num_rands++;
																									}
																	} while(num_rands != (p->actives));

																	/*
																	   printf("\nRPAT follows\n");
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


																	//  If the neural array has been filled with the input
																	//	activation values then we can calculate the connecting
																	//	weights (to this point)


																	rtype_flag_temp   = p->rtype_flag;// Preserve for restoration
																	p->rtype_flag   = NOT_DREAMING;// Set for call to update_...
																	//printf("\n calculating weights.....\n");
																	update_neural_weights( p, neurone ); // calculate the connecting weights up to this point ***************

																	p->rtype_flag   = rtype_flag_temp;// Restore


																	// We set row_count to force a new TPAT/RPAT before re-entry
																	row_count = (p->rows);
									}
	} // End of the outer while loop

	// Make sure that there were enough patterns
	if ( tpat_count != (p->num_pats) )
	{
									printf(  "\nFatal Error - Incorrect number of TPAT definitions.\n"
																		"The number of TPAT definitions must match the\n"
																		"NET specification.\n\n"
																		"Number of TPAT blocks from the NET specification : %d\n"
																		"Number of TPAT blocks actually found             : %d\n",
																		(p->num_pats), tpat_count );
									return( F_FALSE );
	}
	else if ( npat_count != 1 )
	{
									printf("\nFatal Error - There should be exactly 1 NPAT block!\n" );
									return( F_FALSE );
	}
	else if ( row_count != (p->rows)  &&  (p->update_cycles) > 0 )
	{
									printf(  "\nFatal Error - NPAT pattern block should have %d rows,\n"
																		"                only %d rows were actually counted !\n",
																		(p->rows), row_count );
									return( F_FALSE );
	}

	fclose( input_stream ); // Close the input parameter file
	return( F_TRUE ); // and return to main()
}
