#include "brain.h"
int keep_going = F_FALSE;
SDL_Event sdl_event;
//	This function will perform updating on the neural array. The neural
//	elements are updated only after they have all been calculated.
void sync_update_npat( RUN_TIME_TYPE *p, NEURONE_TYPE *neurone )
{
	int 				cycles, cycle_difference;
	int 				i, j, k, q, r, n, s;
	STATE_VECTOR		*h;
	//int					l_squared, copy_l_squared;
	int					active_count;
	int					local_max, count;
	char				sbuf[81];
	//int					local_var, m, e;
	float				*index,*sorted_array;
	int					local_min, start, end, num_active, random_val;
	int					*max_vector, max_count;

	n = (p->rows)*(p->cols) ; // number of nodes

	if( (max_vector = (int*)malloc( (p->states)*sizeof(int) )) == NULL)
	{
		printf("\n malloc error in seq_update()\n");
		exit(1);
	}

	if( (sorted_array = (float*)malloc( (n+1)*sizeof(float) )) == NULL)
	{
		printf("\n malloc error in seq_update()\n");
		exit(1);
	}
	else	//init to zero
	{
		for(j=0;j<=n;j++)
			sorted_array[j] = 0.0;
	}
	if( (index = (float*)malloc( (n+1)*sizeof(float) )) == NULL)
	{
		printf("\n malloc error in seq_update()\n");
		exit(1);
	}
	else	//init to zero
	{
		for(j=0;j<=n;j++)
			index[j] = (float)j;
	}


	// malloc some memory for the temp state vectors...
	if ( (h = (STATE_VECTOR*) malloc( sizeof(STATE_VECTOR*)*n ) ) == NULL )
	{
	  	printf(	"\nNot enough memory for the state vectors.\n") ;
	  	exit(1) ;
	}
	for ( j=0 ; j<n  ; j++ )
	{
		if ( ( (h+j)->states =  (int*)malloc( sizeof(int)*(p->states) ) ) == NULL )
		{
		  	printf(	"\nNot enough memory for the state vectors.\n") ;
		  	exit(1) ;
		}
		else
		{
			  for ( k = 0 ; k < (p->states) ; k++ )
			  {
					(h+j)->states[k] = 0  ;  // Initialise to 0
			  }
		}
	}

	//clearWindow();

	/*
	for( i=0 ; i<n ; i++ )
	{
	   display_neurone_block( p, neurone, i ,"") ;
	}
	*/
	display_neurone_block( p, neurone, n ,"") ;

	for ( cycles = 0 ; cycles < p->update_cycles ; cycles++ )
	{
		active_count = 0;

	   sprintf(sbuf, "Synchronous Neural Update %d (%d maximum)",cycles+1, p->update_cycles ) ;
	   //fprintf(stdout, "\nSynchronous Neural Update %d (%d maximum)\n",cycles+1, p->update_cycles ) ;


	   p->converge_cycles = cycles + 1 ;

	   cycle_difference = 0 ;


	   	for ( j=0 ; j<n ; j++ )
	   	{
			for( k=0;k<n;k++)
       		{
       			for(q=0;q<(p->states);q++)
       			{
       				for(r=0;r<(p->states);r++)
       				{
				           (h+j)->states[r] += (neurone+j)->w[k]->weight[q][r] * ((neurone+k)->states[r]);
				  	}
				}
			}
	   }	// end of the module update cycle

	   //printf("\nfinished an update cycle \n");

		//	square the vectors
	   for (j=0;j<n;j++)
	   {
	   		(neurone+j)->l_squ = square_states((neurone+j)->states,p);
			//printf("j = %d\tk = %d\tl_squ = %d\t",j,k, (neurone+j)->l_squ);
			(neurone+j)->l_copy_squ  = square_states((h+j)->states,p);
			//printf("l_copy_squ = %d\n", (neurone+j)->l_copy_squ);
			sorted_array[j] = (neurone+j)->l_copy_squ;
			index[j] = (float)j;
			(neurone+j)->largest = 0;
	 	}
	 	index[n] = 0.0;

		sort2((unsigned long)n, sorted_array, index);	//sort2 sorts the vectors into ascending order

		//printf("\n");
		//for(j=0;j<=n;j++)
	 	//{
		//	printf("%f\t%f\n",index[j],sorted_array[j]);
	 	//}

		if( sorted_array[ (n - (p->actives-1)) ] == sorted_array[ (n - (p->actives)) ] )
		{
			num_active = 0;
			local_min = sorted_array[ (n - (p->actives)) ];

			// see how many elemets to the left are of the same value
			for(j=(p->actives-1);j>=0;j--)
			{
				if(sorted_array[n-j] != local_min)
				{
					start = j+2;	//starting index
					//printf("start = %d\n",start);
					break;
				}
			}

			//see how many to the right are the same value
			for(j=(p->actives+1);j<=n;j++)
			{
				if(sorted_array[(n-j)] != local_min)
				{
					end = j;	// finishing index
					//printf("end = %d\n",end);
					break;
				}
			}

			for(j=0;j<start-1;j++)
			{
				(neurone+(int)index[(n-j)])->largest = 1;
				//printf("tagging a module as large\tmodule = %d\n",(int)index[(n-j)]);
			}
			// now randomly choose from the set start..end, (p->actives)-start as many.
			num_active = start-1;
			do
			{
				random_val = (int)ceil(((end-start)+1) * ran4());	// pick a random module
				//printf("random_val = %d\n",random_val);
				if( (neurone+(int)index[(n-start-random_val-1)])->largest != 1)
				{
					//printf("\tselected a module to tag, module = %d\n",(int)index[(n-start-random_val-1)]);
					(neurone+(int)index[(n-start-random_val-1)])->largest = 1;
					num_active++;
				}
			}while(num_active != (p->actives));

		}
		else	// if we have p->actives as many, just tag them
		{
			for(j=0;j<(p->actives);j++)
			{

				(neurone+(int)index[(n-j)])->largest = 1;
			}
		}

		// adjust the p->actives nodes and make the rest zero

		/*for(j=0;j<n;j++)
		{
			if((neurone+j)->largest == 1)
			{
				printf("largest == 1\tmodule = %d\nstate vector of this module is:\n",j);
				for(r=0;r<(p->states);r++)
					printf("%d\n",(h+j)->states[r]);

			}
		}*/


	   for(j=0;j<n;j++)
	   {
			if((neurone+j)->largest == 1)
			{
				//printf("\n\nfound a largest state vector...\nj = %d\nstate vector follows:\n",j);
				local_max = 0;
				count = 0;
				max_count = 0;

				//for(r=0;r<(p->states);r++)
				//	printf("%d\n",(h+j)->states[r]);

				for(r=0;r<(p->states);r++)
					max_vector[r] = 0;

				// will need to compare the vector with itself here to see if there is more than one choice for the activation
				// state (ie. a vector like 0220 -> randomly pick one of the '2' sites and set it to one, other elements to zero

				//check each element, and find the largest, and howmany of the largest there are

				for(r=0;r<(p->states);r++)
				{
					//printf("%d\n",(h+j)->states[r]);
					// find the max element
					if( ((h+j)->states[r] > local_max) )
					{
						local_max=(h+j)->states[r];
						count = r;
						max_count = 1;		// count how many there are
					}
					else if( ((h+j)->states[r] == local_max) && (local_max != 0) )
					{
						max_count++;
					}
				}
				if(max_count > 1 && local_max != 0)
				{
					// randomly pick one of the largest elements

					//printf("\n\t\tmax count is greater than 1...\tmax_count = %d\n\n",max_count);
					s = 0;
					for(r=0;r<(p->states);r++)
					{
						if((h+j)->states[r] == local_max)
						{
							max_vector[s] = r;	//find out which elements are the same
							s++;
						}
					}
					count = (int)ceil((max_count) * ran4());	// pick a random state
					count = max_vector[count];
					//printf("chose to change site %d\n",count);
					for(r=0;r<(p->states);r++)
					{
						// change the max element to 1, set the rest to zero
						if(r == count)
						{
							(h+j)->states[r] = 1;
						}
						else
						{
							(h+j)->states[r] = 0;
						}
					}
					(neurone+j)->l_copy_squ = 1;
					//printf("\nmodified state vector follows\n");
					//for(r=0;r<(p->states);r++)
					//	printf("%d\n",(h+j)->states[r]);
				}
				else
				{
					for(r=0;r<(p->states);r++)
					{
						// change the max element to 1, set the rest to zero
						if(r == count)
						{
							(h+j)->states[r] = 1;
						}
						else
						{
							(h+j)->states[r] = 0;
						}
					}
					(neurone+j)->l_copy_squ = 1;
				}
			}
			else
			{
				//printf("making the state vector equal to zero\n");
				for(r=0;r<(p->states);r++)
				{
					// make all of the states equal to zero
					(h+j)->states[r]=0;
				}
				(neurone+j)->l_copy_squ = 0;
			}
			cycle_difference += abs( ((neurone+j)->l_copy_squ) - ((neurone+j)->l_squ));
	   		//printf("cycle_difference = %d\tl_copy_squ = %d\tl_squ = %d\n",cycle_difference,(neurone+j)->l_copy_squ,(neurone+j)->l_squ);
	   }



	   // Now update the input neural array for the next cycle
	   //clearWindow();
	   for ( j=0 ; j<n ; j++ )
	   {
			 	//keep_going = F_FALSE;
       		for(r=0;r<(p->states);r++)
       		{
						(neurone+j)->states[r] = (h+j)->states[r];
					}
					/*
					display_neurone_block(p, neurone, j, sbuf) ; // this was inside the above loop in the original code
					*/
	   }

		 display_neurone_block(p, neurone, n, sbuf) ; // this was inside the above loop in the original code

/*
	   for(int m=0;m<n;m++)
		{
			int local_var = 0;
			for(int e=0;e<(p->states);e++)
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
	   if ( cycle_difference == 0 )
	   {	//printf("***** converged *****\n");
	   		break ;
	   	}
	}
	//printf("\nfunction sync_update() ...done\n");
	free( (void *) h ) ;  // Return the update memory
}



int	square_states(int* states, RUN_TIME_TYPE *p)
{
	int		states_squared = 0;
	int		j;

	//find the length squared of the vector
	for(j=0;j<(p->states);j++)
	{
		states_squared += (int)pow(states[j],2.0);
	}
	//printf("from func square_states:\tstates_squared = %d\n",states_squared);
	return	(states_squared);

}

int update_neural_weights( RUN_TIME_TYPE *p, NEURONE_TYPE *neurone )
{

	int 	j, k, q, r ,n;

	//printf("\n\nEntered function update_neural_weights()\n");
	n = p->rows * p->cols ;

	// 	store the just learnt pattern, but only if
	// 	the user specified a pattern file to go out to.

	// Fill in the pattern file if valid
	if ( p->pattern.flag == F_TRUE )
	{
		write_pattern_file( p->pattern.stream, (p->rows)*(p->cols), neurone, p) ;
	}

	for ( j=0 ; j<n ; j++ )
	{
		for(k=0;k<n;k++)
		{
	   		// Learn the pattern...
	   		for( q=0;q<(p->states);q++)
	   		{
	   			for( r =0; r<(p->states);r++)
	   			{
		       		// self interaction term??
		         	(neurone+j)->w[k]->weight[q][r] += (int)((neurone+j)->states[q])*((neurone+k)->states[r]);

		       	}
	       	}
	     }
	}
	//printf("....\nupdated the neural weights\n....\n");
	return( n ) ;

}
