#include "rand.h"

/* This is a library of random number generators.

   RAN0, RAN1, RAN2 and RAN3 are based on functions of the same name taken
   from "Numerical Recipes".

*/





/* Define some variables that are used globally in the random functions. */

int inextran,inextranp ;

long iyran, irandum, iranbuf[98], ix1,ix2,ix3, maran[56], mjran  ;

float yran, maxran, vranbuf[98] ;



/* ------------------------- RAN0 --------------------------------- */


/* sran0 should be called first to perform initialisation. */
void sran0( unsigned rseed )
{
int j         ;

maxran = (float)(RAND_MAX) ;

srand( rseed ) ;

for ( j=0 ; j<98 ; j++ ) yran = rand() ;  /* Just exercise generator. */
for ( j=0 ; j<98 ; j++ ) vranbuf[j] = (float)rand() ; /* Fill array. */

yran = rand() ;  /* Initialise first yran */

}


/* The ran0 function should return a better random number than just rand. */
float ran0( void )
{
	int j;
   double d ;

	d = 1.0 + (double)( 96.0 * yran / maxran ) ;
   if ( fmod( d, 1.0 ) < 0.5 ) j = (int)d     ;  /* Round down.... */
   else                        j = (int)d + 1 ;  /* ...or up.      */

   /* j should range from a minimum of 1 to a max of 97. We ignore the
      first (ie. 0th) element in the array.
   */
	if ( j > 97 )
   {
     printf("RAN0: Index j ( %d ) out of range...programming error!!", j) ;
     exit(1) ;
   }

	yran     = vranbuf[j]         ;
	vranbuf[j]  = (float)rand()   ;

	return ( yran / maxran ) ;
}


/*  -----------------------------------------------------------  */


/*  ----------------------  RAN1  -----------------------------  */


#define M1 259200L
#define IA1 7141L
#define IC1 54773L
#define RM1 (1.0/(float)M1)
#define M2 134456L
#define IA2 8121L
#define IC2 28411L
#define RM2 (1.0/(float)M2)
#define M3 243000L
#define IA3 4561L
#define IC3 51349L


/* sran1 should be called first to perform initialisation. */
void sran1( unsigned rseed )
{
int j         ;
long lseed    ;

lseed = (long)rseed % IC1 ;  /* Always less than IC1 */
if ( lseed < 0L ) lseed = -lseed ;

ix1 = ( IC1 - lseed ) % M1 ;
ix1 = ( IA1*ix1+IC1 ) % M1         ;
ix2 = ix1 % M2                     ;
ix1 = ( IA1 * ix1 + IC1 ) % M1     ;
ix3 = ix1 % M3                     ;

/* Ignore the 0th element. */
for ( j=1 ; j<98 ; j++ )
{
   ix1        = (IA1*ix1+IC1) % M1   ;
   ix2        = (IA2*ix2+IC2) % M2   ;
   vranbuf[j] = ( (float)ix1 + (float)ix2* RM2 ) * RM1  ;
}

}


/* ran1 should return random numbers after seeding by sran1. */
float ran1( void )
{
   double d ;
	float temp;
	int j;

	ix1 = (IA1*ix1+IC1) % M1 ;
	ix2 = (IA2*ix2+IC2) % M2 ;
	ix3 = (IA3*ix3+IC3) % M3 ;


	d = 1.0 + (double)( 96.0 * (float)ix3 / (float)M3 ) ;
   if ( fmod( d, 1.0 ) < 0.5 ) j = (int)d     ;  /* Round down.... */
   else                        j = (int)d + 1 ;  /* ...or up.      */


   /* j should always be in the range 1 to 97 inclusive. */
	if (  j > 97  ||  j < 1  )

   {
     printf("RAN1: Index j ( %d ) out of range...programming error!!", j) ;
     exit(1) ;
   }

	temp = vranbuf[j] ;
	vranbuf[j] = ( (float)ix1 + (float)ix2 * RM2 ) * RM1 ;

	return (temp) ;
}

#undef M1
#undef IA1
#undef IC1
#undef RM1
#undef M2
#undef IA2
#undef IC2
#undef RM2
#undef M3
#undef IA3
#undef IC3


/*  -----------------------------------------------------------  */


/*  ----------------------  RAN2  -----------------------------  */


#define M 714025L
#define IA 1366L
#define IC 150889L


/* sran2 should be called first to perform initialisation. */
void sran2( unsigned rseed )
{
int j         ;

irandum = (long)rseed ;

/* irandum is always positive. */
if ( (irandum = (IC-irandum) % M) < 0 ) irandum = -irandum ;

for ( j=1 ; j<98 ; j++ )
{
   irandum = (IA * irandum + IC) % M;
	iranbuf[j]= irandum ;
}

irandum = ( IA * irandum + IC ) % M;
iyran   = irandum;

}



float ran2( void )
{
	int j;
   double d ;


	d = 1.0 + (double)( 96.0 * (float)iyran / (float)M ) ;
   if ( fmod( d, 1.0 ) < 0.5 ) j = (int)d     ;  /* Round down.... */
   else                        j = (int)d + 1 ;  /* ...or up.      */

   /* j must always be >=1 and <=97 */
	if (  j > 97  ||  j < 1  )
   {
     printf("RAN2: Index j ( %d ) out of range...programming error!!", j) ;
     exit(1) ;
   }

	iyran    = iranbuf[j]  ;

	irandum  = (IA*irandum+IC) % M ;

	iranbuf[j] = irandum ;

	return ( (float)iyran / (float)M ) ;
}

#undef M
#undef IA
#undef IC


/*  -----------------------------------------------------------  */


/*  ----------------------  RAN3  -----------------------------  */

#define MBIG 1000000000L
#define MSEED 161803398L
#define MZ 0L
#define FAC (1.0/(float)MBIG)


/* sran3 should be called first to perform initialisation. */
void sran3( unsigned rseed )
{
int i, ii, k   ;
long mk        ;


mjran = MSEED - (long)abs(rseed) ;

mjran = mjran % MBIG;

maran[55] = mjran ;
mk = 1      ;

for ( i=1 ; i<=54 ;i++ )
{
   ii = (21*i) % 55 ;
   maran[ii] = mk      ;
   mk = mjran -mk      ;
   if ( mk < MZ ) mk = mk + MBIG ;
   mjran = maran[ii] ;
}

for ( k=1 ; k<=4 ; k++ )
{
   for ( i=1 ; i<=55 ; i++ )
   {
     maran[i] = maran[i] - maran[1+(i+30) % 55];
     if (maran[i] < MZ) maran[i] = maran[i] + MBIG;
	}
}

inextran  = 0  ;   /* Indices for first generated number. */
inextranp = 31 ;   /* 31 is special according to Knuth.   */

}


float ran3( void )
{

if ( ++inextran  == 56 ) inextran  = 1 ;
if ( ++inextranp == 56 ) inextranp = 1 ;
mjran = maran[inextran] - maran[inextranp] ;
if ( mjran < MZ ) mjran += MBIG   ;
maran[inextran] = mjran ;

return ( (float)mjran * FAC ) ;

}

#undef MBIG
#undef MSEED
#undef MZ
#undef FAC

/*  -----------------------------------------------------------  */

/*  ----------------------  RAN4  -----------------------------  */


/*
   sran4 and ran4 are based on RANMARIN/RANMAR - the worlds greatest
   random number generator.
*/


#define IJMAX 31328L
#define KLMAX 30081L

float uran4[98], cran4, cdran4, cmran4 ;
int i97ran4, j97ran4          ;


/* sran4 should be called first to perform initialisation. */
void sran4( unsigned rseed )

{
long ij, kl, i, j, k, l, m ;
int ii, jj, kk ;
float s, t     ;


/* set ij and kl using the system generator.... */

srand( rseed ) ;

kk=rand() % 65536L     ;
for ( ii=0 ; ii<kk ; ii++ ) 
	jj = rand() ; /* Roll through for a while. */

ij = jj;

kk=rand() % 65536L    ;

for ( ii=0 ; ii<kk ; ii++ ) 
	jj = rand() ; /* Roll through for a while. */
	
kl = jj;

/* Now that we have ij and kl we can get into initialisation. */




i = ( (ij/177L) % 177L ) + 2L ;
j = (  ij       % 177L ) + 2L ;
k = ( (kl/169L) % 178L ) + 1L ;
l = (  kl       % 169L )      ;

/* Fill buffer with 97 elements indexed from 1 to 97. Ignore 0th element. */
for ( ii=1 ; ii<98 ; ii++ )
{
   s = 0.0F ;
   t = 0.5F ;

   for ( jj=1 ; jj<25 ; jj++ )
   {
    m = ( ( (i * j) % 179L ) * k ) % 179L ;
    i = j ;
    j = k ;
    k = m ;
    l = ( 53L * l + 1L ) % 169L ;
    if ( ((l*m)%64L) >= 32L ) s = s + t ;
    t = 0.5F * t ;
   }

   uran4[ii] = s ;   /* Fill up the buffer. */
}


cran4  = (float)(   362436.0 / 16777216.0 ) ;
cdran4 = (float)(  7654321.0 / 16777216.0 ) ;
cmran4 = (float)( 16777213.0 / 16777216.0 ) ;

i97ran4 = 97 ;
j97ran4 = 33 ;

}


float ran4( void )
{
float uni ;

uni = uran4[i97ran4] - uran4[j97ran4] ;
if ( uni < 0.0F ) uni = uni + 1.0F ;
uran4[i97ran4] = uni  ;

i97ran4 = i97ran4 - 1 ;
if ( i97ran4 == 0 ) i97ran4 = 97 ;

j97ran4 = j97ran4 - 1 ;
if ( j97ran4 == 0 ) j97ran4 = 97 ;

cran4 = cran4 - cdran4 ;
if ( cran4 < 0.0F ) cran4 = cran4 + cmran4 ;

uni = uni - cran4 ;
if ( uni < 0.0F ) uni = uni + 1.0F ;

return( uni ) ;

}

#undef IJMAX
#undef KLMAX

/*  -----------------------------------------------------------  */


/*  ----------------------  RAN5  -----------------------------  */


#define MBIG1  843314861L
#define MBIG2  453816693L
#define MBIG3  2147483647L
#define RSMALL (1.0/(float)MBIG3)

long ran5seed ;


/* sran5 should be called first to perform initialisation. */
void sran5( unsigned rseed )
{
int j         ;

ran5seed = (long)rseed ;


/* Ignore the 0th element. */
for ( j=1 ; j<98 ; j++ )
{
   ran5seed = ran5seed * MBIG1 + MBIG2 ;
   if ( ran5seed < 0L )
      ran5seed = (long)( (float)ran5seed + (float)MBIG3 + 1.0 ) ;

   vranbuf[j] = (float)ran5seed * RSMALL ;
}

}


/* ran5 should return random numbers after seeding by sran5. */
float ran5( void )
{
   double d   ;
	float u, v ;
	int j      ;


   ran5seed = ran5seed * MBIG1 + MBIG2 ;
   if ( ran5seed < 0L )
      ran5seed = (long)( (float)ran5seed + (float)MBIG3 + 1.0 ) ;

   u = (float)ran5seed * RSMALL ;


	d = 1.0 + (double)( 96.0 * u ) ;
   if ( fmod( d, 1.0 ) < 0.5 ) j = (int)d     ;  /* Round down.... */
   else                        j = (int)d + 1 ;  /* ...or up.      */


   /* j should always be in the range 1 to 97 inclusive. */
	if (  j > 97  ||  j < 1  )
   {
     printf("RAN5: Index j ( %d ) out of range...programming error!!", j) ;
     exit(1) ;
   }

	v = vranbuf[j] ;
	vranbuf[j] = u ;

	return ( v ) ;
}

#undef MBIG1
#undef MBIG2
#undef MBIG3
#undef RSMALL


/*  -----------------------------------------------------------  */



/*  ----------------------  RANF  -----------------------------  */


#define MBIG   2147483647L
#define RSMALL (1.0/(float)MBIG)

long ranfseed ;


/* sranf should be called first to perform initialisation. */
void sranf( unsigned rseed )
{
ranfseed = (long)rseed ;
}


/* ranf should return random numbers after seeding by sranf. */
float ranf( void )
{
   ranfseed = ranfseed * 65539L ;

   if ( ranfseed < 0L )
          ranfseed = (long)( (float)ranfseed + (float)MBIG + 1.0 ) ;

   return ( (float)ranfseed * RSMALL ) ;
}

#undef MBIG
#undef RSMALL



