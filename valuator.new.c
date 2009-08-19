#include <X11/extensions/XInput.h>
#include <string.h>
#include <stdlib.h>
#include <Xm/Text.h>
#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include "paleotypes.h"
#include "paleodefs.h"
#include "paleoexterns.h"
#include "pex.h"
#include "usbmidi.h"
#define MYDIALBOX "dial+buttons"

extern Display *dpy;
/* extern double atof(); */
extern dialsCB();
Widget  dial1_text,dial2_text,dial3_text,dial4_text;
Widget  dial5_text,dial6_text,dial7_text,dial8_text;
Widget scale[8];

extern float value_trans_x,value_trans_y;
extern float value_rot_x,value_rot_y;
extern float value_size;
extern float value_scale[8];
extern int mapproj;

extern int move_cont;
extern int which_pole;


XID  dials_id = 0;

init_dials()
{
}

#include <X11/keysym.h>
label_dials(dev,label)
XDevice *dev;
char *label[];
{
	
}

void updatedialsCB(w, client_data, call_data)
Widget                w;
caddr_t               client_data;
XmAnyCallbackStruct   *call_data;
{

}

build_vd(wid)
Widget wid;
{

}

