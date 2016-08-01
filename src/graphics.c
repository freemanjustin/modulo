#include "brain.h"

/* A general OpenGL initialization function.  Sets all of the initial parameters. */
void InitGL(int Width, int Height)	        // We call this right after our OpenGL window is created.
{
  glViewport(0, 0, Width, Height);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		// This Will Clear The Background Color To Black
  glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
  glDepthFunc(GL_LESS);				// The Type Of Depth Test To Do
  glEnable(GL_DEPTH_TEST);			// Enables Depth Testing
  glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();				// Reset The Projection Matrix

  gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);	// Calculate The Aspect Ratio Of The Window

  glMatrixMode(GL_MODELVIEW);

}

/* Define a simple function to output text to the graphics screen. */
int gprint( int row, int col, char *str )
{

	printf("\n%s", str ) ;  /* Just echo to the screen. */
	fflush( stdout ) ;

	return(0) ;
}

void display_neurone_block( RUN_TIME_TYPE *p, NEURONE_TYPE *neurone, int i, char *sbuf )
{

    double	r,c,x1,y1,x2,y2;
    int		color;
    int		local_var,e;

	char	str[81];

    r = i / g->cols;
    c = i % g->cols;

    x1 = g->xs + (c * g->xw);
    y1 = g->ys + (r * g->yh);
    x2 = (x1 + g->xw)+1;
    y2 = (y1 + g->yh)+1;


	 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
  	 glLoadIdentity();				// Reset The View

	 glTranslatef(x1,y1,-6.0f);


	local_var = 0;
	for(e=0;e<(p->states);e++)
	{
		if( ((neurone+i)->states[e]) == 0)
			local_var++;
		else
		{
			//change the colour
			//color = g2_ink(window, 0.0, 0.0, 0.0);
			//g2_pen(window, color);

			sprintf(str,"%d",e+1);

			//printf("local_var = %d\n", local_var);

			//g2_string(window, x1,y1, str);

			// set the color to white
			glColor3f(0.35,0.95,0.15);

			// and draw a rectangle
			//drawARect(x1,x2,y1,y2);

			break;
		}
	}


	if( local_var == (p->states))
	{
		//color = g2_ink(window, 0.0, 0.0, 0.0); //white
		glColor3f(0.0,0.0,0.0);

		sprintf(str,"0");

		//g2_string(window, x1,y1, str);
		//g2_pen(window, color);
	}

    //drawARect(x1,x2,y1,y2);
    //drawARect(x1,x2,y1,y2);

    //color = g2_ink(window,0.0,0.0,0.0);
    //g2_pen(window, color);
	//g2_string(window, 10.0,10.0, sbuf);

    // sdl 1.2 call
    //SDL_GL_SwapBuffers();
    SDL_GL_SwapWindow(screen);
}

void drawARect(double x1, double x2, double y1, double y2)
{

        //g2_filled_rectangle(window,x1,y1,x2,y2);
		//g2_set_background(int dev, int color)

		glBegin(GL_QUADS);				// start drawing a polygon (4 sided)
  			glVertex3f(-1.0f, 1.0f, 0.0f);		// Top Left
  			glVertex3f( 1.0f, 1.0f, 0.0f);		// Top Right
  			glVertex3f( 1.0f,-1.0f, 0.0f);		// Bottom Right
  			glVertex3f(-1.0f,-1.0f, 0.0f);		// Bottom Left
  		glEnd();					// done with the polygon


        return;
}

void clearWindow()
{
	//g2_clear(window);
    return;
}


void graphicsPrep()
{
    //set the pen colour

    //g2_ink(window,1.0,1.0,1.0);

    return;
}
