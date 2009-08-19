#define MAX_SETS 20000
#define MAX_PMAG 2000
#define RADS 0.017453293

/***   DRAW   ***/
extern int draw_pole;
extern int which_pole;
extern float pole_lat[5];
extern float pole_lon[5];
extern float rot_angle[5][MAX_SETS];
extern float rot_x[5][MAX_SETS];
extern float rot_y[5][MAX_SETS];
extern float rot_z[5][MAX_SETS];
extern float rotplat_angle[5][900];
extern float rotplat_x[5][900];
extern float rotplat_y[5][900];
extern float rotplat_z[5][900];
extern float value_size;

extern float value_rot_x, value_rot_y, value_rot_z;
extern float value_trans_x, value_trans_y, value_trans_z;
