#include <stdio.h>
#include <math.h>
#include "paleotypes.h"
#include "paleodefs.h"
#include "paleoexterns.h"
#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Separator.h>
#include <Xm/CascadeB.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

#define RADS 0.017453293

extern int lith_check;
extern lithCB();
extern lithtogCB();
int lithnum=0;
Widget lithtog[27];
Widget envtog[9];
extern XVisualInfo *vi;

extern int plate_alive[900];

int lith_back[181][360];

lith_maker(filename)
char *filename;
{
    char 	text[128];
    char 	buf[228];
    char	info[228];
    FILE 	*datafile;
    int		lage,lteid,lcode,lbeg,lend;
    float	llat,llon;
    char	ltype[6];
    char 	lcountry[21];
    int		i,j,marker,linelen;
    char	*temp;
    char	tempchar[1];
    int		tempint,templith;
    int		lithtime;

    if ((datafile = fopen(filename,"r")) == NULL) {
        sprintf(text,"Error opening %s\n",filename);
	print_warning(text);
	return(0); }

    sprintf(text,"Reading %s ...",filename);
    display_waiting(text);

    switch (lith_check) {
	case 1: lithtime=0; break;
	case 2: lithtime=1; break;
	case 3: lithtime=10; break;
	case 4: lithtime=26; break;
	case 5: lithtime=46; break;
	case 6: lithtime=59; break;
	case 7: lithtime=70; break;
	case 8: lithtime=88; break;
	case 9: lithtime=92; break;
	case 10: lithtime=116; break;
	case 11: lithtime=134; break;
	case 12: lithtime=150; break;
	case 13: lithtime=166; break;
	case 14: lithtime=196; break;
	case 15: lithtime=210; break;
	case 16: lithtime=244; break;
    }


    lithnum=0;

    while (fgets(buf,200,datafile) != NULL) {

	lithnum++;
	if (lithnum>5000) {
	    printf("greater than 5000 pts!!! \n");
	    break;
	}
	
	linelen=strlen(buf);
	sscanf(buf,"%f%f%d%d%s%d%d%d%n",&llat,&llon,&lage,
		&lteid,ltype,&lcode,&lbeg,&lend,&marker);
	ltype[6]='\0';

/*	printf("%.2f %.2f %d %d %s %d %d %d %d \n",
		llat,llon,lage,
		lteid,ltype,lcode,lbeg,lend,marker); */

	lith_list[lithnum-1].number=lithnum;
	lith_list[lithnum-1].lat=llat;
	lith_list[lithnum-1].lon=llon;
	lith_list[lithnum-1].age=lage;
	lith_list[lithnum-1].plate=lteid;
	strncpy(lith_list[lithnum-1].type,ltype,5);
/*printf("type %s \n",lith_list[lithnum-1].type); */
	lith_list[lithnum-1].typecode=lcode;
	lith_list[lithnum-1].envbegin=lbeg;
	lith_list[lithnum-1].envend=lend;

	if (lage != lithtime || !plate_alive[lith_list[lithnum-1].plate-100] ) {
	    lithnum--; 
	    continue;
	}
/*printf("lithnum: %d age: %d \n",lithnum,lage);*/

	tempchar[0]='\t';
        marker++;

	for (i=marker;i<=linelen;i++) 
		info[i-marker]=buf[i];

	j=1;
	temp = strtok(info,tempchar);
	strncpy(lith_list[lithnum-1].country,temp,20);

	temp = strtok(NULL,tempchar);
	temp = strtok(NULL,tempchar);
	temp = strtok(NULL,tempchar);
	strncpy(lith_list[lithnum-1].stratunit,temp,20);
	temp = strtok(NULL,tempchar);
	lith_list[lithnum-1].reliable=*temp;
	temp = strtok(NULL,tempchar);
	lith_list[lithnum-1].reconage = atoi(temp);
	temp = strtok(NULL,tempchar);
	lith_list[lithnum-1].reconlat = atof(temp);
	temp = strtok(NULL,tempchar);
	lith_list[lithnum-1].reconlon = atof(temp);
	temp = strtok(NULL,tempchar);
	lith_list[lithnum-1].unique = atoi(temp);
	temp = strtok(NULL,tempchar);
	lith_list[lithnum-1].refnum = atoi(temp);
	temp = strtok(NULL,tempchar);
	strncpy(lith_list[lithnum-1].ref1,temp,20);
	temp = strtok(NULL,tempchar);
	strncpy(lith_list[lithnum-1].ref2,temp,20);
	temp = strtok(NULL,tempchar);
	temp = strtok(NULL,tempchar);
	strncpy(lith_list[lithnum-1].date,temp,8);
	temp = strtok(NULL,tempchar);
	temp = strtok(NULL,tempchar);
	temp = strtok(NULL,tempchar);
	strncpy(lith_list[lithnum-1].mgid,temp,15);
		

/*printf("%d %s \n",lith_list[lithnum-1].unique,lith_list[lithnum-1].country);*/

    }

    make_lith();

    make_background();

    remove_waiting();
    sprintf(text,"\n%d Vectors processed\n",lithnum);
    print_message(text);
    fclose(datafile);
}

make_background()
{
    int lat,lon,setnum,ltype,lithicon,templith,templat;
    float llat,llon,xfactor,yfactor;
    int count[181][360],i,lithval;
    float anlat,anlon,rotlat[5],rotlon[5],rotang[5],px,py,pz;
    float a1,a2,a3,a4,a5,rx,ry,rz,gx,gy,gz,vct,sina,cosa,sqrz;

    for(lat=0;lat<180;lat++)
	for(lon=0;lon<359;lon++) {
	    count[lat][lon]=0;
	    lith_back[lat][lon]=0;
	}

    for(setnum=0;setnum<lithnum;setnum++) {
	llat=lith_list[setnum].lat;
	llon=lith_list[setnum].lon; if (llon>=180) llon=360-llon;
	ltype=lith_list[setnum].typecode;

/*** Rotate coordinates ***/

	for(i=0;i<5;i++) {
	    rotlat[i] = plate_list[lith_list[setnum].plate]->latitude[i];
	    rotlon[i] = plate_list[lith_list[setnum].plate]->longitude[i];
	    rotang[i] = plate_list[lith_list[setnum].plate]->angle[i];
	}

    for(i=0;i<5;i++) { if (rotang[i] != 0.0 && rotlat[i] != 0.0) {
	a1 = 90.0*RADS-llat*RADS;
	a2 = llon*RADS;
	px = sin(a1)*cos(a2);
	py = sin(a1)*sin(a2);
	pz = cos(a1);
	a3 = 90.0*RADS-rotlat[i]*RADS;
	a4 = rotang[i]*RADS;
	a5 = rotlon[i]*RADS;
	gx = sin(a3)*cos(a5);
	gy = sin(a3)*sin(a5);
	gz = cos(a3);
	vct = (px*gx)+(py*gy)+(pz*gz);
	rx = cos(a4)*px+(1.0-cos(a4))*vct*gx+sin(a4)*(gy*pz-gz*py);
	ry = cos(a4)*py+(1.0-cos(a4))*vct*gy+sin(a4)*(gz*px-gx*pz);
	rz = cos(a4)*pz+(1.0-cos(a4))*vct*gz+sin(a4)*(gx*py-gy*px);
	sqrz= sqrt(1.0-rz*rz);
	if (rz==1.0 || sqrz==0.0) { cosa=0.0; printf("rz=1 \n"); }
	else {  sina=atan(rz/sqrz);
		cosa=((3.141592654/2.0-sina)*57.29578); }
	anlat = 90.0-cosa;
	anlon = 90.0-(atan2(rx,ry)*57.29578);
	if (anlon>180.0) anlon=anlon-360.0;
	llat = anlat;
	llon = anlon;
    }}

	if (llat<0) llat=llat-1;  lat = (int)llat;
	if (llon<0) llon=llon-1;  lon = (int)llon;
	
	lith_back[lat+90][lon+180]+=ltype;
	count[lat+90][lon+180]+=1;
	}

    for(lat=-90;lat<90;lat++)
	for(lon=-180;lon<180;lon++) {
	    if (count[lat+90][lon+180]) {
		lithval=(int)(lith_back[lat+90][lon+180]/
			count[lat+90][lon+180]);
		switch (lithval) {
		    case 1: lith_back[lat+90][lon+180]=3; break;
		    case 2: lith_back[lat+90][lon+180]=27; break;
		    case 3: lith_back[lat+90][lon+180]=28; break;
		    case 4: lith_back[lat+90][lon+180]=13; break;
		    case 5: lith_back[lat+90][lon+180]=29; break;
		    case 6: lith_back[lat+90][lon+180]=30; break;
		    case 7: lith_back[lat+90][lon+180]=12; break;
		    default: lith_back[lat+90][lon+180]=0; break;
		}
	    }
	}
    

/*
    for(lat=-90;lat<90;lat++)
	for(lon=-180;lon<180;lon++) {
	  count=0;
	  for(setnum=0;setnum<lithnum;setnum++) {
	    llat=lith_list[setnum].lat; 
	    llon=lith_list[setnum].lon; if (lon>=180) lon=360-lon;
	    ltype=lith_list[setnum].typecode;
	    if (llat>lat && llat<=lat+1 && llon>lon && llon<=lon+1) {
		lith_back[lat+90][lon+180]+=ltype;
		count++;
		continue;
	    }
	  }
	  if (count) {
	    lith_back[lat+90][lon+180]=(int)(lith_back[lat+90][lon+180]/count);
	    switch (lith_back[lat+90][lon+180]) {
		case 1: lith_back[lat+90][lon+180]=3; break;
		case 2: lith_back[lat+90][lon+180]=27; break;
		case 3: lith_back[lat+90][lon+180]=28; break;
		case 4: lith_back[lat+90][lon+180]=13; break;
		case 5: lith_back[lat+90][lon+180]=29; break;
		case 6: lith_back[lat+90][lon+180]=30; break;
		case 7: lith_back[lat+90][lon+180]=12; break;
		default: lith_back[lat+90][lon+180]=0; break;
	    }
	  }
	}
*/
    glNewList(LITH_ICON-1,GL_COMPILE);
	glColor3f(.5,.5,.5);
	for(lat=-90;lat<90;lat++)
	  for(lon=-180;lon<180;lon++) {
		if (lat<0) templat=-lat;
		else templat=lat;
		yfactor = 0.017455064/2.0;
		xfactor = 0.017455064/1.7*(90.0-templat)/90.0;
	    lithicon = lith_back[lat+90][lon+180];
	    if (lithicon) {
		glRotatef(lon+0.5,0.0,1.0,0.0);
		glRotatef(0.0-(lat+0.5),1.0,0.0,0.0);
		glScalef(xfactor,yfactor,1.0);
		  glCallList(LITH_ICON+lithicon);
		glScalef(1.0/xfactor,1.0/yfactor,1.0);
		glRotatef(lat+0.5,1.0,0.0,0.0);
		glRotatef(0.0-(lon+0.5),0.0,1.0,0.0);
	    }
	  }
    glEndList();

}



make_lith()
{
  int i,j,k,color,lithicon;
  float factor,angle;
  char factype;
  char facstr[5];
  int len;

    for(i=0;i<lithnum;i++) {
	factor = 0.017455064/3.0;
	if (lith_list[i].envend<lith_list[i].envbegin) 
	    color=lith_list[i].envend;
	else color=lith_list[i].envbegin;
	
	strcpy(facstr,lith_list[i].type);
	len = strlen(facstr);
	for (k=0;k<len;k++) {
	    factype=64;
	    if ((int)facstr[k]>64 && (int)facstr[k]<91) 
	    if (facstr[k]!='M' && facstr[k]!='C' &&
		facstr[k]!='S' && facstr[k]!='L') {
		factype=facstr[k];
		break; 
	    }
	}

	lithicon=(int)factype-64;

   	glNewList(i+LITH_START,GL_COMPILE);
    
	    switch (color) {
		case 0: glColor3f(.5,.1,.5); break;
		case 1: glColor3f(.5,.3,.75); break;
		case 2: glColor3f(.5,.5,1.0); break;
		case 3: glColor3f(0.0,1.0,1.0); break;
		case 4: glColor3f(0.0,1.0,0.5); break;
		case 5: glColor3f(0.0,1.0,0.0); break;
		case 6: glColor3f(1.0,1.0,0.0); break;
		case 7: glColor3f(1.0,0.6,0.1); break;
		case 8: glColor3f(1.0,0.0,0.0); break;
	    }

	    glRotatef(lith_list[i].lon,0.0,1.0,0.0);
	    glRotatef(0.0-lith_list[i].lat,1.0,0.0,0.0);
	    glScalef(factor,factor,1.0);
		glCallList(LITH_ICON+lithicon);
	    glScalef(1.0/factor,1.0/factor,1.0);
	    glRotatef(lith_list[i].lat,1.0,0.0,0.0);
	    glRotatef(0.0-lith_list[i].lon,0.0,1.0,0.0);

    	glEndList();
    }
}

		
build_lith(wid)
Widget wid;
{
   Widget col, rc, radio, label, sep, tog;
   Arg args[2];

/**********************************************************************/
/*  Create the LITH options box.                                      */
/**********************************************************************/
   col = XmCreateRowColumn(wid, "column1", NULL, 0);
   XtManageChild(col);

   label  = XmCreateLabelGadget(col, "Select an Age", NULL, 0);
   XtManageChild(label);
   sep = XmCreateSeparator(col,"sep",NULL,0);
   XtManageChild(sep);
   rc = XmCreateRowColumn(col, "radio", NULL, 0);
   XtManageChild(rc);
   tog = XmCreatePushButtonGadget(col, "0) Present", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,lithCB,1);	
   tog = XmCreatePushButtonGadget(col, "1) Pleistocene", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,lithCB,2);	
   tog = XmCreatePushButtonGadget(col, "10) Vindobonian", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,lithCB,3);	
   tog = XmCreatePushButtonGadget(col, "26) Chattian", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,lithCB,4);	
   tog = XmCreatePushButtonGadget(col, "46) Lutetian", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,lithCB,5);	
   tog = XmCreatePushButtonGadget(col, "59) Thanetian", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,lithCB,6);	
   tog = XmCreatePushButtonGadget(col, "70) Maastrichtian", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,lithCB,7);	
   tog = XmCreatePushButtonGadget(col, "88) Coniacian", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,lithCB,8);	
   tog = XmCreatePushButtonGadget(col, "92) Cenomanian", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,lithCB,9);	
   tog = XmCreatePushButtonGadget(col, "116) Aptian", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,lithCB,10);	
   tog = XmCreatePushButtonGadget(col, "134) Valanginian", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,lithCB,11);	
   tog = XmCreatePushButtonGadget(col, "150) Tithonian", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,lithCB,12);	
   tog = XmCreatePushButtonGadget(col, "166) Callovian", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,lithCB,13);	
   tog = XmCreatePushButtonGadget(col, "196) Pliensbachian", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,lithCB,14);	
   tog = XmCreatePushButtonGadget(col, "210) Norian", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,lithCB,15);	
   tog = XmCreatePushButtonGadget(col, "244) Induan", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,lithCB,16);	
   label  = XmCreateLabelGadget(col, "Other Options", NULL, 0);
   XtManageChild(label);
   sep = XmCreateSeparator(col,"sep",NULL,0);
   XtManageChild(sep);
   tog = XmCreatePushButtonGadget(col, "Update Background", NULL, 0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,lithCB,17);	


}

build_lithpick(wid)
Widget wid;
{
   Widget col, rc, radio, label, sep, tog, cascade;
   Arg args[2];

/**********************************************************************/
/*  Create the LITH options box.                                      */
/**********************************************************************/
   col = XmCreateRowColumn(wid, "column1", NULL, 0);
   XtManageChild(col);

   label  = XmCreateLabelGadget(col, "Select Lith Types", NULL, 0);
   XtManageChild(label);
   sep = XmCreateSeparator(col,"sep",NULL,0);
   XtManageChild(sep);
   rc = XmCreateRowColumn(col, "radio", NULL, 0);
   XtManageChild(rc);
   XtSetArg(args[0], XmNvisual, vi->visual);
   rc = XmCreatePulldownMenu(col, "menupane", args, 1);
   XtSetArg(args[0],XmNset,TRUE);
   lithtog[3]= XmCreateToggleButtonGadget(rc, "C) Conglomerate", args,1);
   XtManageChild(lithtog[3]);
   XtAddCallback(lithtog[3],XmNvalueChangedCallback,lithtogCB,3);	
   lithtog[12]= XmCreateToggleButtonGadget(rc, "L) Carbonate", args,1);
   XtManageChild(lithtog[12]);
   XtAddCallback(lithtog[12],XmNvalueChangedCallback,lithtogCB,12);	
   lithtog[13]= XmCreateToggleButtonGadget(rc, "M) Mudstone", args,1);
   XtManageChild(lithtog[13]);
   XtAddCallback(lithtog[13],XmNvalueChangedCallback,lithtogCB,13);	
   lithtog[19]= XmCreateToggleButtonGadget(rc, "S) Sandstone", args,1);
   XtManageChild(lithtog[19]);
   XtAddCallback(lithtog[19],XmNvalueChangedCallback,lithtogCB,19);	
   XtSetArg(args[0], XmNsubMenuId, rc);
   // cascade = XmCreateCascadeButton(col, "Clastic-Carbonate Sediments", args, 1);
   // XtManageChild(cascade);

   XtSetArg(args[0], XmNvisual, vi->visual);
   rc = XmCreatePulldownMenu(col, "menupane", args, 1);
   XtSetArg(args[0],XmNset,TRUE);
   lithtog[4]= XmCreateToggleButtonGadget(rc, "D) Dolomite", args,1);
   XtManageChild(lithtog[4]);
   XtAddCallback(lithtog[4],XmNvalueChangedCallback,lithtogCB,4);	
   lithtog[5]= XmCreateToggleButtonGadget(rc, "E) Evaporites", args,1);
   XtManageChild(lithtog[5]);
   XtAddCallback(lithtog[5],XmNvalueChangedCallback,lithtogCB,5);	
   lithtog[7]= XmCreateToggleButtonGadget(rc, "G) Gypsum", args,1);
   XtManageChild(lithtog[7]);
   XtAddCallback(lithtog[7],XmNvalueChangedCallback,lithtogCB,7);	
   lithtog[8]= XmCreateToggleButtonGadget(rc, "H) Halite", args,1);
   XtManageChild(lithtog[8]);
   XtAddCallback(lithtog[8],XmNvalueChangedCallback,lithtogCB,8);	
   lithtog[15]= XmCreateToggleButtonGadget(rc, "O) ?", args,1);
   XtManageChild(lithtog[15]);
   XtAddCallback(lithtog[15],XmNvalueChangedCallback,lithtogCB,15);	
   lithtog[16]= XmCreateToggleButtonGadget(rc, "P) Peat", args,1);
   XtManageChild(lithtog[16]);
   XtAddCallback(lithtog[16],XmNvalueChangedCallback,lithtogCB,16);	
   lithtog[18]= XmCreateToggleButtonGadget(rc, "R) Reefs", args,1);
   XtManageChild(lithtog[18]);
   XtAddCallback(lithtog[18],XmNvalueChangedCallback,lithtogCB,18);	
   lithtog[20]= XmCreateToggleButtonGadget(rc, "T) Tillite", args,1);
   XtManageChild(lithtog[20]);
   XtAddCallback(lithtog[20],XmNvalueChangedCallback,lithtogCB,20);	
   XtSetArg(args[0], XmNsubMenuId, rc);
   // cascade = XmCreateCascadeButton(col, "Climatic Sediments", args, 1);
   // XtManageChild(cascade);

   XtSetArg(args[0], XmNvisual, vi->visual);
   rc = XmCreatePulldownMenu(col, "menupane", args, 1);
   XtSetArg(args[0],XmNset,TRUE);
   lithtog[17]= XmCreateToggleButtonGadget(rc, "Q) Bedded Chert", args,1);
   XtManageChild(lithtog[17]);
   XtAddCallback(lithtog[17],XmNvalueChangedCallback,lithtogCB,17);	
   lithtog[22]= XmCreateToggleButtonGadget(rc, "V) Phosphorite", args,1);
   XtManageChild(lithtog[22]);
   XtAddCallback(lithtog[22],XmNvalueChangedCallback,lithtogCB,22);	
   lithtog[23]= XmCreateToggleButtonGadget(rc, "W) Ferromanganese", args,1);
   XtManageChild(lithtog[23]);
   XtAddCallback(lithtog[23],XmNvalueChangedCallback,lithtogCB,23);	
   lithtog[24]= XmCreateToggleButtonGadget(rc, "X) Limonite", args,1);
   XtManageChild(lithtog[24]);
   XtAddCallback(lithtog[24],XmNvalueChangedCallback,lithtogCB,24);	
   lithtog[25]= XmCreateToggleButtonGadget(rc, "Y) Chamosite", args,1);
   XtManageChild(lithtog[25]);
   XtAddCallback(lithtog[25],XmNvalueChangedCallback,lithtogCB,25);	
   lithtog[26]= XmCreateToggleButtonGadget(rc, "Z) Glauconite", args,1);
   XtManageChild(lithtog[26]);
   XtAddCallback(lithtog[26],XmNvalueChangedCallback,lithtogCB,26);
   XtSetArg(args[0], XmNsubMenuId, rc);
   // cascade = XmCreateCascadeButton(col, "Oceanographic Sediments", args, 1);
   // XtManageChild(cascade);

   XtSetArg(args[0], XmNvisual, vi->visual);
   rc = XmCreatePulldownMenu(col, "menupane", args, 1);
   XtSetArg(args[0],XmNset,TRUE);
   lithtog[14]= XmCreateToggleButtonGadget(rc, "N) Nonmarine", args,1);
   XtManageChild(lithtog[14]);
   XtAddCallback(lithtog[14],XmNvalueChangedCallback,lithtogCB,14);	
   XtSetArg(args[0], XmNsubMenuId, rc);
   // cascade = XmCreateCascadeButton(col, "Soils", args, 1);
   // XtManageChild(cascade);

   XtSetArg(args[0], XmNvisual, vi->visual);
   rc = XmCreatePulldownMenu(col, "menupane", args, 1);
   XtSetArg(args[0],XmNset,TRUE);
   lithtog[1]= XmCreateToggleButtonGadget(rc, "A) Andesite", args,1);
   XtManageChild(lithtog[1]);
   XtAddCallback(lithtog[1],XmNvalueChangedCallback,lithtogCB,1);	
   lithtog[2]= XmCreateToggleButtonGadget(rc, "B) Basalt", args,1);
   XtManageChild(lithtog[2]);
   XtAddCallback(lithtog[2],XmNvalueChangedCallback,lithtogCB,2);	
   lithtog[11]= XmCreateToggleButtonGadget(rc, "K) Rhyolite", args,1);
   XtManageChild(lithtog[11]);
   XtAddCallback(lithtog[11],XmNvalueChangedCallback,lithtogCB,11);	
   XtSetArg(args[0], XmNsubMenuId, rc);
   // cascade = XmCreateCascadeButton(col, "Volcanic Sequence", args, 1);
   // XtManageChild(cascade);

   XtSetArg(args[0], XmNvisual, vi->visual);
   rc = XmCreatePulldownMenu(col, "menupane", args, 1);
   XtSetArg(args[0],XmNset,TRUE);
   lithtog[6]= XmCreateToggleButtonGadget(rc, "F) Foidite", args,1);
   XtManageChild(lithtog[6]);
   XtAddCallback(lithtog[6],XmNvalueChangedCallback,lithtogCB,6);	
   lithtog[9]= XmCreateToggleButtonGadget(rc, "I) Granodiorite", args,1);
   XtManageChild(lithtog[9]);
   XtAddCallback(lithtog[9],XmNvalueChangedCallback,lithtogCB,9);	
   lithtog[10]= XmCreateToggleButtonGadget(rc, "J) Granite", args,1);
   XtManageChild(lithtog[10]);
   XtAddCallback(lithtog[10],XmNvalueChangedCallback,lithtogCB,10);	
   XtSetArg(args[0], XmNsubMenuId, rc);
   // cascade = XmCreateCascadeButton(col, "Intrusive Sequence", args, 1);
   // XtManageChild(cascade);

   XtSetArg(args[0], XmNvisual, vi->visual);
   rc = XmCreatePulldownMenu(col, "menupane", args, 1);
   XtSetArg(args[0],XmNset,TRUE);
   lithtog[21]= XmCreateToggleButtonGadget(rc, "U) Uplift", args,1);
   XtManageChild(lithtog[21]);
   XtAddCallback(lithtog[21],XmNvalueChangedCallback,lithtogCB,21);	
   XtSetArg(args[0], XmNsubMenuId, rc);
   // cascade = XmCreateCascadeButton(col, "Cooling Ages", args, 1);
   // XtManageChild(cascade);

   XtSetArg(args[0], XmNvisual, vi->visual);
   rc = XmCreatePulldownMenu(col, "menupane", args, 1);
   XtSetArg(args[0],XmNset,TRUE);
   tog= XmCreatePushButtonGadget(rc, "No Selection", args,1);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,lithtogCB,-100);	
   XtSetArg(args[0], XmNsubMenuId, rc);
   // cascade = XmCreateCascadeButton(col, "Never Mind", args, 1);
   // XtManageChild(cascade);

   XtSetArg(args[0],XmNorientation,XmHORIZONTAL);
   rc = XmCreateRowColumn(col, "radio", args, 1);
   XtManageChild(rc);
   tog = XmCreatePushButtonGadget(rc, "       All       ", args, 1);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,lithtogCB,-1);	
   tog = XmCreatePushButtonGadget(rc, "    Invert    ", args, 1);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,lithtogCB,-2);	

   label  = XmCreateLabelGadget(col, "Select Environment Types", NULL, 0);
   XtManageChild(label);
   sep = XmCreateSeparator(col,"sep",NULL,0);
   XtManageChild(sep);
   rc = XmCreateRowColumn(col, "radio", args, 0);
   XtManageChild(rc);
   XtSetArg(args[0],XmNset,TRUE);
   XtSetArg(args[1],XmNselectColor,15);
   envtog[0]= XmCreateToggleButtonGadget(rc, "0)             - -6000", args,2);
   XtManageChild(envtog[0]);
   XtAddCallback(envtog[0],XmNvalueChangedCallback,lithtogCB,30);	
   envtog[1]= XmCreateToggleButtonGadget(rc, "1) -6000 - -4000", args,1);
   XtManageChild(envtog[1]);
   XtAddCallback(envtog[1],XmNvalueChangedCallback,lithtogCB,31);	
   envtog[2]= XmCreateToggleButtonGadget(rc, "2) -4000 - -200", args,1);
   XtManageChild(envtog[2]);
   XtAddCallback(envtog[2],XmNvalueChangedCallback,lithtogCB,32);	
   envtog[3]= XmCreateToggleButtonGadget(rc, "3)   -200 - -50", args,1);
   XtManageChild(envtog[3]);
   XtAddCallback(envtog[3],XmNvalueChangedCallback,lithtogCB,33);	
   envtog[4]= XmCreateToggleButtonGadget(rc, "4)     -50 -   0", args,1);
   XtManageChild(envtog[4]);
   XtAddCallback(envtog[4],XmNvalueChangedCallback,lithtogCB,34);	
   envtog[5]= XmCreateToggleButtonGadget(rc, "5)         0 - +200", args,1);
   XtManageChild(envtog[5]);
   XtAddCallback(envtog[5],XmNvalueChangedCallback,lithtogCB,35);	
   envtog[6]= XmCreateToggleButtonGadget(rc, "6)   +200 - +1000", args,1);
   XtManageChild(envtog[6]);
   XtAddCallback(envtog[6],XmNvalueChangedCallback,lithtogCB,36);	
   envtog[7]= XmCreateToggleButtonGadget(rc, "7) +1000 - +2000", args,1);
   XtManageChild(envtog[7]);
   XtAddCallback(envtog[7],XmNvalueChangedCallback,lithtogCB,37);	
   envtog[8]= XmCreateToggleButtonGadget(rc, "8) +2000 - ", args,1);
   XtManageChild(envtog[8]);
   XtAddCallback(envtog[8],XmNvalueChangedCallback,lithtogCB,38);	

   XtSetArg(args[0],XmNorientation,XmHORIZONTAL);
   rc = XmCreateRowColumn(col, "radio", args, 1);
   XtManageChild(rc);
   tog = XmCreatePushButtonGadget(rc, "       All       ", args, 1);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,lithtogCB,-3);	
   tog = XmCreatePushButtonGadget(rc, "    Invert    ", args, 1);
   XtManageChild(tog);
   XtAddCallback(tog,XmNactivateCallback,lithtogCB,-4);	

   label  = XmCreateLabelGadget(col, "Hide Lith Info", NULL, 0);
   XtManageChild(label);
   sep = XmCreateSeparator(col,"sep",NULL,0);
   XtManageChild(sep);
   XtSetArg(args[0],XmNorientation,XmHORIZONTAL);
   rc = XmCreateRowColumn(col, "radio", args, 1);
   XtManageChild(rc);
   tog= XmCreateToggleButtonGadget(rc, "Background", args,0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNvalueChangedCallback,lithtogCB,40);	
   tog= XmCreateToggleButtonGadget(rc, "Data", args,0);
   XtManageChild(tog);
   XtAddCallback(tog,XmNvalueChangedCallback,lithtogCB,41);	
}

make_lith_icon()
{
    printf("make_lith_icon\n");

    glNewList(LITH_ICON,GL_COMPILE);
	glBegin(GL_LINE_STRIP);
	    glVertex3f(1.0/2, 1.0/2, 1.0);
	    glVertex3f(-1.0/2, -1.0/2, 1.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-1.0/2, 1.0/2, 1.0);
	    glVertex3f(1.0/2, -1.0/2, 1.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+1,GL_COMPILE);
	glBegin(GL_POLYGON);
	    glVertex3f(0.0, 1.0, 1.0);
	    glVertex3f(1.0, -1.0, 1.0);
	    glVertex3f(-1.0, -1.0, 1.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+2,GL_COMPILE);
	glBegin(GL_POLYGON);
	    glVertex3f(0.0, -1.0, 1.0);
	    glVertex3f(1.0, 1.0, 1.0);
	    glVertex3f(-1.0, 1.0, 1.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+3,GL_COMPILE);
	glBegin(GL_LINE_LOOP);
	    glVertex3f(-1.0, 1.0, 1.0);
	    glVertex3f(-1.0, 0.8, 1.0);
	    glVertex3f(-0.8, 0.8, 1.0);
	    glVertex3f(-0.8, 1.0, 1.0);
	glEnd();
	glBegin(GL_LINE_LOOP);
	    glVertex3f(1.0, 1.0, 1.0);
	    glVertex3f(1.0, 0.8, 1.0);
	    glVertex3f(0.8, 0.8, 1.0);
	    glVertex3f(0.8, 1.0, 1.0);
	glEnd();
	glBegin(GL_LINE_LOOP);
	    glVertex3f(-1.0, -1.0, 1.0);
	    glVertex3f(-1.0, -0.8, 1.0);
	    glVertex3f(-0.8, -0.8, 1.0);
	    glVertex3f(-0.8, -1.0, 1.0);
	glEnd();
	glBegin(GL_LINE_LOOP);
	    glVertex3f(1.0, -1.0, 1.0);
	    glVertex3f(1.0, -0.8, 1.0);
	    glVertex3f(0.8, -0.8, 1.0);
	    glVertex3f(0.8, -1.0, 1.0);
	glEnd();
	glBegin(GL_LINE_LOOP);
	    glVertex3f(0.1, 0.1, 1.0);
	    glVertex3f(0.1, -0.1, 1.0);
	    glVertex3f(-0.1, -0.1, 1.0);
	    glVertex3f(-0.1, 0.1, 1.0);
	glEnd();
	glBegin(GL_POINTS);
	    glVertex3f(0.0, 1.0, 1.0);
	    glVertex3f(-1.0, 0.0, 1.0);
	    glVertex3f(1.0, 0.0, 1.0);
	    glVertex3f(0.0, -1.0, 1.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+4,GL_COMPILE);
	glBegin(GL_LINE_LOOP);
	    glVertex3f(-1.0, -0.5, 1.0);
	    glVertex3f(-0.5, 0.5, 1.0);
	    glVertex3f(1.0, 0.5, 1.0);
	    glVertex3f(0.5, -0.5, 1.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+5,GL_COMPILE);
	glBegin(GL_LINE_LOOP);
	    glVertex3f(0.0, 1.0, 1.0);
	    glVertex3f(1.0, 0.5, 1.0);
	    glVertex3f(1.0, -0.5, 1.0);
	    glVertex3f(0.0, -1.0, 1.0);
	    glVertex3f(-1.0, -0.5, 1.0);
	    glVertex3f(-1.0, 0.5, 1.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+6,GL_COMPILE);
	glBegin(GL_LINE_LOOP);
	    glVertex3f(0.0, -1.0, 1.0);
	    glVertex3f(1.0, 1.0, 1.0);
	    glVertex3f(-1.0, 1.0, 1.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+7,GL_COMPILE);
	glBegin(GL_LINE_LOOP);
	    glVertex3f(0.0, 1.0, 1.0);
	    glVertex3f(1.0, 0.5, 1.0);
	    glVertex3f(1.0, -0.5, 1.0);
	    glVertex3f(0.0, -1.0, 1.0);
	    glVertex3f(-1.0, -0.5, 1.0);
	    glVertex3f(-1.0, 0.5, 1.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+8,GL_COMPILE);
	glBegin(GL_LINE_LOOP);
	    glVertex3f(0.0, 1.0, 1.0);
	    glVertex3f(1.0, 0.5, 1.0);
	    glVertex3f(1.0, -0.5, 1.0);
	    glVertex3f(0.0, -1.0, 1.0);
	    glVertex3f(-1.0, -0.5, 1.0);
	    glVertex3f(-1.0, 0.5, 1.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+9,GL_COMPILE);
	glBegin(GL_LINE_LOOP);
	    glVertex3f(0.0, 1.0, 1.0);
	    glVertex3f(1.0, -1.0, 1.0);
	    glVertex3f(-1.0, -1.0, 1.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+10,GL_COMPILE);
	glBegin(GL_LINE_LOOP);
	    glVertex3f(-1.0, -0.5, 1.0);
	    glVertex3f(-0.85, 0.0, 1.0);
	    glVertex3f(-0.5, 0.35, 1.0);
	    glVertex3f(0.0, 0.5, 1.0);
	    glVertex3f(0.5, 0.35, 1.0);
	    glVertex3f(0.85, 0.0, 1.0);
	    glVertex3f(1.0, -0.5, 1.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+11,GL_COMPILE);
	glBegin(GL_POLYGON);
	    glVertex3f(-1.0, -0.5, 1.0);
	    glVertex3f(-0.85, 0.0, 1.0);
	    glVertex3f(-0.5, 0.35, 1.0);
	    glVertex3f(0.0, 0.5, 1.0);
	    glVertex3f(0.5, 0.35, 1.0);
	    glVertex3f(0.85, 0.0, 1.0);
	    glVertex3f(1.0, -0.5, 1.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+12,GL_COMPILE);
	glBegin(GL_LINE_LOOP);
	    glVertex3f(-1.0, 1.0, 1.0);
	    glVertex3f(1.0, 1.0, 1.0);
	    glVertex3f(1.0, 0.33, 1.0);
	    glVertex3f(-1.0, 0.33, 1.0);
	glEnd();
	glBegin(GL_LINE_LOOP);
	    glVertex3f(-1.0, -1.0, 1.0);
	    glVertex3f(1.0, -1.0, 1.0);
	    glVertex3f(1.0, -0.33, 1.0);
	    glVertex3f(-1.0, -0.33, 1.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(0.0, 1.0, 1.0);
	    glVertex3f(0.0, 0.33, 1.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(0.0, -1.0, 1.0);
	    glVertex3f(0.0, -0.33, 1.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(0.5, 0.33, 1.0);
	    glVertex3f(0.5, -0.33, 1.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-0.5, 0.33, 1.0);
	    glVertex3f(-0.5, -0.33, 1.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+13,GL_COMPILE);
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-1.0, 1.0, 1.0);
	    glVertex3f(-0.7, 1.0, 1.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-0.25, 1.0, 1.0);
	    glVertex3f(0.25, 1.0, 1.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(0.7, 1.0, 1.0);
	    glVertex3f(1.0, 1.0, 1.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-0.8, 0.5, 1.0);
	    glVertex3f(-0.2, 0.5, 1.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(0.2, 0.5, 1.0);
	    glVertex3f(0.8, 0.5, 1.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-1.0, 0.0, 1.0);
	    glVertex3f(-0.7, 0.0, 1.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-0.25, 0.0, 1.0);
	    glVertex3f(0.25, 0.0, 1.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(0.7, 0.0, 1.0);
	    glVertex3f(1.0, 0.0, 1.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-0.8, -0.5, 1.0);
	    glVertex3f(-0.2, -0.5, 1.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(0.2, -0.5, 1.0);
	    glVertex3f(0.8, -0.5, 1.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-1.0, -1.0, 1.0);
	    glVertex3f(-0.7, -1.0, 1.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-0.25, -1.0, 1.0);
	    glVertex3f(0.25, -1.0, 1.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(0.7, -1.0, 1.0);
	    glVertex3f(1.0, -1.0, 1.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+14,GL_COMPILE);
	glBegin(GL_LINE_STRIP);
	    glVertex3f(1.0, 1.0, 1.0);
	    glVertex3f(-1.0, -1.0, 1.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-1.0, 1.0, 1.0);
	    glVertex3f(1.0, -1.0, 1.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+15,GL_COMPILE);
	glBegin(GL_LINE_STRIP);
	    glVertex3f(1.0, 1.0, 1.0);
	    glVertex3f(-1.0, -1.0, 1.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-1.0, 1.0, 1.0);
	    glVertex3f(1.0, -1.0, 1.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+16,GL_COMPILE);
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-1.0, -0.5, 1.0);
	    glVertex3f(0.0, -0.5, 1.0);
	    glVertex3f(-0.85, 0.0, 1.0);
	    glVertex3f(0.0, -0.5, 1.0);
	    glVertex3f(-0.5, 0.35, 1.0);
	    glVertex3f(0.0, -0.5, 1.0);
	    glVertex3f(0.0, 0.5, 1.0);
	    glVertex3f(0.0, -0.5, 1.0);
	    glVertex3f(0.5, 0.35, 1.0);
	    glVertex3f(0.0, -0.5, 1.0);
	    glVertex3f(0.85, 0.0, 1.0);
	    glVertex3f(0.0, -0.5, 1.0);
	    glVertex3f(1.0, -0.5, 1.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+17,GL_COMPILE);
	glBegin(GL_POLYGON);
	    glVertex3f(-1.0, 0.0, 1.0);
	    glVertex3f(-0.93, 0.15, 1.0);
	    glVertex3f(-0.75, 0.25, 1.0);
	    glVertex3f(0.75, 0.25, 1.0);
	    glVertex3f(0.93, 0.15, 1.0);
	    glVertex3f(1.0, 0.0, 1.0);
	    glVertex3f(0.93, -0.15, 1.0);
	    glVertex3f(0.75, -0.25, 1.0);
	    glVertex3f(-0.75, -0.25, 1.0);
	    glVertex3f(-0.93, -0.15, 1.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+18,GL_COMPILE);
	glBegin(GL_LINE_LOOP);
	    glVertex3f(-1.0, 0.0, 1.0);
	    glVertex3f(-0.5, 0.0, 1.0);
	    glVertex3f(-0.25, 0.35, 1.0);
	    glVertex3f(-0.5, 0.85, 1.0);
	    glVertex3f(-0.25, 0.43, 1.0);
	    glVertex3f(0.25, 0.43, 1.0);
	    glVertex3f(0.5, 0.85, 1.0);
	    glVertex3f(0.25, 0.43, 1.0);
	    glVertex3f(0.5, 0.0, 1.0);
	    glVertex3f(1.0, 0.0, 1.0);
	    glVertex3f(0.5, 0.0, 1.0);
	    glVertex3f(0.25, -0.43, 1.0);
	    glVertex3f(0.5, -0.85, 1.0);
	    glVertex3f(0.25, -0.43, 1.0);
	    glVertex3f(-0.25, -0.43, 1.0);
	    glVertex3f(-0.5, -0.85, 1.0);
	    glVertex3f(-0.25, -0.43, 1.0);
	    glVertex3f(-0.5, 0.0, 1.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+19,GL_COMPILE);
	glBegin(GL_POINTS);
	    glVertex3f(-1.0, 1.0, 1.0);
	    glVertex3f(-0.5, 1.0, 1.0);
	    glVertex3f(0.0, 1.0, 1.0);
	    glVertex3f(0.5, 1.0, 1.0);
	    glVertex3f(1.0, 1.0, 1.0);
	    glVertex3f(-1.0, 0.5, 1.0);
	    glVertex3f(-0.5, 0.5, 1.0);
	    glVertex3f(0.0, 0.5, 1.0);
	    glVertex3f(0.5, 0.5, 1.0);
	    glVertex3f(1.0, 0.5, 1.0);
	    glVertex3f(-1.0, 0.0, 1.0);
	    glVertex3f(-0.5, 0.0, 1.0);
	    glVertex3f(0.0, 0.0, 1.0);
	    glVertex3f(0.5, 0.0, 1.0);
	    glVertex3f(1.0, 0.0, 1.0);
	    glVertex3f(-1.0, -0.5, 1.0);
	    glVertex3f(-0.5, -0.5, 1.0);
	    glVertex3f(0.0, -0.5, 1.0);
	    glVertex3f(0.5, -0.5, 1.0);
	    glVertex3f(1.0, -0.5, 1.0);
	    glVertex3f(-1.0, -1.0, 1.0);
	    glVertex3f(-0.5, -1.0, 1.0);
	    glVertex3f(0.0, -1.0, 1.0);
	    glVertex3f(0.5, -1.0, 1.0);
	    glVertex3f(1.0, -1.0, 1.0);
	    glVertex3f(-0.75, 0.75, 1.0);
	    glVertex3f(-0.25, 0.75, 1.0);
	    glVertex3f(0.25, 0.75, 1.0);
	    glVertex3f(0.75, 0.75, 1.0);
	    glVertex3f(-0.75, 0.25, 1.0);
	    glVertex3f(-0.25, 0.25, 1.0);
	    glVertex3f(0.25, 0.25, 1.0);
	    glVertex3f(0.75, 0.25, 1.0);
	    glVertex3f(-0.75, -0.25, 1.0);
	    glVertex3f(-0.25, -0.25, 1.0);
	    glVertex3f(0.25, -0.25, 1.0);
	    glVertex3f(0.75, -0.25, 1.0);
	    glVertex3f(-0.75, -0.75, 1.0);
	    glVertex3f(-0.25, -0.75, 1.0);
	    glVertex3f(0.25, -0.75, 1.0);
	    glVertex3f(0.75, -0.75, 1.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+20,GL_COMPILE);
	glBegin(GL_LINE_STRIP);
	    glVertex3f(1.0, 1.0, 1.0);
	    glVertex3f(-1.0, -1.0, 1.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-1.0, 1.0, 1.0);
	    glVertex3f(1.0, -1.0, 1.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+21,GL_COMPILE);
 	glBegin(GL_LINE_STRIP);
	    glVertex3f(0.0, -1.0, 1.0);
	    glVertex3f(0.0, 1.0, 1.0);
	    glVertex3f(0.5, 0.0, 1.0);
	    glVertex3f(0.0, 1.0, 1.0);
	    glVertex3f(-0.5, 0.0, 1.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+22,GL_COMPILE);
	glBegin(GL_LINE_LOOP);
	    glVertex3f(-0.5, 0.5, 1.0);
	    glVertex3f(0.4, 0.5, 1.0);
	    glVertex3f(0.5, 0.25, 1.0);
	    glVertex3f(0.4, 0.0, 1.0);
	    glVertex3f(-0.5, 0.0, 1.0);
	    glVertex3f(-0.5, -0.5, 1.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+23,GL_COMPILE);
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-0.5, -0.5, 1.0);
	    glVertex3f(-0.5, 0.5, 1.0);
	    glVertex3f(0.0, 0.0, 1.0);
	    glVertex3f(0.5, 0.5, 1.0);
	    glVertex3f(0.5, -0.5, 1.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+24,GL_COMPILE);
	glBegin(GL_LINE_STRIP);
	    glVertex3f(1.0, 1.0, 1.0);
	    glVertex3f(-1.0, -1.0, 1.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-1.0, 1.0, 1.0);
	    glVertex3f(1.0, -1.0, 1.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+25,GL_COMPILE);
	glBegin(GL_LINE_STRIP);
	    glVertex3f(1.0, 1.0, 1.0);
	    glVertex3f(-1.0, -1.0, 1.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-1.0, 1.0, 1.0);
	    glVertex3f(1.0, -1.0, 1.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+26,GL_COMPILE);
	glBegin(GL_LINE_STRIP);
	    glVertex3f(0.7, 0.7, 1.0);
	    glVertex3f(0.5, 0.85, 1.0);
	    glVertex3f(0.0, 1.0, 1.0);
	    glVertex3f(-0.5, 0.85, 1.0);
	    glVertex3f(-0.7, 0.7, 1.0);
	    glVertex3f(-0.85, 0.5, 1.0);
	    glVertex3f(-1.0, 0.0, 1.0);
	    glVertex3f(-0.85, -0.5, 1.0);
	    glVertex3f(-0.7, -0.7, 1.0);
	    glVertex3f(-0.5, -0.85, 1.0);
	    glVertex3f(0.0, -1.0, 1.0);
	    glVertex3f(0.5, -0.85, 1.0);
	    glVertex3f(0.7, -0.7, 1.0);
	    glVertex3f(0.85, -0.5, 1.0);
	    glVertex3f(1.0, 0.0, 1.0);
	    glVertex3f(0.0, 0.0, 1.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+27,GL_COMPILE);
	glColor3f(0.2,0.2,0.2);
	glCallList(LITH_ICON+13);
	glColor3f(0.5,0.5,0.5);
	glCallList(LITH_ICON+19);
    glEndList();
    glNewList(LITH_ICON+28,GL_COMPILE);
	glColor3f(0.2,0.2,0.2);
	glCallList(LITH_ICON+19);
	glColor3f(0.5,0.5,0.5);
	glCallList(LITH_ICON+13);
    glEndList();
    glNewList(LITH_ICON+29,GL_COMPILE);
	glColor3f(0.2,0.2,0.2);
	glCallList(LITH_ICON+12);
	glColor3f(0.5,0.5,0.5);
	glCallList(LITH_ICON+13);
    glEndList();
    glNewList(LITH_ICON+30,GL_COMPILE);
	glColor3f(0.2,0.2,0.2);
	glCallList(LITH_ICON+13);
	glColor3f(0.5,0.5,0.5);
	glCallList(LITH_ICON+12);
    glEndList();


}

make_lith_icon_map()
{
    printf("make_lith_icon_map\n");

    glNewList(LITH_ICON,GL_COMPILE);
	glBegin(GL_LINE_STRIP);
	    glVertex3f(1.0/2, 1.0/2, 0.0);
	    glVertex3f(-1.0/2, -1.0/2, 0.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-1.0/2, 1.0/2, 0.0);
	    glVertex3f(1.0/2, -1.0/2, 0.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+1,GL_COMPILE);
	glBegin(GL_POLYGON);
	    glVertex3f(0.0, 1.0, 0.0);
	    glVertex3f(1.0, -1.0, 0.0);
	    glVertex3f(-1.0, -1.0, 0.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+2,GL_COMPILE);
	glBegin(GL_POLYGON);
	    glVertex3f(0.0, -1.0, 0.0);
	    glVertex3f(1.0, 1.0, 0.0);
	    glVertex3f(-1.0, 1.0, 0.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+3,GL_COMPILE);
	glBegin(GL_LINE_LOOP);
	    glVertex3f(-1.0, 1.0, 0.0);
	    glVertex3f(-1.0, 0.8, 0.0);
	    glVertex3f(-0.8, 0.8, 0.0);
	    glVertex3f(-0.8, 1.0, 0.0);
	glEnd();
	glBegin(GL_LINE_LOOP);
	    glVertex3f(1.0, 1.0, 0.0);
	    glVertex3f(1.0, 0.8, 0.0);
	    glVertex3f(0.8, 0.8, 0.0);
	    glVertex3f(0.8, 1.0, 0.0);
	glEnd();
	glBegin(GL_LINE_LOOP);
	    glVertex3f(-1.0, -1.0, 0.0);
	    glVertex3f(-1.0, -0.8, 0.0);
	    glVertex3f(-0.8, -0.8, 0.0);
	    glVertex3f(-0.8, -1.0, 0.0);
	glEnd();
	glBegin(GL_LINE_LOOP);
	    glVertex3f(1.0, -1.0, 0.0);
	    glVertex3f(1.0, -0.8, 0.0);
	    glVertex3f(0.8, -0.8, 0.0);
	    glVertex3f(0.8, -1.0, 0.0);
	glEnd();
	glBegin(GL_LINE_LOOP);
	    glVertex3f(0.1, 0.1, 0.0);
	    glVertex3f(0.1, -0.1, 0.0);
	    glVertex3f(-0.1, -0.1, 0.0);
	    glVertex3f(-0.1, 0.1, 0.0);
	glEnd();
	glBegin(GL_POINTS);
	    glVertex3f(0.0, 1.0, 0.0);
	    glVertex3f(-1.0, 0.0, 0.0);
	    glVertex3f(1.0, 0.0, 0.0);
	    glVertex3f(0.0, -1.0, 0.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+4,GL_COMPILE);
	glBegin(GL_LINE_LOOP);
	    glVertex3f(-1.0, -0.5, 0.0);
	    glVertex3f(-0.5, 0.5, 0.0);
	    glVertex3f(1.0, 0.5, 0.0);
	    glVertex3f(0.5, -0.5, 0.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+5,GL_COMPILE);
	glBegin(GL_LINE_LOOP);
	    glVertex3f(0.0, 1.0, 0.0);
	    glVertex3f(1.0, 0.5, 0.0);
	    glVertex3f(1.0, -0.5, 0.0);
	    glVertex3f(0.0, -1.0, 0.0);
	    glVertex3f(-1.0, -0.5, 0.0);
	    glVertex3f(-1.0, 0.5, 0.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+6,GL_COMPILE);
	glBegin(GL_LINE_LOOP);
	    glVertex3f(0.0, -1.0, 0.0);
	    glVertex3f(1.0, 1.0, 0.0);
	    glVertex3f(-1.0, 1.0, 0.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+7,GL_COMPILE);
	glBegin(GL_LINE_LOOP);
	    glVertex3f(0.0, 1.0, 0.0);
	    glVertex3f(1.0, 0.5, 0.0);
	    glVertex3f(1.0, -0.5, 0.0);
	    glVertex3f(0.0, -1.0, 0.0);
	    glVertex3f(-1.0, -0.5, 0.0);
	    glVertex3f(-1.0, 0.5, 0.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+8,GL_COMPILE);
	glBegin(GL_LINE_LOOP);
	    glVertex3f(0.0, 1.0, 0.0);
	    glVertex3f(1.0, 0.5, 0.0);
	    glVertex3f(1.0, -0.5, 0.0);
	    glVertex3f(0.0, -1.0, 0.0);
	    glVertex3f(-1.0, -0.5, 0.0);
	    glVertex3f(-1.0, 0.5, 0.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+9,GL_COMPILE);
	glBegin(GL_LINE_LOOP);
	    glVertex3f(0.0, 1.0, 0.0);
	    glVertex3f(1.0, -1.0, 0.0);
	    glVertex3f(-1.0, -1.0, 0.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+10,GL_COMPILE);
	glBegin(GL_LINE_LOOP);
	    glVertex3f(-1.0, -0.5, 0.0);
	    glVertex3f(-0.85, 0.0, 0.0);
	    glVertex3f(-0.5, 0.35, 0.0);
	    glVertex3f(0.0, 0.5, 0.0);
	    glVertex3f(0.5, 0.35, 0.0);
	    glVertex3f(0.85, 0.0, 0.0);
	    glVertex3f(1.0, -0.5, 0.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+11,GL_COMPILE);
	glBegin(GL_POLYGON);
	    glVertex3f(-1.0, -0.5, 0.0);
	    glVertex3f(-0.85, 0.0, 0.0);
	    glVertex3f(-0.5, 0.35, 0.0);
	    glVertex3f(0.0, 0.5, 0.0);
	    glVertex3f(0.5, 0.35, 0.0);
	    glVertex3f(0.85, 0.0, 0.0);
	    glVertex3f(1.0, -0.5, 0.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+12,GL_COMPILE);
	glBegin(GL_LINE_LOOP);
	    glVertex3f(-1.0, 1.0, 0.0);
	    glVertex3f(1.0, 1.0, 0.0);
	    glVertex3f(1.0, 0.33, 0.0);
	    glVertex3f(-1.0, 0.33, 0.0);
	glEnd();
	glBegin(GL_LINE_LOOP);
	    glVertex3f(-1.0, -1.0, 0.0);
	    glVertex3f(1.0, -1.0, 0.0);
	    glVertex3f(1.0, -0.33, 0.0);
	    glVertex3f(-1.0, -0.33, 0.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(0.0, 1.0, 0.0);
	    glVertex3f(0.0, 0.33, 0.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(0.0, -1.0, 0.0);
	    glVertex3f(0.0, -0.33, 0.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(0.5, 0.33, 0.0);
	    glVertex3f(0.5, -0.33, 0.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-0.5, 0.33, 0.0);
	    glVertex3f(-0.5, -0.33, 0.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+13,GL_COMPILE);
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-1.0, 1.0, 0.0);
	    glVertex3f(-0.7, 1.0, 0.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-0.25, 1.0, 0.0);
	    glVertex3f(0.25, 1.0, 0.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(0.7, 1.0, 0.0);
	    glVertex3f(1.0, 1.0, 0.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-0.8, 0.5, 0.0);
	    glVertex3f(-0.2, 0.5, 0.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(0.2, 0.5, 0.0);
	    glVertex3f(0.8, 0.5, 0.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-1.0, 0.0, 0.0);
	    glVertex3f(-0.7, 0.0, 0.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-0.25, 0.0, 0.0);
	    glVertex3f(0.25, 0.0, 0.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(0.7, 0.0, 0.0);
	    glVertex3f(1.0, 0.0, 0.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-0.8, -0.5, 0.0);
	    glVertex3f(-0.2, -0.5, 0.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(0.2, -0.5, 0.0);
	    glVertex3f(0.8, -0.5, 0.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-1.0, -1.0, 0.0);
	    glVertex3f(-0.7, -1.0, 0.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-0.25, -1.0, 0.0);
	    glVertex3f(0.25, -1.0, 0.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(0.7, -1.0, 0.0);
	    glVertex3f(1.0, -1.0, 0.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+14,GL_COMPILE);
	glBegin(GL_LINE_STRIP);
	    glVertex3f(1.0, 1.0, 0.0);
	    glVertex3f(-1.0, -1.0, 0.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-1.0, 1.0, 0.0);
	    glVertex3f(1.0, -1.0, 0.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+15,GL_COMPILE);
	glBegin(GL_LINE_STRIP);
	    glVertex3f(1.0, 1.0, 0.0);
	    glVertex3f(-1.0, -1.0, 0.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-1.0, 1.0, 0.0);
	    glVertex3f(1.0, -1.0, 0.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+16,GL_COMPILE);
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-1.0, -0.5, 0.0);
	    glVertex3f(0.0, -0.5, 0.0);
	    glVertex3f(-0.85, 0.0, 0.0);
	    glVertex3f(0.0, -0.5, 0.0);
	    glVertex3f(-0.5, 0.35, 0.0);
	    glVertex3f(0.0, -0.5, 0.0);
	    glVertex3f(0.0, 0.5, 0.0);
	    glVertex3f(0.0, -0.5, 0.0);
	    glVertex3f(0.5, 0.35, 0.0);
	    glVertex3f(0.0, -0.5, 0.0);
	    glVertex3f(0.85, 0.0, 0.0);
	    glVertex3f(0.0, -0.5, 0.0);
	    glVertex3f(1.0, -0.5, 0.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+17,GL_COMPILE);
	glBegin(GL_POLYGON);
	    glVertex3f(-1.0, 0.0, 0.0);
	    glVertex3f(-0.93, 0.15, 0.0);
	    glVertex3f(-0.75, 0.25, 0.0);
	    glVertex3f(0.75, 0.25, 0.0);
	    glVertex3f(0.93, 0.15, 0.0);
	    glVertex3f(1.0, 0.0, 0.0);
	    glVertex3f(0.93, -0.15, 0.0);
	    glVertex3f(0.75, -0.25, 0.0);
	    glVertex3f(-0.75, -0.25, 0.0);
	    glVertex3f(-0.93, -0.15, 0.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+18,GL_COMPILE);
	glBegin(GL_LINE_LOOP);
	    glVertex3f(-1.0, 0.0, 0.0);
	    glVertex3f(-0.5, 0.0, 0.0);
	    glVertex3f(-0.25, 0.35, 0.0);
	    glVertex3f(-0.5, 0.85, 0.0);
	    glVertex3f(-0.25, 0.43, 0.0);
	    glVertex3f(0.25, 0.43, 0.0);
	    glVertex3f(0.5, 0.85, 0.0);
	    glVertex3f(0.25, 0.43, 0.0);
	    glVertex3f(0.5, 0.0, 0.0);
	    glVertex3f(1.0, 0.0, 0.0);
	    glVertex3f(0.5, 0.0, 0.0);
	    glVertex3f(0.25, -0.43, 0.0);
	    glVertex3f(0.5, -0.85, 0.0);
	    glVertex3f(0.25, -0.43, 0.0);
	    glVertex3f(-0.25, -0.43, 0.0);
	    glVertex3f(-0.5, -0.85, 0.0);
	    glVertex3f(-0.25, -0.43, 0.0);
	    glVertex3f(-0.5, 0.0, 0.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+19,GL_COMPILE);
	glBegin(GL_POINTS);
	    glVertex3f(-1.0, 1.0, 0.0);
	    glVertex3f(-0.5, 1.0, 0.0);
	    glVertex3f(0.0, 1.0, 0.0);
	    glVertex3f(0.5, 1.0, 0.0);
	    glVertex3f(1.0, 1.0, 0.0);
	    glVertex3f(-1.0, 0.5, 0.0);
	    glVertex3f(-0.5, 0.5, 0.0);
	    glVertex3f(0.0, 0.5, 0.0);
	    glVertex3f(0.5, 0.5, 0.0);
	    glVertex3f(1.0, 0.5, 0.0);
	    glVertex3f(-1.0, 0.0, 0.0);
	    glVertex3f(-0.5, 0.0, 0.0);
	    glVertex3f(0.0, 0.0, 0.0);
	    glVertex3f(0.5, 0.0, 0.0);
	    glVertex3f(1.0, 0.0, 0.0);
	    glVertex3f(-1.0, -0.5, 0.0);
	    glVertex3f(-0.5, -0.5, 0.0);
	    glVertex3f(0.0, -0.5, 0.0);
	    glVertex3f(0.5, -0.5, 0.0);
	    glVertex3f(1.0, -0.5, 0.0);
	    glVertex3f(-1.0, -1.0, 0.0);
	    glVertex3f(-0.5, -1.0, 0.0);
	    glVertex3f(0.0, -1.0, 0.0);
	    glVertex3f(0.5, -1.0, 0.0);
	    glVertex3f(1.0, -1.0, 0.0);
	    glVertex3f(-0.75, 0.75, 0.0);
	    glVertex3f(-0.25, 0.75, 0.0);
	    glVertex3f(0.25, 0.75, 0.0);
	    glVertex3f(0.75, 0.75, 0.0);
	    glVertex3f(-0.75, 0.25, 0.0);
	    glVertex3f(-0.25, 0.25, 0.0);
	    glVertex3f(0.25, 0.25, 0.0);
	    glVertex3f(0.75, 0.25, 0.0);
	    glVertex3f(-0.75, -0.25, 0.0);
	    glVertex3f(-0.25, -0.25, 0.0);
	    glVertex3f(0.25, -0.25, 0.0);
	    glVertex3f(0.75, -0.25, 0.0);
	    glVertex3f(-0.75, -0.75, 0.0);
	    glVertex3f(-0.25, -0.75, 0.0);
	    glVertex3f(0.25, -0.75, 0.0);
	    glVertex3f(0.75, -0.75, 0.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+20,GL_COMPILE);
	glBegin(GL_LINE_STRIP);
	    glVertex3f(1.0, 1.0, 0.0);
	    glVertex3f(-1.0, -1.0, 0.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-1.0, 1.0, 0.0);
	    glVertex3f(1.0, -1.0, 0.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+21,GL_COMPILE);
 	glBegin(GL_LINE_STRIP);
	    glVertex3f(0.0, -1.0, 0.0);
	    glVertex3f(0.0, 1.0, 0.0);
	    glVertex3f(0.5, 0.0, 0.0);
	    glVertex3f(0.0, 1.0, 0.0);
	    glVertex3f(-0.5, 0.0, 0.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+22,GL_COMPILE);
	glBegin(GL_LINE_LOOP);
	    glVertex3f(-0.5, 0.5, 0.0);
	    glVertex3f(0.4, 0.5, 0.0);
	    glVertex3f(0.5, 0.25, 0.0);
	    glVertex3f(0.4, 0.0, 0.0);
	    glVertex3f(-0.5, 0.0, 0.0);
	    glVertex3f(-0.5, -0.5, 0.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+23,GL_COMPILE);
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-0.5, -0.5, 0.0);
	    glVertex3f(-0.5, 0.5, 0.0);
	    glVertex3f(0.0, 0.0, 0.0);
	    glVertex3f(0.5, 0.5, 0.0);
	    glVertex3f(0.5, -0.5, 0.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+24,GL_COMPILE);
	glBegin(GL_LINE_STRIP);
	    glVertex3f(1.0, 1.0, 0.0);
	    glVertex3f(-1.0, -1.0, 0.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-1.0, 1.0, 0.0);
	    glVertex3f(1.0, -1.0, 0.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+25,GL_COMPILE);
	glBegin(GL_LINE_STRIP);
	    glVertex3f(1.0, 1.0, 0.0);
	    glVertex3f(-1.0, -1.0, 0.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	    glVertex3f(-1.0, 1.0, 0.0);
	    glVertex3f(1.0, -1.0, 0.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+26,GL_COMPILE);
	glBegin(GL_LINE_STRIP);
	    glVertex3f(0.7, 0.7, 0.0);
	    glVertex3f(0.5, 0.85, 0.0);
	    glVertex3f(0.0, 1.0, 0.0);
	    glVertex3f(-0.5, 0.85, 0.0);
	    glVertex3f(-0.7, 0.7, 0.0);
	    glVertex3f(-0.85, 0.5, 0.0);
	    glVertex3f(-1.0, 0.0, 0.0);
	    glVertex3f(-0.85, -0.5, 0.0);
	    glVertex3f(-0.7, -0.7, 0.0);
	    glVertex3f(-0.5, -0.85, 0.0);
	    glVertex3f(0.0, -1.0, 0.0);
	    glVertex3f(0.5, -0.85, 0.0);
	    glVertex3f(0.7, -0.7, 0.0);
	    glVertex3f(0.85, -0.5, 0.0);
	    glVertex3f(1.0, 0.0, 0.0);
	    glVertex3f(0.0, 0.0, 0.0);
	glEnd();
    glEndList();
    glNewList(LITH_ICON+27,GL_COMPILE);
	glColor3f(0.2,0.2,0.2);
	glCallList(LITH_ICON+13);
	glColor3f(0.5,0.5,0.5);
	glCallList(LITH_ICON+19);
    glEndList();
    glNewList(LITH_ICON+28,GL_COMPILE);
	glColor3f(0.2,0.2,0.2);
	glCallList(LITH_ICON+19);
	glColor3f(0.5,0.5,0.5);
	glCallList(LITH_ICON+13);
    glEndList();
    glNewList(LITH_ICON+29,GL_COMPILE);
	glColor3f(0.2,0.2,0.2);
	glCallList(LITH_ICON+12);
	glColor3f(0.5,0.5,0.5);
	glCallList(LITH_ICON+13);
    glEndList();
    glNewList(LITH_ICON+30,GL_COMPILE);
	glColor3f(0.2,0.2,0.2);
	glCallList(LITH_ICON+13);
	glColor3f(0.5,0.5,0.5);
	glCallList(LITH_ICON+12);
    glEndList();


}

