/******************************************************************************/
/* FILE: data.c                                                               */
/******************************************************************************/
/* This file contains the routines for building the PHIGS display structures  */
/* used in the program.                                                       */
/*                                                                            */
/******************************************************************************/
/* ROUTINES IN FILE:                                                          */
/*    init_data   - Initializes the PHIGS display data structures             */
/*    build_world - Builds the world outline                                  */
/*                                                                            */
/******************************************************************************/

#include <stdio.h>
#include <Xm/Xm.h>
#include <string.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include "paleotypes.h"
#include "paleodefs.h"

extern char *getenv();
extern Widget drawarea;
extern Display *dpy;

#define MAX_SETS 20000
#define RADS 0.017453293

extern float rot_angle[5][MAX_SETS];
extern float rot_x[5][MAX_SETS];
extern float rot_y[5][MAX_SETS];
extern float rot_z[5][MAX_SETS];

extern float pole_lat[5];
extern float pole_lon[5];

float pmagpolelat=0.0,pmagpolelon=0.0;
extern float rmolle[90];
GLfloat fctlpoints[4][4][3] = {{{0, -1, 0}, 
	{-1.33, -.94, 0}, {-1.33, .94, 0}, {0, 1, 0}},
    {{0, -1, 0},{-1.25, -.94, 1.83}, {-1.25, .94, 1.83}, {0, 1, 0}},
    {{0, -1, 0},{ 1.25, -.94, 1.83}, { 1.25, .94, 1.83}, {0, 1, 0}},
    {{0, -1, 0},{ 1.33, -.94, 0.00}, { 1.33, .94, 0.00}, {0, 1, 0}}};
GLfloat bctlpoints[4][4][3] = {{{0, -1, 0}, 
	{-1.33, -.94, 0}, {-1.33, .94, 0}, {0, 1, 0}},
    {{0, -1, 0},{-.94, -.94, -1.63}, {-.94, .94, -1.63}, {0, 1, 0}},
    {{0, -1, 0},{ .94, -.94, -1.63}, { .94, .94, -1.63}, {0, 1, 0}},
    {{0, -1, 0},{ 1.33, -.94, 0.00}, { 1.33, .94, 0.00}, {0, 1, 0}}};
GLfloat mappoints[4][4][3] = {{{-1, -.5, 0}, 
			 {-1   , -.167, 0}, {-1   , .167, 0}, {-1   , .5, 0}},
	{{-.333, -.5, 0},{-.333, -.167, 0}, {-.333, .167, 0}, {-.333, .5, 0}},
	{{ .333, -.5, 0},{ .333, -.167, 0}, { .333, .167, 0}, { .333, .5, 0}},
	{{ 1   , -.5, 0},{ 1   , -.167, 0}, { 1   , .167, 0}, { 1   , .5, 0}}};
GLUnurbsObj *theNurb;
GLfloat knots[8] = {0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0};

extern float value_rot_y,value_rot_x;
extern int mapproj;
extern int lithstat[27];
extern int lithenv[9];


/******************************************************************************/
/* ROUTINE: init_data                                                         */
/******************************************************************************/
/* This routine is used to initialize the PHIGS display structures.           */
/*                                                                            */
/******************************************************************************/
/* INPUT:                                                                     */
/*    None                                                                    */
/******************************************************************************/
/* OUTPUT:                                                                    */
/*    None                                                                    */
/******************************************************************************/
/* GLOBAL VARIABLES USED:                                                     */
/*    None                                                                    */
/******************************************************************************/
/* CALLS:                                                                     */
/*    None                                                                    */
/******************************************************************************/

init_data()
{
    char                *directory, filename[128];
    FILE      		*infile;
    static Ppoint 	text_loc = { 0.65, 0.03 };
    static char  	text[]   = "PALEOMAPPER";
    static Ppoint3 	xhair1[] = { 0.0, -0.07, 1.0, 0.0,  0.07, 1.0 };
    static Ppoint3 	xhair2[] = { -0.07, 0.0, 1.0, 0.07, 0.0, 1.0 };
    int			i,j;


    *filename = '\0';

    strcat(filename,GRIDFILE);

	printf("Filename: %s \n",filename);

    if ((infile = fopen(filename,"r")) == NULL) {
        if ((infile = fopen(GRIDFILE,"r")) == NULL) {
            printf("Error opening %s\n",GRIDFILE);
            exit(1);
	}
    }
    build_world(OUTLINE,infile);
    fclose(infile);


/***  I'm redoing the plot pole structure here.  ***/

    glNewList(POLE,GL_COMPILE);
	glBegin(GL_LINE_STRIP);
			glVertex3f(0.0, -0.07, 1.0);
			glVertex3f(0.0,  0.07, 1.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
			glVertex3f(-0.07, 0.0, 1.0);
			glVertex3f(0.07, 0.0, 1.0 );
	glEnd();
    glEndList();

/***  Initializing the Poles   ***/

    for(i=0; i<5; i++)
    {
	pole_lat[i] = pole_lon[i] = 0.0;
    }

/***  Initialize the Rotations  ***/

  for(j=0; j<5; j++)

    for(i=0; i<MAX_SETS; i++)
    {
	rot_angle[j][i] = 0.0;
	rot_x[j][i] = 0.0;
	rot_y[j][i] = 0.0;
	rot_z[j][i] = 0.0;
    }

/***  Initialize the Lith Pick Toggle buttons  ***/

  for(j=0;j<27;j++)
	lithstat[j]=TRUE;
  for(j=0;j<9;j++)
	lithenv[j]=TRUE;

/***  Make the lithofacies Icons ***/

  make_lith_icon();

}


build_world(id, infile)
int id;
FILE *infile;
{
    Ppoint3 world[550];
    int drawint[550];
    char      	buf[80];
    int		i;
    int 	lat,lon;
    float	x,y,slat,slon;

    printf("Building world");

    glXSwapBuffers(dpy, XtWindow(drawarea));

    i = 1;

    while (fgets(buf,80,infile) != NULL) {
	sscanf(buf,"%f %f %f %d",&world[i].x,&world[i].y,&world[i].z,&drawint[i]);
	printf(".");
	i++;
    }

    printf("done!\n");

    glNewList(OUTLINE,GL_COMPILE);
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_STRIP);
	for(i=1;i<514;i++)
	{
	    if (drawint[i]<1)
	      {
		    glEnd();
		    glBegin(GL_LINE_STRIP);
			glVertex3f((world[i].x),(world[i].y),(world[i].z));
	      }
	    else	glVertex3f((world[i].x),(world[i].y),(world[i].z));

	}
	glEnd();
    glEndList();

    glNewList(MAPGRID,GL_COMPILE);
	glColor3f(1.0, 1.0, 1.0);
	for(lat=-90;lat<100;lat=lat+30) {
	    for(lon=-180;lon<190;lon=lon+45) {
		glBegin(GL_LINE_STRIP);
		    glVertex3f((lon+2)/180.0,lat/180.0,0.0);
		    glVertex3f((lon-2)/180.0,lat/180.0,0.0);
		glEnd();
		glBegin(GL_LINE_STRIP);
		    glVertex3f(lon/180.0,(lat+2)/180.0,0.0);
		    glVertex3f(lon/180.0,(lat-2)/180.0,0.0);
		glEnd();
	    }
	}
	glBegin(GL_LINE_LOOP);
	    glVertex3f( 180.0/180.0, 90.0/180.0,0.0);
	    glVertex3f( 180.0/180.0,-90.0/180.0,0.0);
	    glVertex3f(-180.0/180.0,-90.0/180.0,0.0);
	    glVertex3f(-180.0/180.0, 90.0/180.0,0.0);
	glEnd();
    glEndList();

    glNewList(MERCA,GL_COMPILE);
	glColor3f(1.0, 1.0, 1.0);
	for(lat=-90;lat<100;lat=lat+30) {
	    for(lon=-180;lon<190;lon=lon+45) {
		x = lon*RADS;
	    if (lat<85 || lat>-85)
		y = log(tan((45.0*RADS)+(lat*RADS/2)));
	    if (lat>85)
		y = log(tan((45.0*RADS)+(85.0*RADS/2)));
	    if (lat<-85)
		y = log(tan((45.0*RADS)+(-85.0*RADS/2)));

		glBegin(GL_LINE_STRIP);
		    glVertex2f((x+.11),y);
		    glVertex2f((x-.11),y);
		glEnd();
		glBegin(GL_LINE_STRIP);
		    glVertex2f(x,(y+.11));
		    glVertex2f(x,(y-.11));
		glEnd();
	    }
	}
    glEndList();

/***  Make the Globe  ***/
    glNewList(GLOBE,GL_COMPILE);
	glColor3f(0.0, 0.0, 0.5);
    	theNurb = gluNewNurbsRenderer();
    	gluNurbsProperty(theNurb, GLU_SAMPLING_TOLERANCE, 25.0);
    	gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_FILL);
/*    gluBeginSurface(theNurb);
    gluNurbsSurface(theNurb, 
	    8, knots,
	    8, knots,
	    4 * 3,
	    3,
	    &fctlpoints[0][0][0], 
	    4, 4,
	    GL_MAP2_VERTEX_3);
    gluEndSurface(theNurb);
    gluBeginSurface(theNurb);
    gluNurbsSurface(theNurb, 
	    8, knots,
	    8, knots,
	    4 * 3,
	    3,
	    &bctlpoints[0][0][0], 
	    4, 4,
	    GL_MAP2_VERTEX_3);
    gluEndSurface(theNurb);	*/
    gluBeginSurface(theNurb);
    gluNurbsSurface(theNurb, 
	    8, knots,
	    8, knots,
	    4 * 3,
	    3,
	    &mappoints[0][0][0], 
	    4, 4,
	    GL_MAP2_VERTEX_3);
    gluEndSurface(theNurb);


    glEndList();


}


init_map()
{
    int im;
    float m,n,tempn,ticlen;
    float chi,chihi,chilo,diff,x,y;
    int 	lat,lon,count;
    float	slat,slon;


  if (mapproj == 2) {
    glNewList(MOLLY,GL_COMPILE);
	glColor3f(0.5,0.5,0.5);
	for(m=-60.0;m<=60.0;m=m+30.0) {
	glBegin(GL_LINE_STRIP);
	    for(n=-180.0;n<=180.0;n+=0.1) {

	    im=m;
	    if (im<0) im=-im;
	    if (im==0) chilo=0.0;
	    else chilo=rmolle[im-1];
	    if (im==90) chihi=90.0;
	    else chihi=rmolle[im];
	    if (m<0) diff=0.0-m-im;
	    else diff=m-im;
	    chi=chilo+diff*(chihi-chilo);
	    if (m<0) chi=0.0-chi;
	    x = 2.8284171*(n/180.0)*cos(chi*RADS);
	    y = 1.4142135*sin(chi*RADS);
	    glVertex2f(x,y);
	    }
	glEnd();
        }

	for(n=-180.0;n<=180.0;n+=45.0) {
	glBegin(GL_LINE_STRIP);
	    for(m=-90.0;m<=90.0;m+=0.2) {

	    tempn=n+value_rot_y;
	    if (tempn>225.0) tempn-=405.0;
	    if (tempn<-225.0) tempn+=405.0;
	    if (tempn>180.0 && tempn<225.0) break;
	    if (tempn<-180.0 && tempn>-225.0) tempn=tempn+360.0;

	    im=m;
	    if (im<0) im=-im;
	    if (im==0) chilo=0.0;
	    else chilo=rmolle[im-1];
	    if (im==90) chihi=90.0;
	    else chihi=rmolle[im];
	    if (m<0.0) diff=0.0-m-im;
	    else diff=m-im;
	    chi=chilo+diff*(chihi-chilo);
	    if (m<0.0) chi=0.0-chi;
	    x = 2.8284171*(tempn/180.0)*cos(chi*RADS);
	    y = 1.4142135*sin(chi*RADS);
	    glVertex2f(x,y);
	    }
	glEnd();
        }

 	for(n=-180.0;n<=180.0;n+=360.0) {
	glBegin(GL_LINE_STRIP);
	    for(m=-90.0;m<=90.0;m+=0.2) {

	    im=m;
	    if (im<0) im=-im;
	    if (im==0) chilo=0.0;
	    else chilo=rmolle[im-1];
	    if (im==90) chihi=90.0;
	    else chihi=rmolle[im];
	    if (m<0.0) diff=0.0-m-im;
	    else diff=m-im;
	    chi=chilo+diff*(chihi-chilo);
	    if (m<0.0) chi=0.0-chi;
	    x = 2.8284171*(n/180.0)*cos(chi*RADS);
	    y = 1.4142135*sin(chi*RADS);
	    glVertex2f(x,y);
	    }
	glEnd();
        }
    glEndList();
  }

  if (mapproj == 4) {
    glNewList(STEREO,GL_COMPILE);
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(4,0xAAAA); 
	glColor3f(1.0, 1.0, 1.0);
	for(lat=-90;lat<100;lat+=30) {
	  if (lat==30) glEnable(GL_LINE_STIPPLE);
	  if (lat==0) glDisable(GL_LINE_STIPPLE);
	  glBegin(GL_LINE_STRIP);
	    for(n=-180;n<=180.0;n+=.1) {
	    if (lat<0 && value_rot_x>=0.0) break;
	    if (lat>0 && value_rot_x<0.0) break; 
	    if (lat<0) slat=-(lat*RADS);
	    else slat=(lat*RADS);
	    slon=n*RADS;
	    y = (cos(slat))*(sin(slon))/(1.0+(sin(slat)));
	    x = (cos(slat))*(cos(slon))/(1.0+(sin(slat)));
	    glVertex2f(x,y);
	    }
	  glEnd();
	}
	glDisable(GL_LINE_STIPPLE);

	lat = 0;
	    for(lon=-180;lon<=180;lon+=10) {
	    slat=(lat*RADS);
	    slon=lon*RADS;
	    y = (cos(slat))*(sin(slon))/(1.0+(sin(slat)));
	    x = (cos(slat))*(cos(slon))/(1.0+(sin(slat)));
	    ticlen=.02;
	    if (lon/90.0==(int)(lon/90.0)) ticlen=.04;
	    if (lon==0) ticlen=.06;
		glBegin(GL_LINE_STRIP);
		    glVertex2f(x+ticlen*cos(lon*RADS),y+ticlen*sin(lon*RADS));
		    glVertex2f(x-ticlen*cos(lon*RADS),y-ticlen*sin(lon*RADS));
		glEnd();
	    }
	
    glEndList();
  }

    make_lith_icon_map();
    send_map();
    virtual_value(1,0.0);
}

undo_map()
{

    make_lith_icon();
    return_map();
    virtual_value(1,0.0);

}
