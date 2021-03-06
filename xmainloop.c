#include <Xm/Xm.h>
#include <Intrinsic.h>
#include "pex.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

extern XID  dials_id;
extern XtAppContext app;
extern float value_trans_x,value_trans_y,value_trans_z;
extern float value_rot_x,value_rot_y,value_rot_z;
extern float value_size;

extern Widget  dial1_text,dial2_text,dial3_text,dial4_text;
extern Widget  dial5_text,dial6_text,dial7_text,dial8_text;
extern Widget scale[8];
extern float value_scale[8];

extern int clipping;
int fog_on=TRUE;
extern int mapproj;

extern updateplate();
extern get_persp();

float rot_speed=5.0;

extern float modsize;
float dialrotx=0.0,dialroty=0.0,dialrotz=0.0,dialsize=0.9;
float old_rot_x,old_rot_y,old_rot_z,old_size;
float old_rotlat,old_rotlon,old_rotang;
float old_trans_x,old_trans_y,size_scale,old_scale=2.5;

getdialval(event, val, dev)
int *dev;
int *val;
XDeviceMotionEvent *event;
{
	printf("obselete method called: getdialval\n");
	exit(1);
}

mod_value(dev,value)
int dev,value;
{
	printf("obselete method called: mod_value\n");
	exit(1);
}

display_dialval()
{
    char text[10];

    sprintf(text,"%3.1f",value_size);
    XmTextSetString(dial1_text,text);

    sprintf(text,"%3.1f",value_rot_x);
    XmTextSetString(dial2_text,text);
    /* XmScaleSetValue(scale[1],(int)value_rot_x); */
    value_scale[1]=(int)value_rot_x;

    sprintf(text,"%3.1f",-value_rot_y);
    XmTextSetString(dial4_text,text);
    XmScaleSetValue(scale[3],-(int)value_rot_y);
    value_scale[3]=-(int)value_rot_y;
    
    if (mapproj) {

    	sprintf(text,"%3.1f",(value_trans_x/value_size)*-180.0);
    	XmTextSetString(dial6_text,text);

    	sprintf(text,"%3.1f",(value_trans_y/value_size)*-180.0);
    	XmTextSetString(dial8_text,text);

    }
}

virtual_value(dev,value)
int dev;
float value;
{

    switch(dev) {
	case 1: if ((value_size+0.2*value)>0) {
		  value_size=value_size+0.2*(value);
		  if (!mapproj) value_trans_z=0.9-value_size; 
		  else { 
		    size_scale = value_size/old_scale;
		    value_trans_x=(size_scale)*value_trans_x;
		    value_trans_y=(size_scale)*value_trans_y;
		    old_scale = value_size;
		  }
		  if (fog_on) {
		   glFogf(GL_FOG_START, 3.5-value_trans_z);
		    glFogf(GL_FOG_END, 4.5-value_trans_z);
		  }
		  if (clipping) get_persp(4);
		}
		break;
	case 2: value_rot_x=value_rot_x+rot_speed*(value);
		if (value_rot_x>180.0) value_rot_x-=360;
		if (value_rot_x<-180.0) value_rot_x+=360; 
		break;
	case 3: updateplate(1,(value)*rot_speed);
		break;
	case 4: value_rot_y=value_rot_y+rot_speed*(-value);
		if (value_rot_y>180.0) value_rot_y-=360;
		if (value_rot_y<-180.0) value_rot_y+=360; 
		break;
	case 5: updateplate(2,(value)*rot_speed);
		break;
	case 6: value_trans_x += (value_size/-0.9)*rot_speed*(value)/200.0;
		if (value_trans_x > value_size) value_trans_x -= 2*value_size;
		if (value_trans_x < -value_size) value_trans_x += 2*value_size;
		break;
	case 7: updateplate(3,(value)*rot_speed);
		break;
	case 8: value_trans_y += (value_size/-0.9)*rot_speed*(value)/200.0;
		if (value_trans_y > value_size/2) value_trans_y -= value_size;
		if (value_trans_y < -value_size/2) value_trans_y += value_size;
		break;
    }
    display_dialval();
    draw();
}

mainloop()
{
	XEvent event;
	while(1) {
		XtAppNextEvent(app, &event);
		XtDispatchEvent(&event);	
	}
}

reset_dials(home)
int home;
{

    if (home)
    {
	dialrotx = value_rot_x;
	dialroty = value_rot_y;
	dialrotz = value_rot_z;
	dialsize = value_size;
    }
    else
    {
	value_rot_x = dialrotx;
	value_rot_y = dialroty;
	value_rot_z = dialrotz;
	value_size = dialsize;
	value_trans_z=0.9-value_size;
	draw();
     }

}
