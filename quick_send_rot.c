#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include "paleotypes.h"
#include "paleodefs.h"
#include "paleoexterns.h"
#define  DEGRAD(x)	(3.1415926 * x / 180.0)

#define MAX_SETS 20000
#define MAX_PMAG 2000

extern float rot_angle[5][MAX_SETS];
extern float rot_x[5][MAX_SETS];
extern float rot_y[5][MAX_SETS];
extern float rot_z[5][MAX_SETS];
extern float rotplat_angle[5][900];
extern float rotplat_x[5][900];
extern float rotplat_y[5][900];
extern float rotplat_z[5][900];
extern float pmag_angle[5][MAX_PMAG];
extern float pmag_x[5][MAX_PMAG];
extern float pmag_y[5][MAX_PMAG];
extern float pmag_z[5][MAX_PMAG];

extern int pmagnum;

extern int my_color[MAX_SETS];

extern float pole_lat[5],pole_lon[5];
extern int which_pole;

extern int mapproj;

extern int	alive[MAX_SETS];
extern int	graveyard;

extern drawpole();
extern draw();





average_pole(cont_id)
int cont_id;
{
    int i;
    float sendlat,sendlon,sendang;
    float findlat,findlon,findang;
    float a,b,c;    
    PLATE *ptr; 

    ptr = plate_list[cont_id];

    sendlat=plate_list[cont_id]->latitude[0];
    sendlon=plate_list[cont_id]->longitude[0];
    sendang=plate_list[cont_id]->angle[0];
    for(i=1;i<5;i++) {
	a=plate_list[cont_id]->latitude[i];
	b=plate_list[cont_id]->longitude[i];
	c=plate_list[cont_id]->angle[i];
    	adder(sendlat,sendlon,sendang,a,b,c,&findlat,&findlon,&findang);
    	sendlat=findlat;
    	sendlon=findlon;
    	sendang=findang;
    	ptr->latitude[i+4]=findlat;
    	ptr->longitude[i+4]=findlon;
    	ptr->angle[i+4]=findang; 
    }

    
}


quick_send_rot(cont_id)
int cont_id;
{
    PLATE *plat;
    float lat, lon, angle;
    double x, y, z, phi, theta;
    ELEMENT *ptr;
    PMAG *pmag_ptr;
    int num;

    plat = plate_list[cont_id];
    if (!plat) return;

    lat = plat->latitude[which_pole];
    lon = plat->longitude[which_pole];
    angle = plat->angle[which_pole];


    if (!mapproj) {

            phi = 0.017453292 * (90.0 - lat);
            theta = 0.017453292 * lon;
            x = cos(theta) * sin(phi);
            y = sin(theta) * sin(phi);
            z = cos(phi);


    ptr = head_elem;
    while (ptr != NULL) {
        if (ptr->tectonic_element == cont_id) 
	{
	    num = ptr->phigs_struct;

	    rot_angle[which_pole][num] = angle;
	    rot_x[which_pole][num] = (float)x;
	    rot_y[which_pole][num] = (float)y;
	    rot_z[which_pole][num] = (float)z;
       	}
        ptr = ptr->next;
    }

	    rotplat_angle[which_pole][cont_id-100] = angle;
	    rotplat_x[which_pole][cont_id-100] = (float)x;
	    rotplat_y[which_pole][cont_id-100] = (float)y;
	    rotplat_z[which_pole][cont_id-100] = (float)z;

    num=0;
    for (num=0;num<pmagnum;num++) {
        if (pmag_list[num].plate == cont_id) 
	{
	    pmag_angle[which_pole][num] = angle;
	    pmag_x[which_pole][num] = (float)x;
	    pmag_y[which_pole][num] = (float)y;
	    pmag_z[which_pole][num] = (float)z;
	}
    }

    }
    else {

      ptr = head_elem;
      while (ptr != NULL) {
        if (ptr->tectonic_element == cont_id) 
	{
	    num = ptr->phigs_struct;

	    rot_angle[which_pole][num] = angle;
	    rot_x[which_pole][num] = lon/180.0;
	    rot_y[which_pole][num] = lat/180.0;
	    rot_z[which_pole][num] = cont_id;
       	}
        ptr = ptr->next;
      }
    }	
    
    pole_lon[which_pole] = plat->longitude[which_pole];
    pole_lat[which_pole] = plat->latitude[which_pole];

/*    glColor3f(0.0, 0.0, 0.0);

    drawpole();

    pole_lon[which_pole] = plat->longitude[which_pole];
    pole_lat[which_pole] = plat->latitude[which_pole];

    glColor3f(0.0, 0.8, 1.0);

    drawpole();
*/

}
