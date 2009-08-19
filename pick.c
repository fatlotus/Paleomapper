#include "XPick.h"
#include "paleotypes.h"
#include "paleodefs.h"
#include "paleoexterns.h"
#include "pex.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <Xm/Xm.h>

#define MAX_SETS 20000
extern int vecsette[MAX_SETS];
extern GLboolean projection;
extern infoCB();
extern Widget popinfo;

int info_hits;
Widget helpentry;
int hits_before=0;
int pick_elem, pick_plate;
GLboolean hits_up = FALSE, select_mode = FALSE;

extern Window  draw_win;


int PickMode = GET_INFO;

void processHits (GLint hits, GLuint buffer[])
{
    unsigned int i, j;
    GLuint ii, jj, names, *ptr, temp;
    Widget entry;
    char text[20];
    int primitive[5],info_te[5];
    char *filename[5],tempname[5][40];


    for(i=0;i<5;i++) {
	primitive[i]=0;
	info_te[i] = 0;
    }	

    printf ("hits = %d\n", hits);
    info_hits = hits;
    ptr = (GLuint *) buffer;
    for (i = 0; i < hits; i++) {	/*  for each hit  */
	names = *ptr;
	ptr++; ptr++; ptr++;

	for (j = 0; j < names; j++) {	/*  for each name */
	    printf ("Element %d  ", *ptr);
	    temp = *ptr;
	    get_elem(temp);
	    pick_elem = temp;
	    if (temp>9999) pick_plate = cur_elem->tectonic_element;
	    if (temp>2999 && temp<5000) pick_plate = pmag_list[temp-3000].plate;
	    if (temp>9999) printf("te[%d]: %d  \n",i+1,cur_elem->tectonic_element);
	    if (i<5) {
		if (temp>9999) info_te[i] = cur_elem->tectonic_element;
		if (temp>2999 && temp<5000) info_te[i] = pmag_list[temp-3000].plate;
		primitive[i] = temp;
		filename[i]=cur_elem->header;	
/*		sprintf(tempname[i],filename[i]);
		printf("File: %s  \n",tempname[i]);  */
	    }
	    ptr++;
	}
    }

    if (hits_up) XtUnmanageChild(helpentry); 
    sprintf(text,"Hits = %d",hits);
    helpentry = XmCreatePushButton(popinfo,text,NULL,0);
    XtAddCallback(helpentry, XmNactivateCallback, infoCB,0);	
    XtManageChild(helpentry);

    SetPickButtons(hits,hits_before, primitive, info_te, filename); 

    hits_up = TRUE;
    hits_before = hits;
/*    pick_elem = primitive[0]; */
    pick_plate = info_te[0]; 



}


#define BUFSIZE 512

void pickStuff(x,y)
int x,y;
{
    GLuint selectBuf[BUFSIZE];
    GLint hits;
    GLint viewport[4];

        glGetIntegerv (GL_VIEWPORT, viewport);

        glSelectBuffer (BUFSIZE, selectBuf);
        (void) glRenderMode (GL_SELECT);

        glInitNames();
/*        glPushName(-1);	*/

        glMatrixMode (GL_PROJECTION);
        glPushMatrix ();
        glLoadIdentity ();
/*  create 5x5 pixel picking region near cursor location	*/
        gluPickMatrix ((GLdouble) x, (GLdouble) (viewport[3] - y), 
	    5.0, 5.0, viewport);
	if (projection) glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 200);
	else glOrtho(-1.0, 1.0, -1.0, 1.0, 1.0, 200); 

	glMatrixMode(GL_MODELVIEW);	


/*        gluOrtho2D (0.0, 3.0, 0.0, 3.0);	*/
/*        drawSquares (GL_SELECT);	*/
	select_mode = TRUE;
	draw();
	select_mode = FALSE;

        glMatrixMode (GL_PROJECTION);
        glPopMatrix ();
        glFlush ();

        hits = glRenderMode (GL_RENDER);
        processHits (hits, selectBuf);

	glMatrixMode(GL_MODELVIEW);	

}

do_pick(pick)
XDeviceButtonEvent *pick;
{
    XButtonEvent *be;

/*
printf("Pick: Type %d \n",pick->type);
printf("Pick: x %d \n",pick->x);
printf("Pick: y %d \n",pick->y);
printf("Pick: x_root %d \n",pick->x_root);
printf("Pick: y_root %d \n",pick->y_root);
if (pick->window == pick->root) printf("Window = Root \n");
if (pick->window == pick->subwindow) printf("Window = Subwindow \n");
printf("Pick: root %d \n",(int)pick->root);
printf("Pick: window %d \n",(int)pick->window);
printf("Pick: subwindow %d \n",(int)pick->subwindow);
printf("Pick: state %d \n",pick->state);
printf("Pick: button %d \n",pick->button);
printf("Pick: device_state %d \n",pick->device_state);
printf("Pick: axes_count %d \n",pick->axes_count);
printf("Pick: first_axis %d \n",pick->first_axis);
printf("Pick: axis_data[0] %d \n",pick->axis_data[0]);
*/

    if ((int)pick->window==(int)draw_win) {
/*printf("it works \n");*/
    be=(XButtonEvent *)pick;

    if (pick->state == 2) {

/*	printf("Button 2 sir \n"); */
	switch (PickMode) {
	    case GET_INFO:  { printf("PICK (Get info) \n");
			      pickStuff(be->x,be->y);
			      print_info(pick_elem);
			    } break;
	    case MOVE_PLATE:{ printf("PICK (Move plate)\n");
			      pickStuff(be->x,be->y);
			      if (pick_elem != 2) 
			        moveplate(pick_plate);
			      else  
				printavgpole(pick_elem,pick_plate);
			      draw();
			    } break;
	    case DEL_LABEL: { printf("PICK (Delete label) \n");
			      
			    } break;
	    case DEL_PLATE: { printf("PICK (Delete plate)\n");
			      pickStuff(be->x,be->y);
			      delete_continent(pick_plate);
			      draw();
			    } break;
	    case DEL_ELEM:  { printf("PICK (Delete element)\n");
			      pickStuff(be->x,be->y);
			      delete_element(pick_elem-10000);
			      draw();
			    } break;
	    case DIS_PLATE: { printf("PICK (Display plate)\n");
			      pickStuff(be->x,be->y);
			      display_continent(pick_plate);
			      draw();
			    } break;
	    case DIS_ELEM:  { printf("PICK (Display element)\n");
			      pickStuff(be->x,be->y);
			      display_element(pick_elem-10000);
			      draw();			      
			    } break;
	    case 8:  	    { printf("PICK (Display color?)\n");
			      pickStuff(be->x,be->y);
			      change_info(pick_elem);
			    } break;
	}
    }
    }
}
