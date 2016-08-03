#include "brain.h"

double randInRange(double min, double max){
        if (min==max) return 0.0;
        else return (double)( min + (rand() / (double) (RAND_MAX + 1.0) * (max - min)));
}

/* A general OpenGL initialization function.  Sets all of the initial parameters. */
void InitGL(int Width, int Height)          // We call this right after our OpenGL window is created.
{

  /*
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
           */

        // init opengl at some previous point
        /*
        glViewport(0, 0, Width, Height);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, Width, Height, 0, 1, -1);
        glMatrixMode(GL_MODELVIEW);
        glDisable(GL_DEPTH_TEST);
        */

        glClearColor(0.1f, 0.1f, 0.1f, 0.0f);		// This Will Clear The Background Color To Black
        glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
        glOrtho(-1.0, 1.0, -1.0, 1.0, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
}

/* Define a simple function to output text to the graphics screen. */
int gprint( int row, int col, char *str )
{

        printf("\n%s", str ); /* Just echo to the screen. */
        fflush( stdout );

        return(0);
}

void display_neurone_block( RUN_TIME_TYPE *p, NEURONE_TYPE *neurone, int n, char *sbuf )
{

        double r,c,x1,y1,x2,y2;
        float xc,yc;
        int color;
        int local_var,e;
        int i,j,k;

        float startX, startY; // start draing rects from the lower left

        float box_centerX, box_centerY;
        float box_size_halfX, box_size_halfY;

        // centre of the window is at (0,0)
        // the coordintate system looks like this:
        //
        // (-1,1)----------(0,1)----------(1,1)
        //    |              |              |
        //    |              |              |
        //  (-1,0)---------(0,0)----------(1,0)
        //    |              |              |
        //    |              |              |
        // (-1,-1)----------(0,-1)--------(1,-1)



        //printf("doing display_neurone_block\n");

          //printf("rows = %d, cols = %d\n", g->rows, g->cols);
          //printf("grid size x = %f, grid size y = %f\n",2.0/(float)g->rows, 2.0/(float)g->cols);

          box_size_halfX =  1.0/(float)g->rows-0.005;
          box_size_halfY =  1.0/(float)g->cols-0.005;

          startX = -1.0+box_size_halfX;
          startY = -1.0+box_size_halfY;

          //printf("box_size_halfX = %f, box_size_halfY = %f\n",box_size_halfX, box_size_halfY);

           glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
           glLoadIdentity();				// Reset The View

           glTranslatef(startX,startY,0.0f);  // move to the lower left corner

           // draw each active neuron
           for(k=0; k<n; k++) {
             r = k / g->rows; // get row
             c = k % g->cols; // get cols

             // get box center coordinates
             x1 = 2.0*(float)r/(float)g->rows;
             y1 = 2.0*(float)c/(float)g->cols;

             // set the box color
             local_var = 0;
             for(e=0; e<(p->states); e++) {
               if( ((neurone+k)->states[e]) == 0)
                   local_var++;
               else{
                   glColor3f(0.1,0.8,0.3);
                   break;
               }
             }
             //printf("local_var = %d, p->states = %d\n", local_var, p->states);
             if( local_var == (p->states))
                 glColor3f(1.0,0.0,0.0);


             glLoadIdentity();
             glTranslatef(startX+x1,startY+y1,0.0f);

             glBegin(GL_QUADS);				// start drawing a polygon (4 sided)
             glVertex3f(-box_size_halfX, box_size_halfY, 0.0f);		// Top Left
             glVertex3f( box_size_halfX, box_size_halfY, 0.0f);		// Top Right
             glVertex3f( box_size_halfX,-box_size_halfY, 0.0f);		// Bottom Right
             glVertex3f(-box_size_halfX,-box_size_halfY, 0.0f);		// Bottom Left
             glEnd();					// done with the polygon

           }
           /*
           for(i=0;i<g->rows;i++){
             for(j=0;j<g->cols;j++){
               glLoadIdentity();
               // get box center coordinates
               x1 = 2.0*(float)i/(float)g->rows;
               y1 = 2.0*(float)j/(float)g->cols;
               //printf("x1 = %f, y1 = %f\n", startX+x1, startY+y1);
              glTranslatef(startX+x1,startY+y1,0.0f);		        // Move Right 3 Units

           //glTranslatef(0.0f,0.0f,0.0f);
           glBegin(GL_QUADS);				// start drawing a polygon (4 sided)
           glVertex3f(-box_size_halfX, box_size_halfY, 0.0f);		// Top Left
           glVertex3f( box_size_halfX, box_size_halfY, 0.0f);		// Top Right
           glVertex3f( box_size_halfX,-box_size_halfY, 0.0f);		// Bottom Right
           glVertex3f(-box_size_halfX,-box_size_halfY, 0.0f);		// Bottom Left
           glEnd();					// done with the polygon
         }

       }
       */
           // swap buffers to display, since we're double buffered.
           //SDL_GL_SwapBuffers();
           SDL_GL_SwapWindow(screen);
           return;




/*
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear The Screen And The Depth Buffer
        glLoadIdentity();   // Reset The View

        for(k=0; k<n; k++) {
                r = k / g->cols;
                c = k % g->cols;

                x1 = g->xs + (c * g->xw);
                y1 = g->ys + (r * g->yh);
                x2 = (x1 + g->xw)+1;
                y2 = (y1 + g->yh)+1;




                printf("translate: x1 = %f, y1 = %f\n",x1/g->cols,y1/g->cols);
                glTranslatef(x1/g->cols,y1/g->cols,-1.0f);


                local_var = 0;
                for(e=0; e<(p->states); e++) {
                        if( ((neurone+k)->states[e]) == 0)
                                local_var++;
                        else{
                                //change the colour
                                //color = g2_ink(window, 0.0, 0.0, 0.0);
                                //g2_pen(window, color);

                                sprintf(str,"%d",e+1);

                                //printf("local_var = %d\n", local_var);

                                //g2_string(window, x1,y1, str);

                                // set the color to white
                                //glColor3f(0.1,1.0,0.3);

                                glColor3f(0.1,randInRange(0.0,1.0),1.3);

                                // and draw a rectangle
                                //drawARect(x1,x2,y1,y2);

                                break;
                        }
                }


                if( local_var == (p->states))
                {
                        //color = g2_ink(window, 0.0, 0.0, 0.0); //white
                        glColor3f(1.0,0.0,0.0);

                        sprintf(str,"0");

                        //g2_string(window, x1,y1, str);
                        //g2_pen(window, color);
                }


                //printf("x1 = %f, x2 = %f\n", x1,x2);
                drawARect(x1,x2,y1,y2);
                //drawARect(x1,x2,y1,y2);
        }
        //color = g2_ink(window,0.0,0.0,0.0);
        //g2_pen(window, color);
        //g2_string(window, 10.0,10.0, sbuf);

        // sdl 1.2 call
        //SDL_GL_SwapBuffers();
        SDL_GL_SwapWindow(screen);
        */
}

//void drawARect(double x1, double x2, double y1, double y2)
void drawARect(double x1, double x2, double y1, double y2)
{

        //g2_filled_rectangle(window,x1,y1,x2,y2);
        //g2_set_background(int dev, int color)
        glScalef(4.0, 4.0, 1.0);
        glBegin(GL_QUADS);    // start drawing a polygon (4 sided)
        glVertex3f(-1.0f, 1.0f, 0.0f);    // Top Left
        glVertex3f( 1.0f, 1.0f, 0.0f);    // Top Right
        glVertex3f( 1.0f,-1.0f, 0.0f);    // Bottom Right
        glVertex3f(-1.0f,-1.0f, 0.0f);    // Bottom Left
        glEnd();        // done with the polygon


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
