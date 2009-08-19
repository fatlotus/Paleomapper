#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <math.h>
#include <Xm/Xm.h>
#include "paleotypes.h"
#include "paleodefs.h"
#include "paleoexterns.h"

#define MAX_SETS 20000
#define MAX_PMAG 2000
#define RADS 0.017453293

/***   DRAW   ***/
int draw_pole=FALSE;
int which_pole = 0;
float pole_lat[5];
float pole_lon[5];
float rot_angle[5][MAX_SETS];
float rot_x[5][MAX_SETS];
float rot_y[5][MAX_SETS];
float rot_z[5][MAX_SETS];
float rotplat_angle[5][900];
float rotplat_x[5][900];
float rotplat_y[5][900];
float rotplat_z[5][900];

float value_rot_x = 0.0, value_rot_y = 0.0, value_rot_z = 0.0;


/***   PMAG   ***/
extern pmag_dec();
extern int pmagnum;
int draw_pmag[MAX_PMAG];
int polar_wander=FALSE;
int check1, check2, check3;
int show_pmag_data=TRUE, show_pmag_pole=FALSE;
extern float pmagpolelat,pmagpolelon;
float pmag_angle[5][MAX_PMAG];
float pmag_x[5][MAX_PMAG],pmag_y[5][MAX_PMAG],pmag_z[5][MAX_PMAG];

/***   LITH   ***/
extern int lithnum;
extern int lithstat[27];
extern int lithenv[9];
extern int hidelithback,hidelithdata;

/***   LABELS   ***/
extern do_labels();
int hidelabels=FALSE;

/***   OPENGL   ***/
extern GLboolean select_mode;
extern Display 	*dpy;

/***   DRAW   ***/
extern Widget  drawarea;
int plate_alive[900];
extern int	vecsetnum;
extern int	graveyard;
extern int	alive[MAX_SETS];
float value_trans_x=0.0,value_trans_y=0.0,value_trans_z=0.0;
float value_size=0.9;
int mapproj;
int lowres=TRUE;
int grid_on=TRUE;
int globe;
int hidedata=FALSE;

draw()
{
    int i,j,k,count,drawlith,lithicon,color;
    float factor;
    int s[MAX_SETS],*helps;
    char tempstr[5];
   

/*    printf("Drawing! \n"); */

    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT);

    glTranslatef(0.0, 0.0, 1.7);
	if (select_mode) glPushName(20);
    if (!hidelabels) do_labels();
	if (select_mode) glPopName(); /* 20);*/
    glTranslatef(0.0, 0.0, -1.7);

    switch (mapproj) {
	case 0: glTranslatef(0.0, 0.0, value_trans_z-3.6);
	    	glRotatef(value_rot_x, 1.0, 0.0, 0.0);
	    	glRotatef(value_rot_y, 0.0, 1.0, 0.0);	
	    	glRotatef(value_rot_z, 0.0, 0.0, 1.0);
		glScalef(value_size,value_size,value_size);
/*		if (globe) glCallList(GLOBE);*/
		if (grid_on) glCallList(OUTLINE);
		break;
	case 1: glTranslatef(value_trans_x, value_trans_y, -1.6);		
		glScalef(value_size,value_size,1.0);
		if (globe) glCallList(GLOBE);
		if (grid_on) glCallList(MAPGRID);
		break;
	case 2: glTranslatef(value_trans_x, value_trans_y, -1.6);		
		glScalef(value_size/4.0,value_size/4.0,1.0);
		if (grid_on) glCallList(MOLLY); 
		break;
	case 3: glTranslatef(value_trans_x, value_trans_y, -1.6);		
		glScalef(value_size/5.0,value_size/5.0,1.0);
		if (grid_on) glCallList(MERCA);
		break;
	case 4: glTranslatef(value_trans_x, value_trans_y, -1.6);		
	    	if (value_rot_x>=0.0) glRotatef(value_rot_y-90.0, 0.0, 0.0, 1.0);
		else glRotatef(value_rot_y+90.0, 0.0, 0.0, 1.0);
		glScalef(value_size,value_size,1.0);
		if (grid_on) glCallList(STEREO);
		break;
	default: glTranslatef(value_trans_x, value_trans_y, -1.6);		
		glScalef(value_size,value_size,1.0);
		if (grid_on) glCallList(MAPGRID);
		break;
    }




    /**  CallList requires a positive integer  **/
    /*  so all the elements/vecsets are offset  */
    /*  by a certain number found in paleodefs. */
    /*   ie. (j+DATA_START) = (j+1000)		*/
    /*  1-10 are grids and symbols              */
    /*  200-? are characters for the fontlist   */
    /*  3000-4999 are PMAG or Polar Wander data */
    /*  5000-9999 are for Lithofacies data      */
    /*  10000+ are reserved for regular data    */

    if (draw_pole && !mapproj)
    {
	glColor3f(0.0, 0.8, 1.0);
	if (select_mode) glPushName(POLE);
	glRotatef(pole_lon[which_pole], 0.0, 1.0, 0.0);	
	glRotatef(0.0 - pole_lat[which_pole], 1.0, 0.0, 0.0);
	glCallList(POLE); 
	glRotatef(pole_lat[which_pole], 1.0, 0.0, 0.0);
	glRotatef(0.0 - pole_lon[which_pole], 0.0, 1.0, 0.0);	
	if (select_mode) glPopName(); /* POLE); */	
    }

    if (pmagnum) {
    	if (check1 || check2 || check3) pmag_dec();
    	glColor3f(0.0, 0.8, 1.0);
    	if (show_pmag_data)
    	    for(j=0;j<pmagnum;j++) if (draw_pmag[j]) {
		switch(draw_pmag[j]) {
		    case 1: glColor3f(1.0, 0.0, 0.0);
			    break;
		    case 2: glColor3f(0.0, 1.0, 0.0);
			    break;
		    case 3: glColor3f(0.0, 0.0, 1.0);
			    break;
		}
		if (!mapproj) {
		    if (select_mode) glPushName(j+PMAG_START);
		    for(k=4;k>-1;k--) glRotatef(pmag_angle[k][j],
				pmag_y[k][j],pmag_z[k][j],pmag_x[k][j]); 
		    glCallList(j+PMAG_START);
		    for(k=0;k<5;k++) glRotatef(0.0-pmag_angle[k][j],
				pmag_y[k][j],pmag_z[k][j],pmag_x[k][j]); 
		    if (select_mode) glPopName(); /* j+PMAG_START);	*/
		}
    	    }
    	if (show_pmag_pole && !mapproj) {
	    glColor3f(1.0, 0.0, 1.0);
	    if (select_mode) glPushName(PMAGPOLE);
	    glRotatef(pmagpolelon, 0.0, 1.0, 0.0);	
	    glRotatef(0.0 - pmagpolelat, 1.0, 0.0, 0.0);
	    glCallList(POLE); 
	    glRotatef(pmagpolelat, 1.0, 0.0, 0.0);
	    glRotatef(0.0 - pmagpolelon, 0.0, 1.0, 0.0);	
	    if (select_mode) glPopName(); /* PMAGPOLE); */	
	    /*	printf("lat %.3f lon %.3f \n",pmagpolelat,pmagpolelon); */
    	}
    	if (polar_wander && !mapproj) {
	    /* printf("Draw Polar Wander\n"); */
	    /* make_polar_wander(); */
	    glCallList(PMAG_START-1);
    	}
    }

    if (lithnum) {
        glColor3f(1.0, 1.0, 1.0);
	draw_lith();
	if (!mapproj && !hidelithback) glCallList(LITH_ICON-1);
    }

    for(j=1;j<(vecsetnum+1);j++) 
    {

      if ((alive[j] && !graveyard) || (!alive[j] && graveyard))
      {

	if (select_mode) glPushName(j+DATA_START);

	rot_plate(TRUE,&j);

	get_elem(10000+j);
	getcolor(cur_elem->color);

	if (!hidedata) glCallList(j+DATA_START);

	rot_plate(FALSE,&j);

	if (select_mode) glPopName(); /* j+DATA_START); */	

      }
    }

    if (!select_mode) glXSwapBuffers(dpy, XtWindow(drawarea));  
        glFlush(); 
}

drawpole()
{
    glLoadIdentity();

    glTranslatef(value_trans_x, value_trans_y, value_trans_z-3.5);		

    glRotatef(value_rot_x, 1.0, 0.0, 0.0);
    glRotatef(value_rot_y, 0.0, 1.0, 0.0);	
    glRotatef(value_rot_z, 0.0, 0.0, 1.0);

    glScalef(value_size,value_size,value_size);

	glRotatef(pole_lon[which_pole], 0.0, 1.0, 0.0);	
	glRotatef(0.0 - pole_lat[which_pole], 1.0, 0.0, 0.0);
	glCallList(POLE); 
	glRotatef(pole_lat[which_pole], 1.0, 0.0, 0.0);
	glRotatef(0.0 - pole_lon[which_pole], 0.0, 1.0, 0.0);	

    glFlush(); 
}


getcolor(num)
int num;
{
	switch (num) 
	{
	    case 1: glColor3f(0.4, 0.1, 0.4);
			break;
	    case 2: glColor3f(0.0, 0.0, 1.0);
			break;
	    case 3: glColor3f(0.0, 1.0, 0.0);
			break;
	    case 4: glColor3f(1.0, 1.0, 0.0);
			break;
	    case 5: glColor3f(1.0, 0.5, 0.1);
			break;
	    case 6: glColor3f(1.0, 0.0, 0.0);
			break;
/*	    case 7: glColor3f(0.3, 0.3, 0.1); */
	    case 7: glColor3f(1.0, 1.0, 1.0);
			break;
	    default: glColor3f(1.0, 1.0, 1.0);
			break;
	}
}


rot_plate(forward,num)
int forward,*num;
{
    int j,k;

    j=*num;

  if (!mapproj) 
    if (forward)
    {
	for(k=4;k>-1;k--) glRotatef(rot_angle[k][j],rot_y[k][j],rot_z[k][j],rot_x[k][j]); 
    }
    else
    {
	for(k=0;k<5;k++) glRotatef(0.0-rot_angle[k][j],rot_y[k][j],rot_z[k][j],rot_x[k][j]); 
    }

}

draw_lith()
{
    int i,j,k,count,drawlith,drawenv,lithicon,color,end,begin,im,newpen;
    float factor,x,y,chi,chihi,chilo,diff,lon,slat,slon,reallat;
    char tempstr[5];
    float rmolle[90] = 
	{0.8,1.6,2.4,3.1,3.9,4.7,5.5,6.3,7.1,7.9,8.7,9.4,10.2, 
	11.0,11.8,12.6,13.4,14.2,15.0,15.8,16.6,17.4,18.2,19.0,19.8,20.6, 
	21.4,22.2,23.0,23.8,24.6,25.5,26.3,27.1,27.9,28.7,29.6,30.4,31.2, 
	32.1,32.9,33.8,34.6,35.5,36.3,37.2,38.0,38.9,39.8,40.6,41.5,42.4, 
	43.3,44.2,45.1,46.0,46.9,47.8,48.7,49.7,50.6,51.6,52.5,53.5,54.5,
	55.5,56.5,57.5,58.5,59.5,60.6,61.6,62.7,63.8,65.0,66.1,67.3,68.5, 
	69.7,71.0,72.3,73.6,75.0,76.5,78.1,79.7,81.5,83.5,85.9,90.0};

    for(j=0;j<lithnum;j++) {
	if (!hidelithdata) {
	    if (select_mode) glPushName(j+LITH_START);
	    drawlith=FALSE;
	    drawenv=FALSE;
	    begin=lith_list[j].envbegin;
	    end=lith_list[j].envend;
	    lithicon=0;
	    strcpy(tempstr,lith_list[j].type);
	    for(i=0;i<strlen(tempstr);i++) {
	      if ((int)tempstr[i]>64 && (int)tempstr[i]<91)
		if (lithstat[(int)tempstr[i]-64]) {
		    drawlith=TRUE;
	    	    if (tempstr[i]!='M' && tempstr[i]!='C' &&
		        tempstr[i]!='S' && tempstr[i]!='L') { 
		      lithicon=(int)tempstr[i]-64;
		      break; }
		}
	    }
	    if (end>begin) for(i=begin;i<=end;i++) {
		if (lithenv[i]) drawenv=TRUE; }
	      else for(i=end;i<=begin;i++) 
		if (lithenv[i]) drawenv=TRUE;
	    if (drawlith && drawenv) {
	      factor = 0.017455064/3.0;
	      if (end<begin) 
		color=end;
	      else color=begin;
	      switch (color) {
		case 0: glColor3f(.5,.1,.5); break;
		case 1: glColor3f(.5,.3,.75); break;
		case 2: glColor3f(.5,.5,1.0); break;
		case 3: glColor3f(0.0,1.0,1.0); break;
		case 4: glColor3f(0.0,1.0,0.5); break;
		case 5: glColor3f(0.0,1.0,0.0); break;
		case 6: glColor3f(1.0,1.0,0.0); break;
		case 7: glColor3f(1.0,0.6,0.1); break;
		case 8: glColor3f(1.0,0.0,0.0); break;
	      }

	      switch (mapproj) {
		case 0: for(k=4;k>-1;k--) 
			  glRotatef(rotplat_angle[k][lith_list[j].plate-100],
			  rotplat_y[k][lith_list[j].plate-100],
			  rotplat_z[k][lith_list[j].plate-100],
			  rotplat_x[k][lith_list[j].plate-100]); 
	        	glRotatef(lith_list[j].lon,0.0,1.0,0.0);
	        	glRotatef(0.0-lith_list[j].lat,1.0,0.0,0.0);
	      		break;
		case 1: glTranslatef(lith_list[j].lon/180, 
			  lith_list[j].lat/180, 0.0);
			break;
		case 2: lon=lith_list[j].lon+value_rot_y;
	    		if (lon >  180.0) lon-=360.0;
	    		if (lon < -180.0) lon+=360.0;
	    		im=(int)lith_list[j].lat;
	    		if (im<0) im=-im;
	    		if (im==0) chilo=0.0;
	    		else chilo=rmolle[im-1];
	    		if (im==90) chihi=90.0;
	    		else chihi=rmolle[im];
	    		if (lith_list[j].lat<0.0) diff=0.0-lith_list[j].lat-im;
	    		else diff=lith_list[j].lat-im;
	    		chi=chilo+diff*(chihi-chilo);
	    		if (lith_list[j].lat<0.0) chi=0.0-chi;
	    		x = 2.8284171*(lon/180.0)*cos(chi*RADS);
	    		y = 1.4142135*sin(chi*RADS);
			glTranslatef(x, y, 0.0);
	      		glScalef(3.0,3.0,1.0);
			break;
		case 3: x = lith_list[j].lon*RADS;
	    		if (lith_list[j].lat<85.0 || lith_list[j].lat>-85.0)
			    y = log(tan((45.0*RADS)+(lith_list[j].lat*RADS/2)));
	    		if (lith_list[j].lat>85.0)
			    y = log(tan((45.0*RADS)+(85.0*RADS/2)));
	    		if (lith_list[j].lat<-85.0)
			    y = log(tan((45.0*RADS)+(-85.0*RADS/2)));
			glTranslatef(x, y, 0.0);
	      		glScalef(5.0,5.0,1.0);
			break;
		case 4: reallat = value_rot_x - (int)(value_rot_x/360)*360;
	    		if (reallat>180.0) reallat=-reallat+2*(reallat-180);
	    		if (reallat<-180.0) reallat=-reallat-2*(reallat+180);
	    		if (lith_list[j].lat<0) slat=-(lith_list[j].lat*RADS);
	    		else slat=(lith_list[j].lat*RADS);
	    		slon=lith_list[j].lon*RADS;
	    		y = (cos(slat))*(sin(slon))/(1.0+(sin(slat)));
	    		x = (cos(slat))*(cos(slon))/(1.0+(sin(slat)));
/*	    		if (value_rot_x<0.0) glRotatef(-180.0,0.0,0.0,1.0);
*/
			glTranslatef(x, y, 0.0);
			break;
	      }

	      glScalef(factor,factor,1.0);
		glCallList(LITH_ICON+lithicon);
	      glScalef(1.0/factor,1.0/factor,1.0);

	      switch (mapproj) {
		case 0: glRotatef(lith_list[j].lat,1.0,0.0,0.0);
	        	glRotatef(0.0-lith_list[j].lon,0.0,1.0,0.0);
	        	for(k=0;k<5;k++) 
			  glRotatef(-rotplat_angle[k][lith_list[j].plate-100],
			  rotplat_y[k][lith_list[j].plate-100],
			  rotplat_z[k][lith_list[j].plate-100],
			  rotplat_x[k][lith_list[j].plate-100]); 
	      		break;
		case 1: glTranslatef(-lith_list[j].lon/180, 
			  -lith_list[j].lat/180, 0.0);
			break;
		case 2: glScalef(1/3.0,1/3.0,1.0);
	      		glTranslatef(-x, -y, 0.0);
			break;
		case 3: glScalef(1/5.0,1/5.0,1.0);
	      		glTranslatef(-x, -y, 0.0);
			break;
		case 4: glTranslatef(-x, -y, 0.0);
			break;
	      }
	    }
	    if (select_mode) glPopName(); /* j+LITH_START); */	
	}
    }

}
