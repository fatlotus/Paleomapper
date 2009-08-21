#include <X11/Intrinsic.h>
#include <X11/extensions/XInput.h>
#include <XPick.h>
#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/DrawingA.h>
#include <Xm/ToggleBG.h>
#include <Xm/PushBG.h>
#include <Xm/LabelG.h>
#include <Xm/BulletinB.h>
#include <Xm/Text.h>
#include <Xm/FileSB.h>
#include <Xm/MessageB.h>
#include <Xm/SelectioB.h>
#include <Xm/Separator.h>
#include <Xm/MainW.h>
#include <Xm/CascadeB.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include "paleodefs.h"
#include <stdio.h>
#include "usbmidi.h"

/***   PMAG   ***/
extern enterpmagCB();
extern pmagFileOkCB();
Widget pmag_box, pmag_text, pmagfile_sel;

/***   GENERAL DATA   ***/
extern genFileOkCB();
Widget genfile_sel;

/***   LITH   ***/
extern lithFileOkCB();
Widget lithfile_sel, lith_box, lithpick_box;

/***   LABELS   ***/
extern get_font();
extern init_labels();

/***   OPENGL   ***/
extern get_persp();
extern PEXexposeCB();
extern PEXinputCB();
extern PEXresizeCB();
extern PEXmoveCB();
static String   fallbackResources[] = {
    "*sgiMode: true",           /* try to enable IRIX 5.2+ look & feel */
    "*useSchemes: all",         /* and SGI schemes */
/*    "*mainw*title: Paleomapper","*menubar*width: 50", */
    "*drawarea*width: 500", "*drawarea*height: 500", 
    NULL
};
static int dblBuf[] = {
    GLX_DOUBLEBUFFER, GLX_RGBA, GLX_DEPTH_SIZE, 16,
    GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1, GLX_BLUE_SIZE, 1,
    None
};
/* static int dblBuf[] = { 0 }; */
static int *snglBuf = &dblBuf[1];
XVisualInfo  *vi;
GLboolean    doubleBuffer = GL_TRUE, viewportUpdateNeeded = GL_TRUE, made_current = GL_FALSE;
GLboolean projection = FALSE;
GLXContext  	cx;
Colormap     	cmap;
Display 	*dpy;

/***   MOTIF   ***/
Widget CreateFileSelect();
/* extern promptCB();  this one is mine */
int    promptCB();
extern dataFileOkCB();
extern rotFileOkCB();
extern toggleCB();
extern pushbuttCB();
extern pickmodeCB();
extern removewidCB();
extern enterplateCB();
extern exitokCB();
extern clearokCB();
extern dialsCB();
extern optmenuCB();
extern build_vd();
extern build_lith();
extern build_lithpick();
extern build_help();
Widget  drawarea, rotfile_sel, datafile_sel, mainw, menubar;
Widget  cont_text, lat_text, lon_text, ang_text;
Widget  pole_text, age_text1, age_text2, rot_text;
Widget  working, inform, prompt, warning, exitwarning, clearwarning;
Widget  options_box, plate_box, vd_box, about_box;
Window  draw_win;
XtAppContext app;
Arg   myargs[1];

XDevice *dials = NULL;
int     DeviceMotion = -1;


/*********************/
/***   FUNCTIONS   ***/
/*********************/




open_phigs(argc,argv)
int  *argc;
char *argv[];
{
    Pixmap pixmap;
    Widget  mainwidget, mainob, mainabout;
    Widget  mainpb, mainpmag, mainvd, mainlith, mainlithpick;	
    Arg args[5];
    int n, counter = 0;	


/*    mainwidget = XtInitialize("main", "Paleomap", NULL, 0, argc, argv); */

    mainwidget = XtAppInitialize(&app, "Paleomap", NULL, 0, argc, argv, 
                                 fallbackResources,NULL,0);	
    
            
    dpy = XtDisplay(mainwidget);

/*    XtVaSetValues(mainwidget, XtNtitle, "Paleomapper", NULL); 	*/

    /* find an OpenGL-capable RGB visual with depth buffer */
    vi = glXChooseVisual(dpy, DefaultScreen(dpy), dblBuf);
    if (vi == NULL) {
        vi = glXChooseVisual(dpy, DefaultScreen(dpy), snglBuf);
        if (vi == NULL)
            XtAppError(app, "no RGB visual with depth buffer");
        doubleBuffer = GL_FALSE;
    }

    /* create an OpenGL rendering context */
    cx = glXCreateContext(dpy, vi, /* no display list sharing */ None,
        /* favor direct */ GL_TRUE);
    if (cx == NULL)
        XtAppError(app, "could not create rendering context");

    /* create the Colormap because the default setup is bad! */
    cmap = XCreateColormap(dpy, RootWindow(dpy, vi->screen),
        vi->visual, AllocNone);

    /*
     * Establish the visual, depth, and colormap of the toplevel
     * widget _before_ the widget is realized.
     */

    XtVaSetValues(mainwidget, XtNvisual, vi->visual, XtNdepth, vi->depth,
                  XtNcolormap, cmap, NULL);
    XtAddEventHandler(mainwidget,StructureNotifyMask,FALSE,PEXmoveCB,NULL); 

    /* create main window */
    mainw = XmCreateMainWindow(mainwidget, "mainw", NULL, 0);
    XtManageChild(mainw);
	
    /* create menu bar */
	Arg args_lesstif[3];
	
	XtSetArg(args_lesstif[counter], XmNrowColumnType, XmMENU_BAR); counter++;
		
    menubar = XmCreateRowColumn(mainw, "menubar", args_lesstif, counter); counter = 0;
    XtManageChild(menubar);
	
    init_choice();
    /* create the options Dialog Box */
    XtSetArg(myargs[0], XmNvisual, vi->visual);
    options_box = XmCreateBulletinBoardDialog(mainw, "Options", myargs, 1);
    mainob = XmCreateRowColumn(options_box, "OptBox", NULL, 0);
    XtManageChild(mainob);

    /* create the plate Dialog Box */
    XtSetArg(myargs[0], XmNvisual, vi->visual);
    plate_box = XmCreateBulletinBoardDialog(mainw, "Plate_Options", myargs, 1);
    mainpb = XmCreateRowColumn(plate_box, "PlateDialog", NULL, 0);
    XtManageChild(mainpb);	

    /* create the PMAG Dialog Box */
    XtSetArg(myargs[0], XmNvisual, vi->visual);
    pmag_box = XmCreateBulletinBoardDialog(mainw, "PMAG_Options", myargs, 1);
    mainpmag = XmCreateRowColumn(pmag_box, "PMAGDialog", NULL, 0);
    XtManageChild(mainpmag);	

    /* create the Virtual Dial Box */
    XtSetArg(myargs[0], XmNvisual, vi->visual);
    vd_box = XmCreateBulletinBoardDialog(mainw, "Virtual_Dials", myargs, 1);
    mainvd = XmCreateRowColumn(vd_box, "VDBDialog", NULL, 0);
    XtManageChild(mainvd);	

    /* create the Lith Dialog Box */
    XtSetArg(myargs[0], XmNvisual, vi->visual);
    lith_box = XmCreateBulletinBoardDialog(mainw, "Lith_Data", myargs, 1);
    mainlith = XmCreateRowColumn(lith_box, "LithDialog", NULL, 0);
    XtManageChild(mainlith);	

    /* create the Lith Pick Box */
    XtSetArg(myargs[0], XmNvisual, vi->visual);
    lithpick_box = XmCreateBulletinBoardDialog(mainw, "Lith_Options", myargs, 1);
    mainlithpick = XmCreateRowColumn(lithpick_box, "LithPick", NULL, 0);
    XtManageChild(mainlithpick);	

    /* create the About PMT Box */
    XtSetArg(myargs[0], XmNvisual, vi->visual);
    about_box = XmCreateBulletinBoardDialog(mainw, "About_PMT", myargs, 1);
    mainabout = XmCreateRowColumn(about_box, "AboutDialog", NULL, 0);
    XtManageChild(mainabout);	

    build_vd(mainvd);

    build_pmag(mainpmag);

    build_pb(mainpb);
    
    build_lith(mainlith);
    
    build_lithpick(mainlithpick);
    build_about(mainabout);

/*    build_help();*/

/**********************************************************************/
/*  Create upper panel area for setting view options, etc...          */
/**********************************************************************/
    build_panel(mainob);

/**********************************************************************/
/*  Create file selection dialog to read files.                       */
/**********************************************************************/
    datafile_sel = CreateFileSelect(mainw,"data/*",dataFileOkCB);
    pmagfile_sel = CreateFileSelect(mainw,"pmag/*",pmagFileOkCB);
    genfile_sel  = CreateFileSelect(mainw,"*",genFileOkCB);
    lithfile_sel = CreateFileSelect(mainw,"lith/*",lithFileOkCB);
    rotfile_sel  = CreateFileSelect(mainw,"rot/*",rotFileOkCB);
    rotfile_sel  = CreateFileSelect(mainw,"*.rot",rotFileOkCB);

/**********************************************************************/
/*  Create dialogs for general use.                                   */
/**********************************************************************/
    build_dialogs(mainw);


    /* Create Drawing area for 3D */ 


/*    drawarea = XtVaCreateManagedWidget("drawarea", glwMDrawingAreaWidgetClass,
                                     mainw, GLwNvisualInfo, vi, NULL);	*/

/*    drawarea = XtCreateManagedWidget("drawarea", xmDrawingAreaWidgetClass,mainw,NULL,0);	*/

    drawarea = XtCreateManagedWidget("drawarea", xmDrawingAreaWidgetClass,mainw,NULL,0);

/*    XtManageChild(drawarea);	*/

    XtAddCallback(drawarea,XmNexposeCallback,PEXexposeCB,1);
    XtAddCallback(drawarea,XmNinputCallback, PEXinputCB, 1);
    XtAddCallback(drawarea,XmNresizeCallback, PEXresizeCB, 0);
    XtAddEventHandler(drawarea,StructureNotifyMask,FALSE,PEXresizeCB,1);

    XmMainWindowSetAreas(mainw, menubar, NULL, NULL, NULL, drawarea);

    XtRealizeWidget(mainwidget);


    glXMakeCurrent(dpy, XtWindow(drawarea), cx);	
    made_current = GL_TRUE;

    /* setup OpenGL state */

    get_persp(0);
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE,GL_LINEAR);
    glFogf(GL_FOG_START, 3.5);
    glFogf(GL_FOG_END, 4.5);
/*    glFogf(GL_FOG_DENSITY, 50.0); */

    get_font(0);
    init_labels();



/**********************************************************************/
/*  Create PHIGS window for displaying graphics.                      */
/**********************************************************************/

    draw_win = XtWindow(drawarea);





    
    /* Initialize MIDI support */
    init_midi(app);

}


build_pmag(wid)
Widget wid;
{
   Widget col, rc, radio, label, sep, tog;
   Arg args[1];

/**********************************************************************/
/*  Create the PMAG options box.                                      */
/**********************************************************************/
   col = XmCreateRowColumn(wid, "column1", NULL, 0);
   XtManageChild(col);

   label  = XmCreateLabelGadget(col, "viewing", NULL, 0);
   XtManageChild(label);
   sep = XmCreateSeparator(col,"sep",NULL,0);
   XtManageChild(sep);
   XtSetArg(args[0],XmNorientation,XmHORIZONTAL);
   radio = XmCreateRadioBox(col, "radio", args, 0);
   XtManageChild(radio); 
   tog = XmCreateToggleButtonGadget(radio, "PMAG Data", args, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNarmCallback,toggleCB,16);	
   tog = XmCreateToggleButtonGadget(radio, "Polar Wander", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNarmCallback,toggleCB,18);	
   tog = XmCreateToggleButtonGadget(radio, "Polar Dispersion", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNarmCallback,toggleCB,17);	

   col = XmCreateRowColumn(wid, "column2", NULL, 0);
   XtManageChild(col);

   label  = XmCreateLabelGadget(col, "display", NULL, 0);
   XtManageChild(label);
   sep = XmCreateSeparator(col,"sep",NULL,0);
   XtManageChild(sep);
   rc = XmCreateRowColumn(col, "radio", NULL, 0);
   XtManageChild(rc);
   tog = XmCreateToggleButtonGadget(rc, "Hide Data", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNvalueChangedCallback,toggleCB,13);	
/*   tog = XmCreateToggleButtonGadget(rc, "Hide Uncertainty", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNvalueChangedCallback,toggleCB,14);	
*/   tog = XmCreateToggleButtonGadget(rc, "Hide Pole", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNvalueChangedCallback,toggleCB,15);	

   label  = XmCreateLabelGadget(col, "data", NULL, 0);
   XtManageChild(label);
   sep = XmCreateSeparator(col,"sep",NULL,0);
   XtManageChild(sep);
   rc = XmCreateRowColumn(col, "radio", NULL, 0);
   XtManageChild(rc);
   tog = XmCreateToggleButtonGadget(rc, "All Plates", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNvalueChangedCallback,toggleCB,10);	
   tog = XmCreateToggleButtonGadget(rc, "Reliable Plates", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNvalueChangedCallback,toggleCB,11);	
   XtSetArg(args[0],XmNorientation,XmHORIZONTAL);
   rc = XmCreateRowColumn(col, "one", args, 1);
   XtManageChild(rc);
   tog = XmCreateToggleButtonGadget(rc, "One Plate", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNvalueChangedCallback,toggleCB,12);	
   XtSetArg(args[0],XmNcolumns,3);
   pmag_text  = XmCreateText(rc, "text", args, 1);
   XtManageChild(pmag_text);

   tog = XmCreatePushButtonGadget(col, "Update PMAG Data", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,enterpmagCB,1);	
   tog = XmCreatePushButtonGadget(col, "Reset PW Variables", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,pushbuttCB,1);	



}

build_pb(wid)
Widget wid;
{
   Widget col, rc, radio, label, sep, tog;
   Arg args[1];

/**********************************************************************/
/*  Create the third column of the options box.                       */
/**********************************************************************/
   col = XmCreateRowColumn(wid, "column3", NULL, 0);
   XtManageChild(col);

   /*******************************************************************/
   /*  Create the plate location and modification box.                */
   /*******************************************************************/

   XtSetArg(args[0],XmNorientation,XmHORIZONTAL);
   rc = XmCreateRowColumn(col, "value", args, 1);
   XtManageChild(rc);
   label  = XmCreateLabelGadget(rc, "tectonic #", NULL, 0);
   XtManageChild(label);
   XtSetArg(args[0],XmNcolumns,3);
   cont_text  = XmCreateText(rc, "text", args, 1);
   XtManageChild(cont_text);

   XtSetArg(args[0],XmNorientation,XmHORIZONTAL);
   rc = XmCreateRowColumn(col, "number", args, 1);
   XtManageChild(rc);
   label  = XmCreateLabelGadget(rc, "pole", NULL, 0);
   XtManageChild(label);
   XtSetArg(args[0],XmNcolumns,1);
   pole_text  = XmCreateText(rc, "text", args, 1);
   XtManageChild(pole_text);
   tog = XmCreatePushButtonGadget(rc, "<", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,toggleCB,9);	
   tog = XmCreatePushButtonGadget(rc, ">", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,toggleCB,8);

   XtSetArg(args[0],XmNorientation,XmHORIZONTAL);
   rc = XmCreateRowColumn(col, "age", args, 1);
   XtManageChild(rc);
   label  = XmCreateLabelGadget(rc, "     ", NULL, 0);
   XtManageChild(label);
   XtSetArg(args[0],XmNcolumns,5);
   age_text1  = XmCreateText(rc, "end", args, 1);
   XtManageChild(age_text1);
   label  = XmCreateLabelGadget(rc, "-", NULL, 0);
   XtManageChild(label);
   XtSetArg(args[0],XmNcolumns,5);
   age_text2  = XmCreateText(rc, "end", args, 1);
   XtManageChild(age_text2);
   label  = XmCreateLabelGadget(rc, "ma", NULL, 0);
   XtManageChild(label);

   XtSetArg(args[0],XmNorientation,XmHORIZONTAL);
   rc = XmCreateRowColumn(col, "value", args, 1);
   XtManageChild(rc);
   label  = XmCreateLabelGadget(rc, "lat  ", NULL, 0);
   XtManageChild(label);
   XtSetArg(args[0],XmNcolumns,10);
   lat_text  = XmCreateText(rc, "text", args, 1);
   XtManageChild(lat_text);

   XtSetArg(args[0],XmNorientation,XmHORIZONTAL);
   rc = XmCreateRowColumn(col, "value", args, 1);
   XtManageChild(rc);
   label  = XmCreateLabelGadget(rc, "lon ", NULL, 0);
   XtManageChild(label);
   XtSetArg(args[0],XmNcolumns,10);
   lon_text  = XmCreateText(rc, "text", args, 1);
   XtManageChild(lon_text);

   XtSetArg(args[0],XmNorientation,XmHORIZONTAL);
   rc = XmCreateRowColumn(col, "value", args, 1);
   XtManageChild(rc);
   label  = XmCreateLabelGadget(rc, "Ang", NULL, 0);
   XtManageChild(label);
   XtSetArg(args[0],XmNcolumns,10);
   ang_text  = XmCreateText(rc, "text", args, 1);
   XtManageChild(ang_text);

   tog = XmCreatePushButtonGadget(col, "Update Plate", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,enterplateCB,1);	

}



build_panel(wid)
Widget wid;
{
   Widget col, rc, radio, label, sep, tog;
   Arg args[1];

/**********************************************************************/
/*  Create the first column of the options box.                       */
/**********************************************************************/
   col = XmCreateRowColumn(wid, "column1", NULL, 0);
   XtManageChild(col);

   /*******************************************************************/
   /*  Create the viewing options radio box.                          */
   /*******************************************************************/
   label  = XmCreateLabelGadget(col, "viewing", NULL, 0);
   XtManageChild(label);
   sep = XmCreateSeparator(col,"sep",NULL,0);
   XtManageChild(sep);
   XtSetArg(args[0],XmNorientation,XmHORIZONTAL);
   radio = XmCreateRadioBox(col, "radio", args, 1);
   XtManageChild(radio); 
   tog = XmCreateToggleButtonGadget(radio, "persp    ", args, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNvalueChangedCallback,toggleCB,1);	
   tog = XmCreateToggleButtonGadget(radio, "ortho", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNvalueChangedCallback,toggleCB,2);	
   XtSetArg(args[0],XmNorientation,XmHORIZONTAL);
   radio = XmCreateRadioBox(col, "radio", args, 1);
   XtManageChild(radio); 
   tog = XmCreateToggleButtonGadget(radio, "3D        ", args, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNvalueChangedCallback,toggleCB,18);	
   tog = XmCreateToggleButtonGadget(radio, "2D", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNvalueChangedCallback,toggleCB,19);	
   XtSetArg(args[0],XmNorientation,XmHORIZONTAL);
   radio = XmCreateRadioBox(col, "radio", args, 1);
   XtManageChild(radio); 
   tog = XmCreateToggleButtonGadget(radio, "low res", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNvalueChangedCallback,toggleCB,21);	
   tog = XmCreateToggleButtonGadget(radio, "high res", args, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNvalueChangedCallback,toggleCB,20);	

   /*******************************************************************/
   /*  Create the display options toggle box.                         */
   /*******************************************************************/
   label  = XmCreateLabelGadget(col, "display", NULL, 0);
   XtManageChild(label);
   sep = XmCreateSeparator(col,"sep",NULL,0);
   XtManageChild(sep);
   rc = XmCreateRowColumn(col, "radio", NULL, 0);
   XtManageChild(rc);
   tog = XmCreateToggleButtonGadget(rc, "Hide Labels", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNvalueChangedCallback,toggleCB,3);	
   tog = XmCreateToggleButtonGadget(rc, "Hide Data", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNvalueChangedCallback,toggleCB,4);	
   tog = XmCreateToggleButtonGadget(rc, "Hide Fog", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNvalueChangedCallback,toggleCB,22);	
   tog = XmCreateToggleButtonGadget(rc, "Depth Clipping", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNvalueChangedCallback,toggleCB,5);	
   tog = XmCreateToggleButtonGadget(rc, "Hide Pulldown Menus", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNvalueChangedCallback,toggleCB,23);	

   label  = XmCreateLabelGadget(col, "dials", NULL, 0);
   XtManageChild(label);
   sep = XmCreateSeparator(col,"sep",NULL,0);
   XtManageChild(sep);
   XtSetArg(args[0],XmNorientation,XmHORIZONTAL);
   rc = XmCreateRowColumn(col, "increment", args, 1);
   XtManageChild(rc);
   XtSetArg(args[0],XmNcolumns,4);
   rot_text  = XmCreateText(rc, "text", args, 1);
   XtManageChild(rot_text);
   tog = XmCreatePushButtonGadget(rc, "Rot Speed", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,dialsCB,3);	
   XtSetArg(args[0],XmNorientation,XmHORIZONTAL);
   rc = XmCreateRowColumn(col, "buttons", args, 1);
   XtManageChild(rc);
   tog = XmCreatePushButtonGadget(rc, "  Home  ", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,dialsCB,1);	
   tog = XmCreatePushButtonGadget(rc, "Restore", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,dialsCB,2);	

   label  = XmCreateLabelGadget(col, "window", NULL, 0);
   XtManageChild(label);
   sep = XmCreateSeparator(col,"sep",NULL,0);
   XtManageChild(sep);
   XtSetArg(args[0],XmNorientation,XmHORIZONTAL);
   rc = XmCreateRowColumn(col, "buttons", args, 1);
   XtManageChild(rc);
   tog = XmCreatePushButtonGadget(rc, "  Close  ", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,toggleCB,6);
   tog = XmCreatePushButtonGadget(rc, "Refresh", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,toggleCB,7);	


/**********************************************************************/
/*  Create the second column of the options box.                      */
/**********************************************************************/
   col = XmCreateRowColumn(wid, "column2", NULL, 0);
   XtManageChild(col);

   /*******************************************************************/
   /*  Create the picking modes radio box.                            */
   /*******************************************************************/
   label  = XmCreateLabelGadget(col, "picking", NULL, 0);
   XtManageChild(label);
   sep = XmCreateSeparator(col,"sep",NULL,0);
   XtManageChild(sep);
   radio = XmCreateRadioBox(col, "radio", NULL, 0);
   XtManageChild(radio);
   tog = XmCreateToggleButtonGadget(radio, "Get Info", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNvalueChangedCallback,pickmodeCB,GET_INFO);	
   tog = XmCreateToggleButtonGadget(radio, "Move Plate", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNvalueChangedCallback,pickmodeCB,MOVE_PLATE);	
/*   tog = XmCreateToggleButtonGadget(radio, "Delete Label", NULL, 0);
   XtManageChild(tog); 
   XtAddCallback(tog,XmNvalueChangedCallback,pickmodeCB,DEL_LABEL);	
*/   tog = XmCreateToggleButtonGadget(radio, "Delete Plate", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNvalueChangedCallback,pickmodeCB,DEL_PLATE);	
   tog = XmCreateToggleButtonGadget(radio, "Delete Elem", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNvalueChangedCallback,pickmodeCB,DEL_ELEM);	
   tog = XmCreateToggleButtonGadget(radio, "Display Plate", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNvalueChangedCallback,pickmodeCB,DIS_PLATE);	
   tog = XmCreateToggleButtonGadget(radio, "Display Elem", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNvalueChangedCallback,pickmodeCB,DIS_ELEM);	

}

build_about(wid)
Widget wid;
{
   Widget col, rc, radio, label, sep, tog, scroll;
   Arg args[8];
   char buffer[128];
   char newstring[10000];
   int newstring_length = 0;
   char infile[80];
   FILE *fpin;
   int i;
   
   // return; // System commands do not work. Added by Jeremy Archer, Jun 27, 2008
   // System commands changed out for git revisions. Jeremy Archer, Aug 21, 2009

/**********************************************************************/
/*  Create the row/column of the about box.                       */
/**********************************************************************/
   col = XmCreateRowColumn(wid, "column", NULL, 0);
   XtManageChild(col);

   XtSetArg(args[0],XmNentryAlignment,XmALIGNMENT_CENTER);
   rc = XmCreateRowColumn(col, "about", args, 1);
   XtManageChild(rc);
   label  = XmCreateLabelGadget(rc, 
	"Paleogeographic Mapping Tools", NULL, 0);
   XtManageChild(label);
   sep = XmCreateSeparator(rc,"sep",NULL,0);
   XtManageChild(sep);

    /* system("\mv pmt.info pmt.info%; awk '{if ($1 != \"PMT\") print $0}' pmt.info% > pmt.info");
     * system("ls -l /usr/people/rowley/Paleomapper/pmt | awk '{i=NF-1; j=NF-2; k=NF-3; print \"PMT last modified \"$k,$j,$i}' >> pmt.info");
     */
    
    /* sprintf(infile,"pmt.info");
     * if ((fpin = fopen(infile,"r")) == NULL)
     * 	{ printf("Error opening %s\n",infile); } 
     * if (fgets(buffer,127,fpin) != NULL) {
     *     strcpy(newstring,buffer);
     * 	while (fgets(buffer,127,fpin) != NULL) 
     *         strcat(newstring,buffer);
     * 	fclose(fpin);
     * }
     */
    
    char temp[1000];
    
    #ifdef GIT_VERSION
    sprintf(temp, "Git version: %s\n\n", GIT_VERSION);
    strcpy(newstring, temp);
    newstring_length += strlen(temp);
    #endif
    
    sprintf(temp, "%s", ABOUT);
    strcpy(newstring + newstring_length, temp);
    newstring_length += strlen(temp);
    
    newstring[newstring_length] = 0;
    
    XtSetArg(args[0], XmNvisual, vi->visual); 
    XtSetArg(args[1], XmNeditMode, XmMULTI_LINE_EDIT); 
    XtSetArg(args[2], XmNvalue, newstring); 
    XtSetArg(args[3], XmNheight, 200); 
    XtSetArg(args[4], XmNwidth, 400); 
    XtSetArg(args[5], XmNeditable, FALSE); 
    XtSetArg(args[6], XmNwordWrap, TRUE); 
    XtSetArg(args[7], XmNscrollHorizontal, FALSE); 
    scroll = XmCreateScrolledText(rc, "help", args, 8); 
    XtManageChild(scroll);

}


Widget CreateFileSelect(parent,filter,call)
Widget parent;
char   *filter;
int    (*call) ();
{
    int n;
    Arg      args[10];
    XmString prompt;
    Widget   select;

    n = 0;
    prompt = XmStringCreate(filter, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNtextColumns, 80); n++;
    XtSetArg(args[n], XmNvisibleItemCount, 15); n++;
    XtSetArg(args[n], XmNdirMask,prompt ); n++;
    XtSetArg(args[n], XmNvisual, vi->visual); n++;
    
    
    select = XmCreateFileSelectionDialog(parent, "Select File", args, n);
    XtAddCallback(select, XmNokCallback, call, 1);	
    XtAddCallback(select, XmNcancelCallback, call, 2);	
    XtAddCallback(select, XmNhelpCallback, call, 3);	

    return(select);
}

build_dialogs(parent)
Widget parent;
{
    int n;
    Arg      args[10];

    n = 1;
    XtSetArg(args[0], XmNvisual, vi->visual); 
    working = XmCreateWorkingDialog(parent,"working",args,n);
    XtAddCallback(working,XmNokCallback,removewidCB,1);	
    prompt = XmCreatePromptDialog(parent,"prompt",args,n);
    XtUnmanageChild(XmSelectionBoxGetChild(prompt,XmDIALOG_HELP_BUTTON));
    XtAddCallback(prompt,XmNokCallback,promptCB,1);	
    XtAddCallback(prompt,XmNcancelCallback,promptCB,-1);	
    inform = XmCreateInformationDialog(parent,"inform",args,n);
    XtAddCallback(inform,XmNokCallback,removewidCB,1);	
    warning = XmCreateWarningDialog(parent,"warning",args,n);
    XtAddCallback(warning,XmNokCallback,removewidCB,1);	
    exitwarning = XmCreateQuestionDialog(parent,"Exit",args,n);
    XtUnmanageChild(XmMessageBoxGetChild(exitwarning,XmDIALOG_HELP_BUTTON));
    XtAddCallback(exitwarning, XmNokCallback, exitokCB, 0);	
    XtAddCallback(exitwarning, XmNcancelCallback, removewidCB, 1);	
    clearwarning = XmCreateQuestionDialog(parent,"Clear",args,n);
    XtUnmanageChild(XmMessageBoxGetChild(clearwarning,XmDIALOG_HELP_BUTTON));
    XtAddCallback(clearwarning, XmNokCallback, clearokCB, 0);	
    XtAddCallback(clearwarning, XmNcancelCallback, removewidCB, 1);	
}

static int input_recieved;

prompt_message(text)
char *text;
{
    int n;
    Arg args[10];
    XmString string;
    char *input;
    XEvent  event;

    n = 0;
    while (*text == ' ') text++;
    string = XmStringCreateLtoR(text,XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNselectionLabelString, string); n++;
    XtSetValues(prompt,args,n);
    XmTextSetString(XmSelectionBoxGetChild(prompt,XmDIALOG_TEXT),"");
    XtManageChild(prompt);
    input_recieved = 0;
    while (!input_recieved) {
        XtAppNextEvent(app,&event);
        XtDispatchEvent(&event); 
    }
    if (input_recieved == 1) {
        input = XmTextGetString(XmSelectionBoxGetChild(prompt,XmDIALOG_TEXT));
        strcpy(text,input);
    } else if (input_recieved == -1) {
	sprintf(text,"CANCEL");
    } else {
	*text = '\0';
    }

}

promptCB(w, client_data, call_data)
Widget                w;
caddr_t               client_data;
XmAnyCallbackStruct   *call_data;
{
    input_recieved = (int) client_data;
}

print_message(text)
char *text;
{
    int n;
    Arg args[10];
    XmString string;

    n = 0;
    while (*text == ' ') text++;
    string = XmStringCreateLtoR(text,XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNmessageString, string); n++;
    XtSetValues(inform,args,n);
    XtManageChild(inform);
}

print_warning(text)
char *text;
{
    int n;
    Arg args[10];
    XmString string;

    n = 0;
    while (*text == ' ') text++;
    string = XmStringCreateLtoR(text,XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNmessageString, string); n++;
    XtSetValues(warning,args,n);
    XtManageChild(warning);
}

clear_warning(text)
char *text;
{
    int n;
    Arg args[10];
    XmString string;

    n = 0;
    while (*text == ' ') text++;
    string = XmStringCreateLtoR(text,XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNmessageString, string); n++;
    XtSetValues(clearwarning,args,n);
    XtManageChild(clearwarning);
}

display_waiting(text)
char *text;
{
    int n;
    Arg args[10];
    XmString string;

    n = 0;
    while (*text == ' ') text++;
    string = XmStringCreateLtoR(text,XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNmessageString, string); n++;
    XtSetValues(working,args,n);
    XtManageChild(working);
    flush_events();
}

remove_waiting()
{
    XtUnmanageChild(working);
}

flush_events()
{
    XEvent  event;

    XSync(dpy,0);
    sleep(1);
    while (XPending(dpy) > 0) {
        while (QLength(dpy) > 0) {
            XtAppNextEvent(app,&event);
            XtDispatchEvent(&event);
        }
    }
}

