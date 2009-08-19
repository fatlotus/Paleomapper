#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <math.h>
#include "paleotypes.h"
#include "paleodefs.h"
#include "paleoexterns.h"


#define MAX_PMAG 2000
#define RADS 0.017453293

extern int mapproj;
int pmagnum;
extern int draw_pmag[MAX_PMAG];
extern int one_pmag;

extern do_recon();
extern float PaleoTime;
extern int      AutoDel, Bell, RefPlate;
extern char     *save_file;
extern float    start_time, end_time, del_time;
static int      pw_AutoDel, pw_RefPlate;
static char     *pw_save_file;
static float    pw_start_time, pw_end_time, pw_del_time;
extern int pw_once;

extern int plate_alive[900];
extern int	graveyard;

extern int check1,check2,check3;
int check4=0,check5=0,check6=0;

extern int polar_wander;
extern int show_pmag_data, show_pmag_pole;
extern float pmagpolelat,pmagpolelon;
extern float maprotlat[5],maprotlon[5],maprotang[5];

extern build_help();

int nopv,nop;
float kappa,alpha;


pmag_reliable()
{
    int  j,choose;

    for(j=0;j<pmagnum;j++) 
    {
	
	choose = pmag_list[j].plate;
	do {
		if (choose<=100 || choose>907) break;
		if (choose>=150 && choose<=200) break;
		if (choose==102 || choose==145) break;
		if (choose>210 && choose<=300) break;
		if (choose>=302 && choose<=305) break;
		if (choose>=307 && choose<=500) break;
		if (choose>501 && choose<=700) break;
		if (choose>709 && choose<=801) break;
		if (choose>802 && choose<=906) break;
		draw_pmag[j] = 2;
/*		printf(" %d ",choose); */
	   } while(FALSE);
    }
/*    printf("\n");*/
}



get_pmag_pole()
{
    float sx,sy,sz,px,py,pz,mx,my,mz;
    float resultant,rlat,rlon;
    int i,j;
    float alat,alon,anlat,anlon,rotlat[5],rotlon[5],rotang[5];
    float a1,a2,a3,a4,a5,rx,ry,rz,gx,gy,gz,vct,sina,cosa,sqrz;
    float df,ex,pr,as;
    int noplist[900];

    for(i=0;i<900;i++) noplist[i]=0;
 
    sx=0.0;
    sy=0.0;
    sz=0.0;
    nopv=0;
    nop=0;

    for(j=0;j<pmagnum;j++) {
	if (draw_pmag[j]) {

	nopv++;
/*printf("noplist %d \n",noplist[pmag_list[j].plate-100]);*/
	if (!noplist[pmag_list[j].plate-100]) {
	    nop++;
	    noplist[pmag_list[j].plate-100]=TRUE;
	}
	alat = pmag_list[j].lat;
	alon = pmag_list[j].lon;
	for(i=0;i<5;i++) {
	    rotlat[i] = plate_list[pmag_list[j].plate]->latitude[i];
	    rotlon[i] = plate_list[pmag_list[j].plate]->longitude[i];
	    rotang[i] = plate_list[pmag_list[j].plate]->angle[i];
	}

    for(i=0;i<5;i++) { if (i==0 || rotang[i] != 0.0) {
/*printf("lat %.2f lon %.2f ang %.2f \n",rotlat[i],rotlon[i],rotang[i]);*/
	a1 = 90.0*RADS-alat*RADS;
	a2 = alon*RADS;
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
	alat = anlat;
	alon = anlon;
	}}
	sx = rx + sx;
	sy = ry + sy;
	sz = rz + sz;

/*printf("anlat %.3f anlon %.3f sx %.3f sy %.3f sz %.3f \n",
	anlat,anlon,sx,sy,sz);*/
		

/*
		px = sin((90-pmag_list[j].lat)*RADS)*cos(pmag_list[j].lon*RADS);
		py = sin((90-pmag_list[j].lat)*RADS)*sin(pmag_list[j].lon*RADS);
		pz = cos((90-pmag_list[j].lat)*RADS);
	
		sx = px + sx;
		sy = py + sy;
		sz = pz + sz;
*/

	}
    }

    if (sx != 0.0 && sy != 0.0 && sz != 0.0) 
/*    if (sx != 0.0 || sy != 0.0 || sz != 0.0) */
    {

    show_pmag_pole=TRUE;

    resultant = sqrt(sx*sx+sy*sy+sz*sz);
    kappa = (nopv-1)/(nopv-resultant);
	df = (nopv - resultant)/resultant;
	ex = 1.0/(nopv-1.0);
	pr = pow(20.0,ex);
	pr = pr - 1.0;
	pr = df*pr;
	pr = 1.0 - pr;
	as = atan(pr/sqrt(1.0-pr*pr));
    alpha = (RADS*90-as)/RADS;

	/*  Calculate mean pole  */
    mx = sx/resultant;
    my = sy/resultant;
    mz = sz/resultant;

/*    if (mx<0)
    {
    	mx = 0-mx;
    	my = 0-my;
    	mz = 0-mz;
    }
*/
    rlat = 90-acos(mz)/RADS;

    if (mx<0.001 && mx>-0.001 && my<0.001 && my>-0.001) {
    	rlon=0.0;
    	printf("points are the same \n");
    }
    else {
    	rlon = atan(my/mx)/RADS;
    	if (mx<0) rlon = rlon + 180;	
    }

    pmagpolelat=rlat;
    pmagpolelon=rlon;

    }

    else { show_pmag_pole=FALSE; }
}


pmag_dec()
{
  int j,pmag_alive;
  float midage;

    /*******  PMAG Decision Maker  *******/
    /**  this will decide which data to draw  **/


    /****  Set all true for the display checks given  ****/

    for(j=0;j<pmagnum;j++) draw_pmag[j]=FALSE;
    if (check1) for(j=0;j<pmagnum;j++) draw_pmag[j]=1;
    if (check2) pmag_reliable();
    if (check3) for(j=0;j<pmagnum;j++) 
	if (one_pmag == pmag_list[j].plate) draw_pmag[j]=3;

    /****  Set false for all plates not alive and
	  for all data not within the Paleotime window  ****/

    for(j=0;j<pmagnum;j++) {
	pmag_alive=plate_alive[pmag_list[j].plate-100];
	if (!pmag_alive || graveyard) draw_pmag[j]=FALSE;
	midage = (pmag_list[j].old+pmag_list[j].young)/2.0;
	if (PaleoTime<(midage-10.0) || PaleoTime>(midage+10.0))
	    draw_pmag[j]=FALSE; 
	if (draw_pmag[j]) { 
	    printf("midage: %.3f  uncert: %.1f  draw: %d  ",
		midage,pmag_list[j].uncert,draw_pmag[j]); 
	    if (pmag_list[j].uncert==0) 
		printf("WARNING: PMAG Uncertainty = 0");
	    printf("\n"); 
	} 
    }

/* printf("PaleoTime: %.3f \n",PaleoTime); */


    if (!check6) get_pmag_pole();

}

make_apwp()
{
}

set_pw_var(filename)
char *filename;
{
    char input[80];

    pw_save_file = filename;
    sprintf(input,"Please enter the starting time for the PW Path (0): ");
     prompt_message(input);
     if (!(strcmp(input,"CANCEL"))) return(0);
     pw_start_time = atof(input);
    sprintf(input,"Please enter the ending time for the PW Path (100): ");
     prompt_message(input);
     if (!(strcmp(input,"CANCEL"))) return(0);
     pw_end_time = (*input==NULL) ? 100 : atof(input);
    sprintf(input,"Please enter the delta time for the PW Path (10): ");
     prompt_message(input);
     if (!(strcmp(input,"CANCEL"))) return(0);
     pw_del_time = (*input==NULL) ? 10 : atof(input);
    sprintf(input, "Enter the ID of the fixed plate (000)");
     prompt_message(input);
     if (!(strcmp(input,"CANCEL"))) return(0);
     pw_RefPlate = atoi(input);
/*    sprintf(input,"Autodelete non-existent data? (Yes)");
     prompt_message(input);
     if (!(strcmp(input,"CANCEL"))) return(0);
     pw_AutoDel = (!(*input == 'n' || *input == 'N')) ? TRUE : FALSE;
*/
    make_polar_wander();
    pw_once = TRUE;
}


make_polar_wander()
{
  FILE *pwfile;
  char text[128];
  int pmagin[MAX_PMAG];
  float time, oldptime, timepolelat,timepolelon;
  int i,j,k,pmag_alive,timepole;
  float midage;
  int  choose;
  float phi,theta,polex,poley,polez;
    float sx,sy,sz,px,py,pz,mx,my,mz;
    float resultant,rlat,rlon;
    float alat,alon,anlat,anlon,rotlat[5],rotlon[5],rotang[5];
    float a1,a2,a3,a4,a5,rx,ry,rz,gx,gy,gz,vct,sina,cosa,sqrz;
    int noplist[900];

    oldptime=PaleoTime;
/*    save_file = "/usr/people/rowley/Paleomapper/rot/newworld.rot";*/
    save_file = pw_save_file;
/*    AutoDel = pw_AutoDel;*/
    AutoDel = 0;
    RefPlate = pw_RefPlate;
    start_time = pw_start_time;
    end_time = pw_end_time;
    del_time = pw_del_time;
    

    Bell = FALSE;

    pwfile = fopen("/usr/people/rowley/Paleomapper/pmag/pw.tmp","w+");

  for (k=1; k<=3; k++) {
    time=start_time;
    for(j=0;j<pmagnum;j++) pmagin[j]=FALSE;
    if (k==1 && check1) for(j=0;j<pmagnum;j++) pmagin[j]=1;
    if (k==2 && check2) {
    	for(j=0;j<pmagnum;j++) {
	    choose = pmag_list[j].plate;
	    do {
		if (choose<=100 || choose>907) break;
		if (choose>=150 && choose<=200) break;
		if (choose==102 || choose==145) break;
		if (choose>210 && choose<=300) break;
		if (choose>=302 && choose<=305) break;
		if (choose>=307 && choose<=500) break;
		if (choose>501 && choose<=700) break;
		if (choose>709 && choose<=801) break;
		if (choose>802 && choose<=906) break;
		pmagin[j] = 2;
	   } while(FALSE);
    	}
    }
    if (k==3 && check3) for(j=0;j<pmagnum;j++) 
	if (one_pmag == pmag_list[j].plate) pmagin[j]=3;

    /****  Set false for all plates not alive and
	  for all data not within the Paleotime window  ****/

    for(j=0;j<pmagnum;j++) {
	pmag_alive=plate_alive[pmag_list[j].plate-100];
	if (!pmag_alive || graveyard) pmagin[j]=FALSE;
    }
  if ((k==1 && check1) || (k==2 && check2) || (k==3 && check3))
  while (time <= end_time) {
    printf("time: %.3f \n",time);
    PaleoTime=time;
    do_recon(); 

    /***  From get_pmag_pole();  ***/

    for(i=0;i<900;i++) noplist[i]=0;
 
    sx=0.0; sy=0.0; sz=0.0; nopv=0; nop=0;

    for(j=0;j<pmagnum;j++) {
	midage = (pmag_list[j].old+pmag_list[j].young)/2.0;
	if (time>=(midage-10.0) && time<=(midage+10.0) && pmagin[j]) {
/*	if (pmagin[j]) { */

	nopv++;
	if (!noplist[pmag_list[j].plate-100]) {
	    nop++;
	    noplist[pmag_list[j].plate-100]=TRUE;
	}
	alat = pmag_list[j].lat;
	alon = pmag_list[j].lon;
	for(i=0;i<5;i++) {
	    rotlat[i] = plate_list[pmag_list[j].plate]->latitude[i];
	    rotlon[i] = plate_list[pmag_list[j].plate]->longitude[i];
	    rotang[i] = plate_list[pmag_list[j].plate]->angle[i];
	}
/*printf("lat %.2f lon %.2f ang %.2f\n",rotlat[0],rotlon[0],rotang[0]);*/

	    for(i=0;i<5;i++) { if (i==0 || rotang[i] != 0.0) {
		a1 = 90.0*RADS-alat*RADS;
		a2 = alon*RADS;
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
		alat = anlat;
		alon = anlon;
	    } }
	    sx = rx + sx;
	    sy = ry + sy;
	    sz = rz + sz;
	}
    }

    if (sx != 0.0 || sy != 0.0 || sz != 0.0)
    {
   	timepole=TRUE;
    	resultant = sqrt(sx*sx+sy*sy+sz*sz);

	/*  Calculate mean pole  */
    	mx = sx/resultant;
    	my = sy/resultant;
    	mz = sz/resultant;

    	rlat = 90-acos(mz)/RADS;
	
    	if (mx<0.001 && mx>-0.001 && my<0.001 && my>-0.001) {
    	    rlon=0.0;
    	    printf("points are the same \n"); }
    	  else {
    	    rlon = atan(my/mx)/RADS;
    	    if (mx<0) rlon = rlon + 180; }

    	timepolelat=rlat;
    	timepolelon=rlon;
	printf("tplat %.2f  tplon %.2f \n",timepolelat,timepolelon);
	fprintf(pwfile,"%.2f %.2f \n",timepolelat,timepolelon);
    }
    else  timepole=FALSE; 

    time+=del_time; 
    }
    fprintf(pwfile,"999.9 999.9 \n");
  }
    PaleoTime=oldptime;
    do_recon();

    rewind(pwfile);
    glNewList(PMAG_START-1,GL_COMPILE);
    glBegin(GL_LINE_STRIP);
    glColor3f(1.0, 0.0, 0.0); k=1;
    while (fgets(text,127,pwfile) != NULL) {
	sscanf(text,"%f %f",&timepolelat,&timepolelon);
/*	    printf("%f,%f \n",timepolelat,timepolelon); */
        if (timepolelat>900 && timepolelon>900) { k++;
	    if (k==2) glColor3f(0.0, 1.0, 0.0);
	    if (k==3) glColor3f(0.0, 0.0, 1.0);
	    glEnd();
	    glBegin(GL_LINE_STRIP);
	}
        else {
	phi = 0.017453292 * (90.0 - timepolelat);
        theta = 0.017453292 * timepolelon;
        polez = cos(theta) * sin(phi);
        polex = sin(theta) * sin(phi);
        poley = cos(phi);
	glVertex3f(polex, poley, polez);
	}
    }	
    glEnd();
    glEndList();
    fclose(pwfile);
    printf("done constructing polar wander path!\n");
    Bell = TRUE;
}

flip(num)
int num;
{

    switch(num) {
	case 1: if (check1) check1=FALSE;
		else check1=TRUE;
		break;
	case 2: if (check2) check2=FALSE;
		else check2=TRUE;
		break;
	case 3: if (check3) check3=FALSE;
		else check3=TRUE;
		break;
	case 4: if (check4) { check4=FALSE; show_pmag_data=TRUE; }
		else { check4=TRUE; show_pmag_data=FALSE; }
		break;
	case 5: if (check5) check5=FALSE;
		else check5=TRUE;
		break;
	case 6: if (check6) { check6=FALSE; show_pmag_pole=TRUE; }
		else { check6=TRUE; show_pmag_pole=FALSE; }
		break;
    }
}

pmag_maker(filename)
char *filename;
{
    char 	text[128];
    char 	buf[128];
    FILE 	*datafile;
    int		num,teid,i=0,pmagnumalive=0;
    float	yage,oage,plat,plon,puncert;


    if ((datafile = fopen(filename,"r")) == NULL) {
        sprintf(text,"Error opening %s\n",filename);
	print_warning(text);
	return(0); }

    sprintf(text,"Reading %s ...",filename);
    display_waiting(text);

    pmagnum=0;

    while (fgets(buf,100,datafile) != NULL) {

	sscanf(buf,"%d%d%f%f%f%f%f",&num,&teid,&yage,&oage,&plat,&plon,&puncert);


	if (puncert<20.0) {
        pmagnum++;
	pmag_list[pmagnum-1].unique=num;
	pmag_list[pmagnum-1].plate=teid;
	pmag_list[pmagnum-1].young=yage;
	pmag_list[pmagnum-1].old=oage;
	pmag_list[pmagnum-1].lat=plat;
	pmag_list[pmagnum-1].lon=plon;
	pmag_list[pmagnum-1].uncert=puncert;
	if (plate_alive[teid-100]) pmagnumalive++;
	}
    }

    make_pmag();

    remove_waiting();
    sprintf(text,"\n%d Vectors processed\n\n%d Vectors on current plates\n",
		pmagnum,pmagnumalive);
    print_message(text);
    fclose(datafile);
}


make_pmag()
{
  int i,j;
  float factor,angle;

    for(i=0;i<pmagnum;i++) {
	factor = pmag_list[i].uncert*0.017455064/2.0;
	if (factor==0) factor = 1.0*0.017455064/2.0;
   	glNewList(i+PMAG_START,GL_COMPILE);
    
	    glRotatef(pmag_list[i].lon,0.0,1.0,0.0);
	    glRotatef(0.0-pmag_list[i].lat,1.0,0.0,0.0);
    	    glBegin(GL_LINE_STRIP);
			glVertex3f(0.0, factor, 1.0);
			glVertex3f(0.0, 0.0-factor, 1.0);
	    glEnd();
	    glBegin(GL_LINE_STRIP);
			glVertex3f(factor, 0.0, 1.0);
			glVertex3f(0.0-factor, 0.0, 1.0 );
	    glEnd();
	    glBegin(GL_LINE_LOOP);
	        for (j=0;j<100;j++) {
			angle = 2*3.1415926535897*j/100;
			glVertex3f(factor*cos(angle),factor*sin(angle),1.0);
		}
	    glEnd();
	    glRotatef(pmag_list[i].lat,1.0,0.0,0.0);
	    glRotatef(0.0-pmag_list[i].lon,0.0,1.0,0.0);

    	glEndList();
    }
}
		


