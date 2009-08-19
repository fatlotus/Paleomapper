#include <stdio.h>
#include <string.h>
#include "paleotypes.h"
#include "paleodefs.h"
#include "paleoexterns.h"

/**************************************************************************/
/* This subroutine initializes the arrays for searching and               */
/* returning a geologic age name, based on the time in millions of years. */
/*                                                                        */
/* The geologic age names and thier absolutetime boundaries are read in   */
/* from the file = "FileName".  The expected format is:                   */
/*                                                                        */
/* line 1: baseage in Ma., topage in Ma.                                  */
/* line 2: Geologic Age Name                                              */
/*                                                                        */
/* for example:                                                           */
/*                                                                        */
/* 144, 66.4                                                              */
/* Cretaceous                                                             */
/*                                                                        */
/* and the geologoloc name ("Cretaceous" in this case) will be displayed  */
/* when the age in Ma is greater than or equal to 144 Ma and Less than    */
/* 66.4 Ma.                                                               */
/*                                                                        */
/**************************************************************************/
/*                                                                        */
/* It was written on June 5, 1989 by Malcolm I. Ross and is               */
/*     COPYRIGHTED by Earth In Motion Technologies from that date.        */
/*                                                                        */
/**************************************************************************/
/*                                                                        */
/* Include the relavent declarations and commons                          */
/*                                                                        */
/**************************************************************************/

extern char  *getenv();
static float baseage[500],topage[500];
static char  agename[500][50];
static int   numagetimes;
extern recon_label();

ageinit()
{
    int i,j;
    FILE *infile;
    char *directory;
    char filename[] = "agedata";


    if ((infile = fopen(filename,"r")) == NULL) {
        if ((infile = fopen(AGEFILE,"r")) == NULL) {
            printf("Error opening agedata file: %s!\n",AGEFILE);
            exit(1);
	}
    }

    i = 0;
/*    while (fscanf(infile,"%f,%f\n",&baseage[i],&topage[i]) != EOF) {
	fgets(agename[i], 50, infile);
	printf("\n:%s:%f:%f:\n",agename[i],baseage[i],topage[i]);	
	j=0;
	printf(":%c:\n",agename[i][j]);		
	while ((agename[i][j] != '\n') && (j < 50)) j++;
	printf(":%d:%d:%s:",i,j,agename[i]);	
	agename[i][j] = '\0';	
        i++;
    }				*/

	for (i=0;i<34;i++) 
	{
		fscanf(infile,"%f,%f\n",&baseage[i],&topage[i]);
		fgets(agename[i], 50, infile);
		j=0;
		while ((agename[i][j] != '\n') && (j < 50)) j++;
		agename[i][j] = '\0';	
	/*	printf("inside %d\n",i);	*/
	}
	
    numagetimes = i;
}

/**************************************************************************/
/* This returns a geologic age name, based on the time in millions of     */
/* years.  The data has been initialized by SUBROUTINE AGEINIT.           */
/*                                                                        */
/* The geologic age names and thier absolute time boundaries are read in  */
/* from a user specified file.                                            */
/*                                                                        */
/* for example:                                                           */
/*                                                                        */
/* 144, 66.4                                                              */
/* Cretaceous                                                             */
/*                                                                        */
/* and the geologoloc name ("Cretaceous" in this case) will be displayed  */
/* when the age in Ma is less than or equal to 144 Ma and greater than    */
/* 66.4 Ma.                                                               */
/*                                                                        */
/**************************************************************************/
/*                                                                        */
/* It was written on June 5, 1989 by Malcolm I. Ross and is               */
/*     COPYRIGHTED by Earth In Motion Technologies from that date.        */
/*                                                                        */
/**************************************************************************/

char *geotime(dtime)
double dtime;
{
    float time;
    int   i;
    
    time = (dtime == 0.0) ? 0.000001 : (float) dtime;

    for(i=0; i<numagetimes; i++) {
	if ((time <= baseage[i]) && (time > topage[i]))
	    return(agename[i]);
    }
    return(NULL);
}

ptime(name)
char *name;
{

    recon_label(name);

}
