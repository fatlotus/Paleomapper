#include "paleodefs.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

#define PI  3.1415926
#define RADIANS(x)      ((PI * x) / 180.0)

extern GLboolean projection;
extern float value_trans_z;
extern clipping;
extern int winheight,winwidth;
extern int fog_on;

extern fix_labels();

get_persp(type)
int type;
{


    switch(type) {
	case 0: 
    		glClearDepth(1.0);	
    		glClearColor(0.0, 0.0, 0.0, 0.0);
    		glMatrixMode(GL_PROJECTION);	
/*    		glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 20); */	
    		glOrtho(-1.0, 1.0, -1.0, 1.0, 1.0, 20); 
    		glMatrixMode(GL_MODELVIEW);
		fix_labels();	
		break;
	case 1:
    		projection = TRUE;
    		glClearDepth(1.0);
    		glClearColor(0.0, 0.0, 0.0, 0.0);
    		glMatrixMode(GL_PROJECTION);	
    		glLoadIdentity();
		if (clipping) glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 0.0 - (value_trans_z-3.5));	
    		else glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 200);	
    		glMatrixMode(GL_MODELVIEW);	
		fix_labels();	
		draw();
		break;
	case 2: 
    		projection = FALSE;
    		glClearDepth(1.0);
    		glClearColor(0.0, 0.0, 0.0, 0.0);
    		glMatrixMode(GL_PROJECTION);	
    		glLoadIdentity();
		if (clipping) glOrtho(-1.0, 1.0, -1.0, 1.0, 1.0, 0.0 - (value_trans_z-3.5));	
    		else glOrtho(-1.0, 1.0, -1.0, 1.0, 1.0, 200);	
    		glMatrixMode(GL_MODELVIEW);	
		fix_labels();	
		draw();
		break;
	case 3: 
		if (!clipping) clipping = TRUE;
		else clipping = FALSE;

		if (projection) get_persp(1);
		else get_persp(2);

		break;
	case 4:
		if (projection) get_persp(1);
		else get_persp(2);
		break;
    }
}

fog()
{

    if (fog_on) { 
	fog_on = FALSE;
/*	glFogf(GL_FOG_START, 4.5-value_trans_z);*/
	glDisable(GL_FOG);
    }
    else {
	fog_on = TRUE;
	glEnable(GL_FOG);
/*	glFogf(GL_FOG_START, 3.5-value_trans_z); */
    }

}
