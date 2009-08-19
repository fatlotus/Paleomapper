/******************************************************************************/
/* FILE: compare.c                                                            */
/******************************************************************************/
/* Contains the routine for comparing new elements with existing ones.        */
/*                                                                            */
/******************************************************************************/
/* ROUTINES IN FILE:                                                          */
/*    (int) compare - compares most recent header with all existing ones      */
/*                                                                            */
/******************************************************************************/

#include <string.h>
#include "paleotypes.h"
#include "paleodefs.h"
#include "paleoexterns.h"

/******************************************************************************/
/* ROUTINE: COMPARE                                                           */
/******************************************************************************/
/* Function COMPARE compares a header read in most recently with all the      */
/* others already read in.  The function returns TRUE if the header matches   */
/* one that already exists.  In that case the data needs to be appended to    */
/* the existing structure returned in the 'cur_elem' global pointer.          */
/* Otherwise the function returns a FALSE, indicating that a new data         */
/* structure needs to be created.  In any event, the 'new_cont' flag is set   */
/* to indicate whether or not a new tectonic plate needs to be created for    */
/* the data.                                                                  */
/*                                                                            */
/******************************************************************************/
/* INPUT:                                                                     */
/*    cont        - Continent number from header                              */
/*    appears     - Appearance time from header                               */
/*    dappears    - Disappearance time from header                            */
/*    ident_type  - 2 character id type flag from header                      */
/*    idnumber    - ID type number from header                                */
/******************************************************************************/
/* OUTPUT:                                                                    */
/*    new_cont    - create new tectonic plate for element flag                */
/******************************************************************************/
/* GLOBAL VARIABLES USED:                                                     */
/*    cur_elem    - pointer to matching element for appending data            */
/******************************************************************************/
/* CALLS:                                                                     */
/*    None                                                                    */
/******************************************************************************/
int compare(te1, appears, dappears, ident_type, idnumber, new_cont)
int te1, idnumber, *new_cont;
float appears, dappears;
char *ident_type;
{
    ELEMENT *ptr;

    *new_cont = TRUE;
    ptr = head_elem;

/******************************************************************************/
/* Loop through all existing entries looking for a match.                     */
/******************************************************************************/
    while (ptr != NULL) {
        if (te1 == ptr->tectonic_element) {
	    *new_cont = FALSE;
/*            if ((appears == ptr->old_time) && (dappears == ptr->young_time) &&
  	        (idnumber == ptr->id_type_num) && !strncmp(ident_type,ptr->id_type,2)) {
	        cur_elem = ptr;
	        return(TRUE);
            } */
	}
	ptr = ptr->next;
    }
    return(FALSE);
}
