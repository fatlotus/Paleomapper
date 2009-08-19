#include <stdio.h>
#include <Xm/Xm.h>
#include <math.h>
#include "paleotypes.h"
#include "paleodefs.h"
#include "paleoexterns.h"

extern float PaleoTime;

extern float pmagpolelat,pmagpolelon;

extern int nopv,nop;
extern float kappa,alpha;

extern int move_cont;
extern Widget prompt;

extern int which_pole;
extern int winx,winy,winwd,winht;
static char dumpfilename[80];
static int dumpnum;

print_info(id)
int id;
{
    ELEMENT      *ptr;
    char         string[300];
    int 	np,ndp;
    char 	my_str[2];

  printf("id %d \n",id);

  if (id>9999) {
    ptr = head_elem;
    while (ptr != NULL) {
        if (ptr->phigs_struct == id-10000) {
	    if (ptr->continent<100) 
		if (ptr->continent<10) 
		    strcpy(my_str,"XX");
		else strcpy(my_str,"X\0");
	    else strcpy(my_str,"\0");
            sprintf(string,"GENERAL DATA\nFile: %s\n"
		"Tec Elem %03d   ID# %4d\n"
		"Old %6.1f   Young %6.1f\n"
		"ID type %s   %s\n"
		"Cont %d%s\nColor %d %s",
                    ptr->header,
                    ptr->tectonic_element,ptr->id_type_num,
		    ptr->old_time,ptr->young_time,
                    ptr->id_type,ptr->ltype,
                    ptr->continent,my_str,
                    ptr->color,ptr->subp_id);
            print_message(string);
            break;
        }
        ptr = ptr->next;
    }
  }
  if (id>2999 && id<5000) {
	sprintf(string,"PMAG Data\nNumber %03d\nPlate ID %03d\n"
		"Young %6.1f\nOld %6.1f\nLatitude %6.1f\n"
		"Longitude %6.1f\nUncertainty %6.1f\n",
		pmag_list[id-3000].unique,
		pmag_list[id-3000].plate,
		pmag_list[id-3000].young,
		pmag_list[id-3000].old,
		pmag_list[id-3000].lat,
		pmag_list[id-3000].lon,
		pmag_list[id-3000].uncert);
            print_message(string);

  }
  if (id>4999 && id<10000) {
	sprintf(string,"Lithofacies Data\nCountry: %s\n"
		"Unique # %6d  Display # %6d\n"
		"Plate ID %6d  Age       %6d\n"
		"Latitude %4.1f  Longitude %4.1f\n"
		"Lith Type: %s  Lith Code: %d\n"
		"Environment: Begin %d  End %d\n"
		"Stratagraphic Unit: %s\nReliability %c\n",
		lith_list[id-LITH_START].country,
		lith_list[id-LITH_START].unique,
		lith_list[id-LITH_START].number,
		lith_list[id-LITH_START].plate,
		lith_list[id-LITH_START].age,
		lith_list[id-LITH_START].lat,
		lith_list[id-LITH_START].lon,
		lith_list[id-LITH_START].type,
		lith_list[id-LITH_START].typecode,
		lith_list[id-LITH_START].envbegin,
		lith_list[id-LITH_START].envend,
		lith_list[id-LITH_START].stratunit,
	  (char)lith_list[id-LITH_START].reliable);
            print_message(string);

  }
  if (id == PMAGPOLE) {
	np=0;ndp=0;
	sprintf(string,"PMAG Fisher Mean Pole\nLatitude %6.1f\n"
		"Longitude %6.1f\nKappa %6.1f\nAlpha %6.1f\n"
		"Number of Plates %3d\nNumber of Data Points%3d\n",
		pmagpolelat,pmagpolelon,kappa,alpha,nop,nopv);
            print_message(string);
  }	
}

change_info(id)
int id;
{
    ELEMENT      *ptr;
    char         string[300];
    int 	np,ndp;
    char 	my_str[2];

  printf("id %d \n",id);

  if (id>9999) {
    ptr = head_elem;
    while (ptr != NULL) {
        if (ptr->phigs_struct == id-10000) {
            ptr->color=ptr->color+1;
	    if (ptr->color > 7) ptr->color=0;
	    cur_elem = ptr;
	    printf("Color %d \n",ptr->color);
	    draw();
            break;
        }
        ptr = ptr->next;
    }
  }
  if (id>2999 && id<5000) {
	sprintf(string,"PMAG Data\nNumber %03d\nPlate ID %03d\n"
		"Young %6.1f\nOld %6.1f\nLatitude %6.1f\n"
		"Longitude %6.1f\nUncertainty %6.1f\n",
		pmag_list[id-3000].unique,
		pmag_list[id-3000].plate,
		pmag_list[id-3000].young,
		pmag_list[id-3000].old,
		pmag_list[id-3000].lat,
		pmag_list[id-3000].lon,
		pmag_list[id-3000].uncert);
            print_message(string);

  }
  if (id>4999 && id<10000) {
	sprintf(string,"Lithofacies Data\nCountry: %s\n"
		"Unique # %6d  Display # %6d\n"
		"Plate ID %6d  Age       %6d\n"
		"Latitude %4.1f  Longitude %4.1f\n"
		"Lith Type: %s  Lith Code: %d\n"
		"Environment: Begin %d  End %d\n"
		"Stratagraphic Unit: %s\nReliability %c\n",
		lith_list[id-LITH_START].country,
		lith_list[id-LITH_START].unique,
		lith_list[id-LITH_START].number,
		lith_list[id-LITH_START].plate,
		lith_list[id-LITH_START].age,
		lith_list[id-LITH_START].lat,
		lith_list[id-LITH_START].lon,
		lith_list[id-LITH_START].type,
		lith_list[id-LITH_START].typecode,
		lith_list[id-LITH_START].envbegin,
		lith_list[id-LITH_START].envend,
		lith_list[id-LITH_START].stratunit,
	  (char)lith_list[id-LITH_START].reliable);
            print_message(string);

  }
  if (id == PMAGPOLE) {
	np=0;ndp=0;
	sprintf(string,"PMAG Fisher Mean Pole\nLatitude %6.1f\n"
		"Longitude %6.1f\nKappa %6.1f\nAlpha %6.1f\n"
		"Number of Plates %3d\nNumber of Data Points%3d\n",
		pmagpolelat,pmagpolelon,kappa,alpha,nop,nopv);
            print_message(string);
  }	
}

printavgpole(elem,plate)
int elem,plate;
{
    char         string[300];
    PLATE *plat;
    float lat,lon,ang;

    average_pole(move_cont); 

    if (which_pole == 0) {
	lat = plate_list[move_cont]->latitude[0];
	lon = plate_list[move_cont]->longitude[0];
	ang = plate_list[move_cont]->angle[0]; }
    else {
	lat = plate_list[move_cont]->latitude[4+which_pole];
	lon = plate_list[move_cont]->longitude[4+which_pole];
	ang = plate_list[move_cont]->angle[4+which_pole]; }

	sprintf(string,"Summed Rotation Pole\n"
		"Latitude %6.1f\n"
		"Longitude %6.1f\nAngle %6.1f\n", 
		lat,lon,ang);
            print_message(string); 
}


get_elem(id)
int id;
{
    ELEMENT      *ptr;

  if (id>9999) {
    ptr = head_elem;
    while (ptr != NULL) {
        if (ptr->phigs_struct == id-10000) {
		cur_elem = ptr;
	}
	ptr = ptr->next;
    }
  }
    
}




extern float  PaleoTime;
extern int    RefPlate;

save_data()
{
    char filename[80],text[128];
    FILE *outfile;
    int  i,j;
    float lat,lon,ang,time1,time2;

    sprintf(filename,"Enter save set file name");
    prompt_message(filename);
    if ((outfile = fopen(filename,"w")) == NULL) {
        sprintf(text,"Error opening %s\n",filename);
        print_warning(text);
        return(0); }

    for (i=1; i<MAX_CONTINENTS; i++) {
        if (plate_list[i] != NULL) {
	  average_pole(i);
	  for (j=0;j<5;j++) {
	    if (j == 0) {
		lat = plate_list[move_cont]->latitude[0];
		lon = plate_list[move_cont]->longitude[0];
		ang = plate_list[move_cont]->angle[0];
		time1 = 0.0;
		time2 = PaleoTime; }
	    else {
		lat = plate_list[move_cont]->latitude[4+j];
		lon = plate_list[move_cont]->longitude[4+j];
		ang = plate_list[move_cont]->angle[4+j]; 
		time1 = PaleoTime;
		time2 = plate_list[move_cont]->pole_age[j]; }
            fprintf(outfile,"%4d %4d  %6.2f %6.2f %8.3f %8.3f %8.3f %03d"
			" %8.3f %8.3f %8.3f\n",
                    i,j,
		    time1,time2,
                    plate_list[i]->latitude[j],
                    plate_list[i]->longitude[j],
                    plate_list[i]->angle[j],
		    RefPlate,
		    lat,lon,ang);
	  }
	}
    }
    fclose(outfile);
}

hsi2rgb (hue, saturation, intensity, color)
/*	Pcobundl *color;	*/
Ppoint3 *color;
double hue, saturation, intensity;

{

/* funtion to convert HSI color specification to RGB.       *\
|  The algorithm is adapted from Foley and VanDam,          
|  which returns RGB values in the range 0..1 and has a
|  color wheel where Hue=0 is red, Hue=120 is green, and
|  hue=240 is blue.
| 
|  This algorithm uses the E&S color wheel, which has hue=0
|  as blue, hue=120 as red, and hue=240 as green.  It returns
|  RGB values as INTEGERS in the range 0..255. 
\*                                                         */

double my_hue, my_saturation, my_intensity, fractional_part;
double cand1, cand2, cand3;
double my_red, my_green, my_blue;
int sextant;

my_hue = hue;
my_saturation = saturation;
my_intensity = intensity;

while (my_hue > 360.0)
   my_hue = my_hue - 360.0;
while (my_hue < 0.0)
   my_hue = my_hue + 360.0;
if (my_hue == 360.0) my_hue = 0.0;

if (my_intensity > 1.0) my_intensity = 1.0;
if (my_intensity < 0.0) my_intensity = 0.0;
if (my_saturation > 1.0) my_saturation = 1.0;
if (my_saturation < 0.0) my_saturation = 0.0;

if (my_saturation == 0.0) {
   my_red = my_intensity;
   my_green = my_intensity;
   my_blue = my_intensity;
}
else {
   my_hue = my_hue/60.0;
   sextant = my_hue;  /* floor of a real */
                           /* blue - 0, magenta - 1, red - 2 */
                           /* yellow - 3, green - 4, cyan - 5 */
   fractional_part = my_hue - sextant;
   cand1 = my_intensity * (1 - my_saturation);
   cand2 = my_intensity * (1 - (my_saturation * fractional_part));
   cand3 = my_intensity * (1 - (my_saturation * (1 - fractional_part)));
   switch (sextant) {
      case 0:
           my_red = cand3;
           my_green = cand1;
           my_blue = my_intensity;
           break;
      case 1:
           my_red = my_intensity;
           my_green = cand1;
           my_blue = cand2;
           break;
      case 2:
           my_red = my_intensity;
           my_green = cand3;
           my_blue = cand1;
           break;
      case 3:
           my_red = cand2;
           my_green = my_intensity;
           my_blue = cand1;
           break;
      case 4:
           my_red = cand1;
           my_green = my_intensity;
           my_blue = cand3;
           break;
      case 5:
           my_red = cand1;
           my_green = cand2;
           my_blue = my_intensity;
   }
}
color->x = (float) my_red;
color->y = (float) my_green;
color->z = (float) my_blue;
}   


get_image(num)
int num;
{
/* XImage *xi;
float width,height; */
char cmd_string[90];
char infilename[80],outfilename[80];

    switch (num) {

/*	case 1: sprintf(cmd_string, "snapshot");
    		system (cmd_string); 
		break; */
	case 1: sprintf(dumpfilename,"Enter root of dump file name (I'll append .#)");
		prompt_message(dumpfilename); 
		dumpnum=0;
		break;
	case 2: sprintf(infilename,"Enter string to find image files");
		prompt_message(infilename);
		if (*infilename != '\0') {
		  sprintf(outfilename,"Enter output movie file name");
		  prompt_message(outfilename);
		  if (*outfilename != '\0') {
		    sprintf(cmd_string, "makemovie -o movies/%s -c mvc2 -r 30 %s.*",outfilename,infilename);
    		    system (cmd_string); } }
		break;
	case 3: sprintf(outfilename,"Enter movie file name");
		prompt_message(outfilename);
		if (*outfilename != '\0') {
		    sprintf(cmd_string, "movieplayer movies/%s",outfilename);
    		    system (cmd_string); }
		break;
	case 4: sprintf(infilename,"Enter name of rgb image file");
		prompt_message(infilename);
		if (*infilename != '\0') {
		  sprintf(outfilename,"Enter output name of tiff file");
		  prompt_message(outfilename);
		  if (*outfilename != '\0') {
		    sprintf(cmd_string, "imgcopy -ftiff %s %s",infilename,outfilename);
    		    system (cmd_string); } }
		break;
	case 5: if (*dumpfilename != '\0') {
		  draw();
		  sprintf(outfilename,"%s.%03d",dumpfilename,dumpnum);
		  sprintf(cmd_string, "scrsave %s %d %d %d %d",
			outfilename,winx,winx+winwd,winy,winy-winht);
		  system (cmd_string); 
/*		  printf("%s\n",cmd_string);
		  printf("x %d y %d w %d h %d \n",
			winx,winx+winwd,winy,winy+winht); */
		  dumpnum++;
		  printf("done gettting image %s\n",outfilename); }
		break;
    }

}

/*    xi = XGetImage(dpy,draw_win,0,0,50,50,AllPlanes,XYPixmap);*/
/*    sprintf(cmd_string, "xwd -frame -out tmp.xwd -id 0x%x", draw_win);*/
		  /* sprintf(cmd_string, "xwd -root -nobdrs -out %s",outfilename);*/
		  /* sprintf(cmd_string, "dump/xwp/xwpick -format gif %s",outfilename);*/
