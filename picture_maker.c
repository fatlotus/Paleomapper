#include <stdio.h>
#include <string.h>
#include <math.h>
#include "paleotypes.h"
#include "paleodefs.h"
#include "paleoexterns.h"

#define MAX_VECS 20000
#define MAX_SETS 20000
#define MAX_HOLD 180
#define RES .01

Ppoint3 vectors[MAX_VECS];
int    poslin[MAX_VECS];
int	vecsetnum = 0;
/*int 	vecsette[MAX_SETS];*/
int	alive[MAX_SETS];
int 	all_vectors=0;
float maplat[100000],maplon[100000];
int	mappen[100000];
int	mapsetfinder[MAX_SETS];

extern int plate_alive[900];
extern int polygons;

extern int mapproj;
extern int lowres;

int picture_maker(filename)
char *filename;
{
    char text[128];
    char buf[128];
    char ident_type[3],coast_type[3];
    char cont_str[4];
    char spid[80];
    FILE *datafile;
    int nov, total_vectors;
    int pen, dt;
    int te1, cont, idnumber, ecolor;
    float appears, dappears;
    float alat, alon;
    double x, y, z, phi, theta;
    int append, new_cont;
    char tempchar;
    int tempint;
    char ltype[6];
    float deltax,deltay,deltaz;
    char *filetoken,*token;
    int gottoken=0;

/*******************************************************************/
/* SET UP INITIAL VARIABLES */
/* NUMBER OF VECTORS IN THE VECTORS ARRAY      */
/* NUMBER OF VECTORS READ IN A STRING (?)      */
/* TOTAL NUMBER OF VECTORS PROCESSED FROM ONE  */
/*******************************************************************/
    nov = 0;
    total_vectors = 0;
    mapsetfinder[0]=0;
    strcpy(coast_type,"CS");

    if ((datafile = fopen(filename,"r")) == NULL) {
        sprintf(text,"Error opening %s\n",filename);
	print_warning(text);
	return(0); }

/*******************************************************************/
/* We must read a header first to get the tectonic group number, so we can */
/* compare it to other headers read in.*/
/* */
/* This is the header read.*/
/*******************************************************************/
    sprintf(text,"Reading %s ...",filename);
    display_waiting(text);
/*    while (fgets(text,127,datafile) != NULL) {;
        vecsetnum++;
	text[80] = '\0';
        fscanf(datafile,"%d%f%f%2s%d%d%d%*c",&te1,&appears,&dappears,
               ident_type,&idnumber,&cont,&ecolor);  
        fgets(spid,79,datafile);
	spid[30] = '\0';				*/

    while (fgets(text,127,datafile) != NULL) {;
        vecsetnum++;
/*        printf("Vec Set Number: %d \n",vecsetnum); */
        sscanf(text,"%3s%c%d%d%d%f%f%2s%5s%d%*c",cont_str,&tempchar,&idnumber,
               &te1,&tempint,&appears,&dappears,ident_type,ltype,&ecolor);  
/*	drawcolor[vecsetnum]=ecolor;*/
/*	vecsette[vecsetnum]=te1; */
	alive[vecsetnum]=TRUE;
	sprintf(spid,"\0");
	sscanf(cont_str,"%d",&cont);

/*******************************************************************/
/* First, we need to check to see if the tectonic element number,
   the data type and the data type number, have been used before. If they
   have, then it must be "APPENDED" to the already used structure, instead
   of given a new tectonic group and subpicture. This comparison is done by
   subroutine COMPARE.
 
   next we need to assign the header variables to the right slots in the arrays,
   based on the comparison we just did. We don't do this
   if we are doing an include, because it has already been done.
*/
/*******************************************************************/
/*        append = compare(cont, appears, dappears, ident_type, idnumber,
			 &new_cont);	*/
        append = compare(te1, appears, dappears, ident_type, idnumber,
			 &new_cont);
if (append) printf("Appending %d \n",vecsetnum);
if (new_cont) printf("NEW CONT  ");
if (new_cont) {
    }
        if (!append) {
            if (head_elem == NULL) {
	        head_elem = (ELEMENT *)malloc(sizeof(ELEMENT));
		tail_elem = head_elem;
            }
	    else {
		tail_elem->next = (ELEMENT *)malloc(sizeof(ELEMENT));
		tail_elem = tail_elem->next;
	    }
	    cur_elem = tail_elem;
            cur_elem->next = NULL;
/*            cur_elem->phigs_struct = Next_Phigs_Struct++; */
	    cur_elem->phigs_struct = vecsetnum;
            cur_elem->tectonic_element = te1;
            cur_elem->old_time = appears;
            cur_elem->young_time = dappears;
            cur_elem->id_type_num = idnumber;
            cur_elem->continent = cont;
            cur_elem->color = ecolor;
	    strncpy(cur_elem->ltype,ltype,6);
            strncpy(cur_elem->id_type,ident_type,3);
            strncpy(cur_elem->subp_id,spid,30);
/*            strncpy(cur_elem->header,text,81);*/
	    if (!gottoken) {
	    //	sprintf(filetoken,"No Name");
	    	token = strtok(filename,"/");
	    	while (token != NULL) {
		    filetoken = token;
	            token = strtok(NULL,"/"); }
		gottoken=TRUE;
	    }
	    sprintf(cur_elem->header,filetoken);
        }

        do {
	    fgets(buf,80,datafile);
	    sscanf(buf,"%f%f%d",&alat,&alon,&pen);

/*******************************************************************/
/* first see if what is read in is the end of string marker.
   if it is, read the next header. if not, convert to cartesian coord.
*/
/*******************************************************************/
            if (alat > 90.0) break;

	    maplat[all_vectors] = alat;
	    maplon[all_vectors] = alon;
            phi = 0.017453292 * (90.0 - alat);
            theta = 0.017453292 * alon;
            x = cos(theta) * sin(phi);
            y = sin(theta) * sin(phi);
            z = cos(phi);

            if (nov == MAX_VECS) {
        	sprintf(text,"Truncating Element!!!!\n");
		print_warning(text);
        	sprintf(text,"Maximum number of %d vectors exceeded in element:\n",
                        MAX_VECS);
		print_warning(text);
        	sprintf(text,"Please split element apart!\n");
		print_warning(text);

                do {
                    fgets(buf,80,datafile);
                    sscanf(buf,"%f",&alat);
                } while (alat <= 90.0);
                break;
            }

            vectors[nov].x = (float)y;
            vectors[nov].y = (float)z;
            vectors[nov].z = (float)x;


            poslin[nov] = (pen == 2) ? TRUE : FALSE;
		if (polygons) poslin[nov]+=10; 
/*printf("pt %d pl %d \n",polygons,poslin[nov]);*/
	    mappen[all_vectors] = poslin[nov];
	    if (lowres) {
		if (ident_type[0]==coast_type[0] && ident_type[1]==coast_type[1]) {
		    if (pen == 2) {
			deltax = vectors[nov].x-vectors[nov-1].x;
			deltay = vectors[nov].y-vectors[nov-1].y;
			deltaz = vectors[nov].z-vectors[nov-1].z;
			if (deltax<0.0) deltax = 0.0 - deltax;
			if (deltay<0.0) deltay = 0.0 - deltay;
			if (deltaz<0.0) deltaz = 0.0 - deltaz;
/*printf("dx %.3f \n",deltax);*/
			if (deltax<RES && deltay<RES && deltaz<RES) {
				nov--;
				total_vectors--;
				all_vectors--;
			}
		    }
		}
	    }
            nov++;
            total_vectors++;
	    all_vectors++;
	} while (1);   /*  SEE BREAK WHEN alat > 90  */

	poslin[0] = FALSE;  /* Force 'Move To' for first data point */
		if (polygons) poslin[0]+=10; 
/*printf("pt %d pl %d \n",polygons,poslin[0]);*/

if (new_cont) printf("%d\n",cur_elem->tectonic_element);
	plate_alive[(cur_elem->tectonic_element)-100]=TRUE;
/*	if (cur_elem->tectonic_element==301) for (dt=2;dt<4;dt++) {
	    plate_alive[300+dt-100]=TRUE;
	} 
*/	mapsetfinder[vecsetnum]=all_vectors;

        send_vector(nov, vectors, poslin, append, new_cont);
/*	else send_map(nov,maplat,maplon,poslin,append,new_cont); */
/*	printf("nov: %d \n", nov); */
        nov = 0;
    }


    remove_waiting();
    sprintf(text,"\n%d Vectors processed\n",total_vectors);
    print_message(text);
    fclose(datafile);
    draw();
}

gen_maker(filename)
char *filename;
{
    char text[128], buf[128], ident_type[3], coast_type[3];
    char cont_str[4], spid[80];
    FILE *datafile;
    int nov, total_vectors, pen, te1, cont, idnumber, ecolor, symbol;
    float appears, dappears, alat, alon;
    double x, y, z, phi, theta;
    int append, new_cont, tempint;
    char tempchar, ltype[6];
    float deltax, deltay, deltaz;

    nov = 0;
    total_vectors = 0;
    mapsetfinder[0]=0;
    strcpy(coast_type,"CS");

    if ((datafile = fopen(filename,"r")) == NULL) {
        sprintf(text,"Error opening %s\n",filename);
	print_warning(text);
	return(0); }

    sprintf(text,"Reading %s ...",filename);
    display_waiting(text);

    while (fgets(text,127,datafile) != NULL) {;
        vecsetnum++;
/*        printf("Vec Set Number: %d \n",vecsetnum); */
/*        sscanf(text,"%3s%c%d%d%d%f%f%2s%5s%d%*c",
		cont_str,&tempchar,&idnumber,&te1,&tempint,
		&appears,&dappears,ident_type,ltype,&ecolor); */
	ecolor=3;symbol=0;
        sscanf(text,"%f%f%d%d%d",
		&alat,&alon,&te1,&ecolor,&symbol);  
printf("ecolor %d  symbol %d\n",ecolor,symbol);
	appears=1000;dappears=0;idnumber=0;pen=1;cont=0;
	alive[vecsetnum]=TRUE;
	sprintf(spid,"\0");
/*	sscanf(cont_str,"%d",&cont);*/

        append = compare(te1, appears, dappears, ident_type, idnumber,
			 &new_cont);

	if (append) printf("Appending %d \n",vecsetnum);
	if (new_cont) printf("NEW CONT  ");
	if (new_cont) {
	    }
        if (!append) {
            if (head_elem == NULL) {
	        head_elem = (ELEMENT *)malloc(sizeof(ELEMENT));
		tail_elem = head_elem;
            }
	    else {
		tail_elem->next = (ELEMENT *)malloc(sizeof(ELEMENT));
		tail_elem = tail_elem->next;
	    }
	    cur_elem = tail_elem;
            cur_elem->next = NULL;
	    cur_elem->phigs_struct = vecsetnum;
            cur_elem->tectonic_element = te1;
            cur_elem->old_time = appears;
            cur_elem->young_time = dappears;
            cur_elem->id_type_num = idnumber;
            cur_elem->continent = cont;
            cur_elem->color = ecolor;
	    strncpy(cur_elem->ltype,ltype,6);
            strncpy(cur_elem->id_type,ident_type,3);
            strncpy(cur_elem->subp_id,spid,30);
	    sprintf(cur_elem->header,"No Name");
        }

        do {
/*	    fgets(buf,80,datafile);
	    sscanf(buf,"%f%f%d",&alat,&alon,&pen);

            if (alat > 90.0) break;
*/
	    if (!symbol) switch(nov) {
		case 0: alat+=.25; alon+=.25; break;
		case 1: alat-=0.5; break;
		case 2: alon-=0.5; break;
		case 3: alat+=0.5; break;
		case 4: alon+=0.5; break;
		case 5: alat-=0.5; alon-=0.5; break;
		case 6: alon+=0.5; break;
		case 7: alat+=0.5; alon-=0.5; break;
	    }
	      else if (nov==0) pen=3;
/*	      else break;  */

	    maplat[all_vectors] = alat;
	    maplon[all_vectors] = alon;
            phi = 0.017453292 * (90.0 - alat);
            theta = 0.017453292 * alon;
            x = cos(theta) * sin(phi);
            y = sin(theta) * sin(phi);
            z = cos(phi);

            if (nov == MAX_VECS) {
       		sprintf(text,"Truncating Element!!!!\n");
		print_warning(text);
        	sprintf(text,"Maximum number of %d vectors exceeded in element:\n",
                        MAX_VECS);
		print_warning(text);
        	sprintf(text,"Please split element apart!\n");
		print_warning(text);

                do {
                    fgets(buf,80,datafile);
                    sscanf(buf,"%f",&alat);
                } while (alat <= 90.0);
                break;
            }

            vectors[nov].x = (float)y;
            vectors[nov].y = (float)z;
            vectors[nov].z = (float)x;

            poslin[nov] = pen;
	    mappen[all_vectors] = poslin[nov];
	    if (lowres) {
		if (ident_type[0]==coast_type[0] && ident_type[1]==coast_type[1]) {
		    if (pen == 2) {
			deltax = vectors[nov].x-vectors[nov-1].x;
			deltay = vectors[nov].y-vectors[nov-1].y;
			deltaz = vectors[nov].z-vectors[nov-1].z;
			if (deltax<0.0) deltax = 0.0 - deltax;
			if (deltay<0.0) deltay = 0.0 - deltay;
			if (deltaz<0.0) deltaz = 0.0 - deltaz;
			if (deltax<RES && deltay<RES && deltaz<RES) {
				nov--;
				total_vectors--;
				all_vectors--;
			}
		    }
		}
	    }
            nov++;
            total_vectors++;
	    all_vectors++;
	} while (nov<8);   /*  SEE BREAK WHEN alat > 90  */

/*	poslin[0] = FALSE; */  /* Force 'Move To' for first data point */

	if (new_cont) printf("%d\n",cur_elem->tectonic_element);
	plate_alive[(cur_elem->tectonic_element)-100]=TRUE;
	mapsetfinder[vecsetnum]=all_vectors;
        send_vector(nov, vectors, poslin, append, new_cont);
        nov = 0;
    }
    remove_waiting();
    sprintf(text,"\n%d Vectors processed\n",total_vectors);
    print_message(text);
    fclose(datafile);
    draw();
}
