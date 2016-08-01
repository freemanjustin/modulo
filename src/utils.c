#include "brain.h"

/* Scan the string for a !. If one is found then \x0 is substituted.
   If the last character in the string is \n then this is converted
   into a \x0. */
char *fix_string( char *c )
{
	char *ptr ;

	if ( (ptr=strchr( c, '!' ) ) != NULL ) *ptr='\x0' ;

	if ( strlen(c) > 0 && *(c+strlen(c)-1) == '\n' ) *(c+strlen(c)-1) = '\x0' ;

	return(c) ;
}


/* Make a string uppercase. */
char *uppercase( char *c )
{
	int j ;

	for ( j=0 ; j<(int)strlen(c) ; j++ )  *(c+j) = (char)toupper( *(c+j) ) ;
	   
	return(c) ;
}


/* Left justify and right pack a string. */
char *lr_pack( char *c )
{
	int j ;

	/* Left pack. */
	while( strlen(c)>0 && *c==' ' )
	    for( j=0 ; j<(int)strlen(c) ; j++ ) *(c+j) = *(c+j+1) ;

	/* Get rid of blanks on the right. */
	while( strlen(c)>0 && *(c+strlen(c)-1)==' ' ) *(c+strlen(c)-1) = '\x0' ; 

	return( c ) ;
}


// Write activation pattern to the end of a pattern file
void write_pattern_file( FILE *pattern_stream, int n, NEURONE_TYPE *neurone, RUN_TIME_TYPE *p)
{
	int j, k, local_var, zero ;

	zero = 0;

	fseek( pattern_stream, 0L, SEEK_END ) ; // Set for append

	for(j=0;j<n;j++)
	{
		local_var = 0;
		for(k=0;k<(p->states);k++)
		{
			if((neurone+j)->states[k] == 0)
				local_var++;
			else
			{
				local_var+=1;
				fwrite( &(local_var), sizeof(int), 1, pattern_stream ) ;
				local_var-=1;
				break;
			}
		}
		if( local_var == (p->states))
		{
			fwrite( &(zero), sizeof(int), 1, pattern_stream ) ;
		}
	}
}

// 	This function will read the pattern file and determine
// 	just how many patterns there are, this values is returned.
int get_number_of_patterns( FILE *pattern_stream )
{
	int r1, c1, pattern_count ;
	long fp1, fp2 ;

	fseek( pattern_stream, 0L, SEEK_SET ) ; // Set pattern file start

	// Get rows and columns... 
	fread( &r1, sizeof(int), 1, pattern_stream ) ;
	fread( &c1, sizeof(int), 1, pattern_stream ) ;

	fp1 = ftell( pattern_stream ) ; // Get current file position

	fseek( pattern_stream, 0L, SEEK_END ) ; // Go to the end of file
	fp2 = ftell( pattern_stream ) ; // Get end of file position

	pattern_count = (int)( fp2 - fp1 + 1 ) / (r1*c1*sizeof(int)) ;

	return( pattern_count ) ;
}


void report_patterns( FILE *report_stream, FILE *pattern_stream )
{
	int *na, r1, c1, n, j, pattern_number ;

	fseek( pattern_stream, 0L, SEEK_SET ) ; // Set pattern file start

	// Get rows and columns
	fread( &r1, sizeof(int), 1, pattern_stream ) ;
	fread( &c1, sizeof(int), 1, pattern_stream ) ;

	fprintf( report_stream, "\n\n\n\t *** Pattern file contents follow :"
				     		"\n\n     # of rows : %d"
				       		"\n     # of cols : %d",
							r1, c1);

	n = r1 * c1 ; // This is the number of neural elements

	na = ( int * ) malloc(sizeof(int)*n) ;  // Get memory

	// Output the pattern contents
	pattern_number = 0 ;
	while ( (int)fread( na, sizeof(int), n, pattern_stream ) == n )
	{
	   pattern_number++ ;
	   fprintf( report_stream, "\n\n\n *** Pattern number : %d\n", pattern_number) ;
	   for( j=0 ; j<n ; j++ )
	   {
	       fprintf( report_stream, "%d", *(na+j) ) ;
	       if( (j+1) % c1 == 0)
	       	fprintf( report_stream, "\n") ;
	   }
	}
	free( (void *) na ) ; // Free memory
}

// The activations in neurone are matched against the patterns held in
// the file pattern stream. The status of the match is returned
void match_patterns( NEURONE_TYPE *neurone, FILE *pattern_stream, 
		     		 int number_of_patterns, MATCH_TYPE *match,
		     		 RUN_TIME_TYPE *p  )
{
	int 	*na, r1, c1, n, i, s1, pn ;
	int		local_var, local_act, k ;
	float	 p1;

	fseek( pattern_stream, 0L, SEEK_SET ) ; // Set pattern file start

	// Get rows and columns
	fread( &r1, sizeof(int), 1, pattern_stream ) ;
	fread( &c1, sizeof(int), 1, pattern_stream ) ;

	n = r1 * c1 ; // This is the number of neural elements

	na = ( int * ) malloc( sizeof(int) * n) ;  // Get memory

	// Get in the patterns
	pn = 0 ;
	while ((int)fread(na, sizeof(int), n, pattern_stream) == n)
	{
	   pn++ ;
	   if ( pn > number_of_patterns )
	   {
	     printf("Fatal Error - pattern mismatch, this is programming problem!!" );
	     exit(1) ;
	   }

	   s1 = 0 ;
	   for ( i=0 ; i<n ; i++ )
	   {
	     	// determine the current activation 
			local_var = 0;
			local_act = 0;
			for(k=0;k<(p->states);k++)
			{
				if((neurone+i)->states[k] == 0)
					local_var++;
				else
				{
					local_act = local_var+1 ;
					break;
				}
			}
			if( local_var == (p->states))
				local_act = 0 ;

			if ( abs( local_act - *(na+i) ) == 0 ) 
					s1++ ; // Count matches 
		}		
	

	   // Get the percentage match
	   p1 = 100.0F * (float)s1 / (float)n ; // % positively matched


	   // If p1 or p2 >= p->match_perc then the criteria for match acceptance is fulfilled.
	   // If there was no match then increment the last element of 'match'.
	   
	   if ( p1 >= p->match_perc )
	   {
	     (match+pn-1)->pos = (match+pn-1)->pos + 1 ;
	     (match+pn-1)->pos_con = (match+pn-1)->pos_con + p->converge_cycles ;
	     break ;
	   }
	   else if ( pn == number_of_patterns ) // If no match then increment
	   {
	      //  If we didn't match one of the learnt patterns then two possible
	      //  conditions arise. Convergance may be to a spurious pattern or we
	      //  may not have converged at all. If convergence was to a spurious
	      //  pattern then converge_cycles < update_cycles. Otherwise the
	      //  search will have iterated to the maximum update_cycles without
	      //  converging.
	     if ( p->converge_cycles < p->update_cycles ) // No match, convergence
	     { 
	       (match+pn)->pos = (match+pn)->pos + 1 ;  // Count spurious
	       (match+pn)->pos_con = (match+pn)->pos_con + p->converge_cycles ;
	     }
	     else  // No pattern match and no convergence
	     {
	       (match+pn)->inv = (match+pn)->inv + 1 ;  // Count non-convergence
	       (match+pn)->inv_con = (match+pn)->inv_con + p->converge_cycles ;
	     }
	   }

	}
	free( (void *) na ) ; // Free memory
}


void report_header( FILE *report_stream, RUN_TIME_TYPE *p )
{
	time_t report_time ;

	fseek( report_stream, 0L, SEEK_END ) ; // Set to end of file 

	// Stick in a form feed and a report header
	report_time = time(NULL) ;

	fprintf( report_stream, "%s %s %s",
	  						"\f\n"
	  		"\t\t*****************************************************************\n\n"
	  		"\t\t        BRAINMOD RANDOM REPORT   ", ctime(&report_time),"\n"
	  		"\t\t*****************************************************************\n") ;

	fprintf( report_stream,
			"\n\n"
			"\n\t Number of modules in net    : %d "
			"\n\t Number of states per module : %d "
			"\n\t Number of active modules    : %d "
			"\n\t Match threshold percentage to accept   : %f\n ",  
							  p->rows*p->cols,
							  p->states,
							  p->actives,
							  p->match_perc );
}

// function report_matches():
//
// rt is the number of random inputs to the neural net.
// n is the number of elements in the match array.
// match is the match array from which statistics are drawn.

void report_matches( FILE *report_stream, int rt, int n, MATCH_TYPE *match )
{
	int i, tot ;
	float perc, av_pos_con, av_inv_con, av_con ;

	// Get stats for the unmatched patterns
	perc = ((float)((match+n-1)->pos)) / ((float)rt) * 100.0  ;

	if ( (match+n-1)->pos > 0 )
	   av_pos_con = (float)((match+n-1)->pos_con)/ (float)((match+n-1)->pos) ;
	else
	   av_pos_con = 0.0F ;

	// The the convergence count for the non-convergent situation. This
	//    should always work out to be update_cycles.
	if ( (match+n-1)->inv > 0 )
	   av_inv_con = (float)((match+n-1)->inv_con)/ (float)((match+n-1)->inv) ;
	else
	   av_inv_con = 0.0F ;

	fprintf( report_stream,"\n\n\n\t *** Pattern match stats follow ....\n");
	fprintf( report_stream,"\n\t     # of random patterns presented : %d\n", rt);
	fprintf( report_stream,"\n\t     # of spurious convergences : %d   ie.   %5.2f %% of total.",(match+n-1)->pos, perc);
	fprintf( report_stream,"\n\t     # of non-convergent pattern match attempts : %4.2f", av_pos_con);
	fprintf( report_stream,"\n\t     Pattern - index of the learnt pattern.");
	fprintf( report_stream,"\n\t     +Match  - # of matches to learnt pattern.");
	fprintf( report_stream,"\n\t     +Conv   - average convergence count to positive match.");
	fprintf( report_stream,"\n\t     Total Match = Match");
	fprintf( report_stream,"\n\t     Total %% - 100 * (Total Match)/(# of random patterns presented)");
	fprintf( report_stream," \n\t     Conv = ((+Match * +Conv) + (-Match * -Conv)) / (+Match + -Match) ");
	fprintf( report_stream,"\n\n");
	fprintf( report_stream," \n\tPattern  +Match  +Conv  Total Match  Total %% Conv");
	fprintf( report_stream,"\n\t-------  ------  -----------  -------  -----");
			    
	// We exclude the last (unmatched) item from the following report list
	for ( i=0 ; i<(n-1) ; i++ )
	{
	   tot  = (match+i)->pos + (match+i)->inv     ;
	   perc = ((float)tot) / ((float)rt) * 100.0  ;

	   // Determine the average convergence rates to each of the patterns
	   if ( (match+i)->pos > 0 )
	     av_pos_con = (float)((match+i)->pos_con)/ (float)((match+i)->pos) ;
	   else
	     av_pos_con = 0.0F ;

	   if ( (match+i)->inv > 0 )
	     av_inv_con = (float)((match+i)->inv_con)/ (float)((match+i)->inv) ;
	   else
	     av_inv_con = 0.0F ;

	   if ( tot > 0 )
	     av_con = ( ( (float)(match+i)->pos * av_pos_con ) +
	                ( (float)(match+i)->inv * av_inv_con )   ) / (float)tot ;
	   else
	     av_con = 0.0F ;

	   fprintf( report_stream,
	   		"\n\t  %3d     %3d     %4.1f       %4d        %5.2f    %4.1f",
		    (i+1),
		    (match+i)->pos,
		    av_pos_con,            
		    tot,
		    perc,
		    av_con  ) ;
	}
	// Finally finish up the report
	fprintf( report_stream, "\n\n\t End of report\t\t:)\n" ) ;
}





// Initialise the n elements of the match array to zero
void match_initialise( int n, MATCH_TYPE *match )
{
	int i ;

	for ( i=0 ; i<n ; i++ )
	{
	  (match+i)->pos = 0 ;
	  (match+i)->inv = 0 ;
	  (match+i)->pos_con = 0 ;
	  (match+i)->inv_con = 0 ;
	}
}

