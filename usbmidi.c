#include <stdlib.h>
#include <stdio.h>
#include "usbmidi.h"
#include <Xm/Xm.h> 
#include <sys/socket.h>
#include "draw.h"
#include "paleotypes.h"
#include "paleodefs.h"
#include <math.h>
#include "paleoexterns.h"

/* Private methods */
void read_from_file();
int check_for_input(void * ignored);
int schedule_redraw(void * ignored, void * ignored2);
int work_to_do();

/* Constants */
#define MIDI_DEVICE "/dev/midi"
#define MIDI_BUFFER_LEN 5
#define TRUE 1
#define FALSE 0

/* Global variables */
double virtual_dials[6] = {0,0,0,0,0,0};
char channels[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
XtAppContext app;
XtIntervalId redraw_interval = NULL;

/* Functions declared elsewhere, that should be in a header file */
virtual_value(int dial, float value);
draw();
updateplate(int axis, float value);

/* Same, but with global variables */
extern int move_cont;

/* Private state variables */
FILE * file;
midi_byte_buffer[5];
midi_bbl = 0;



/* 
 * Initializes the /dev/midi listener in the Xt
 * event loop. Public method.
 */
void init_midi(XtAppContext a) {
	app = a;
	
	if ((file = fopen(MIDI_DEVICE, "r")) == NULL) {
		perror(MIDI_DEVICE);
		exit(1);
	}
	
	XtAppAddWorkProc(app, check_for_input, NULL);
}

/* 
 * Gets the value of a virtual dial, constructed
 * from the USB midi inputs. Public method.
 */
double get_dialval(int dial) {
	double value;
	
	if (dial < 0 || dial > 6) return 0;
	
	return virtual_dials[dial];
}

/*
 * Uses a busy timer to check for new input. Private method.
 */
int check_for_input(void * ignored) {
	fd_set rread;
	int changed;
	struct timeval tv;
	int needs_redraw = FALSE;
	
	while(TRUE) {
		FD_ZERO(&rread);
		FD_SET(fileno(file),&rread);
			
		tv.tv_sec = 0;
		tv.tv_usec = 100;
		
		changed = select(fileno(file)+1, &rread, NULL, NULL, &tv);
		
		if (changed > 0) {
			read_from_file();
			
			needs_redraw = TRUE;
			
		} else if (changed < 0) {
			perror("select()");
			exit(1);
		} else {
			break;
		}
	}
	
	if (needs_redraw) {
		if (redraw_interval != NULL) {
			XtRemoveTimeOut(redraw_interval);
		}
		
		redraw_interval = XtAppAddTimeOut(app, 10, schedule_redraw, NULL);
	}
	
	return FALSE;
}

/*
 * Updates the content of the `virtual_dials` variable based
 * on the value of `channels`. Private method. Not
 * thread safe.
 */
void update_dials() {
	virtual_dials[0] = (int)(channels[0] / 125.0 * 90.0 - 45.0) * 2;
	virtual_dials[1] = (int)(channels[1] / 125.0 * 90.0 - 45.0) * 4;
	
	if (virtual_dials[0] >  90.0) virtual_dials[0] =  90.0;
	if (virtual_dials[0] < -90.0) virtual_dials[0] = -90.0;
	
	value_rot_y = virtual_dials[0];
	value_rot_x = virtual_dials[1];

	
	
	virtual_dials[2] = (int)(channels[2]  / 125.0 * 90.0 - 45.0) * 2.0 +
                           (int)(channels[12] / 125.0 * 100.0) / 50.0 +
	                   (int)(channels[13] / 125.0 * 100.0) / 500.0;
	
	virtual_dials[3] = (int)(channels[3]  / 125.0 * 90.0 - 45.0) * 4.0 +
	                   (int)(channels[10] / 125.0 * 100.0) / 100.0 +
	                   (int)(channels[11] / 125.0 * 100.0) / 1000.0;
	
	virtual_dials[4] = (int)(channels[4]  / 125.0 * 90 - 45.0) * 4 +
	                   (int)(channels[8]  / 125.0 * 100.0) / 25.0 +	
	                   (int)(channels[10] / 125.0 * 100.0) / 250.0;
	
	
	/* <evilhack> */
	PLATE * plat = plate_list[move_cont];
	
	if (plat) {
		plat->latitude[which_pole] = 0;
		plat->longitude[which_pole] = 0;
		plat->angle[which_pole] = 0;
		
		updateplate(1, (int)virtual_dials[2]);
		updateplate(2, (int)virtual_dials[3]);
		updateplate(3, (int)virtual_dials[4]);
	}
	
	/* </evilhack> */
	
	
	virtual_dials[5] = (pow(channels[5] / 125.0f + 1.0f, 2) - 1.0f) * 10.0f + 0.5f;
	
	value_size = (float)virtual_dials[5];
	value_trans_z = 0.9-value_size;
}

/*
 * Redraws the OpenGL context. Called from XtAppAddWorkProc();
 * Private method.
 */
int schedule_redraw(void * ignored, void * ignored2) {
	display_dialval();
	draw();
		
	return TRUE;
}

/*
 * Processes a MIDI message from the /dev/midi. Called from
 * check_for_input. Private method.
 */
void read_from_file() {
	unsigned char last_byte = getc(file);
	
	if (last_byte >= 0x80) {
		switch(last_byte >> 4) {
		case 0xB:
			if (midi_byte_buffer[1] == 0 || midi_byte_buffer == 127) break;
			
			channels[midi_byte_buffer[0] - 1] = midi_byte_buffer[1] - 1;
			update_dials();
			break;
		}
		
		midi_bbl = 0;
	} else {
		if (midi_bbl >= MIDI_BUFFER_LEN) {
			printf("MIDI buffer overflow in usbmidi.c\n");
			
			exit(1);
		}
		 
		midi_byte_buffer[midi_bbl] = last_byte;
		++midi_bbl;
	}
}
