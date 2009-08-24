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
static char *dlabels[] = 
     {"X ROT","Y ROT","SCALE","","LAT","LON","ANG",""};
static char *blanks[] = {"","","","","","","",""};

init_dials()
{
	printf("obselete method called: init_dials\n");
	exit(1);
	
	/* Dials moved to USB/MIDI interface */
}

#include <X11/keysym.h>
label_dials(dev,label)
XDevice *dev;
char *label[];
{
    XStringFeedbackControl feeder;
    /* KeySym message[64]; */
    KeySym message[8][8];
    int i,j,start,len;

    for (i=0; i<8; i++) {
	len = strlen(label[i]);
	if (len > 8) 
	   len = 8;
	start = 4 - len / 2;
	for (j=0; j<8; j++) {
	    if (j < start) 
	       /* message[i*8+j] = XK_space; */
	       message[i][j] = XK_space;
	    else if (j < start+len) 
	       message[i][j] = (KeySym) label[i][j-start];
	    else 
	       message[i][j] = XK_space;
	} /* end j */
       feeder.class = StringFeedbackClass;
       feeder.length = sizeof(XStringFeedbackControl) + 8 * sizeof(KeySym);
       feeder.id = i;
       feeder.num_keysyms = 8;
       feeder.syms_to_display = message[i];
/*       XChangeFeedbackControl(dpy, dev, DvString, &feeder);*/	
   /*    XChangeFeedbackControl(dpy, dev, DvString, &feeder); */
    } /* end i */
}

void updatedialsCB(w, client_data, call_data)
Widget                w;
caddr_t               client_data;
XmAnyCallbackStruct   *call_data;
{
	printf("obselete method called: updatedialsCB\n");
	exit(1);
}


build_vd(wid)
Widget wid;
{
   Widget box, col, rc, radio, label, sep, tog;
   Widget tics[5], nums[5];
   Arg args[2],sargs[10],targs[5];
   char tic_name[10];
   int i,n,sn,tn;
   Dimension margin;
   char *text;
            XFontStruct *font=NULL;
            XmFontList fontlist=NULL;

/**********************************************************************/
/*  Create the Vitual Dials box.                                      */
/**********************************************************************/
   col = XmCreateRowColumn(wid, "column1", args, 0);
   XtManageChild(col);

   XtSetArg(args[0],XmNorientation,XmHORIZONTAL);
   rc = XmCreateRowColumn(col, "##**## DIAL 2 ##**##", args, 1);
   XtManageChild(rc);
   XtSetArg(args[0],XmNrecomputeSize,FALSE);
   XtSetArg(args[1],XmNwidth,120);
   label  = XmCreateLabelGadget(rc, "F1) 3D Lat   ", args,2);
   XtManageChild(label);
   XtSetArg(args[0],XmNcolumns,5);
   dial2_text  = XmCreateText(rc, "text", args, 1);
   XtManageChild(dial2_text);
   n=0;
/*
   XtVaGetValues(dial2_text, XmNheight, &margin, NULL);
   printf("hei %d \n",margin);
   XtVaGetValues(dial2_text, XmNwidth, &margin, NULL);
   printf("wid %d \n",margin);
*/
   sn=0;
   
   XtSetArg(sargs[sn],XmNorientation,XmHORIZONTAL);sn++;
   XtSetArg(sargs[sn],XmNscaleMultiple,10);sn++; 
/* XtSetArg(sargs[sn],XmNdecimalPoints,0);sn++; */
   XtSetArg(sargs[sn],XmNmaximum,180);sn++;
   XtSetArg(sargs[sn],XmNminimum,-180);sn++;
   XtSetArg(sargs[sn],XmNscaleWidth,217);sn++;
   XtSetArg(sargs[sn],XmNvalue, 0); sn++;
   XtSetArg(sargs[sn],XmNshowValue, FALSE);sn++;
   
   scale[1] = XmCreateScale(rc, "Scale Number One", sargs, sn);
   XtAddCallback(scale[1],XmNdragCallback,dialsCB,21);
   XtAddCallback(scale[1],XmNvalueChangedCallback,dialsCB,21);
   XtManageChild(scale[1]);
  

   tn=0;
/*
   XtSetArg(targs[tn],XmNseparatorType,XmSINGLE_LINE);tn++;
   XtSetArg(targs[tn],XmNorientation,XmVERTICAL);tn++;
   XtSetArg(targs[tn],XmNwidth,5);tn++;
   XtSetArg(targs[tn],XmNheight,7);tn++;
   
   for (i=0; i<5 ; i++) {
    sprintf(tic_name, "tic_%d", i);
    tics[i]=XmCreateSeparatorGadget(scale[1],tic_name,targs,tn);
   }
   XtManageChildren(tics, 5);
*/
   font=XLoadQueryFont(dpy,"-*-helvetica-medium-r-normal--8-*-*-*-*-*-iso8859-1");
   fontlist=XmFontListCreate(font,XmSTRING_DEFAULT_CHARSET);
   XtSetArg(args[0],XmNfontList,fontlist);
    nums[0] = XmCreateLabelGadget(scale[1], "-180",args,1);
    nums[1] = XmCreateSeparatorGadget(scale[1],"tic1",targs,tn);
    nums[2] = XmCreateLabelGadget(scale[1], "0",args,1);
    nums[3] = XmCreateSeparatorGadget(scale[1],"tic2",targs,tn);
    nums[4] = XmCreateLabelGadget(scale[1], "+180",args,1);
   /* XtManageChildren(nums, 5);*/
 
   XtSetArg(args[0],XmNorientation,XmHORIZONTAL);
   rc = XmCreateRowColumn(col, "##**## DIAL 4 ##**##", args, 1);
   XtManageChild(rc);
   XtSetArg(args[0],XmNrecomputeSize,FALSE);
   XtSetArg(args[1],XmNwidth,120);
   label  = XmCreateLabelGadget(rc, "F2) 3D Lon  ", args,2);
   XtManageChild(label);
   XtSetArg(args[0],XmNcolumns,5);
   dial4_text  = XmCreateText(rc, "text", args, 1);
   XtManageChild(dial4_text);

   scale[3] = XmCreateScale(rc,"scale",sargs,sn);
   XtAddCallback(scale[3],XmNdragCallback,dialsCB,23);
   XtAddCallback(scale[3],XmNvalueChangedCallback,dialsCB,23);
   XtManageChild(scale[3]);
   
   XtSetArg(args[0],XmNfontList,fontlist);
    nums[0] = XmCreateLabelGadget(scale[3], "-180",args,1);
    nums[1] = XmCreateSeparatorGadget(scale[3],"tic1",targs,tn);
    nums[2] = XmCreateLabelGadget(scale[3], "0",args,1);
    nums[3] = XmCreateSeparatorGadget(scale[3],"tic2",targs,tn);
    nums[4] = XmCreateLabelGadget(scale[3], "+180",args,1);
   /* XtManageChildren(nums, 5);*/

   XtSetArg(args[0],XmNorientation,XmHORIZONTAL);
   rc = XmCreateRowColumn(col, "##**## DIAL 3 ##**##", args, 1);
   XtManageChild(rc);
   XtSetArg(args[0],XmNrecomputeSize,FALSE);
   XtSetArg(args[1],XmNwidth,120);
   label  = XmCreateLabelGadget(rc, "F3/K7/K8) Rot Lat ", args,2);
   XtManageChild(label);
   XtSetArg(args[0],XmNcolumns,5);
   dial3_text  = XmCreateText(rc, "text", args, 1);
   XtManageChild(dial3_text);

   scale[2] = XmCreateScale(rc,"scale",sargs,sn);
   XtAddCallback(scale[2],XmNdragCallback,dialsCB,22);
   XtAddCallback(scale[2],XmNvalueChangedCallback,dialsCB,22);
   XtManageChild(scale[2]);
   XtSetArg(args[0],XmNfontList,fontlist);
    nums[0] = XmCreateLabelGadget(scale[2], "-180",args,1);
    nums[1] = XmCreateSeparatorGadget(scale[2],"tic1",targs,tn);
    nums[2] = XmCreateLabelGadget(scale[2], "0",args,1);
    nums[3] = XmCreateSeparatorGadget(scale[2],"tic2",targs,tn);
    nums[4] = XmCreateLabelGadget(scale[2], "+180",args,1);
   /* XtManageChildren(nums, 5); */

   XtSetArg(args[0],XmNorientation,XmHORIZONTAL);
   rc = XmCreateRowColumn(col, "##**## DIAL 5 ##**##", args, 1);
   XtManageChild(rc);
   XtSetArg(args[0],XmNrecomputeSize,FALSE);
   XtSetArg(args[1],XmNwidth,120);
   label  = XmCreateLabelGadget(rc, "F4/K5/K6) Rot Lon", args,2);
   XtManageChild(label);
   XtSetArg(args[0],XmNcolumns,5);
   dial5_text  = XmCreateText(rc, "text", args, 1);
   XtManageChild(dial5_text);

   scale[4] = XmCreateScale(rc,"scale",sargs,sn);
   XtAddCallback(scale[4],XmNdragCallback,dialsCB,24);
   XtAddCallback(scale[4],XmNvalueChangedCallback,dialsCB,24);
   XtManageChild(scale[4]);
   XtSetArg(args[0],XmNfontList,fontlist);
    nums[0] = XmCreateLabelGadget(scale[4], "-180",args,1);
    nums[1] = XmCreateSeparatorGadget(scale[4],"tic1",targs,tn);
    nums[2] = XmCreateLabelGadget(scale[4], "0",args,1);
    nums[3] = XmCreateSeparatorGadget(scale[4],"tic2",targs,tn);
    nums[4] = XmCreateLabelGadget(scale[4], "+180",args,1);
   /* XtManageChildren(nums, 5); */

   XtSetArg(args[0],XmNorientation,XmHORIZONTAL);
   rc = XmCreateRowColumn(col, "##**## DIAL 7 ##**##", args, 1);
   XtManageChild(rc);
   XtSetArg(args[0],XmNrecomputeSize,FALSE);
   XtSetArg(args[1],XmNwidth,120);
   label  = XmCreateLabelGadget(rc, "F5/K3/K4) Rot Ang", args,2);
   XtManageChild(label);
   XtSetArg(args[0],XmNcolumns,5);
   dial7_text  = XmCreateText(rc, "text", args, 1);
   XtManageChild(dial7_text);

   scale[6] = XmCreateScale(rc,"scale",sargs,sn);
   XtAddCallback(scale[6],XmNdragCallback,dialsCB,26);
   XtAddCallback(scale[6],XmNvalueChangedCallback,dialsCB,26);
   XtManageChild(scale[6]);
   XtSetArg(args[0],XmNfontList,fontlist);
    nums[0] = XmCreateLabelGadget(scale[6], "-180",args,1);
    nums[1] = XmCreateSeparatorGadget(scale[6],"tic1",targs,tn);
    nums[2] = XmCreateLabelGadget(scale[6], "0",args,1);
    nums[3] = XmCreateSeparatorGadget(scale[6],"tic2",targs,tn);
    nums[4] = XmCreateLabelGadget(scale[6], "+180",args,1);
   /* XtManageChildren(nums, 5); */

   XtSetArg(args[0],XmNorientation,XmHORIZONTAL);
   rc = XmCreateRowColumn(col, "##**## DIAL 6 ##**##", args, 1);
   XtManageChild(rc);
   XtSetArg(args[0],XmNrecomputeSize,FALSE);
   XtSetArg(args[1],XmNwidth,120);
   label  = XmCreateLabelGadget(rc, "6) 2D Lon  ", args,2);
   XtManageChild(label);
   XtSetArg(args[0],XmNcolumns,5);
   dial6_text  = XmCreateText(rc, "text", args, 1);
   XtManageChild(dial6_text);
   tog = XmCreatePushButtonGadget(rc, "<", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,dialsCB,14);	
   tog = XmCreatePushButtonGadget(rc, ">", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,dialsCB,15);

   XtSetArg(args[0],XmNorientation,XmHORIZONTAL);
   rc = XmCreateRowColumn(col, "##**## DIAL 8 ##**##", args, 1);
   XtManageChild(rc);
   XtSetArg(args[0],XmNrecomputeSize,FALSE);
   XtSetArg(args[1],XmNwidth,120);
   label  = XmCreateLabelGadget(rc, "8) 2D Lat   ", args,2);
   XtManageChild(label);
   XtSetArg(args[0],XmNcolumns,5);
   dial8_text  = XmCreateText(rc, "text", args, 1);
   XtManageChild(dial8_text);
   tog = XmCreatePushButtonGadget(rc, "<", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,dialsCB,18);	
   tog = XmCreatePushButtonGadget(rc, ">", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,dialsCB,19);

   XtSetArg(args[0],XmNorientation,XmHORIZONTAL);
   rc = XmCreateRowColumn(col, "##**## DIAL 1 ##**##", args, 1);
   XtManageChild(rc);
   XtSetArg(args[0],XmNrecomputeSize,FALSE);
   XtSetArg(args[1],XmNwidth,120);
   label  = XmCreateLabelGadget(rc, "F6) Scale", args,2);
   XtManageChild(label);
   XtSetArg(args[0],XmNcolumns,5);
   dial1_text  = XmCreateText(rc, "text", args, 1);
   XtManageChild(dial1_text);
   tog = XmCreatePushButtonGadget(rc, "<", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,dialsCB,4);	
   tog = XmCreatePushButtonGadget(rc, ">", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,dialsCB,5);

   XtSetArg(args[0],XmNrecomputeSize,FALSE);
   XtSetArg(args[1],XmNwidth,40);
   label  = XmCreateLabelGadget(rc, "  ", args,2);
   XtManageChild(label);
   XtSetArg(args[0],XmNrecomputeSize,TRUE);
   tog = XmCreatePushButtonGadget(rc, "Update Dials",args,1);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,updatedialsCB,1);	

}

