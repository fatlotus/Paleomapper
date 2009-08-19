#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <math.h>
#include <string.h>
#include "paleotypes.h"
#include "paleodefs.h"
#include "paleoexterns.h"

extern GLboolean select_mode;
extern Display *dpy;
extern GLboolean projection;
static int label_proj=1.0;

static LABEL label[10];
static int label_num=0;
static int recon_label_num=20;

extern float PaleoTime;

add_label()
{
    char        text[80];
    int         size;
    char        input[255];
    Ppoint      loc;
    float	color;
    Ppoint3	values;

    sprintf(text,"Enter label to be displayed (max 30 chars.)\n");
    prompt_message(text);

    sprintf(input,"Enter label size from 1 - 12 (1) ",
	"WARNING: All labels will be set to this size! ");
    prompt_message(input);
    size = atoi(input);
    if (size == 0) size = 1;

    sprintf(input,"Enter label color (0=blue, 120=red, 240=green)\n");
    prompt_message(input);
    color = atof(input);

    sprintf(input,"%s\n%s\n%s","Enter Coordinates for Label: x,y",
			       "Where (0.0, 0.0) is the Lower Left",
                               "and (1.0, 1.0) is the Upper Right");
    prompt_message(input);
    sscanf(input,"%f,%f",&loc.x,&loc.y);

    label[label_num].x = (loc.x*3.6)-1.8;
    label[label_num].y = (loc.y*3.6)-1.8;
    if (!projection) {
	label[label_num].x = label[label_num].x/1.8;
	label[label_num].y = label[label_num].y/1.8;
    }
    strcpy(label[label_num].words,text);
/*    label[label_num].draw = TRUE;	*/
    label[label_num].draw = size;
	get_font(label[label_num].draw);
    hsi2rgb((double)color,1.0,1.0,&values);
/*    printf("r g b  %f %f %f \n",values.x,values.y,values.z);*/
    label[label_num].color[0] = values.x;
    label[label_num].color[1] = values.y;
    label[label_num].color[2] = values.z;


    label_num++;

}

delete_label()
{
    int i;

    for(i=0;i<10;i++) {
	label[i].draw=FALSE;
    }

    label_num=0;

    if (recon_label_num != 20) {
	recon_label_num = label_num;
	label_num++;
    }

}


get_font(size)
int size;
{

    XFontStruct *fontInfo;
    Font id;
    unsigned int first, last;
    char fontstr[90];
    int fontsize1,fontsize2;

/*
    switch(size)
    {
	case 0: fontInfo = XLoadQueryFont(dpy,
	"-adobe-courier-bold-r-normal--34-240-100-100-m-200-iso8859-1"); 
		break;
	case 1: fontInfo = XLoadQueryFont(dpy,
	"-*-courier-bold-r-normal--34-*-*-*-m-200-iso8859-1"); 
		break;
	case 2: fontInfo = XLoadQueryFont(dpy,
	"-*-courier-bold-r-normal--25-*-*-*-m-150-iso8859-1"); 
		break;
    }
*/
    switch(size)
    {
	case 0: fontsize1=34; fontsize2=200;
		break;
	case 1: fontsize1=34; fontsize2=200;
		break;
	case 2: fontsize1=26; fontsize2=159;
		break;
	case 3: fontsize1=25; fontsize2=150;
		break;
	case 4: fontsize1=24; fontsize2=140;
		break;
	case 5: fontsize1=20; fontsize2=120;
		break;
	case 6: fontsize1=19; fontsize2=110;
		break;
	case 7: fontsize1=18; fontsize2=110;
		break;
	case 8: fontsize1=17; fontsize2=100;
		break;
	case 9: fontsize1=14; fontsize2=80;
		break;
	case 10: fontsize1=12; fontsize2=70;
		break;
	case 11: fontsize1=11; fontsize2=70;
		break;
	case 12: fontsize1=10; fontsize2=60;
		break;
	default: fontsize1=34; fontsize2=200;
		break;
    }

/*
    sprintf(fontstr,
	"-*-courier-bold-r-normal--%d-*-*-*-m-%d-iso8859-1",
	fontsize1,fontsize2);

    fontInfo = XLoadQueryFont(dpy,fontstr);

	fontInfo = XLoadQueryFont(dpy,  
            "-adobe-helvetica-medium-r-normal--17-120-100-100-p-88-iso8859-1");

    fontInfo = XLoadQueryFont(dpy,"Screen15"); 
*/
    fontInfo = XLoadQueryFont(dpy,
	"fixed"); 


    if (fontInfo == NULL) {
        printf ("no font found\n");
        exit (0);
    } 

    id = fontInfo->fid;
    first = fontInfo->min_char_or_byte2;
    last = fontInfo->max_char_or_byte2; 
 
    glXUseXFont(id, first, last-first+1, FONT_START+first);
}

printString(char *s)
{
    glPushAttrib (GL_LIST_BIT);
    glListBase(FONT_START);
    glCallLists(strlen(s), GL_UNSIGNED_BYTE,(unsigned char *)s);
    glPopAttrib ();
}

init_labels()
{
    int	i;

    for(i=0;i<10;i++) label[i].draw = FALSE;

}
    
do_labels()
{
    int	i;

    glColor3f(1.0,1.0,1.0);

    for(i=0;i<10;i++)  if (label[i].draw)
    {
/*	printf("x,y %f %f \n",label[i].x,label[i].y); */
/*	get_font(label[i].draw);	*/
	glColor3f(label[i].color[0],label[i].color[1],label[i].color[2]);
	if (select_mode) glPushName(10+i);
	glRasterPos3f(label[i].x,label[i].y,-3.5);
	printString(label[i].words);
	if (select_mode) glPopName(); /* 10+i); */	
    }

/*    getcolor(10);	
    glRasterPos3f(-3.5,-3.5,-3.5);
    printString("0 mya"); */

}

fix_labels()
{
  int i;

    if (projection && !label_proj) { for (i=0;i<label_num;i++) {
	label[i].x = label[i].x*1.8;
	label[i].y = label[i].y*1.8;	}
	label_proj = projection;
    }
    if (!projection && label_proj) { for (i=0;i<label_num;i++) {
	label[i].x = label[i].x/1.8;
	label[i].y = label[i].y/1.8;    }
	label_proj = projection;
    }

}

recon_label(name)
char *name;
{

    if (recon_label_num==20) {
	recon_label_num=label_num;
	label_num++;
    }

    label[recon_label_num].x = -1.8; /*-1.8*/
    label[recon_label_num].y = -1.7; /*-1.7*/
    if (!label_proj) {
	label[recon_label_num].x = label[recon_label_num].x/1.8;
	label[recon_label_num].y = label[recon_label_num].y/1.8;
    }
    sprintf(label[recon_label_num].words,"%s %.0f %s",name,PaleoTime,"my");
    label[recon_label_num].draw = 1;
    label[recon_label_num].color[0] = 1.0;
    label[recon_label_num].color[1] = 1.0;
    label[recon_label_num].color[2] = 1.0;

}


