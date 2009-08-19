#include <stdio.h>
#include "paleotypes.h"
#include "paleodefs.h"

ELEMENT 	*head_elem = NULL;
ELEMENT 	*cur_elem = NULL;
ELEMENT 	*tail_elem = NULL;
CONTINENT       *cont_list[MAX_CONTINENTS];
PLATE		*plate_list[MAX_CONTINENTS];
PMAG		pmag_list[2000];
LITH		lith_list[5000];
int  		Next_Phigs_Struct = START_STRUCT;

main(argc,argv)
int   argc;
char *argv[];
{

    ageinit();

    open_phigs(&argc,argv);
    init_data(); 
    
    init_dials(); 
/*    init_colour_table();*/

    if (argc == 2) picture_maker(argv[1]);

    mainloop();

}
