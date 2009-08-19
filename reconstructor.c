#include <stdio.h>
#include <math.h>
#include "pex.h"
#include "paleotypes.h"
#include "paleodefs.h"
#include "paleoexterns.h"

# define MAXPLATESINMODEL 500
# define POSSIBLEPLATESINMODEL 1001
# define ARE 57.29577951308
# define NINETYDIVR 1.570796326795

extern char *geotime();
extern int lithnum;
extern make_background();

static ROTATION	initial_table [MAXPLATESINMODEL];
static ROTATION	zeroed_table [MAXPLATESINMODEL];
static int  	index_number [POSSIBLEPLATESINMODEL];
static int 	numplates;
char     *save_file;
float    start_time, end_time, del_time;
int      AutoDel;
static int      Setup = FALSE;
int      Bell = FALSE;
float  PaleoTime;
int    RefPlate;
int    Animate = FALSE;

step_recon(step)
int step;
{
    if (!Setup) return(-1);

    if (!step) {
	Bell = FALSE;
        for (PaleoTime = start_time; PaleoTime <= end_time; PaleoTime += del_time)
            do_recon();
	Bell = TRUE;
        return(0);
    }

    PaleoTime += (float)step * del_time;

    if (PaleoTime > end_time) {
	PaleoTime = end_time;
        // print_warning("End Time Exceeded!! Animation Complete\n");
	return(-1);
    } else if (PaleoTime < start_time) {
	PaleoTime = start_time;
        // print_warning("Attempted to decrement past Start Time!!");
	return(-1);
    }

    do_recon();
}

reconstruction(filename)
char *filename;
{
    char input[80];

    save_file = filename;

    if (Animate) {
        sprintf(input,"Please enter the starting time for the animation(0): ");
         prompt_message(input);
         if (!(strcmp(input,"CANCEL"))) return(0);
         sscanf(input, "%f", &start_time);
         
	 			 PaleoTime = start_time;
	 			 
	 			 
	 			 
        sprintf(input,"Please enter the ending time for the animation (100): ");
         prompt_message(input);
         if (!(strcmp(input,"CANCEL"))) return(0);
         
         
         if (*input == NULL) {
           end_time = 100;
         } else {
           sscanf(input, "%f", &end_time);
         }
         
         
         
         
        sprintf(input,"Please enter the delta time for the animation (10): ");
         prompt_message(input);
         if (!(strcmp(input,"CANCEL"))) return(0);
         
         if (*input == NULL) {
           del_time = 10;
         } else {
           sscanf(input, "%f", &del_time);
         }
	Setup = TRUE;
    } else {
        sprintf(input,"Please enter the time for reconstruction: ");
        prompt_message(input);
        if (!(strcmp(input,"CANCEL"))) return(0);
        
        sscanf(input, "%f", &PaleoTime);
    }

    sprintf(input, "Enter the ID of the fixed plate (000)");
    prompt_message(input);
    if (!(strcmp(input,"CANCEL"))) return(0);
    RefPlate = atoi(input);

    sprintf(input,"Autodelete non-existent data? (Yes)");
    prompt_message(input);
    if (!(strcmp(input,"CANCEL"))) return(0);
    AutoDel = (!(*input == 'n' || *input == 'N')) ? TRUE : FALSE;

    do_recon();
}

do_recon()
{
    bzero(index_number, sizeof(int) * POSSIBLEPLATESINMODEL);
    bzero(initial_table, sizeof(ROTATION) * MAXPLATESINMODEL);
    bzero(zeroed_table, sizeof(ROTATION) * MAXPLATESINMODEL);
    numplates = 0;

    if (make_initial_table(save_file, PaleoTime)) {
        print_warning(" Error in calculation of initial table!\n");
        return(-1);
    }
    if (reduce_to_zero (0)) {
        print_warning(" Error in calculation of RTZ!\n");
	return(-1);
    }

    if (AutoDel) delete_by_time(PaleoTime,PaleoTime + 1.0, TRUE);
    ptime(geotime(PaleoTime));
    solverot(RefPlate);


    if (lithnum) make_background();
    if (Bell) {
        XBell(dpy, -5);
        XFlush(dpy);
    }
}

int make_initial_table (infile, time)
char *infile;
float time;

/* 
This function takes a standard rotation model file, reads it, and returns a 
densed packed table, called "initial_table", which contains 5 elements

0 : Lat
1 : Lon
2 : Angle
3 : Relative plate
4 : Fixed plate ID

and represents the pole for all the plates in the database for a particular
"time".  The poles are not reduced to zero, rather they still retain their 
original relative plate.  This table is used to reduce the tree to 000, but 
this routine does not do that!  It also returns the number of plates in the 
model ("numplates"), and produces the "index_number" table, which is a sparse 
array, and points each plate to the right element of the dense pack array.

It is modified extensively after code by C.R. Scotese.
It is copyrighted 1990 by Malcolm I. Ross, all rights reserved.
Modified slightly by Jeremy Archer - 2008
*/
{
    double fn,fo,fp,fq,fr,fs;
    char LongString[120];
    int id1,id2,ref1,ref2;
    int n,i,j,k,status,iplate,ichoice,imov,ifix;
    float t1,rlat1,rlon1,rang1,t2,rlat2,rlon2,rang2;
    float delta,fptemp,rlat,rlon,rang;
    FILE *filepointer;
    ROTATION *ptr;

/*    if (time == 0) return (-1); */

/* Start by opening the data file */
		
		
    if ((filepointer = fopen (infile,"r")) == NULL) {
        fprintf (stdout,"\nFailed to open file %s\n", infile);
        return (1);
    }

    id2 = -1;
    t2 = -1.0;
    rlat2 = -1.0;
    rlon2 = -1.0;
    rang2 = -1.0;
    ref2 = -1;
    ptr = initial_table;

    do {

/* Read the data file line by line until we bracket the time of interest 
   between t1, and t2 */

        do {
	    if (id2 != 999) {
                id1 = id2;
                t1 = t2;
                rlat1 = rlat2;
                rlon1 = rlon2;
                rang1 = rang2;
                ref1 = ref2;
	    }
	    if (!fgets(LongString, 120, filepointer)) {
	    					if (feof(filepointer)) {
	    						printf (" unexpected EOF after record:\n");
	    					} else {
                	printf (" error in read from disk at record:\n");
                }
                printf ("%d %f %f %f %f %d\n",id2,t2,rlat2,rlon2,rang2,ref2);
								fclose(filepointer);
                return(1);
	    }
	    else {
                sscanf (LongString,"%d %f %f %f %f %d", &id2,&t2,&rlat2,&rlon2,&rang2,&ref2);
      }
	} while ((id2 == 999) || (time > t2));

/* Now we determine the pole based on the bounding values, and where the time of
interest falls in that range */

        if (ref1 != ref2) {
            printf ("\nError in reference frame\n");
            return (1);
        }

        delta = (t2-time)/(t2-t1);
        fn = rlat2;
        fo = rlon2;
        fp = rang2;
        fptemp = -1.0 * fp;     /* this subtracts the two poles instead of adding */
        fq = rlat1;
        fr = rlon1;
        fs = rang1;
        adder (fn,fo,fptemp,fq,fr,fs,&rlat,&rlon,&rang);
        rang = rang*delta;
        fn = rlat2;
        fo = rlon2;
        fp = rang2;
        fq = rlat;
        fr = rlon;
        fs = rang;

        adder (fn,fo,fp,fq,fr,fs,&rlat,&rlon,&rang);

/* And store the pole in the array.  NOTE: It is not yet reduced to pole (000)  */

        ptr->latitude = rlat;
        ptr->longitude = rlon;
        ptr->angle = rang;
        ptr->reference_plate = ref1;
        ptr->fixed_plate_id = id1;

        index_number[id1] = numplates;
        numplates++;
        ptr++;

/* Loop until we find a new plate number to work on, or we get the end of file */

        do {
	    if (!fgets(LongString, 120, filepointer)) {
                fclose (filepointer);
		return(0);
	    }
            sscanf(LongString,"%d\t%f\t%f\t%f\t%f\t%d",&id2,&t2,&rlat2,&rlon2,&rang2,&ref2);
        } while ((id2 == id1) || (id2 == 999));
        
    } while (1);
    return(0);
}


int reduce_to_zero (topnode)
int topnode;

/* 
This function takes the "initial_table", reads it, and returns a densed packed
table, called "zeroed_table", which contains 4 elements
0 : Lat
1 : Lon
2 : Angle
3 : Fixed plate ID
and represents the pole for all the plates in the database for a particular "time".
The poles are reduced to 000.  "topnode" is usually 000 when you start, but
can be anything.  If 'topnode" is not 000, then this function checks to see if the plate
that is presently being worked on has "topnode" somewhere above it in the hierarchy.
The utility of this is that you don't have to re-solve the whole
hierarchy when you have moved only one plate.  you can therefore solve for all the
plates below the moved plate, and save calculation time.

It is modified extensively after code by C.R. Scotese.
It is copyrighted 1990 by Malcolm I. Ross, all rights reserved.
*/
{
    double fn,fo,fp,fq,fr,fs;
    int n,i,RefPlate,k,status,ichoice,imov,ifix, counter;
    float t1,rlat1,rlon1,rang1,t2,rlat2,rlon2,rang2,rlat,rlon,rang;
    unsigned char topnode_above_me();

/* Now if "topnode" is 000, then for every plate in the hierarchy, solve to 000.
If topnode is not 000, then the initial solution has been done, and we need to
resolve the hierarchy only if the plate of interest has "topnode" (ie the plate
we moved) above it in the hierarchy.  If it doesn't, then the rotation is still valid. */

    counter = 0;
    for (counter = 0; counter < numplates; counter++) {
        if (topnode != 0) {
            if ((!topnode_above_me(initial_table[counter].fixed_plate_id, topnode))) 
		continue;
        }
        fn = initial_table[counter].latitude;
        fo = initial_table[counter].longitude;
        fp = initial_table[counter].angle;
        RefPlate = initial_table[counter].reference_plate;

/* if the RefPlate is equal to 000, then we are reduced to pole, so go no further */
/* Otherwise, then add whatever pole is there, and see if that pole points to 000 */

	while (RefPlate != 0) {
            k = index_number[RefPlate];
            fq = initial_table[k].latitude;
            fr = initial_table[k].longitude;
            fs = initial_table[k].angle;
            adder (fn,fo,fp,fq,fr,fs,&rlat,&rlon,&rang);
            fn = rlat;
            fo = rlon;
            fp = rang;
            RefPlate = initial_table[k].reference_plate;
	}

/* if the RefPlate is equal to 000, then we are done, so assign values to zeroed_table */

        zeroed_table[counter].latitude = fn;
        zeroed_table[counter].longitude = fo;
        zeroed_table[counter].angle = fp;
        zeroed_table[counter].fixed_plate_id = initial_table[counter].fixed_plate_id;
    }
    return(0);
}

unsigned char topnode_above_me(node,topnode)
int node;
int topnode;

/* 
This function checks the "initial_table" to see if the plate called "topnode"
appears in the hierarchy above the plate called "node".

It is copyrighted 1990 by Malcolm I. Ross, all rights reserved.
*/

{
    int i, j, index2next, RefPlate;


    if (node == topnode) return (TRUE);

    index2next = index_number [node];
    RefPlate = initial_table[index2next].reference_plate;

    do {
        if (RefPlate == topnode) return (TRUE);
        if (RefPlate == 000) return (FALSE);
        index2next = index_number [RefPlate];
        RefPlate = initial_table[index2next].reference_plate;
    } while(1);
}

solverot(ifix)
int ifix;

/* 
This function takes the "zeroed_table", reads it, and updates the 
display based on the fixed plate.
It can display them as reduced to 000, or relative to any other plate.

It is modified extensively after code by C.R. Scotese.
It is copyrighted 1990 by Malcolm I. Ross, all rights reserved.
*/

{
    int i, j;
    double fn,fo,fp,fq,fr,fs;
    float rlat,rlon,rang;
    PLATE *ptr;

    for (i = 0; i < numplates; i++) {
	if (ifix == 0) {
            rlat = zeroed_table[i].latitude;
            rlon = zeroed_table[i].longitude;
            rang = zeroed_table[i].angle;
        }
	else if (ifix == zeroed_table[i].fixed_plate_id) {
		rlat = rlon = rang = 0.0;
	}
	else {
            j = index_number[ifix];
            fn = zeroed_table[i].latitude;
            fo = zeroed_table[i].longitude;
            fp = zeroed_table[i].angle;
            fq = zeroed_table[j].latitude;
            fr = zeroed_table[j].longitude;
            fs = zeroed_table[j].angle;
            fs = -fs;
            adder (fn,fo,fp,fq,fr,fs,&rlat,&rlon,&rang);
	}
	ptr = plate_list[zeroed_table[i].fixed_plate_id];
	if (ptr != NULL) {
/*printf("A (%d) fpid %d lat %.3f  lon %.3f  ang %.3f \n",i,zeroed_table[i].fixed_plate_id,
zeroed_table[i].latitude,zeroed_table[i].longitude,zeroed_table[i].angle);
*/	    ptr->latitude[0] = rlat;
	    ptr->longitude[0] = rlon;
	    ptr->angle[0] = rang;
/*printf("B (%d) fpid %d lat %.3f  lon %.3f  ang %.3f \n",i,zeroed_table[i].fixed_plate_id,
zeroed_table[i].latitude,zeroed_table[i].longitude,zeroed_table[i].angle);
*/	    quick_send_rot(zeroed_table[i].fixed_plate_id);
	}
    }
}

adder(a, b, c, d, e, f, rlat, rlon, rang)
double a,b,c,d,e,f;
float *rlat,*rlon,*rang;
{
    double fa, fb, fc, fd, fe, ff, fg, fh, fi, fj, fk, fl;
    double fm, fn, fo, fp, fq, fr, fs, ft, fu, fv, fw, fx;

/* Some optimizing "If's".  This is the most-used routine of the bunch */

    if (c == (- f)) {
        *rlat = 90.0;
        *rlon = 0.0;
        *rang = 0.0;
        return;
    }
        
    if (c == 0.0) {
        *rlat = d;
        *rlon = e;
        *rang = f;
        return;
    }
        
    if (f == 0.0) {
        *rlat = a;
        *rlon = b;
        *rang = c;
        return;
    }

/* Well, we got to do it! */

    fn = a / ARE;
    fo = b / ARE;
    fp = c / ARE;
    fq = d / ARE;
    fr = e / ARE;
    fs = f / ARE;

    fa = cos(fp / 2.0);  
    fb = (sin(fp / 2.0) * sin(NINETYDIVR - fn)) * cos(fo);
    fc = (sin(fp / 2.0) * sin(NINETYDIVR - fn)) * sin(fo);
    fd = sin(fp / 2.0) * cos(NINETYDIVR - fn);
    fe = cos(fs / 2.0);
    ff = (sin(fs / 2.0) * sin(NINETYDIVR - fq)) * cos(fr);
    fg = (sin(fs / 2.0) * sin(NINETYDIVR - fq)) * sin(fr);
    fh = sin(fs / 2.0) * cos(NINETYDIVR - fq);

    fi = (((fa * fe) - (fb * ff)) - (fc * fg)) - (fd * fh);
    fj = (((fa * ff) + (fb * fe)) - (fc * fh)) + (fd * fg);
    fk = (((fa * fg) + (fb * fh)) + (fc * fe)) - (fd * ff);
    fl = (((fa * fh) - (fb * fg)) + (fc * ff)) + (fd * fe);
    fm = (acos(fi) * 2.0) * ARE;

    if (fm == 0.0) {
        *rlat = 90.0;
        *rlon = 0.0;
        *rang = 0.0;
        return;
    }

    if (fm > 180.0) fm = fm - 360.0;
    fv = fl / sin(fm / (2 * ARE));
    ft = 90.0 - (acos(fv) * ARE);
    if (fm < 0.0) ft = - ft;

    if (fk == 0.0) {
        *rlat = 90.0;
        *rlon = 0.0;
        *rang = 0.0;
        return;
    }
        
    fu = atan2(fk,fj) * ARE;
    if (fu > 180.0) fu = fu - 360.0;
    *rlat = ft;
    *rlon = fu;
    *rang = fm;
}
