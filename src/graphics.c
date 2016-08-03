#include "brain.h"

/* A general OpenGL initialization function.  Sets all of the initial parameters. */
void InitGL(int Width, int Height)          // We call this right after our OpenGL window is created.
{
        glClearColor(0.1f, 0.1f, 0.1f, 0.0f);   // This Will Clear The Background Color To Black
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-1.0, 1.0, -1.0, 1.0, 1, -1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
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
        startY = 1.0-box_size_halfY;

        //printf("box_size_halfX = %f, box_size_halfY = %f\n",box_size_halfX, box_size_halfY);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);       // Clear The Screen And The Depth Buffer
        glLoadIdentity();           // Reset The View

        glTranslatef(startX,startY,0.0f);     // move to the lower left corner

        // draw each active neuron
        for(k=0; k<n; k++) {
                r = k % g->rows; // get row
                c = k / g->cols; // get cols

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
                        glColor3f(0.0,0.0,0.0);


                glLoadIdentity();
                glTranslatef(startX+x1,startY-y1,0.0f);

                glBegin(GL_QUADS);    // start drawing a polygon (4 sided)
                glVertex3f(-box_size_halfX, box_size_halfY, 0.0f); // Top Left
                glVertex3f( box_size_halfX, box_size_halfY, 0.0f); // Top Right
                glVertex3f( box_size_halfX,-box_size_halfY, 0.0f); // Bottom Right
                glVertex3f(-box_size_halfX,-box_size_halfY, 0.0f); // Bottom Left
                glEnd();      // done with the polygon

        }

        SDL_GL_SwapWindow(screen);

        return;

}
