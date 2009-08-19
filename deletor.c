#include <math.h>
#include <string.h>
#include <ctype.h>
#include "paleotypes.h"
#include "paleodefs.h"
#include "paleoexterns.h"

#define MAX_SETS 20000

extern int alive[MAX_SETS];
extern int	vecsetnum;

extern plate_alive[900];

int graveyard=FALSE;


delete_continent(id)
int id;
{
    ELEMENT *ptr;

    ptr = head_elem;
    while (ptr != NULL) {
        if (ptr->tectonic_element == id) 
            remove_item(ptr->phigs_struct);
        ptr = ptr->next;
    }
    plate_alive[id-100]=FALSE;
}

delete_element(id)
int id;
{
    remove_item(id);
}

delete_by_type()
{
    char    input[80];
    char    code[3];
    int     data_num;
    int     i;
    ELEMENT *ptr;

    sprintf(input,"Enter 2 character code for data to delete: ");
    prompt_message(input);
    for (i=0; i<2; i++) {
        code[i] = (islower(input[i])) ? toupper(input[i]) : input[i];
    }
    code[2] = '\0';

    sprintf(input,"Enter data type number: ");
    prompt_message(input);
    data_num = atoi(input);
    ptr = head_elem;
    while (ptr != NULL) {
        if (!strncmp(code,ptr->id_type,2)) {
            if (data_num == ptr->id_type_num || data_num == 99)
                remove_item(ptr->phigs_struct);
            }
        ptr = ptr->next;
    }
}

delete_by_age()
{
    char    input[255];
    float   young_del_time, old_del_time;

    sprintf(input,"Enter the younger cutoff age, in millions of years.\n%s%s",
            "All data younger than this date will be deleted.\n",
            "Younger cutoff age:");
    prompt_message(input);
    young_del_time = (float) atof(input);
    sprintf(input,"Enter the older cutoff age, in millions of years.\n%s%s",
            "All data older than this date will be deleted\n",
            "Older cutoff age: ");
    prompt_message(input);
    old_del_time = (float) atof(input);
    delete_by_time(young_del_time, old_del_time, FALSE);
}

display_continent(id)
int id;
{
    ELEMENT *ptr;

    ptr = head_elem;
    while (ptr != NULL) {
        if (ptr->tectonic_element == id) 
            display_item(ptr->phigs_struct);
        ptr = ptr->next;
    }
    plate_alive[id-100]=TRUE;
}

display_element(id)
int id;
{
    display_item(id);
}

display_all()
{
    ELEMENT *ptr;

    ptr = head_elem;
    while (ptr != NULL) {
        display_item(ptr->phigs_struct);
        ptr = ptr->next;
    }
}

delete_by_time(young,old,restore)
float young, old;
int   restore;
{
    ELEMENT *ptr;

    ptr = head_elem;
    if (restore) {
        while (ptr != NULL) {
            if ((ptr->old_time < young) || (ptr->young_time > old)) 
                remove_item(ptr->phigs_struct);
	    else
		display_item(ptr->phigs_struct);
            ptr = ptr->next;
        }
    } else {
        while (ptr != NULL) {
            if ((ptr->old_time < young) || (ptr->old_time > old)) 
                remove_item(ptr->phigs_struct);
            ptr = ptr->next;
        }
    }
}

remove_item(id)
int id;
{
    /* kill the element */

    alive[id] = FALSE;
}

display_item(id)
int id;
{
    /* kill the element */

    alive[id] = TRUE;
}

