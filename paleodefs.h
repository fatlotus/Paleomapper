/*****************************************************************/
/*  Program Constants                                            */
/*****************************************************************/
#define ENV_VAR         "PALEO_DIR"
#define MAX_CONTINENTS	1000
#define TRUE		1
#define FALSE		0

/*****************************************************************/
/*  Data file name containing world outline                      */
/*****************************************************************/
#define GRIDFILE	"world.dat"
#define AGEFILE		"agedata"

/*****************************************************************/
/*  Phigs Structure Labels for top level nodes in display        */
/*****************************************************************/
/*
#define OUTLINE		1
#define WORLD		2
#define LABELS		3
#define DATA		4
#define GRAVEYARD       5
#define POLE		6
#define START_STRUCT	100	*/

#define OUTLINE		1
#define POLE		2
#define PMAGPOLE	3
#define MAPGRID		4
#define MOLLY		5
#define MERCA		6
#define STEREO		7
#define GLOBE		8
#define LITH_GRID	9
#define LITH_ICON	10
#define START_STRUCT	100
#define FONT_START	200	
#define PMAG_START	3000	
#define LITH_START	5000	
#define DATA_START	10000	

/*****************************************************************/
/*  Node labels for WORLD viewing structure                      */
/*****************************************************************/
#define VIEWINDEX	1
#define TIME		5
#define DEPTHCUE	6

/*****************************************************************/
/*  Node labels for individual CONTINENT structures              */
/*****************************************************************/
#define P1ROTY		1
#define P1ROTX		2
#define C1ROTZ		3
#define C1ROTX		4
#define C1ROTY		5

/*****************************************************************/
/*  Node labels for individual ELEMENT structures                */
/*****************************************************************/
#define DISPLAYBIT	1

/*****************************************************************/
/*  Name set labels  						 */
/*****************************************************************/
#define BIT1		10
#define BIT2		11

/*****************************************************************/
/*  Workstation and view parameters                  		 */
/*****************************************************************/
#define  WS1    	1
#define  VIEW1  	1

/*****************************************************************/
/*  Function Key Definitions                      		 */
/*****************************************************************/
#define FLABEL		1
#define FDATA		2
#define FCLIP		3
#define FVIEW		4
#define FRESET		7

/*****************************************************************/
/*  Picking mode identifiers                      		 */
/*****************************************************************/
#define GET_INFO	1
#define MOVE_PLATE	2
#define DEL_LABEL	7
#define DEL_PLATE	3
#define DEL_ELEM	4
#define DIS_PLATE	5
#define DIS_ELEM	6

/*****************************************************************/
/*  Macro Functions                               		 */
/*****************************************************************/
#define RGB(x) ((int)(x/10.0) + 10)
