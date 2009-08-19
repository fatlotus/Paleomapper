typedef struct continent_struct {
	float	latitude;
	float	longitude;
	float	angle;
	int	phigs_struct;
} CONTINENT;

typedef struct element_struct {
	struct element_struct *next;
	int	tectonic_element;
	int	id_type_num;
	int	continent;
	int	color;
	int	phigs_struct;
	float	old_time;
	float	young_time;
	char	header[81];
	char	id_type[3];
	char	subp_id[31];
	char 	ltype[6];
} ELEMENT;

typedef struct rot_struct {
	float  	latitude;
	float	longitude;
	float	angle;
	int	reference_plate;
	int	fixed_plate_id;
} ROTATION;

typedef struct menu_struct {
	char *text;
	int (*callback)();
	struct menu_struct *submenu;
	int  data;
} MENUSTRUCT;

typedef struct plate_struct {
	float	latitude[9];
	float	longitude[9];
	float	angle[9];
	int	phigs_struct;	/* Next_Phigs_Struct++  */
	float	pole_age[4];
} PLATE;

typedef struct label_struct {
	float	x;
	float	y;
	char	words[30];
	int	draw;		/* Also stores the size */
	float	color[3];	/* R(0) G(1) B(2) */
} LABEL;

typedef struct pmag_struct {
	struct pmag_struct *next;
	int	unique;
	int	plate;
	float	young;
	float	old;
	float	lat;
	float	lon;
	float	uncert;
} PMAG;

typedef struct lith_struct {
	int	number;
	float	lat;
	float	lon;
	int	age;
	int	plate;
	char	type[6];
	int	typecode;
	int	envbegin;
	int	envend;
	char	country[21];
	char	stratunit[21];
	char	reliable;
	int	reconage;
	float	reconlat;
	float	reconlon;
	int	unique;
	int	refnum;
	char	ref1[21];
	char	ref2[21];
	char	date[9];
	char	mgid[16];
} LITH;

typedef struct {
    float	x;	/* x coordinate */
    float	y;	/* y coordinate */
} Ppoint;

typedef struct {
    float	x;	/* x coordinate */
    float	y;	/* y coordinate */
    float	z;	/* z coordinate */
} Ppoint3;
