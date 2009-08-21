/************************************************************
Compile with:  cc -lm -o dattopoly dattopoly.c

Program to convert PGAP format Paleomapper data files
to PGAP format Paleomapper polygon files.

************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

FILE *infile, *outfile;

main( argc, argv )
    int     argc;
    char    *argv[];
{
  float sumlat,sumlon,avglat,avglon;
  int pts,i,pen;
  char text[120],name[80];
  float lat[20000],lon[20000];

    if (argc == 2) {
    	infile = fopen(argv[1],"r");
	strncpy(name,argv[1],80);
	printf("Input File:  %s\n",name);
	for(i=0;i<80;i++) {
	    if (name[i]=='.' && name[i+1]=='d' &&
		name[i+2]=='a' && name[i+3]=='t') 
	    {
		name[i+1]='p';
		name[i+2]='o';
		name[i+3]='l';
		name[i+4]='y';
		break;
	    }
	}
	printf("Output File: %s\n",name);
	outfile = fopen(name,"w");
    }
    else {
	printf("No input filename specified!! \n");
	return(0);
    }

    while (fgets(text,120,infile) != NULL) {;
	fprintf(outfile,"%s",text);
/*        sscanf(text,"%3s%c%d%d%d%f%f%2s%5s%d%*c",cont_str,&tempchar,&idnumber,
               &te1,&tempint,&appears,&dappears,ident_type,ltype,&ecolor);  
*/
	sumlat=0.0; sumlon=0.0; pts=-1;
        do {
	    pts++;
	    fgets(text,120,infile);
	    sscanf(text,"%f%f%d",&lat[pts],&lon[pts],&pen);
	    if (lat[pts]>90.0) break;
	    sumlat+=lat[pts]; sumlon+=lon[pts];
	} while (lat[pts]<90.0);
	avglat=sumlat/pts; avglon=sumlon/pts;

	if (lat[0] != lat[pts-1] || lon[0] != lon[pts-1]) 
	{
/*printf("lat[0] %.2f  lat[pts-1] %.2f \n",lat[0],lat[pts-1]);*/
	    printf("Data Loop Not Completed!\n");
	    printf("Completing Loop...");
	    lat[pts+1]=lat[pts]; lat[pts]=lat[0];
	    lon[pts+1]=lon[pts]; lon[pts]=lon[0];
	    pts++;
	    printf("done!\n");
	}
	
	for(i=0;i<pts-1;i++) {
	    fprintf(outfile,"  %.2f  %.2f  3\n",lat[i],lon[i]);
	    fprintf(outfile,"  %.2f  %.2f  2\n",lat[i+1],lon[i+1]);
	    fprintf(outfile,"  %.2f  %.2f  2\n",avglat,avglon);
	}
	fprintf(outfile,"  99.00   0.00  3\n");
    }

    fclose(infile);
    fclose(outfile);
}

