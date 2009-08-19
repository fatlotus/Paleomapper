/*
c This subroutine is design to take the tectonic group number,the
c sub picture number,and produces a named vector, sets it color, 
c pick condition, pickid, etc. and then sends it to the E&S.
c
c It was written by Malcolm I. Ross on June 26, 1986 
c    Copyrighted on 10/1/1986 by Malcolm I. Ross
c         all rights reserved
*/

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <math.h>
#include "paleotypes.h"
#include "paleodefs.h"
#include "paleoexterns.h"

#define MAX_VECS 20000
#define MAX_SETS 20000
#define MAX_HOLD 180
#define MAX_PMAG 2000
#define RADS 0.017453293


float maplat[100000],maplon[100000];
int	mappen[100000];
int	mapsetfinder[MAX_SETS];

extern int mapproj;
extern int vecsetnum;
extern int pmagnum;
float  pmag_age[MAX_PMAG];
extern float value_rot_y,value_rot_x;

float rmolle[90] = 
	{0.8,1.6,2.4,3.1,3.9,4.7,5.5,6.3,7.1,7.9,8.7,9.4,10.2, 
	11.0,11.8,12.6,13.4,14.2,15.0,15.8,16.6,17.4,18.2,19.0,19.8,20.6, 
	21.4,22.2,23.0,23.8,24.6,25.5,26.3,27.1,27.9,28.7,29.6,30.4,31.2, 
	32.1,32.9,33.8,34.6,35.5,36.3,37.2,38.0,38.9,39.8,40.6,41.5,42.4, 
	43.3,44.2,45.1,46.0,46.9,47.8,48.7,49.7,50.6,51.6,52.5,53.5,54.5,
	55.5,56.5,57.5,58.5,59.5,60.6,61.6,62.7,63.8,65.0,66.1,67.3,68.5, 
	69.7,71.0,72.3,73.6,75.0,76.5,78.1,79.7,81.5,83.5,85.9,90.0};

extern getcolor();
extern int polygons;



send_vector(nov, vectors , poslin, append, new_cont)
int nov, append, new_cont;
Ppoint3 vectors[MAX_VECS];
int poslin[MAX_VECS];
{
    CONTINENT	*cur_cont;
    PLATE	*cur_plate;
    Ppoint3     *ptr;
    int   	i, lastdraw, *drawint, start;
    float x,y,z;
GLUtriangulatorObj *tessobj;
GLdouble	vert[3];
GLfloat 	data[MAX_VECS][3];


    if (new_cont) {
        cur_cont = (CONTINENT *)malloc(sizeof(CONTINENT));
	cur_cont->latitude = cur_cont->longitude = cur_cont->angle = 0.;
	cur_cont->phigs_struct = Next_Phigs_Struct++;
        cont_list[cur_elem->tectonic_element] = cur_cont;

        cur_plate = (PLATE *)malloc(sizeof(PLATE));
	cur_plate->phigs_struct = Next_Phigs_Struct++;
	for(i=0;i<5;i++) {
	cur_plate->latitude[i] = cur_plate->longitude[i] = cur_plate->angle[i] = 0.;
        cur_plate->pole_age[i] = 0;
	}
	plate_list[cur_elem->tectonic_element] = cur_plate;
	    


/*        cont_list[cur_elem->continent] = cur_cont;*/

    }


/**********   Opeg GL Stuff   *************/

    if (!polygons) {
/*  printf("NO POLYGONS  %d\n",polygons);*/
    glNewList(vecsetnum+10000,GL_COMPILE);
/*	getcolor(cur_elem->color);*/
        glBegin(GL_LINE_STRIP);
        for(i=0;i<nov;i++)
        {
            if (!poslin[i])
    	    {
    	        glEnd();
	        glBegin(GL_LINE_STRIP);
	            glVertex3f((vectors[i].x),(vectors[i].y),(vectors[i].z));
	    }
            else if (poslin[i]==3) {
    	    	glEnd();
        	glBegin(GL_POINTS);
	        glVertex3f((vectors[i].x),(vectors[i].y),(vectors[i].z)); 
            }
            else {  glVertex3f((vectors[i].x),(vectors[i].y),(vectors[i].z));
	    }
        }
    glEnd();
    glEndList();
    }

    else {
    glNewList(vecsetnum+10000,GL_COMPILE);
/*	getcolor(cur_elem->color);
        glColor3f(0.3, 0.3, 0.3);*/
	tessobj=gluNewTess();
	gluTessCallback(tessobj, GLU_BEGIN, glBegin);
	gluTessCallback(tessobj, GLU_VERTEX, glVertex3fv);
	gluTessCallback(tessobj, GLU_END, glEnd);
	gluBeginPolygon(tessobj);
        for(i=0;i<nov;i++)
        {
/*printf("%d ",i);*/
/*printf("poslin[i] %d \n",poslin[i]);*/
	    if (!(poslin[i]-10))
	    {
/*		    if ((i-start)==2) {
			printf("adding points\n");
			gluTessVertex(tessobj, vert, &data[i-2]);
		    } 
*/
	        gluEndPolygon(tessobj);
	        start=i;
/*printf("poslin[i+2] %d \n",poslin[i+2]);*/
	        gluDeleteTess(tessobj);
	        tessobj=gluNewTess();
	        gluTessCallback(tessobj, GLU_BEGIN, glBegin);
	        gluTessCallback(tessobj, GLU_VERTEX, glVertex3fv);
	        gluTessCallback(tessobj, GLU_END, glEnd);
	        gluBeginPolygon(tessobj);
	          vert[0]=vectors[i].x; vert[1]=vectors[i].y; vert[2]=vectors[i].z; 
	          data[i][0]=vert[0]; data[i][1]=vert[1]; data[i][2]=vert[2];
		  gluTessVertex(tessobj, vert, &data[i]);
	    }
            else {  
        	    vert[0]=vectors[i].x; vert[1]=vectors[i].y; vert[2]=vectors[i].z; 
        	    data[i][0]=vert[0]; data[i][1]=vert[1]; data[i][2]=vert[2];
		    if (vert[0]!=data[start][0] || vert[1]!=data[start][1] || vert[2]!=data[start][2])
		        gluTessVertex(tessobj, vert, &data[i]);
	    }
	}	    
/*		    if ((i-start)==2) {
			printf("end adding points\n");
			gluTessVertex(tessobj, vert, &data[i-2]);
		    }
*/	
	gluEndPolygon(tessobj);

        gluDeleteTess(tessobj);   

/*printf("\n");*/
    glEndList();
    }

}

send_map()
{
    float px,py,pz,x,y,slat,slon,reallat;
    int i,j,k,n,newpen,start;
    GLUtriangulatorObj *tessobj;
    GLdouble	vert[3];
    GLfloat 	data[MAX_VECS][3];
    float alat,alon,anlat,anlon;
    float a1,a2,a3,a4,a5,rx,ry,rz,gx,gy,gz,vct,sina,cosa,sqrz;
    float displaylat[20000],displaylon[20000],displaypen[20000];
    float maprotlat[5],maprotlon[5],maprotang[5];
    int im;
    float chi,chihi,chilo,diff;

    i=0;j=0;k=0;n=0;

 
    for(i=1;i<vecsetnum+1;i++) {
	get_elem(10000+i);
/*printf("i %d msf %d vsn %d \n",i,mapsetfinder[i],cur_elem->phigs_struct);*/
	k=0;
	for(j=mapsetfinder[i-1];j<mapsetfinder[i];j++) {

	maprotlat[0] = plate_list[cur_elem->tectonic_element]->latitude[0];
	maprotlon[0] = plate_list[cur_elem->tectonic_element]->longitude[0];
	maprotang[0] = plate_list[cur_elem->tectonic_element]->angle[0];

	alat=maplat[j];
	alon=maplon[j];

    	a1 = 90.0*RADS-alat*RADS;
	a2 = alon*RADS;
	px = sin(a1)*cos(a2);
	py = sin(a1)*sin(a2);
	pz = cos(a1);
/*printf("px %.3f py %.3f pz %.3f \n",px,py,pz);*/
	a3 = 90.0*RADS-maprotlat[0]*RADS;
	a4 = maprotang[0]*RADS;
	a5 = maprotlon[0]*RADS;
	gx = sin(a3)*cos(a5);
	gy = sin(a3)*sin(a5);
	gz = cos(a3);
	vct = (px*gx)+(py*gy)+(pz*gz);
	rx = cos(a4)*px+(1.0-cos(a4))*vct*gx+sin(a4)*(gy*pz-gz*py);
	ry = cos(a4)*py+(1.0-cos(a4))*vct*gy+sin(a4)*(gz*px-gx*pz);
	rz = cos(a4)*pz+(1.0-cos(a4))*vct*gz+sin(a4)*(gx*py-gy*px);
	sqrz= sqrt(1.0-rz*rz);
	if (rz==1.0 || sqrz==0.0) {cosa=0.0;/*printf("rz=1\n");*/}
	else {  sina=atan(rz/sqrz);
		cosa=((3.141592654/2.0-sina)*57.29578); }
	anlat = 90.0-cosa;
	anlon = 90.0-(atan2(rx,ry)*57.29578);
	if (anlon>180.0) anlon=anlon-360.0;
	displaylat[k]= anlat;
	displaylon[k] = anlon;
	displaypen[k] = mappen[j];
/*printf("i %d j %d k %d \n",i,j,k);*/
	k++;
	}

/**********   Opeg GL Stuff   *************/


    glNewList(i+10000,GL_COMPILE);
/*	getcolor(cur_elem->color); */
/*printf("color %d \n",cur_elem->color); */
    if (displaypen[k-1]<5 || mapproj==4) glBegin(GL_LINE_STRIP);
    else {
	tessobj=gluNewTess();
	gluTessCallback(tessobj, GLU_BEGIN, glBegin);
	gluTessCallback(tessobj, GLU_VERTEX, glVertex3fv);
	gluTessCallback(tessobj, GLU_END, glEnd);
	gluBeginPolygon(tessobj);
    }
/*      else glBegin(GL_TRIANGLES);	*/
        for(n=0;n<k;n++)
        {
    switch (mapproj) {
	case 1: x=displaylon[n]/180.0;
	    	y=displaylat[n]/180.0;
		
		break;
	case 2: 
	    displaylon[n]+=value_rot_y;
	    if (displaylon[n] >  180.0) displaylon[n]-=360.0;
	    if (displaylon[n] < -180.0) displaylon[n]+=360.0;
	    im=(int)displaylat[n];
	    if (im<0) im=-im;
	    if (im==0) chilo=0.0;
	    else chilo=rmolle[im-1];
	    if (im==90) chihi=90.0;
	    else chihi=rmolle[im];
	    if (displaylat[n]<0.0) diff=0.0-displaylat[n]-im;
	    else diff=displaylat[n]-im;
	    chi=chilo+diff*(chihi-chilo);
	    if (displaylat[n]<0.0) chi=0.0-chi;
	    x = 2.8284171*(displaylon[n]/180.0)*cos(chi*RADS);
	    y = 1.4142135*sin(chi*RADS);
/*printf("x %.3f y %.3f \n",x,y);*/
	    break;
	case 3:
	    x = displaylon[n]*RADS;
	    if (displaylat[n]<85.0 || displaylat[n]>-85.0)
		y = log(tan((45.0*RADS)+(displaylat[n]*RADS/2)));
	    if (displaylat[n]>85.0)
		y = log(tan((45.0*RADS)+(85.0*RADS/2)));
	    if (displaylat[n]<-85.0)
		y = log(tan((45.0*RADS)+(-85.0*RADS/2)));
/*	    printf("lat %.3f lon %.3f x %.3f y %.3f \n",displaylat[n],displaylon[n],x,y);*/
	    break;
	case 4:
	    reallat = value_rot_x - (int)(value_rot_x/360)*360;
	    if (reallat>180.0) reallat=-reallat+2*(reallat-180);
	    if (reallat<-180.0) reallat=-reallat-2*(reallat+180);
	    if (displaylat[n]<0 && reallat>=0.0) {newpen = TRUE; break;}
	    if (displaylat[n]>0 && reallat<0.0) {newpen = TRUE; break;} 
	    if (newpen) { displaypen[n]=FALSE; newpen=FALSE; }
	    if (displaylat[n]<0) slat=-(displaylat[n]*RADS);
	    else slat=(displaylat[n]*RADS);
	    slon=displaylon[n]*RADS;
	    y = (cos(slat))*(sin(slon))/(1.0+(sin(slat)));
	    x = (cos(slat))*(cos(slon))/(1.0+(sin(slat)));
	    if (value_rot_x<0.0)  y=y*-1.0;
    }

    if (displaypen[k-1]<5 || mapproj==4) {
/*printf("NO POLYGONS\n");*/
        if (!displaypen[n] )
    	{
    	    glEnd();
	    glBegin(GL_LINE_STRIP);
	        glVertex2f(x,y);
	}
          else  glVertex2f(x,y);
    }

      else {
/*printf("POLYGONS\n");*/
	   if (!(displaypen[n]-10))
	    {
	        gluEndPolygon(tessobj);
	        start=n;
	        gluDeleteTess(tessobj);
	        tessobj=gluNewTess();
	        gluTessCallback(tessobj, GLU_BEGIN, glBegin);
	        gluTessCallback(tessobj, GLU_VERTEX, glVertex3fv);
	        gluTessCallback(tessobj, GLU_END, glEnd);
	        gluBeginPolygon(tessobj);
	          vert[0]=x; vert[1]=y; vert[2]=0.0; 
	          data[n][0]=vert[0]; data[n][1]=vert[1]; data[n][2]=vert[2];
		  gluTessVertex(tessobj, vert, &data[n]);
	    }
            else {  
        	    vert[0]=x; vert[1]=y; vert[2]=0.0; 
        	    data[n][0]=vert[0]; data[n][1]=vert[1]; data[n][2]=vert[2];
		    if (vert[0]!=data[start][0] || vert[1]!=data[start][1] || vert[2]!=data[start][2])
		        gluTessVertex(tessobj, vert, &data[n]);
	    }
      }
    }
    if (displaypen[k-1]<5 || mapproj==4) glEnd();
    else {
	gluEndPolygon(tessobj);
        gluDeleteTess(tessobj);   
    }
    glEndList();

    }


}

return_map()
{
double x,y,z,phi,theta;
int i,j,start;
GLUtriangulatorObj *tessobj;
GLdouble	vert[3];
GLfloat 	data[MAX_VECS][3];

  for(i=1;i<vecsetnum+1;i++) {
    get_elem(10000+i);

    glNewList(i+10000,GL_COMPILE);
/*    getcolor(cur_elem->color);*/
      if (mappen[mapsetfinder[i-1]]<5) {
	glBegin(GL_LINE_STRIP);
      	for(j=mapsetfinder[i-1];j<mapsetfinder[i];j++) {

            phi = 0.017453292 * (90.0 - maplat[j]);
            theta = 0.017453292 * maplon[j];
            x = cos(theta) * sin(phi);
            y = sin(theta) * sin(phi);
            z = cos(phi);

            if (!mappen[j]) {
    	        glEnd();
	        glBegin(GL_LINE_STRIP);
	            glVertex3f((float)y,(float)z,(float)x);
	    }
            else if (mappen[j]==3) {
    	    	glEnd();
        	glBegin(GL_POINTS);
	        glVertex3f((float)y,(float)z,(float)x); 
            }
            else   glVertex3f((float)y,(float)z,(float)x);
        }
        glEnd();
      }
      else {
/*	getcolor(cur_elem->color);
        glColor3f(0.3, 0.3, 0.3);*/
	tessobj=gluNewTess();
	gluTessCallback(tessobj, GLU_BEGIN, glBegin);
	gluTessCallback(tessobj, GLU_VERTEX, glVertex3fv);
	gluTessCallback(tessobj, GLU_END, glEnd);
	gluBeginPolygon(tessobj);
      	for(j=mapsetfinder[i-1];j<mapsetfinder[i];j++) 
        {

            phi = 0.017453292 * (90.0 - maplat[j]);
            theta = 0.017453292 * maplon[j];
            x = cos(theta) * sin(phi);
            y = sin(theta) * sin(phi);
            z = cos(phi);

/*printf("%d ",i);*/
/*printf("poslin[i] %d \n",poslin[i]);*/
	    if (!(mappen[j]-10))
	    {
	        gluEndPolygon(tessobj);
	        start=j;
	        gluDeleteTess(tessobj);
	        tessobj=gluNewTess();
	        gluTessCallback(tessobj, GLU_BEGIN, glBegin);
	        gluTessCallback(tessobj, GLU_VERTEX, glVertex3fv);
	        gluTessCallback(tessobj, GLU_END, glEnd);
	        gluBeginPolygon(tessobj);
	          vert[0]=(float)y; vert[1]=(float)z; vert[2]=(float)x; 
	          data[j][0]=vert[0]; data[j][1]=vert[1]; data[j][2]=vert[2];
		  gluTessVertex(tessobj, vert, &data[j]);
	    }
            else {  
        	    vert[0]=(float)y; vert[1]=(float)z; vert[2]=(float)x; 
        	    data[j][0]=vert[0]; data[j][1]=vert[1]; data[j][2]=vert[2];
		    if (vert[0]!=data[start][0] || vert[1]!=data[start][1] || vert[2]!=data[start][2])
		        gluTessVertex(tessobj, vert, &data[j]);
	    }
	}	    
	
	gluEndPolygon(tessobj);

        gluDeleteTess(tessobj);   

      }
    glEndList();
    }

}
		
