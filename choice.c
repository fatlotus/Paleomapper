#include <stdio.h>
#include "paleotypes.h"
#include "paleodefs.h"
#include "pex.h"
#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Separator.h>
#include <Xm/CascadeB.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>


extern delete_by_type();
extern delete_by_age();
extern display_all();
extern labelCB();
extern mainmenuCB();
extern optmenuCB();
extern lithCB();
extern infoCB();
extern toggleCB();
extern pickmodeCB();
extern pullmenuCB();
extern helpmenuCB();

static MENUSTRUCT removemenu[] = {
    {"By Data Type", delete_by_type, NULL, 1},
    {"By Age", delete_by_age, NULL, 2},
    {NULL,0,0,0}
};

static MENUSTRUCT main[] = {
    {"About PMT", mainmenuCB, NULL, 12},
    {"Get Data", mainmenuCB, NULL, 1},
    {"Get PMAG Data", mainmenuCB, NULL, 9},
    {"Get General Data", mainmenuCB, NULL, 10},
    {"Reconstruction", mainmenuCB, NULL, 2},
    {"Animation", mainmenuCB, NULL, 3},
    {"Remove", mainmenuCB, removemenu, 6},
    {"Clear Data", mainmenuCB, NULL, 11},
    {"Re-Display All Data", display_all, NULL, 0},
    {"Save Data", mainmenuCB, NULL, 4},
    {"Quit", mainmenuCB, NULL, 0},
};

Widget te_entry[5],prim_entry[5],name_entry[5];
Widget submenu1,submenu2,submenu3;
Widget pullmenu,cascade,popinfo,pullmenu2;
extern Widget menubar, mainw;
extern XVisualInfo *vi;
extern removewidCB();
extern GLboolean hits_up;
extern char name[5][50];
Widget polytog, cliptog;
Widget help_scroll[20];
char helpnamestr[20][128];


init_choice()
{
    int n,i, count;
    Arg args[1], menuargs[1],bargs[5];
    Widget entry,tog;
    Widget submenu,pullmenu3,pullmenu4,pullmenu5;

    XtSetArg(menuargs[0], XmNvisual, vi->visual);
    pullmenu = XmCreatePulldownMenu(menubar, "menupane", menuargs, 1);
    pullmenu2 = XmCreatePulldownMenu(menubar, "menupane", menuargs, 1);
    pullmenu3 = XmCreatePulldownMenu(menubar, "menupane", menuargs, 1);
    pullmenu4 = XmCreatePulldownMenu(menubar, "menupane", menuargs, 1);
    pullmenu5 = XmCreatePulldownMenu(menubar, "menupane", menuargs, 1);

    count = sizeof (main) / sizeof (MENUSTRUCT);
    
    for (i=0; i<count-1; i++) {
	if (main[i].submenu != NULL) {
            XtSetArg(args[0],XmNvisual, vi->visual);
            submenu = XmCreatePulldownMenu(pullmenu,"submenu",args,1);
	    SetPulldownButtons(submenu,main[i].submenu);
	    XtSetArg(args[0],XmNsubMenuId,submenu);
	    entry = XmCreateCascadeButton(pullmenu,main[i].text,args,1);
  	}
	else {
	    entry = XmCreatePushButton(pullmenu,main[i].text,args,0);
	}
	XtAddCallback(entry, XmNactivateCallback, main[i].callback,main[i].data);	
        XtManageChild(entry);
    }

    i = count-1;
    entry = XmCreateSeparator(pullmenu,"sep",NULL,0); 
    XtManageChild(entry);

    entry = XmCreatePushButton(pullmenu, main[i].text, args, 0);
    XtAddCallback(entry, XmNactivateCallback, main[i].callback,main[i].data);	
    XtManageChild(entry);
    XtSetArg(args[0], XmNsubMenuId, pullmenu);
    cascade = XmCreateCascadeButton(menubar, "Main  ", args, 1);
    XtManageChild(cascade);
    
    /* create Boxes Pulldown Menu */
    entry = XmCreatePushButton(pullmenu2, "Main Options", args, 0);
    XtAddCallback(entry, XmNactivateCallback, mainmenuCB,5);	
    XtManageChild(entry);
    entry = XmCreatePushButton(pullmenu2, "Plate Options", args, 0);
    XtAddCallback(entry, XmNactivateCallback, optmenuCB,1);	
    XtManageChild(entry);
    entry = XmCreatePushButton(pullmenu2, "PMAG Options", args, 0);
    XtAddCallback(entry, XmNactivateCallback, optmenuCB,2);	
    XtManageChild(entry);
    entry = XmCreatePushButton(pullmenu2, "Virtual Dials", args, 0);
    XtAddCallback(entry, XmNactivateCallback, optmenuCB,3);	
    XtManageChild(entry);
    entry = XmCreatePushButton(pullmenu2, "Get Lith Data", args, 0);
    XtAddCallback(entry, XmNactivateCallback, optmenuCB,4);	
    XtManageChild(entry);
    entry = XmCreatePushButtonGadget(pullmenu2, "Lith Options", args, 0);
    XtManageChild(entry);
    XtAddCallback(entry,XmNactivateCallback,lithCB,18);	
    XtSetArg(args[0], XmNsubMenuId, pullmenu2);
    cascade = XmCreateCascadeButton(menubar, "Boxes  ", args, 1);
    XtManageChild(cascade);

    /* create Options Pulldown Menu */
    entry = XmCreatePushButton(pullmenu3, "Refresh", args, 0);
    XtAddCallback(entry, XmNactivateCallback, toggleCB,7);	
    XtManageChild(entry);
         XtSetArg(args[0],XmNvisual, vi->visual);
        submenu = XmCreatePulldownMenu(pullmenu3,"submenu",args,1);
        XtSetArg(args[0],XmNset, TRUE);
    	entry = XmCreateToggleButtonGadget(submenu, "Ortho", args, 1);
    	XtAddCallback(entry, XmNvalueChangedCallback, pullmenuCB,2);	
    	XtManageChild(entry);
    	entry = XmCreateToggleButtonGadget(submenu, "2D Mode", args, 0);
    	XtAddCallback(entry, XmNvalueChangedCallback, pullmenuCB,3);	
    	XtManageChild(entry);
    	entry = XmCreateToggleButtonGadget(submenu, "High Res", args, 0);
    	XtAddCallback(entry, XmNvalueChangedCallback, pullmenuCB,1);	
    	XtManageChild(entry);
	XtSetArg(args[0],XmNsubMenuId,submenu);
    entry = XmCreateCascadeButton(pullmenu3,"Viewing",args,1);
    XtManageChild(entry);
        XtSetArg(args[0],XmNvisual, vi->visual);
        submenu = XmCreatePulldownMenu(pullmenu3,"submenu",args,1);
	entry = XmCreatePushButton(submenu, "Rectilinear", args, 0);
	XtAddCallback(entry, XmNactivateCallback,pullmenuCB,4 );
	XtManageChild(entry);
	entry = XmCreatePushButton(submenu, "Molleweide", args, 0);
	XtAddCallback(entry, XmNactivateCallback, pullmenuCB,5 );
	XtManageChild(entry);
	entry = XmCreatePushButton(submenu, "Mercator", args, 0);
	XtAddCallback(entry, XmNactivateCallback, pullmenuCB,6 );
	XtManageChild(entry);
	entry = XmCreatePushButton(submenu, "Stereo", args, 0);
	XtAddCallback(entry, XmNactivateCallback, pullmenuCB,7 );
	XtManageChild(entry);
	XtSetArg(args[0],XmNsubMenuId,submenu);
    entry = XmCreateCascadeButton(pullmenu3,"2D Proj",args,1);
    XtAddCallback(entry, XmNactivateCallback, mainmenuCB,6);
    XtManageChild(entry);
        XtSetArg(args[0],XmNvisual, vi->visual);
        submenu = XmCreatePulldownMenu(pullmenu3,"submenu",args,1);
   	tog = XmCreateToggleButtonGadget(submenu, "HideLabels", NULL, 0);
   	XtManageChild(tog);
   	XtAddCallback(tog,XmNvalueChangedCallback,toggleCB,3);	
   	tog = XmCreateToggleButtonGadget(submenu, "HideData", NULL, 0);
   	XtManageChild(tog);
   	XtAddCallback(tog,XmNvalueChangedCallback,toggleCB,4);	
   	tog = XmCreateToggleButtonGadget(submenu, "HideFog", NULL, 0);
   	XtManageChild(tog);
   	XtAddCallback(tog,XmNvalueChangedCallback,toggleCB,22);	
   	cliptog = XmCreateToggleButtonGadget(submenu, "DepthClipping", NULL, 0);
   	XtManageChild(cliptog);
   	XtAddCallback(cliptog,XmNvalueChangedCallback,toggleCB,5);	
   	tog = XmCreateToggleButtonGadget(submenu, "HideGrid", NULL, 0);
   	XtManageChild(tog);
   	XtAddCallback(tog,XmNvalueChangedCallback,pullmenuCB,8);	
   	polytog = XmCreateToggleButtonGadget(submenu, "ShowPolygons", NULL, 0);
   	XtManageChild(polytog);
   	XtAddCallback(polytog,XmNvalueChangedCallback,pullmenuCB,20);	
   	tog = XmCreateToggleButtonGadget(submenu, "ShowGlobe", NULL, 0);
   	XtManageChild(tog);
   	XtAddCallback(tog,XmNvalueChangedCallback,pullmenuCB,21);	
	XtSetArg(args[0],XmNsubMenuId,submenu);
    entry = XmCreateCascadeButton(pullmenu3,"Display",args,1);
    XtAddCallback(entry, XmNactivateCallback, mainmenuCB,6);
    XtManageChild(entry);
        XtSetArg(args[0],XmNvisual, vi->visual);
        submenu = XmCreatePulldownMenu(pullmenu3,"submenu",args,1);
	entry = XmCreatePushButton(submenu, "GetInfo", args, 0);
	XtAddCallback(entry, XmNactivateCallback,pickmodeCB,GET_INFO );
	XtManageChild(entry);
	entry = XmCreatePushButton(submenu, "MovePlate", args, 0);
	XtAddCallback(entry, XmNactivateCallback, pickmodeCB,MOVE_PLATE);
	XtManageChild(entry);
	entry = XmCreatePushButton(submenu, "DeletePlate", args, 0);
	XtAddCallback(entry, XmNactivateCallback, pickmodeCB,DEL_PLATE);
	XtManageChild(entry);
	entry = XmCreatePushButton(submenu, "DeleteElem", args, 0);
	XtAddCallback(entry, XmNactivateCallback, pickmodeCB,DEL_ELEM);
	XtManageChild(entry);
	entry = XmCreatePushButton(submenu, "DisplayPlate", args, 0);
	XtAddCallback(entry, XmNactivateCallback, pickmodeCB,DIS_PLATE);
	XtManageChild(entry);
	entry = XmCreatePushButton(submenu, "DisplayElem", args, 0);
	XtAddCallback(entry, XmNactivateCallback, pickmodeCB,DIS_ELEM);
	XtManageChild(entry);
	entry = XmCreatePushButton(submenu, "PickColor", args, 0);
	XtAddCallback(entry, XmNactivateCallback, pickmodeCB,8);
	XtManageChild(entry);
	XtSetArg(args[0],XmNsubMenuId,submenu);
    entry = XmCreateCascadeButton(pullmenu3,"Picking",args,1);
    XtAddCallback(entry, XmNactivateCallback, mainmenuCB,6);
    XtManageChild(entry);
        XtSetArg(args[0],XmNvisual, vi->visual);
        submenu = XmCreatePulldownMenu(pullmenu3,"submenu",args,1);
	entry = XmCreatePushButton(submenu, "Insert Label", args, 0);
	XtAddCallback(entry, XmNactivateCallback,mainmenuCB,8);
	XtManageChild(entry);
	entry = XmCreatePushButton(submenu, "Clear Labels", args, 0);
	XtAddCallback(entry, XmNactivateCallback,pullmenuCB,9);
	XtManageChild(entry);
	XtSetArg(args[0],XmNsubMenuId,submenu);
    entry = XmCreateCascadeButton(pullmenu3,"Labels",args,1);
    XtAddCallback(entry, XmNactivateCallback, mainmenuCB,6);
    XtManageChild(entry);
    XtSetArg(args[0], XmNsubMenuId, pullmenu3);
    cascade = XmCreateCascadeButton(menubar, "Options  ", args, 1);
    XtManageChild(cascade);

    entry = XmCreatePushButton(pullmenu4, "Choose Image", args, 0);
    XtAddCallback(entry, XmNactivateCallback, pullmenuCB,10);	
    XtManageChild(entry);
    entry = XmCreatePushButton(pullmenu4, "Dump Image", args, 0);
    XtAddCallback(entry, XmNactivateCallback, pullmenuCB,19);	
    XtManageChild(entry);
    entry = XmCreatePushButton(pullmenu4, "Convert Image", args, 0);
    XtAddCallback(entry, XmNactivateCallback, pullmenuCB,18);	
    XtManageChild(entry);
    entry = XmCreatePushButton(pullmenu4, "Make Movie", args, 0);
    XtAddCallback(entry, XmNactivateCallback, pullmenuCB,11);	
    XtManageChild(entry);
    entry = XmCreatePushButton(pullmenu4, "Watch Movie", args, 0);
    XtAddCallback(entry, XmNactivateCallback, pullmenuCB,12);	
    XtManageChild(entry);
    XtSetArg(args[0], XmNsubMenuId, pullmenu4);
    cascade = XmCreateCascadeButton(menubar, "Images", args, 1);
    XtManageChild(cascade);

    entry = XmCreatePushButton(pullmenu5, "Main", args, 0);
    XtAddCallback(entry, XmNactivateCallback, helpmenuCB,1);	
    XtManageChild(entry);
        XtSetArg(args[0],XmNvisual, vi->visual);
        submenu = XmCreatePulldownMenu(pullmenu5,"submenu",args,1);
	entry = XmCreatePushButton(submenu, "Main Options", args, 0);
	XtAddCallback(entry, XmNactivateCallback,helpmenuCB,11 );
	XtManageChild(entry);
	entry = XmCreatePushButton(submenu, "PMAG Options", args, 0);
	XtAddCallback(entry, XmNactivateCallback, helpmenuCB,12 );
	XtManageChild(entry);
	entry = XmCreatePushButton(submenu, "Plate Options", args, 0);
	XtAddCallback(entry, XmNactivateCallback, helpmenuCB,13 );
	XtManageChild(entry);
	entry = XmCreatePushButton(submenu, "Lith Options", args, 0);
	XtAddCallback(entry, XmNactivateCallback, helpmenuCB,14 );
	XtManageChild(entry);
	entry = XmCreatePushButton(submenu, "Lith Pick", args, 0);
	XtAddCallback(entry, XmNactivateCallback,helpmenuCB,15 );
	XtManageChild(entry);
	entry = XmCreatePushButton(submenu, "Virtual Dials", args, 0);
	XtAddCallback(entry, XmNactivateCallback,helpmenuCB,16 );
	XtManageChild(entry);
	XtSetArg(args[0],XmNsubMenuId,submenu);
    entry = XmCreateCascadeButton(pullmenu5, "Boxes", args, 1);
    XtAddCallback(entry, XmNactivateCallback, helpmenuCB,-2);	
    XtManageChild(entry);
    entry = XmCreatePushButton(pullmenu5, "Options", args, 0);
    XtAddCallback(entry, XmNactivateCallback, helpmenuCB,3);	
    XtManageChild(entry);
    entry = XmCreatePushButton(pullmenu5, "Images", args, 0);
    XtAddCallback(entry, XmNactivateCallback, helpmenuCB,4);	
    XtManageChild(entry);
    entry = XmCreatePushButton(pullmenu5, "General", args, 0);
    XtAddCallback(entry, XmNactivateCallback, helpmenuCB,5);	
    XtManageChild(entry);
    entry = XmCreatePushButton(pullmenu5, "Commands", args, 0);
    XtAddCallback(entry, XmNactivateCallback, helpmenuCB,6);	
    XtManageChild(entry);
    entry = XmCreatePushButton(pullmenu5, "User Notes", args, 0);
    XtAddCallback(entry, XmNactivateCallback, helpmenuCB,17);	
    XtManageChild(entry);
    XtSetArg(args[0], XmNsubMenuId, pullmenu5);
    cascade = XmCreateCascadeButton(menubar, "Help", args, 1);
    XtManageChild(cascade);

    /***  CREATE  POPUP  INFO  MENU  ***/
    XtSetArg(args[0], XmNvisual, vi->visual);
    popinfo = XmCreatePopupMenu(mainw, "PopInfo", args, 1);

    entry = XmCreatePushButton(popinfo,"Close",args,0);
    XtAddCallback(entry, XmNactivateCallback, infoCB,0);	
    XtManageChild(entry);

    entry = XmCreateSeparator(popinfo,"sep",NULL,0); 
    XtManageChild(entry); 

    XtSetArg(args[0],XmNvisual, vi->visual);
    submenu1 = XmCreatePulldownMenu(popinfo,"submenu1",args,1);
    XtSetArg(args[0],XmNsubMenuId,submenu1);
    entry = XmCreateCascadeButton(popinfo,"Tectonic Element",args,1);
    XtAddCallback(entry, XmNactivateCallback, infoCB,0);	
    XtManageChild(entry);

    XtSetArg(args[0],XmNvisual, vi->visual);
    submenu2 = XmCreatePulldownMenu(popinfo,"submenu2",args,1);
    XtSetArg(args[0],XmNsubMenuId,submenu2);
    entry = XmCreateCascadeButton(popinfo,"Set Number",args,1);
    XtAddCallback(entry, XmNactivateCallback, infoCB,0);	
    XtManageChild(entry);

    XtSetArg(args[0],XmNvisual, vi->visual);
    submenu3 = XmCreatePulldownMenu(popinfo,"submenu3",args,1);
    XtSetArg(args[0],XmNsubMenuId,submenu3);
    entry = XmCreateCascadeButton(popinfo,"Name",args,1);
    XtAddCallback(entry, XmNactivateCallback, infoCB,0);	
    XtManageChild(entry);

	/*** Pick Buttons are set later when we have values ***/
}

SetPulldownButtons(parent,menu)
Widget parent;
MENUSTRUCT *menu;
{
    int i;
    Widget entry;

    i = 0;
    while (menu[i].text != NULL) {
        entry = XmCreatePushButton(parent, menu[i].text, NULL, 0);
        XtAddCallback(entry, XmNactivateCallback, menu[i].callback,menu[i].data);	
        XtManageChild(entry);
	i++;
    }
}


SetPickButtons(hits,before,number,info_te,filename)
int hits,before,number[5],info_te[5],*filename[];
{
    int i;
    char str_te[20];
    char text[20];

    if (hits>5) hits=5;
    if (before>5) before=5;

    if (hits_up) for(i=0;i<before;i++)
    {
	XtUnmanageChild(te_entry[i]);
	XtUnmanageChild(prim_entry[i]);
	XtUnmanageChild(name_entry[i]);
    }
	

    for(i=0;i<hits;i++)
    {
	sprintf(text,"%d",info_te[i]);	
	te_entry[i] = XmCreatePushButton(submenu1,text,NULL,0);
	XtAddCallback(te_entry[i], XmNactivateCallback, infoCB,0);	
	XtManageChild(te_entry[i]);

	sprintf(text,"%d",number[i]);	
	prim_entry[i] = XmCreatePushButton(submenu2,text,NULL,0);
	XtAddCallback(prim_entry[i], XmNactivateCallback, infoCB,0);	
	XtManageChild(prim_entry[i]);

	name_entry[i] = XmCreatePushButton(submenu3,filename[i],NULL,0);
	XtAddCallback(name_entry[i], XmNactivateCallback, infoCB,10+i);	
	XtManageChild(name_entry[i]);
    }

}


build_help(box) 
int box;
{
  char infile[80];
  FILE *fpin;
  char buffer[128];
  char newstring[10000];
  Pixmap pixmap;
  Widget  mainhelp, mainw2;	
  Arg args[10];
  int n,i;	
  XmString string;
  XmString label;
  Widget help_box;

    i=box;

    sprintf(infile,"help/help.%d",i);

    if ((fpin = fopen(infile,"r")) == NULL)
    	{ printf("Error opening %s\n",infile); } 
    rewind(infile);
    if (fgets(buffer,127,fpin) != NULL) {
        strcpy(newstring,"\0");
	strcpy(helpnamestr[i],buffer); 
	for (n=0;n<strlen(buffer);n++) 
		if (buffer[n]=='\n') buffer[n]='\0'; 

    	string = XmStringCreateLtoR(buffer,XmSTRING_DEFAULT_CHARSET);
    	while (fgets(buffer,127,fpin) != NULL) 
            strcat(newstring,buffer);
    	fclose(fpin);
    }

    label = XmStringCreateLtoR("Save",XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[0], XmNvisual, vi->visual);
    XtSetArg(args[1], XmNmessageString, string); 
    XtSetArg(args[2], XmNhelpLabelString, label); 
    help_box = XmCreateQuestionDialog(mainw,"Help",args,3);
    XtAddCallback(help_box,XmNokCallback,helpmenuCB,9);	
    XtAddCallback(help_box,XmNcancelCallback,helpmenuCB,10);	
    XtAddCallback(help_box,XmNhelpCallback,helpmenuCB,101+i);	
    XtSetArg(args[0], XmNvisual, vi->visual); 
    XtSetArg(args[1], XmNeditMode, XmMULTI_LINE_EDIT); 
    XtSetArg(args[2], XmNvalue, newstring); 
    XtSetArg(args[3], XmNheight, 200); 
    XtSetArg(args[4], XmNwidth, 400); 
    XtSetArg(args[5], XmNeditable, TRUE); 
    XtSetArg(args[6], XmNwordWrap, TRUE); 
    XtSetArg(args[7], XmNscrollHorizontal, FALSE); 
    help_scroll[i] = XmCreateScrolledText(help_box, "help", args, 8); 
    XtManageChild(help_scroll[i]);
    XtManageChild(help_box);
	
}

savehelp(i)
int i;
{
char *newstring;
  char infile[80];
  FILE *fpin;

    sprintf(infile,"help/help.%d",i);
    if ((fpin = fopen(infile,"w")) == NULL)
    	{ printf("Error opening %s\n",infile); } 
    fprintf(fpin,"%s",helpnamestr[i]);
    newstring = XmTextGetString(help_scroll[i]);
    fprintf(fpin,"%s",newstring);
    fclose(fpin);
  
}
