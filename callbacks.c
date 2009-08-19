#include <Xm/Xm.h>
#include <Xm/Text.h>
#include <X11/keysym.h>
#include "XPick.h"
#include "paleotypes.h"
#include "paleodefs.h"
#include "paleoexterns.h"
/*** #include "pex.h"
/***  This is stuff for GL  ***/
/*
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <Xm/MainW.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/CascadeB.h>
#include <Xm/Frame.h>
#include <Xm/Scale.h>
*/
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#define v3f glVertex3f

extern GLboolean doubleBuffer, viewportUpdateNeeded;
/* extern Display dpy; */
extern GLXContext cx;


extern clear_warning();
extern key_box();
extern display_dialval();
extern void draw();
extern int PickMode;
extern int Animate;
extern Widget popinfo;
extern Widget datafile_sel, rotfile_sel, pmagfile_sel;
extern Widget genfile_sel, lithfile_sel;
extern Widget options_box, plate_box, pmag_box;
extern Widget vd_box, lith_box, lithpick_box;
extern Widget exitwarning, about_box;
extern Widget polytog, cliptog;
extern Widget lithtog[27], envtog[9];
extern int graveyard;
extern int draw_pole;
extern int polar_wander;
extern int mapproj;
extern int lithnum;
extern int pmagnum;
int maptype=1;
extern int lowres;
extern int grid_on;
extern Widget scale[8];
float value_scale[8];
extern float rot_speed;
extern make_background();
int lithstat[27],lithenv[9],hidelithback,hidelithdata;
extern GLboolean projection;
int winheight=500,winwidth=500;
int polygons=0;
extern int globe;
int lith_check;
extern int hidelabels;
extern int hidedata;
static int hidemenubar=FALSE;
extern Widget menubar,mainw,drawarea,mainwidget;
int clipping = FALSE;
extern int vecsetnum, all_vectors;
static int set_pw_file;
int pw_once;
int winx,winy,winwd,winht;

void updateViewport(Widget w)
{
    Dimension width, height;

/*	printf("Updating Viewport! \n");	*/

    XtVaGetValues(w, XmNwidth, &width, XmNheight, &height, NULL);
	winheight=(GLint) height;
	winwidth=(GLint) width;
/*printf("w %d h %d \n",winwidth,winheight);*/
	if (height>width) glViewport(0, (GLint)((height-width)/2),
		(GLsizei) width, (GLsizei) width);
	  else glViewport((GLint)((width-height)/2), 0,  
		(GLsizei) height, (GLsizei) height);
/*    glViewport(0, 0, (GLsizei) width, (GLsizei) height);*/
    viewportUpdateNeeded = GL_FALSE;	
}


void infoCB(w, client_data, call_data)
Widget                w;
caddr_t               client_data;
XmAnyCallbackStruct   *call_data;
{

    switch((int)client_data) {
	case 0: {} 
		break; 
	case 10: case 11: case 12: case 13: case 14:
/*		prompt_message(name[((int)client_data)-10]); */
		break;
	default: {} 
		break; 
    }
}



void removewidCB(w, client_data, call_data)
Widget                w;
caddr_t               client_data;
XmAnyCallbackStruct   *call_data;
{
    if ((int)client_data<5)
    	XtUnmanageChild(w);	
/*      else savehelp((int)client_data-5); */
}

void optmenuCB(w, client_data, call_data)
Widget                w;
caddr_t               client_data;
XmAnyCallbackStruct   *call_data;
{
    switch((int)client_data) {
	case 1: XtManageChild(plate_box);
		break;
	case 2: XtManageChild(pmag_box);
		break;
	case 3: XtManageChild(vd_box);
		display_dialval();
		break;
	case 4: XtManageChild(lith_box);
		break;
    }
}

void helpmenuCB(w, client_data, call_data)
Widget                w;
caddr_t               client_data;
XmAnyCallbackStruct   *call_data;
{
    switch((int)client_data) {
	case 1: case 2: case 3: case 4: case 5: 
	case 11: case 12: case 13: case 14: case 15: 
	case 16: case 17:
		build_help((int)client_data-1);
		break;
	case 6: key_box();
		break;
	case 9: XtUnmanageChild(w);
		break;
	case 10:XtUnmanageChild(w);
		break;
/*	case 11: case 12: case 13: case 14: case 15: 
	case 16: case 17: case 18: case 19: case 20: 
		savehelp((int)client_data-11);
		break;
*/    }
    if ((int)client_data > 100) savehelp((int)client_data-101);
}

void pullmenuCB(w, client_data, call_data)
Widget                w;
caddr_t               client_data;
XmAnyCallbackStruct   *call_data;
{
    switch((int)client_data) {
	case 1: if (lowres) lowres=FALSE; 
		else lowres=TRUE;
		draw();
		break;
	case 2: if (projection) get_persp(2);
		else get_persp(1);
		break;
	case 3: if (mapproj) { mapproj=FALSE; undo_map(); }
		else { mapproj=maptype; init_map(); }
		draw();
		break;
	case 4: case 5: case 6: case 7: 
		maptype=(int)client_data-3;
		if (mapproj) { mapproj=maptype; init_map(); draw(); }
		break;
	case 8: if (grid_on) grid_on=FALSE;
		else grid_on=TRUE;
		draw();
		break;
	case 9: delete_label();
		draw();
		break;
	case 10: case 11: case 12:
		get_image((int)client_data-9);
		break;
	case 13: case 14: case 15: case 16: case 17: 
		printf("Help Menus have been Moved!\n");
		break;
	case 18:get_image(4);
		break;
	case 19:get_image(5);
		break;
	case 20:if (polygons==1) polygons=0;
		  else polygons=1;
		printf("POLYGONS %d\n",polygons);
		break;
	case 21:if (!globe) globe=TRUE;
		  else globe=FALSE;
		break;
   }
}

void mainmenuCB(w, client_data, call_data)
Widget                w;
caddr_t               client_data;
XmAnyCallbackStruct   *call_data;
{
    switch((int)client_data) {
	case 1: { XtManageChild(datafile_sel);
		} break;
	case 2: { XtManageChild(rotfile_sel);
		  Animate = FALSE;
		} break;
	case 3: { XtManageChild(rotfile_sel);
		  Animate = TRUE;
		} break;
	case 4: { save_data();
		} break;
	case 5: { XtManageChild(options_box);
		  init_speed();
		} break;
	case 8: { add_label();
		} break;
	case 9: { XtManageChild(pmagfile_sel);
		} break;
	case 10: { XtManageChild(genfile_sel);
		} break;
	case 11: { clear_warning("Clear All Data?");
		} break;
	case 12: { XtManageChild(about_box);
		} break;
	case 0: { XtManageChild(exitwarning);
		} break;
    }
}

void pushbuttCB(w, client_data, call_data)
Widget                w;
caddr_t               client_data;
XmAnyCallbackStruct   *call_data;
{
    switch((int)client_data) {
	case 1: set_pw_file = TRUE;
		XtManageChild(rotfile_sel);
		break;
    }
}


void toggleCB (w, client_data, callback_data)
Widget   w;
caddr_t               client_data;
XmToggleButtonCallbackStruct *callback_data;
{
    switch((int)client_data) {
	case 1: { get_persp(1);
		} break;
	case 2: { get_persp(2);
		} break;
	case 3: { if (hidelabels) hidelabels=FALSE;
		    else hidelabels=TRUE;
		  draw();
		} break;
	case 4: { if (hidedata) hidedata=FALSE;
		    else hidedata=TRUE;
		  draw();
		} break;
	case 5: { get_persp(3);
		} break;
	case 6: { XtUnmanageChild(options_box);
		} break;
	case 7: { draw();
		} break;
	case 8: { up_pole();
		} break;
	case 9: { down_pole();
		} break;
	case 10:{ flip(1); pmag_dec(); draw();
		} break;
	case 11:{ flip(2); pmag_dec(); draw();
		} break;
	case 12:{ flip(3); pmag_dec(); draw();
		} break;
	case 13:{ flip(4); draw();
		} break;
	case 14:{ flip(5); draw();
		} break;
	case 15:{ flip(6); draw();
		} break;
	case 16:{ polar_wander=FALSE; draw();
		} break;
	case 17:{ polar_wander=TRUE; 
		if (!pw_once) {
		    set_pw_file = TRUE;
		    XtManageChild(rotfile_sel);
		     }
		else make_polar_wander();
		draw();
		} break;
	case 18:{ mapproj=FALSE; undo_map(); draw();
		} break;
	case 19:{ mapproj=TRUE; init_map(); draw();
		} break;
	case 20:{ lowres=FALSE; draw();
		} break;
	case 21:{ lowres=TRUE; draw();
		} break;
	case 22:{ fog(); draw();
		} break;
	case 23:{ if (hidemenubar) { XtManageChild(menubar); hidemenubar=FALSE; }
		    else { XtUnmanageChild(menubar); hidemenubar=TRUE; }
		  XtUnmanageChild(drawarea);
		  updateViewport(drawarea);
		  XtManageChild(drawarea);
		  draw();
		} break;
	case 24:{ polar_wander=2;
		  make_apwp();
		  draw();
		} break;

    }
}

void PEXinputCB(w, client_data, call_data)
Widget                w;
caddr_t               client_data;
XmAnyCallbackStruct   *call_data;
{
    XEvent *ev;
    XButtonEvent *be;
    XKeyReleasedEvent *ke;
    KeySym key;

/*	printf("Getting Input! \n");	*/

    if (call_data->reason == XmCR_INPUT) {
	ev = call_data->event;
        if (ev->type == ButtonPress){
	    be=(XButtonEvent *)ev;
	    if ( be->button == Button1) {

		pickStuff(be->x,be->y);			
	      XmMenuPosition(popinfo, (XButtonPressedEvent *)be);
              XtManageChild(popinfo); 
	    }
/*	    else if ( be->button == Button2 ) {

		pickStuff(be->x,be->y);	

	    }
*/	    if ( be->button == Button3 ) { 
		printf("doesn't work\n");
		
	    }
	}
        else if (ev->type == ButtonRelease) {
	    be=(XButtonEvent *)ev;
	    if ( be->button == Button3 ) { 
		printf("doesn't work\n");
		pickStuff(be->x,be->y);			
	      XmMenuPosition(popinfo, (XButtonPressedEvent *)be);
              XtManageChild(popinfo); 
		printf("doesn't work\n");
	    }
 	}	
        else if (ev->type == KeyRelease) {
	    ke = (XKeyReleasedEvent *)ev;
	    key = XLookupKeysym(ke, ke->state);
/*	    if (key == XK_Up) { step_recon(1); set_value(); draw();}
	    else if (key == XK_Down) { step_recon(-1); set_value(); draw();}
	    else if (key == XK_Right) { step_recon(0); set_value(); draw();}
	    else if (key == XK_Escape) XtManageChild(options_box);
	    else if (key == XK_F1) {XtManageChild(vd_box); display_dialval();}
*/
	    switch (key) {
		case XK_Up: step_recon(1); set_value(); draw(); break;
		case XK_Down: step_recon(-1); set_value(); draw(); break;
		case XK_F1: XtManageChild(options_box);	init_speed(); break;
		case XK_F2: XtManageChild(plate_box); break;
		case XK_F3: XtManageChild(pmag_box); break;
		case XK_F4: XtManageChild(vd_box); display_dialval(); break;
		case XK_F11: XtManageChild(lith_box); break;
		case XK_F12: XtManageChild(lithpick_box); break;
		case XK_Return: draw(); break;
		case XK_p: if (polygons==1) 
		    {
			polygons=0; 
			XtVaSetValues(polytog,XmNset,FALSE,NULL);
		    }
		    else {
			polygons=1; 
			XtVaSetValues(polytog,XmNset,TRUE,NULL);
		    }
			printf("POLYGONS %d\n",polygons);
 			break;
		case XK_c: if (clipping) XtVaSetValues(cliptog,XmNset,FALSE,NULL);
			else XtVaSetValues(cliptog,XmNset,TRUE,NULL);
			get_persp(3); 
			break;
		case XK_question: printf("key list\n");
			key_box();
			break; 
		case XK_space: printf("Get Data\n");
			XtManageChild(datafile_sel);
			break; 
		case XK_P: printf("Get PMAG Data\n");
			XtManageChild(pmagfile_sel);
			break; 
		case XK_L: printf("Get Lith Data\n");
			XtManageChild(lithfile_sel);
			break; 
		case XK_G: printf("Get General Data\n");
			XtManageChild(genfile_sel);
			break; 
		case XK_r: printf("Reconstruction\n");
		  	Animate = FALSE;
			XtManageChild(rotfile_sel);
			break; 
		case XK_a: printf("Animation\n");
		  	Animate = TRUE;
			XtManageChild(rotfile_sel);
			break; 
		case XK_k: printf("Kill Data\n");
			clear_warning("Clear All Data?");
			break; 
		case XK_s: printf("Save Data\n");
			save_data();
			break; 
		case XK_q: printf("Quit\n");
			XtManageChild(exitwarning);
			break; 
		case XK_o: printf("Projection\n");
			if (projection) get_persp(2);
			else get_persp(1);
			break; 
		case XK_Escape: printf("2-D Mode\n");
			if (mapproj) { mapproj=FALSE; undo_map(); }
			else { mapproj=maptype; init_map(); }
			draw();
			break; 
		case XK_F5: printf("Rectiliniear Projection\n");
			maptype=1;
			if (mapproj) { 
			mapproj=maptype; init_map(); draw(); }
			break; 
		case XK_F6: printf("Molliweide Projection\n");
			maptype=2;
			if (mapproj) { 
			mapproj=maptype; init_map(); draw(); }
			break; 
		case XK_F7: printf("Mercator Projection\n");
			maptype=3;
			if (mapproj) { 
			mapproj=maptype; init_map(); draw(); }
			break; 
		case XK_F8: printf("Stereonet Projection\n");
			maptype=4;
			if (mapproj) { 
			mapproj=maptype; init_map(); draw(); }
			break; 
		case XK_d: printf("Hide Data\n");
			if (hidedata) hidedata=FALSE;
		    	else hidedata=TRUE;
		  	draw();
			break; 
		case XK_f: printf("Hide Fog\n");
			fog(); draw();
			break; 
		case XK_g: printf("Show Globe\n");
			if (!globe) globe=TRUE;
		  	else globe=FALSE;
			draw();
			break; 
		case XK_l: printf("Insert Label\n");
			add_label();
			break; 
		case XK_h: printf("General Help\n");
			build_help(4);
			break; 
		case XK_i: printf("Get Image\n");
			get_image(5);
			break; 
		case XK_1: printf("Get Info\n");
        		PickMode = GET_INFO;
	    		graveyard = FALSE;
	    		draw_pole = FALSE;
			draw();
			break; 
		case XK_2: printf("Move Plate\n");
        		PickMode = MOVE_PLATE;
	    		graveyard = FALSE;
	    		draw_pole = TRUE;
			draw();
			break; 
		case XK_3: printf("Delete Plate\n");
        		PickMode = DEL_PLATE;
	    		graveyard = FALSE;
	    		draw_pole = FALSE;
			draw();
			break; 
		case XK_4: printf("Delete Elem\n");
        		PickMode = DEL_ELEM;
	    		graveyard = FALSE;
	    		draw_pole = FALSE;
			draw();
			break; 
		case XK_5: printf("Display Plate\n");
        		PickMode = DIS_PLATE;
	    		graveyard = TRUE;
	    		draw_pole = FALSE;
			draw();
			break; 
		case XK_6: printf("Display Elem\n");
        		PickMode = DIS_ELEM;
	    		graveyard = TRUE;
	    		draw_pole = FALSE;
			draw();
			break; 
		case XK_7: printf("Pick Color\n");
        		PickMode = 8;
	    		graveyard = FALSE;
	    		draw_pole = FALSE;
			draw();
			break; 
	    }

	}
    }
} 

/***********************************************************************
*
* handling move events for the drawing area
*
***********************************************************************/

/* void PEXmoveCB(w, client, call)
  Widget		w;
  int			client;
  XmAnyCallbackStruct	*call;	*/
void PEXmoveCB(Widget w, XtPointer clientData, XEvent * event, Boolean * cont)
{
    Position x,y;
    Dimension width, height;


/*    printf("Moving! \n");	*/
    XtVaGetValues(w, XmNwidth, &width, XmNheight, &height, NULL);
    XtVaGetValues(w, XmNx, &x, XmNy, &y, NULL);
    winx=(int)x;
    winy=1024-(int)y-38;
    winwd=(int)width;
    winht=(int)height-38;
    /* printf("x %d y %d w %d h %d \n",winx,winy,winwd,winht); */
    draw(); 

}

/***********************************************************************
*
* handling expose events for the drawing area
*
***********************************************************************/

void PEXexposeCB(w, client, call)
  Widget		w;
  int			client;
  XmAnyCallbackStruct	*call;
{
/*  Pws_st ws_state;	  

  if (call->reason == XmCR_EXPOSE) {
    pinq_ws_st(&ws_state);
    if(ws_state==PWS_ST_WSOP) predraw_all_structs(client,PFLAG_ALWAYS);
  }
*/
/*    printf("Exposing! \n"); */

    draw();	
}

void dataFileOkCB (w, client_data, callback_data)
Widget   w;
caddr_t               client_data;
XmSelectionBoxCallbackStruct *callback_data;
{
    char *s;
    int button, error;

    button = (int) client_data;
    	if (button==1) {
	    XmStringGetLtoR(callback_data->value, XmSTRING_DEFAULT_CHARSET, &s);
	    XtUnmanageChild (datafile_sel);
	    if (callback_data->reason == XmCR_OK) 
	        picture_maker(s); }
    	if (button==2) XtUnmanageChild (datafile_sel);
	if (button==3) build_help(5);
}

void pmagFileOkCB (w, client_data, callback_data)
Widget   w;
caddr_t               client_data;
XmSelectionBoxCallbackStruct *callback_data;
{
    char *s;
    int button, error;

    button = (int) client_data;
    	if (button==1) {
	    XmStringGetLtoR(callback_data->value, XmSTRING_DEFAULT_CHARSET, &s);
	    XtUnmanageChild (pmagfile_sel);
	    if (callback_data->reason == XmCR_OK) 
	        pmag_maker(s); }
    	if (button==2) XtUnmanageChild (pmagfile_sel);
	if (button==3) build_help(6);
}

void genFileOkCB (w, client_data, callback_data)
Widget   w;
caddr_t               client_data;
XmSelectionBoxCallbackStruct *callback_data;
{
    char *s;
    int button, error;

    button = (int) client_data;
    	if (button==1) {
	    XmStringGetLtoR(callback_data->value, XmSTRING_DEFAULT_CHARSET, &s);
	    XtUnmanageChild (genfile_sel);
	    if (callback_data->reason == XmCR_OK) 
	        gen_maker(s); }
    	if (button==2) XtUnmanageChild (genfile_sel);
	if (button==3) build_help(7);
}

void rotFileOkCB (w, client_data, callback_data)
Widget   w;
caddr_t               client_data;
XmSelectionBoxCallbackStruct *callback_data;
{
    char *s;
    int button, error;

    button = (int) client_data;
    	if (button==1) {
	    XmStringGetLtoR(callback_data->value, XmSTRING_DEFAULT_CHARSET, &s);
	    XtUnmanageChild (rotfile_sel);
	    if (callback_data->reason == XmCR_OK) 
		if (set_pw_file) { set_pw_var(s); 
				set_pw_file = FALSE; }
	        else reconstruction(s);
		draw();
	    set_value(); }
    	if (button==2) XtUnmanageChild (rotfile_sel);
	if (button==3) build_help(8);
}

void lithFileOkCB (w, client_data, callback_data)
Widget   w;
caddr_t               client_data;
XmSelectionBoxCallbackStruct *callback_data;
{
    char *s;
    int button, error;

    button = (int) client_data;
    	if (button==1) {
	    XmStringGetLtoR(callback_data->value, XmSTRING_DEFAULT_CHARSET, &s);
	    XtUnmanageChild (lithfile_sel);
	    if (callback_data->reason == XmCR_OK) 
	        lith_maker(s); }
    	if (button==2) XtUnmanageChild (lithfile_sel);
	if (button==3) build_help(9);
}

/***********************************************************************
*
* handling resize events for the drawing area
*
***********************************************************************/

void PEXresizeCB(w, client, call)
  Widget			w;
  int				client;
  XEvent			*call;
{

  if (call->type == ConfigureNotify) {

    /***  GL stuff  ***/

    /* don't try OpenGL until window is realized! */
    if (XtIsRealized(w)) updateViewport(w);
        else viewportUpdateNeeded = GL_TRUE;

/*    printf("Resizing! \n"); */

    draw(); 



  }
}

void pickmodeCB (w, client_data, callback_data)
Widget   w;
caddr_t               client_data;
XmToggleButtonCallbackStruct *callback_data;
{
/*    static int graveyard = FALSE; */

    if (callback_data->set) {
        PickMode = (int)client_data;
/*printf("PickMode %d \n",PickMode);*/

	if (PickMode == DIS_PLATE || PickMode == DIS_ELEM) 
	    graveyard = TRUE;
	else 
	    graveyard = FALSE;

	if (PickMode != MOVE_PLATE)
	    draw_pole = FALSE;

	draw();

    }
}

void exitokCB(w, client_data, call_data)
Widget                w;
caddr_t               client_data;
XmAnyCallbackStruct   *call_data;
{
    XtUnmanageChild (exitwarning);
 
    if (call_data->reason == XmCR_OK) {
/*        pclose_ws(1);
        pclose_phigs();*/
        exit(0);
    }
} 

void clearokCB(w, client_data, call_data)
Widget                w;
caddr_t               client_data;
XmAnyCallbackStruct   *call_data;
{
    XtUnmanageChild (exitwarning);
 
    if (call_data->reason == XmCR_OK) {
	printf("Clear Data\n");
	vecsetnum=0;
	all_vectors=0;
	head_elem=NULL;
	lithnum=0;
	pmagnum=0;
    }
} 

void dialsCB (w, client_data, callback_data)
Widget   w;
caddr_t               client_data;
XmToggleButtonCallbackStruct *callback_data;
{
  int val,num;

    num=(int)client_data;
    switch(num) {
	case 1: reset_dials(TRUE);
		break;
	case 2: reset_dials(FALSE);
		break;
	case 3: set_speed();
		break;
	case 4: virtual_value(1,-1.0);
		break;
	case 5: virtual_value(1,1.0);
		break;
	case 6: virtual_value(2,-1.0);
		break;
	case 7: virtual_value(2,1.0);
		break;
	case 8: virtual_value(3,-1.0);
		break;
	case 9: virtual_value(3,1.0);
		break;
	case 10: virtual_value(4,-1.0);
		break;
	case 11: virtual_value(4,1.0);
		break;
	case 12: virtual_value(5,-1.0);
		break;
	case 13: virtual_value(5,1.0);
		break;
	case 14: virtual_value(6,-1.0);
		break;
	case 15: virtual_value(6,1.0);
		break;
	case 16: virtual_value(7,-1.0);
		break;
	case 17: virtual_value(7,1.0);
		break;
	case 18: virtual_value(8,-1.0);
		break;
	case 19: virtual_value(8,1.0);
		break;
	case 20: case 21: case 22: case 23: 
	case 24: case 25: case 26: case 27: 
		XmScaleGetValue(scale[num-20],&val);
/*		printf("val %d old %.3f \n",val,value_scale[num-20]);*/
		virtual_value(num-19,(val-value_scale[num-20])/rot_speed);
		value_scale[num-20]=val;
		break;
    }
}

void lithCB (w, client_data, call_data)
Widget   w;
caddr_t               client_data;
XmAnyCallbackStruct *call_data;
{
int num;
    num = (int)client_data;

    switch(num) {
	case 1: case 2: case 3: case 4: case 5:
	case 6: case 7: case 8: case 9: case 10:
	case 11: case 12: case 13: case 14: case 15:
	case 16:
		lith_check=num;
		XtManageChild(lithfile_sel);
		break;
	case 17: make_background();
		draw();
		break;
	case 18: XtManageChild(lithpick_box);
		break;
    }
}

void lithtogCB (w, client_data, call_data)
Widget   w;
caddr_t               client_data;
XmAnyCallbackStruct *call_data;
{
int num,i;
    num = (int)client_data;

    switch(num) {
	case 1: case 2: case 3: case 4: case 5:
	case 6: case 7: case 8: case 9: case 10:
	case 11: case 12: case 13: case 14: case 15:
	case 16: case 17: case 18: case 19: case 20:
	case 21: case 22: case 23: case 24: case 25:
	case 26: if (lithstat[num]) {
		    lithstat[num]=FALSE; }
		  else {
		    lithstat[num]=TRUE; }
		draw();
		break;
	case -1: for(i=1;i<27;i++) {
		    XtVaSetValues(lithtog[i],XmNset,TRUE,NULL);
		    lithstat[i]=TRUE; }
		draw();
		break;
	case -2: for(i=1;i<27;i++) {
		    if (lithstat[i]) {
		    	XtVaSetValues(lithtog[i],XmNset,FALSE,NULL);
		    	lithstat[i]=FALSE; }
		      else {
		    	XtVaSetValues(lithtog[i],XmNset,TRUE,NULL);
		    	lithstat[i]=TRUE; } 
		    }
		draw();
		break;
	case 30: case 31: case 32: case 33: case 34: 
	case 35: case 36: case 37: case 38: case 39: 
		if (lithenv[num-30]) {
		    lithenv[num-30]=FALSE; }
		  else {
		    lithenv[num-30]=TRUE; }
		draw();
		break;
	case -3: for(i=0;i<9;i++) {
		    XtVaSetValues(envtog[i],XmNset,TRUE,NULL);
		    lithenv[i]=TRUE; }
		draw();
		break;
	case -4: for(i=0;i<9;i++) {
		    if (lithenv[i]) {
		    	XtVaSetValues(envtog[i],XmNset,FALSE,NULL);
		    	lithenv[i]=FALSE; }
		      else {
		    	XtVaSetValues(envtog[i],XmNset,TRUE,NULL);
		    	lithenv[i]=TRUE; } 
		    }
		draw();
		break;
	case 40: if (hidelithback) hidelithback=FALSE;
		  else hidelithback=TRUE;
		draw();
		break;
	case 41: if (hidelithdata) hidelithdata=FALSE;
		  else hidelithdata=TRUE;
		draw();
		break;

    }
}
