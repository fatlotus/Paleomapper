#include "paleotypes.h"
#include "paleodefs.h"
#include "paleoexterns.h"
#include <Xm/Text.h>

extern Widget cont_text, lat_text, lon_text, ang_text;
extern Widget pole_text, age_text1, age_text2;
int move_cont = 0;
extern int draw_pole;
extern int which_pole;
extern float pole_lon[5];
extern float pole_lat[5];
extern draw();
extern Widget dial3_text, dial5_text, dial7_text;
extern float PaleoTime;
extern Widget scale[8];
extern float value_scale[8];


moveplate(id)
int id;
{
    printf("Move Plate \n");

    if (plate_list[id]) {        move_cont = id;
        set_value();
        show_pole();
    printf("On cont_list \n");

    }
}

set_value()
{
    CONTINENT      *cont;
    PLATE	   *plat;
    char text[10];

    cont = cont_list[move_cont];
    plat = plate_list[move_cont];
    if (!cont) return;
    sprintf(text,"%03d",move_cont);
    XmTextSetString(cont_text,text);
    sprintf(text,"%8.3f",plat->latitude[which_pole]);
    XmTextSetString(lat_text,text);
    sprintf(text,"%8.3f",plat->longitude[which_pole]);
    XmTextSetString(lon_text,text);
    sprintf(text,"%8.3f",plat->angle[which_pole]);
    XmTextSetString(ang_text,text);

    sprintf(text,"%3.3f",plat->latitude[which_pole]);
    XmTextSetString(dial3_text,text);
    XmScaleSetValue(scale[2],(int)plat->latitude[which_pole]);
    value_scale[2]=(int)plat->latitude[which_pole];

    sprintf(text,"%3.3f",plat->longitude[which_pole]);
    XmTextSetString(dial5_text,text);
    XmScaleSetValue(scale[4],(int)plat->longitude[which_pole]);
    value_scale[4]=(int)plat->longitude[which_pole];

    sprintf(text,"%3.3f",plat->angle[which_pole]);
    XmTextSetString(dial7_text,text);
    XmScaleSetValue(scale[6],(int)plat->angle[which_pole]);
    value_scale[6]=(int)plat->angle[which_pole];

    sprintf(text,"%d",which_pole+1);
    XmTextSetString(pole_text,text);

    if (which_pole==0) {
    sprintf(text,"%5.1f",0);
    XmTextSetString(age_text1,text); 
    sprintf(text,"%5.1f",PaleoTime);
    XmTextSetString(age_text2,text); }
    else {
    sprintf(text,"%5.1f",PaleoTime);
    XmTextSetString(age_text1,text); 
    sprintf(text,"%5.1f",plat->pole_age[which_pole]);
    XmTextSetString(age_text2,text); }

}

updateplate(axis,value)
int axis;
float value;
{
    PLATE	   *plat;
    char	   text[10];

    plat = plate_list[move_cont];
    if (!plat) return;
    value = value / 10.0;
    switch (axis) {
	case 1 : { /* cont->latitude += value; */
		   plat->latitude[which_pole] += value*10.0;
    		   sprintf(text,"%8.3f",plat->latitude[which_pole]);
    		   XmTextSetString(lat_text,text);
		   sprintf(text,"%3.3f",plat->latitude[which_pole]);
		   XmTextSetString(dial3_text,text);
		   XmScaleSetValue(scale[2],(int)plat->latitude[which_pole]);
		   value_scale[2]=(int)plat->latitude[which_pole];
		 } break;
	case 2 : { plat->longitude[which_pole] += value*10.0;
   		   sprintf(text,"%8.3f",plat->longitude[which_pole]);
    		   XmTextSetString(lon_text,text);
		   sprintf(text,"%3.3f",plat->longitude[which_pole]);
		   XmTextSetString(dial5_text,text);
		   XmScaleSetValue(scale[4],(int)plat->longitude[which_pole]);
		   value_scale[4]=(int)plat->longitude[which_pole];
		 } break;
	case 3 : { plat->angle[which_pole] += value*10.0;
   		   sprintf(text,"%8.3f",plat->angle[which_pole]);
    		   XmTextSetString(ang_text,text);
		   sprintf(text,"%3.3f",plat->angle[which_pole]);
		   XmTextSetString(dial7_text,text);
		   XmScaleSetValue(scale[6],(int)plat->angle[which_pole]);
		   value_scale[6]=(int)plat->angle[which_pole];
		 } break;
    }

    quick_send_rot(move_cont);

    plate_list[move_cont] = plat;


}

show_pole()
{
    PLATE	   *plat;
    CONTINENT      *cont;
    
    cont = cont_list[move_cont];
    plat = plate_list[move_cont];
    if (!plat) return;
    draw_pole = TRUE;
    pole_lon[which_pole] = plat->longitude[which_pole];
    pole_lat[which_pole] = plat->latitude[which_pole];

/*    draw();	*/
 
}


up_pole()
{
    PLATE	   *plat;
    char text[2];

    plat = plate_list[move_cont];
    if (!plat) return;

    if (which_pole<4)	
    {
	which_pole++;

	set_value();
	show_pole();
	draw();

    }
}

down_pole()
{
    PLATE	   *plat;
    char text[2];

    plat = plate_list[move_cont];
    if (!plat) return;

    if (which_pole>0)	
    {

	plat->angle[which_pole] = 0.0;
    	quick_send_rot(move_cont);

	which_pole--;

	set_value();
	show_pole();
	draw();
    }

}
