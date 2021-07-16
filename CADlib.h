typedef struct labelgen{
	int prefix, suffix;
	char merger;
}LabelGenerator;

typedef struct cad{
	struct hierarchy *root;
	double width, height;
	struct labelgen *auto_label;
}CAD2D;

typedef struct hierarchy{
	struct cad *cad_file;
	struct hierarchy **child;
	struct entity **object;
}Hierarchy;

typedef struct color{
	float r, g, b;
}Color;

typedef struct point{
	double x, y;
}Point2D;

typedef struct entity{
	struct label *tag;
	struct color col;
	struct text *txt;
	struct point *points;
	struct point center;
	char fill;
	int p_count;
	double thick;
}Entity;

typedef struct label{
	char name[8];
	char type[15];
}Label;

typedef struct text{
	int size;
	char *str;
}Text;

Hierarchy *c2d_create_hierarchy_new(CAD2D *cad);
Hierarchy *c2d_create_hierarchy_child(CAD2D *cad, Hierarchy *parent);

CAD2D *c2d_start_default();
CAD2D *c2d_start_halfcustom(double width, double height);
CAD2D *c2d_start_fullcustom(double width, double height, const Hierarchy *h);

Label *c2d_add_point(CAD2D * cad, Point2D p, Color *c, double thickness);
Label *c2d_add_point_coordinate(CAD2D * cad, double x, double y, Color *c, double thickness);
Label *c2d_add_point_custom(CAD2D *cad, Point2D p, Color *c, double thickness, const Hierarchy *h);
Label *c2d_add_point_custom_label(CAD2D *cad, Point2D p, Color *c, double thickness, const Hierarchy *h, const Label *l);
Label *c2d_add_line(CAD2D *cad, Point2D p[2], Color *c, double thickness);
Label *c2d_add_line_points(CAD2D *cad, Point2D p1, Point2D p2, Color *c, double thickness);
Label *c2d_add_line_custom(CAD2D *cad, Point2D p[2], Color *c, double thickness, const Hierarchy *h);
Label *c2d_add_line_custom_label(CAD2D *cad, Point2D p[2], Color *c, double thickness, const Hierarchy *h, const Label *l);
Label *c2d_add_arc(CAD2D *cad, Point2D center, double r, double start_angle, double end_angle, Color *c, double thickness, char f);
Label *c2d_add_arc_custom(CAD2D *cad, Point2D center, double r, double start_angle, double end_angle, Color *c, double thickness, char f, const Hierarchy *h);
Label *c2d_add_arc_custom_label(CAD2D *cad, Point2D center, double r, double start_angle, double end_angle, Color *c, double thickness, char f, const Hierarchy *h, const Label *l);
Label *c2d_add_circle(CAD2D *cad, Point2D center, double r, Color *c, double thickness, char f);
Label *c2d_add_circle_custom(CAD2D *cad, Point2D center, double r, Color *c, double thickness, char f, const Hierarchy *h);
Label *c2d_add_circle_custom_label(CAD2D *cad, Point2D center, double r, Color *c, double thickness, char f, const Hierarchy *h, const Label *l);
Label *c2d_add_polyline(CAD2D *cad, Point2D *ps, int p_count, double thickness, Color *c);
Label *c2d_add_polyline_custom(CAD2D *cad, Point2D *ps, int p_count, Color *c, double thickness, const Hierarchy *h);
Label *c2d_add_polyline_custom_label(CAD2D *cad, Point2D *ps, int p_count, Color *c, double thickness, const Hierarchy *h, const Label *l);
Label *c2d_add_polygon(CAD2D *cad, Point2D *ps, int p_count, Color *c, double thickness, char f);
Label *c2d_add_polygon_custom(CAD2D *cad, Point2D *ps, int p_count, Color *c, double thickness, char f, const Hierarchy *h);
Label *c2d_add_polygon_custom_label(CAD2D *cad, Point2D *ps, int p_count, Color *c, double thickness, char f, const Hierarchy *h, const Label *l);
Label *c2d_add_text(CAD2D *cad, Point2D p, char *text, int size, Color *c);
Label *c2d_add_text_custom(CAD2D *cad, Point2D p, char *text, int size, Color *c, const Hierarchy *h);
Label *c2d_add_text_custom_label(CAD2D *cad, Point2D p, char *text, int size, Color *c, const Hierarchy *h, const Label *l);
Label *c2d_add_ellipse(CAD2D *cad, Point2D cent, double width, double height, Color *c, double thickness, char f);
Label *c2d_add_ellipse_custom(CAD2D *cad, Point2D cent, double width, double height, Color *c, double thickness, char f, const Hierarchy *h);
Label *c2d_add_ellipse_custom_label(CAD2D *cad, Point2D cent, double width, double height, Color *c, double thickness, char f, const Hierarchy *h, const Label *l);

void c2d_snap(CAD2D *cad, const Label *ls, const Label *lt);
double c2d_measure(CAD2D *cad, const Label *ls, const Label *lt);

void c2d_export(CAD2D *cad, char *file_name, char *options);
CAD2D *c2d_import(char *file_name, char *options);
