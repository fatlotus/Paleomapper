#include <string.h>
#include <Xm/Text.h>
#include "paleotypes.h"
#include "paleodefs.h"
#include "paleoexterns.h"

extern Widget cont_text, lat_text, lon_text, ang_text;
extern Widget pole_text, age_text2;
extern Widget rot_text, pmag_text;
extern double atof();
extern draw();
extern int which_pole;
extern float rot_speed;

extern float value_rot_x;
extern float value_rot_y;
extern float value_rot_z;

int one_pmag=0;

void enterplateCB(w, client_data, call_data)
Widget                w;
caddr_t               client_data;
XmAnyCallbackStruct   *call_data;
{
    char *input_string;
    int  cont;
    int  plat;
    float age;
    float lat, lon, ang;
    PLATE *ptr;

    input_string = XmTextGetString(cont_text);
    plat = atoi(input_string);
    XtFree(input_string);

    ptr = plate_list[plat];
    if (!ptr) return;

    input_string = XmTextGetString(lat_text);
    lat = atof(input_string);
    XtFree(input_string);

    input_string = XmTextGetString(lon_text);
    lon = atof(input_string);
    XtFree(input_string);

    input_string = XmTextGetString(ang_text);
    ang = atof(input_string);
    XtFree(input_string);

    if (which_pole>0) {
    input_string = XmTextGetString(age_text2);
    age = atof(input_string);
    XtFree(input_string); }

/*    input_string = XmTextGetString(pole_text);
    which_pole = atof(input_string);
    XtFree(input_string);	*/


    ptr->latitude[which_pole] = lat;
    ptr->longitude[which_pole] = lon;
    ptr->angle[which_pole] = ang;
    if (which_pole>0) ptr->pole_age[which_pole] = age;

    moveplate(plat);
    quick_send_rot(plat);
    draw();
}

set_speed()
{
    char *input_string;

    input_string = XmTextGetString(rot_text);
    rot_speed = atof(input_string);
    XtFree(input_string);

    draw();
}

void enterpmagCB(w, client_data, call_data)
Widget                w;
caddr_t               client_data;
XmAnyCallbackStruct   *call_data;
{
    char *input_string;

    input_string = XmTextGetString(pmag_text);
    one_pmag = atoi(input_string);
    XtFree(input_string);

    draw();
}

void init_speed()
{
    char text[10];

    sprintf(text,"%2.3f",rot_speed);
    XmTextSetString(rot_text,text);

    sprintf(text,"%3d",one_pmag);
    XmTextSetString(pmag_text,text);
}

