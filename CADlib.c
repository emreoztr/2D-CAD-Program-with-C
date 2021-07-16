#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include "CADlib.h"

#define DEF_WIDTH 1500
#define DEF_HEIGHT 1500


Hierarchy *c2d_create_hierarchy_new(CAD2D *cad){
	Hierarchy *new_hier;

	new_hier=(Hierarchy*)malloc(sizeof(Hierarchy));
	new_hier->child=(Hierarchy**)calloc(1,sizeof(Hierarchy*));
	new_hier->child[0]=NULL;
	new_hier->object=(Entity**)calloc(1, sizeof(Entity*));
	new_hier->object[0]=NULL;
	new_hier->cad_file=cad;
	cad->root=new_hier;

	return (new_hier);
}

Hierarchy *c2d_create_hierarchy_child(CAD2D *cad, Hierarchy *parent){
	Hierarchy *child_hier;
	int i;

	child_hier=(Hierarchy*)malloc(sizeof(Hierarchy));
	child_hier->child=(Hierarchy**)calloc(1,sizeof(Hierarchy*));
	child_hier->child[0]=NULL;
	child_hier->object=(Entity**)calloc(1, sizeof(Entity*));
	child_hier->object[0]=NULL;
	child_hier->cad_file=cad;

	for(i=0; parent->child[i]!=NULL; i++);

	parent->child=(Hierarchy**)realloc(parent->child, (i+2)*sizeof(Hierarchy*));
	parent->child[i]=child_hier;
	parent->child[i+1]=NULL;

	return (child_hier);
}

Hierarchy *find_hierarchy(Hierarchy *root, const Hierarchy *h){
	Hierarchy *ans=NULL;

	if(root==h){
		ans=root;
	}

	else{
		for(int i=0; root->child[i]!=NULL && ans==NULL; i++){
			ans=find_hierarchy(root->child[i], h);
		}
	}

	return ans;
}

Entity *create_empty_entity(Entity ***objects){
	int i;
	Entity *new_entity;

	new_entity=(Entity*)malloc(sizeof(Entity));
	new_entity->p_count=0;
	new_entity->txt=NULL;
	new_entity->col.r=0;
	new_entity->col.g=0;
	new_entity->col.b=0;
	for(i=0; (*objects)[i]!=NULL; i++);
	(*objects)=(Entity**)realloc((*objects), (i+2)*sizeof(Entity*));
	(*objects)[i]=new_entity;
	(*objects)[i+1]=NULL;
	return (new_entity);
}

Entity *find_entity_label(Hierarchy *root, const Label *l){
	Entity *holder=NULL;
	Entity *temp;

	if(root!=NULL){
		for(int j=0; root->object[j]!=NULL && holder==NULL; j++){
			temp=root->object[j];
			if(strncmp(temp->tag->name, l->name, strlen(l->name))==0){
				holder=temp;
			}
		}
		for(int i=0; root->child[i]!=NULL && holder==NULL; i++){
			holder=find_entity_label(root->child[i], l);
		}
	}
	return(holder);
}













LabelGenerator *generate_labelsys(CAD2D *cad){
	cad->auto_label=(LabelGenerator*)malloc(sizeof(LabelGenerator));
	cad->auto_label->prefix=0;
	cad->auto_label->suffix=0;
	cad->auto_label->merger=65;
	return (cad->auto_label);
}

LabelGenerator *increase_labelsys(CAD2D *cad){
	LabelGenerator *cad_label;

	cad_label=cad->auto_label;

	if(cad_label->suffix>=999){
		if(cad_label->prefix<999){
			cad_label->prefix+=1;
			cad_label->suffix=0;
		}
		else if(cad_label->merger<255){
			cad_label->merger+=1;
			cad_label->prefix=0;
			cad_label->suffix=0;
		}
		else
			cad_label=NULL;
	}
	else
		cad_label->suffix+=1;

	return(cad_label);
}

Label *generate_label(CAD2D *cad, char *type){
	int prefix_size=1, suffix_size=1;
	char prefix[4], suffix[4];
	Label *new_label=(Label*)malloc(sizeof(Label));

	prefix[3]='\0';
	suffix[3]='\0';

	if(cad->auto_label->prefix!=0)
		prefix_size=log10(cad->auto_label->prefix)+1;

	if(cad->auto_label->suffix!=0)
		suffix_size=log10(cad->auto_label->suffix)+1;

	for(int i=0; i<(3-prefix_size); i++){
		prefix[i]='0';
	}
	for(int i=0; i<(3-suffix_size); i++){
		suffix[i]='0';
	}

	sprintf((prefix+(3-prefix_size)), "%d", cad->auto_label->prefix);
	sprintf((suffix+(3-suffix_size)), "%d", cad->auto_label->suffix);

	new_label->name[7]='\0';
	sprintf(new_label->name, "%s%c%s", prefix, cad->auto_label->merger, suffix);
	strcpy(new_label->type,type);

	increase_labelsys(cad);

	return new_label;
}

Label *generate_cus_label(CAD2D *cad, const char *name, const char *type){
	Label *new_label=(Label*)malloc(sizeof(Label));

	strcpy(new_label->name, name);
	strcpy(new_label->type, type);
	return(new_label);
}















CAD2D *c2d_start_default(){
	CAD2D *new_cad;

	new_cad=(CAD2D*)malloc(sizeof(CAD2D));
	
	new_cad->width=DEF_WIDTH;
	new_cad->height=DEF_HEIGHT;
	generate_labelsys(new_cad);
	c2d_create_hierarchy_new(new_cad);

	return(new_cad);
}

CAD2D *c2d_start_halfcustom(double width, double height){
	CAD2D *new_cad;

	new_cad=(CAD2D*)malloc(sizeof(CAD2D));
	
	new_cad->width=width;
	new_cad->height=height;

	generate_labelsys(new_cad);
	c2d_create_hierarchy_new(new_cad);

	return(new_cad);
}

CAD2D *c2d_start_fullcustom(double width, double height, const Hierarchy *h){
	CAD2D *new_cad;
	int i;

	new_cad=(CAD2D*)malloc(sizeof(CAD2D));
	
	new_cad->width=width;
	new_cad->height=height;

	generate_labelsys(new_cad);
	new_cad->root=find_hierarchy(h->cad_file->root, h);

	return(new_cad);
}




















Label *c2d_add_point(CAD2D * cad, Point2D p, Color *c, double thickness){
	Entity *place;

	place=create_empty_entity(&(cad->root->object));
	place->p_count=1;
	place->points=(Point2D*)calloc(1, sizeof(Point2D));
	place->points[0]=p;

	if(c==NULL){
		place->col.r=0;
		place->col.g=0;
		place->col.b=0;
	}
	else{
		place->col=(*c);
	}

	place->txt=NULL;
	place->thick=thickness;
	place->fill='e';

	place->tag=generate_label(cad, "point");

	return (place->tag);
}

Label *c2d_add_point_coordinate(CAD2D * cad, double x, double y, Color *c, double thickness){
	Point2D p;

	p.x=x;
	p.y=y;

	return(c2d_add_point(cad, p, c, thickness));
}

Label *c2d_add_point_custom(CAD2D *cad, Point2D p, Color *c, double thickness, const Hierarchy *h){
	Entity *holder;
	Hierarchy *found_h;
	found_h=find_hierarchy(cad->root, h);
	holder=create_empty_entity(&(found_h->object));

	if(c==NULL){
		holder->col.r=0;
		holder->col.g=0;
		holder->col.b=0;
	}
	else{
		holder->col=(*c);
	}

	holder->p_count+=1;
	holder->points=(Point2D*)calloc(1, sizeof(Point2D));
	holder->points[0]=p;
	holder->thick=thickness;
	holder->fill='e';

	holder->tag=generate_label(cad, "point");
	return(holder->tag);
}

Label *c2d_add_point_custom_label(CAD2D *cad, Point2D p, Color *c, double thickness, const Hierarchy *h, const Label *l){
	Entity *holder;
	Hierarchy *found_h;
	Label *return_lab=NULL;

	if(l->name[0]>='0' && l->name[0]<='9'){
		printf("!!!CUSTOM LABEL NAME CAN'T START WITH A NUMBER!!!\n");
		return_lab=NULL;
	}
	else{
		found_h=find_hierarchy(cad->root, h);
		holder=create_empty_entity(&(found_h->object));
	
		if(c==NULL){
			holder->col.r=0;
			holder->col.g=0;
			holder->col.b=0;
		}
		else{
			holder->col=(*c);
		}
	
		holder->p_count+=1;
		holder->points=(Point2D*)calloc(1, sizeof(Point2D));
		holder->points[0]=p;
		holder->tag=generate_cus_label(cad, l->name, "point");
		holder->fill='e';
		holder->thick=thickness;
		return_lab=holder->tag;
	}
	return(return_lab);
}

Label *c2d_add_line(CAD2D *cad, Point2D p[2], Color *c, double thickness){
	Entity *holder;

	holder=create_empty_entity(&(cad->root->object));

	if(c==NULL){
		holder->col.r=0;
		holder->col.g=0;
		holder->col.b=0;
	}
	else{
		holder->col=(*c);
	}

	holder->p_count+=2;
	holder->points=p;
	holder->tag=generate_label(cad, "line");
	holder->thick=thickness;
	holder->fill='e';
	return(holder->tag);
}

Label *c2d_add_line_points(CAD2D *cad, Point2D p1, Point2D p2, Color *c, double thickness){
	Point2D *p;

	p=(Point2D*)calloc(2, sizeof(Point2D));
	p[0]=p1;
	p[1]=p2;
	return(c2d_add_line(cad, p, c, thickness));
}

Label *c2d_add_line_custom(CAD2D *cad, Point2D p[2], Color *c, double thickness, const Hierarchy *h){
	Entity *holder;
	Hierarchy *found_h;

	found_h=find_hierarchy(cad->root, h);
	holder=create_empty_entity(&(found_h->object));

	if(c==NULL){
		holder->col.r=0;
		holder->col.g=0;
		holder->col.b=0;
	}
	else{
		holder->col=(*c);
	}

	holder->p_count+=2;
	holder->points=(Point2D*)calloc(2, sizeof(Point2D));
	holder->points=p;
	holder->thick=thickness;
	holder->fill='e';
	holder->tag=generate_label(cad, "line");
	return(holder->tag);
}

Label *c2d_add_line_custom_label(CAD2D *cad, Point2D p[2], Color *c, double thickness, const Hierarchy *h, const Label *l){
	Entity *holder;
	Hierarchy *found_h;
	Label *return_lab;

	if(l->name[0]>='0' && l->name[0]<='9'){
		printf("!!!CUSTOM LABEL NAME CAN'T START WITH A NUMBER!!!\n");
		return_lab=NULL;
	}
	else{
		found_h=find_hierarchy(cad->root, h);
		holder=create_empty_entity(&(found_h->object));
	
		if(c==NULL){
			holder->col.r=0;
			holder->col.g=0;
			holder->col.b=0;
		}
		else{
			holder->col=(*c);
		}
	
		holder->p_count+=2;
		holder->points=(Point2D*)calloc(2, sizeof(Point2D));
		holder->points=p;
		holder->fill='e';
		holder->thick=thickness;
		holder->tag=generate_cus_label(cad, l->name, "line");
		return_lab=holder->tag;
	}
	return(return_lab);
}

double angle_to_rad(double angle){
	return((angle*M_PI)/180);
}

Label *c2d_add_arc(CAD2D *cad, Point2D center, double r, double start_angle, double end_angle, Color *c, double thickness, char f){
	Entity *holder;
	int j=0;
	double start_rad, end_rad;

	start_rad=angle_to_rad(start_angle);
	end_rad=angle_to_rad(end_angle);
	holder=create_empty_entity(&(cad->root->object));

	if(c==NULL){
		holder->col.r=0;
		holder->col.g=0;
		holder->col.b=0;
	}
	else{
		holder->col=(*c);
	}

	holder->tag=generate_label(cad, "arc");
	for(double i=start_rad; i<=end_rad; i+=0.001){
		if(j==0)
			holder->points=(Point2D*)calloc(1, sizeof(Point2D));
		else{
			holder->points=(Point2D*)realloc(holder->points, (j+1)*sizeof(Point2D));
		}
		holder->points[j].x=center.x+cos(i)*r;
		holder->points[j].y=center.y+sin(i)*r;
		j+=1;
	}
	holder->center=center;
	holder->fill=f;
	holder->thick=thickness;
	holder->p_count=j;
	return(holder->tag);
}

Label *c2d_add_arc_custom(CAD2D *cad, Point2D center, double r, double start_angle, double end_angle, Color *c, double thickness, char f, const Hierarchy *h){
	Entity *holder;
	Hierarchy *found_h;
	int j=0;
	double start_rad, end_rad;

	found_h=find_hierarchy(cad->root, h);
	start_rad=angle_to_rad(start_angle);
	end_rad=angle_to_rad(end_angle);
	holder=create_empty_entity(&(found_h->object));

	if(c==NULL){
		holder->col.r=0;
		holder->col.g=0;
		holder->col.b=0;
	}
	else{
		holder->col=(*c);
	}

	holder->tag=generate_label(cad, "arc");
	for(double i=start_rad; i<=end_rad; i+=0.001){
		if(j==0)
			holder->points=(Point2D*)calloc(1, sizeof(Point2D));
		else{
			holder->points=(Point2D*)realloc(holder->points, (j+1)*sizeof(Point2D));
		}
		holder->points[j].x=center.x+cos(i)*r;
		holder->points[j].y=center.y+sin(i)*r;
		j+=1;
	}
	holder->p_count=j;
	holder->center=center;
	holder->thick=thickness;
	holder->fill=f;
	return(holder->tag);
}

Label *c2d_add_arc_custom_label(CAD2D *cad, Point2D center, double r, double start_angle, double end_angle, Color *c, double thickness, char f, const Hierarchy *h, const Label *l){
	Entity *holder;
	Hierarchy *found_h;
	int j=0;
	double start_rad, end_rad;
	Label *return_lab=NULL;

	if(l->name[0]>='0' && l->name[0]<='9'){
		printf("!!!CUSTOM LABEL NAME CAN'T START WITH A NUMBER!!!\n");
		return_lab=NULL;
	}
	else{
		found_h=find_hierarchy(cad->root, h);
		start_rad=angle_to_rad(start_angle);
		end_rad=angle_to_rad(end_angle);
		holder=create_empty_entity(&(found_h->object));
	
		if(c==NULL){
			holder->col.r=0;
			holder->col.g=0;
			holder->col.b=0;
		}
		else{
			holder->col=(*c);
		}
	
		holder->tag=generate_cus_label(cad, l->name, "arc");

		return_lab=holder->tag;
		for(double i=start_rad; i<=end_rad; i+=0.001){
			if(j==0)
				holder->points=(Point2D*)calloc(1, sizeof(Point2D));
			else{
				holder->points=(Point2D*)realloc(holder->points, (j+1)*sizeof(Point2D));
			}
			holder->points[j].x=center.x+cos(i)*r;
			holder->points[j].y=center.y+sin(i)*r;
			j+=1;
		}
		holder->p_count=j;
	}
	holder->thick=thickness;
	holder->fill=f;
	holder->center=center;
	return(return_lab);
}

Label *c2d_add_circle(CAD2D *cad, Point2D center, double r, Color *c, double thickness, char f){
	return(c2d_add_arc(cad, center, r, 0, 360, c, thickness, f));
}

Label *c2d_add_circle_custom(CAD2D *cad, Point2D center, double r, Color *c, double thickness, char f, const Hierarchy *h){
	return(c2d_add_arc_custom(cad, center, r, 0, 360, c, thickness, f, h));
}

Label *c2d_add_circle_custom_label(CAD2D *cad, Point2D center, double r, Color *c, double thickness, char f, const Hierarchy *h, const Label *l){
	return(c2d_add_arc_custom_label(cad, center, r, 0, 360, c, thickness, f, h, l));
}

Label *c2d_add_polyline(CAD2D *cad, Point2D *ps, int p_count, double thickness, Color *c){
	Entity *holder;

	holder=create_empty_entity(&(cad->root->object));

	if(c==NULL){
		holder->col.r=0;
		holder->col.g=0;
		holder->col.b=0;
	}
	else{
		holder->col=(*c);
	}

	holder->points=ps;
	holder->p_count=p_count;
	holder->thick=thickness;
	holder->tag=generate_label(cad, "pline");

	return(holder->tag);
}

Label *c2d_add_polyline_custom(CAD2D *cad, Point2D *ps, int p_count, Color *c, double thickness, const Hierarchy *h){
	Entity *holder;
	Hierarchy *found_h;

	found_h=find_hierarchy(cad->root, h);
	holder=create_empty_entity(&(found_h->object));

	if(c==NULL){
		holder->col.r=0;
		holder->col.g=0;
		holder->col.b=0;
	}
	else{
		holder->col=(*c);
	}

	holder->points=ps;
	holder->p_count=p_count;
	holder->thick=thickness;
	holder->tag=generate_label(cad, "pline");

	return(holder->tag);
}

Label *c2d_add_polyline_custom_label(CAD2D *cad, Point2D *ps, int p_count, Color *c, double thickness, const Hierarchy *h, const Label *l){
	Entity *holder;
	Hierarchy *found_h;
	Label *return_lab=NULL;

	if(l->name[0]>='0' && l->name[0]<='9'){
		printf("!!!CUSTOM LABEL NAME CAN'T START WITH A NUMBER!!!\n");
		return_lab=NULL;
	}
	else
	{
		found_h=find_hierarchy(cad->root, h);
		holder=create_empty_entity(&(found_h->object));
	
		if(c==NULL){
			holder->col.r=0;
			holder->col.g=0;
			holder->col.b=0;
		}
		else{
			holder->col=(*c);
		}
	
		holder->points=ps;
		holder->p_count=p_count;
		holder->thick=thickness;
		holder->tag=generate_cus_label(cad, l->name, "pline");
		return_lab=holder->tag;
	}

	return(return_lab);
}

Label *c2d_add_polygon(CAD2D *cad, Point2D *ps, int p_count, Color *c, double thickness, char f){
	Entity *holder;
	int i;

	holder=create_empty_entity(&(cad->root->object));

	if(c==NULL){
		holder->col.r=0;
		holder->col.g=0;
		holder->col.b=0;
	}
	else{
		holder->col=(*c);
	}

	for(i=0; i<p_count; i++){
		if(i==0)
			holder->points=(Point2D*)calloc(1, sizeof(Point2D));
		else{
			holder->points=(Point2D*)realloc(holder->points, (i+1)*sizeof(Point2D));
		}
		holder->points[i]=ps[i];
	}
	holder->points=(Point2D*)realloc(holder->points, (i+1)*sizeof(Point2D));
	holder->points[i]=ps[0];
	holder->p_count=i+1;
	holder->fill=f;
	holder->thick=thickness;
	holder->tag=generate_label(cad, "pgon");

	return(holder->tag);
}

Label *c2d_add_polygon_custom(CAD2D *cad, Point2D *ps, int p_count, Color *c, double thickness, char f, const Hierarchy *h){
	Entity *holder;
	Hierarchy *found_h;
	int i;

	found_h=find_hierarchy(cad->root, h);

	holder=create_empty_entity(&(found_h->object));

	if(c==NULL){
		holder->col.r=0;
		holder->col.g=0;
		holder->col.b=0;
	}
	else{
		holder->col=(*c);
	}
	for(i=0; i<p_count; i++){
		if(i==0)
			holder->points=(Point2D*)calloc(1, sizeof(Point2D));
		else{
			holder->points=(Point2D*)realloc(holder->points, (i+1)*sizeof(Point2D));
		}
		holder->points[i]=ps[i];
	}
	holder->points=(Point2D*)realloc(holder->points, (i+1)*sizeof(Point2D));
	holder->points[i]=ps[0];
	holder->p_count=i+1;
	holder->fill=f;
	holder->thick=thickness;
	holder->tag=generate_label(cad, "pgon");

}

Label *c2d_add_polygon_custom_label(CAD2D *cad, Point2D *ps, int p_count, Color *c, double thickness, char f, const Hierarchy *h, const Label *l){
	Entity *holder;
	Hierarchy *found_h;
	int i;
	Label *return_lab=NULL;

	if(l->name[0]>='0' && l->name[0]<='9'){
		printf("!!!CUSTOM LABEL NAME CAN'T START WITH A NUMBER!!!\n");
		return_lab=NULL;
	}
	else{
		found_h=find_hierarchy(cad->root, h);
	
		holder=create_empty_entity(&(found_h->object));
	
		if(c==NULL){
			holder->col.r=0;
			holder->col.g=0;
			holder->col.b=0;
		}
		else{
			holder->col=(*c);
		}
		for(i=0; i<p_count; i++){
			if(i==0)
				holder->points=(Point2D*)calloc(1, sizeof(Point2D));
			else{
				holder->points=(Point2D*)realloc(holder->points, (i+1)*sizeof(Point2D));
			}
			holder->points[i]=ps[i];
		}
		holder->points=(Point2D*)realloc(holder->points, (i+1)*sizeof(Point2D));
		holder->points[i]=ps[0];
		holder->p_count=i+1;
		holder->fill=f;
		holder->thick=thickness;
		return_lab=generate_label(cad, "pgon");
		holder->tag=return_lab;
	}
	return(return_lab);
}

Label *c2d_add_text(CAD2D *cad, Point2D p, char *text, int size, Color *c){
	Entity *holder;

	holder=create_empty_entity(&(cad->root->object));

	if(c==NULL){
		holder->col.r=0;
		holder->col.g=0;
		holder->col.b=0;
	}
	else{
		holder->col=(*c);
	}

	holder->txt=(Text*)malloc(sizeof(Text));
	holder->txt->str = (char*)malloc(strlen(text) * sizeof(char));
	strcpy(holder->txt->str, text);
	holder->txt->size=size;
	holder->p_count=1;
	holder->points=(Point2D*)malloc(sizeof(Point2D));
	holder->points[0]=p;
	holder->tag=generate_label(cad, "text");
	return(holder->tag);
}

Label *c2d_add_text_custom(CAD2D *cad, Point2D p, char *text, int size, Color *c, const Hierarchy *h){
	Entity *holder;
	Hierarchy *found_h;

	found_h=find_hierarchy(cad->root, h);
	holder=create_empty_entity(&(found_h->object));

	if(c==NULL){
		holder->col.r=0;
		holder->col.g=0;
		holder->col.b=0;
	}
	else{
		holder->col=(*c);
	}

	holder->txt=(Text*)malloc(sizeof(Text));
	holder->txt->str=text;
	holder->txt->size=size;
	holder->p_count=1;
	holder->points=(Point2D*)malloc(sizeof(Point2D));
	holder->points[0]=p;
	holder->tag=generate_label(cad, "text");
	return(holder->tag);
}

Label *c2d_add_text_custom_label(CAD2D *cad, Point2D p, char *text, int size, Color *c, const Hierarchy *h, const Label *l){
	Entity *holder;
	Hierarchy *found_h;
	Label *return_lab=NULL;

	if(l->name[0]>='0' && l->name[0]<='9'){
		printf("!!!CUSTOM LABEL NAME CAN'T START WITH A NUMBER!!!\n");
		return_lab=NULL;
	}
	else{
		found_h=find_hierarchy(cad->root, h);
		holder=create_empty_entity(&(found_h->object));
	
		if(c==NULL){
			holder->col.r=0;
			holder->col.g=0;
			holder->col.b=0;
		}
		else{
			holder->col=(*c);
		}
	
		holder->txt=(Text*)malloc(sizeof(Text));
		holder->txt->str=text;
		holder->txt->size=size;
		holder->p_count=1;
		holder->points=(Point2D*)malloc(sizeof(Point2D));
		holder->points[0]=p;
		holder->tag=generate_cus_label(cad, l->name, "text");
		return_lab=holder->tag;
	}
	return(return_lab);
}

Label *c2d_add_ellipse(CAD2D *cad, Point2D cent, double width, double height, Color *c, double thickness, char f){
	Entity *holder;
	int k=0;

	holder=create_empty_entity(&(cad->root->object));
	if(c==NULL){
			holder->col.r=0;
			holder->col.g=0;
			holder->col.b=0;
	}
	else{
		holder->col=(*c);
	}

	holder->center=cent;

	for(double i=0.0; i<=2*M_PI; i+=0.01){
		if(k==0)
			holder->points=(Point2D*)calloc(1, sizeof(Point2D));
		else
			holder->points=(Point2D*)realloc(holder->points, (k+1)*sizeof(Point2D));
		holder->points[k].x=cos(i)*(width/2.0)+cent.x;
		holder->points[k].y=sin(i)*(height/2.0)+cent.y;
		k++;
	}
	holder->p_count=k;
	holder->fill=f;
	holder->thick=thickness;
	holder->tag=generate_label(cad, "ellipse");
	return(holder->tag);
}

Label *c2d_add_ellipse_custom(CAD2D *cad, Point2D cent, double width, double height, Color *c, double thickness, char f, const Hierarchy *h){
	Entity *holder;
	Hierarchy *found_h;
	int k=0;

	found_h=find_hierarchy(cad->root, h);
	holder=create_empty_entity(&(found_h->object));
	if(c==NULL){
			holder->col.r=0;
			holder->col.g=0;
			holder->col.b=0;
	}
	else{
		holder->col=(*c);
	}

	holder->center=cent;

	for(double i=0.0; i<=2*M_PI; i+=0.01){
		if(k==0)
			holder->points=(Point2D*)calloc(1, sizeof(Point2D));
		else
			holder->points=(Point2D*)realloc(holder->points, (k+1)*sizeof(Point2D));
		holder->points[k].x=cos(i)*(width/2.0)+cent.x;
		holder->points[k].y=sin(i)*(height/2.0)+cent.y;
		k++;
	}
	holder->p_count=k;
	holder->fill=f;
	holder->thick=thickness;
	holder->tag=generate_label(cad, "ellipse");
	return(holder->tag);
}

Label *c2d_add_ellipse_custom_label(CAD2D *cad, Point2D cent, double width, double height, Color *c, double thickness, char f, const Hierarchy *h, const Label *l){
	Entity *holder;
	Hierarchy *found_h;
	Label *return_lab=NULL;
	int k=0;
	if(l->name[0]>='0' && l->name[0]<='9'){
		printf("!!!CUSTOM LABEL NAME CAN'T START WITH A NUMBER!!!\n");
		return_lab=NULL;
	}
	else
	{
		found_h=find_hierarchy(cad->root, h);
		holder=create_empty_entity(&(found_h->object));
		if(c==NULL){
				holder->col.r=0;
				holder->col.g=0;
				holder->col.b=0;
		}
		else{
			holder->col=(*c);
		}
	
		holder->center=cent;
	
		for(double i=0.0; i<=2*M_PI; i+=0.01){
			if(k==0)
				holder->points=(Point2D*)calloc(1, sizeof(Point2D));
			else
				holder->points=(Point2D*)realloc(holder->points, (k+1)*sizeof(Point2D));
			holder->points[k].x=cos(i)*(width/2.0)+cent.x;
			holder->points[k].y=sin(i)*(height/2.0)+cent.y;
			k++;
		}
		holder->p_count=k;
		holder->fill=f;
		holder->thick=thickness;
		return_lab=generate_cus_label(cad, l->name, "ellipse");
		holder->tag=return_lab;
	}
	return(return_lab);
}























double calculate_distance(Point2D p1, Point2D p2){
	return(sqrt(pow(p1.x-p2.x, 2.0) + pow(p1.y-p2.y, 2.0)));
}

Point2D find_center(Entity *e){
	Point2D p;
	int p_c;
	p.x=0;
	p.y=0;
	if(strncmp(e->tag->type, "pgon", 4)==0){
		p_c=e->p_count-1;
	}
	else
		p_c=e->p_count;

	for(int i=0; i<p_c; i++){
		p.x+=e->points[i].x;
		p.y+=e->points[i].y;
	}
	p.x/=(double)(p_c);
	p.y/=(double)(p_c);
	return(p);
}



Point2D *find_closest_point(Entity *e1, Entity *e2, char option){
	Point2D *ps;
	double min, temp;
	int i=0, k=0;

	ps=(Point2D*)calloc(2, sizeof(Point2D));

	if((option)=='s'){
		if(strncmp(e1->tag->type, "arc", 3)==0 && strncmp(e2->tag->type, "arc", 3)==0){
			ps[0]=e1->center;
			ps[1]=e2->center;
		}
		else if(strncmp(e1->tag->type, "arc", 3)==0){
			ps[1]=find_center(e2);
			ps[0]=e1->center;
		}
		else{
			min=calculate_distance(e1->points[0], e2->points[0]);
			ps[0]=e1->points[0];
			ps[1]=e2->points[0];
			for(i=1; i<e2->p_count; i++){
				for(k=1; k<e1->p_count; k++){
					temp=calculate_distance(e1->points[i], e2->points[k]);
					if(temp<min){
						min=temp;
						ps[0]=e1->points[i];
						ps[1]=e2->points[k];
					}
				}	
			}
		}
	}
	else if((option)=='m'){
		if(strncmp(e1->tag->type, "arc", 3)==0 && strncmp(e2->tag->type, "arc", 3)==0){
			ps[0]=e1->center;
			ps[1]=e2->center;
		}
		else{
			min=calculate_distance(e1->points[0], e2->points[0]);
			ps[0]=e1->points[0];
			ps[1]=e2->points[0];
			for(i=1; i<(e2->p_count); i++){
				for(k=1; k<(e1->p_count); k++){
					temp=calculate_distance(e1->points[k], e2->points[i]);
					if(temp<min){
						min=temp;
						ps[0]=e1->points[k];
						ps[1]=e2->points[i];
					}
				}	
			}
		}
	}
	return ps;
}

void c2d_snap(CAD2D *cad, const Label *ls, const Label *lt){
	Entity *e_s, *e_t;
	double x_dif, y_dif;
	Point2D *ps;

	e_s=find_entity_label(cad->root, ls);
	e_t=find_entity_label(cad->root, lt);

	if(e_s!=NULL && e_t!=NULL){
		ps=find_closest_point(e_s, e_t, 's');
		x_dif=ps[1].x-ps[0].x;
		y_dif=ps[1].y-ps[0].y;
		for(int i=0; i<e_s->p_count; i++){
			(e_s->points[i].x)+=x_dif;
			(e_s->points[i].y)+=y_dif;
		}
		if(strncmp(e_s->tag->type, "arc", 3)==0){
			e_s->center.x+=x_dif;
			e_s->center.y+=y_dif;
		}
		free(ps);
	}
}

double c2d_measure(CAD2D *cad, const Label *ls, const Label *lt){
	Entity *e_s, *e_t;
	double ans;
	Point2D *ps;

	e_s=find_entity_label(cad->root, ls);
	e_t=find_entity_label(cad->root, lt);
	if(e_s!=NULL && e_t!=NULL){
		ps=find_closest_point(e_s, e_t, 'm');
	}
	ans=calculate_distance(ps[0], ps[1]);
	free(ps);
	return(ans);
}















void hierarchy_export(Hierarchy *h, FILE *fp){
	int txt_len;
	double thickness=-1.0;
	Entity **holder;
	holder=h->object;
	if(h!=NULL){
		fwrite("{", 1, 1, fp);
		for(int i=0; holder[i]!=NULL; i++){
			fwrite("[", 1, 1, fp);
			fwrite(holder[i]->tag->name, 1, 7, fp);
			fwrite(",", 1, 1, fp);
			fwrite(holder[i]->tag->type, 1, strlen(holder[i]->tag->type), fp);
			fwrite(",", 1, 1, fp);
			fwrite(&(holder[i]->col.r), sizeof(float), 1, fp);
			fwrite(&(holder[i]->col.g), sizeof(float), 1, fp);
			fwrite(&(holder[i]->col.b), sizeof(float), 1, fp);
			fwrite(",", 1, 1, fp);
			fwrite(&(holder[i]->p_count), sizeof(int), 1, fp);
			fwrite(",", 1, 1, fp);
			fwrite(&(holder[i]->fill), 1, 1, fp);
			fwrite(",", 1, 1, fp);
			if(holder[i]->txt==NULL)
				fwrite(&(holder[i]->thick), 1, sizeof(double), fp);
			else
				fwrite(&thickness, 1, sizeof(double), fp);
			fwrite("(", 1, 1, fp);
			if(strncmp(holder[i]->tag->type, "arc", 3)==0){
				fwrite(&(holder[i]->center.x), sizeof(double), 1, fp);
				fwrite(&(holder[i]->center.y), sizeof(double), 1, fp);
			}
			for(int j=0; j<holder[i]->p_count; j++){
				fwrite(&(holder[i]->points[j].x), sizeof(double), 1, fp);
				fwrite(&(holder[i]->points[j].y), sizeof(double), 1, fp);
			}
			fwrite(")", 1, 1, fp);
			if(strcmp(holder[i]->tag->type, "text")==0){
				txt_len=strlen(holder[i]->txt->str);
				fwrite("$", 1, 1, fp);
				fwrite(&(holder[i]->txt->size), sizeof(int), 1, fp);
				fwrite(&txt_len, sizeof(int), 1, fp);
				fwrite(holder[i]->txt->str, 1, txt_len, fp);
				fwrite("$", 1, 1, fp);
			}
			fwrite("]", 1, 1, fp);
		}
		for(int k=0; h->child[k]!=NULL; k++){
			hierarchy_export(h->child[k], fp);
		}
		fwrite("}", 1, 1, fp);
	}
}

FILE *hierarchy_import(CAD2D *cad,Hierarchy *h, FILE *fp){
	int i, txt_len;
	char check;
	bool stop=false;
	Entity *obj;
	Label *lab;

	do{
		fread(&check, 1, 1, fp);
		if(check=='['){
			obj=create_empty_entity(&(h->object));
			lab=(Label*)malloc(sizeof(Label));
			fread(lab->name, 1, 7, fp);
			lab->name[7]='\0';
			fread(&check, 1, 1, fp);
			fread(&check, 1, 1, fp);
			for(i=0; check!=','; i++){
				lab->type[i]=check;
				fread(&check, 1, 1, fp);
			}
			lab->type[i]='\0';
			obj->tag=lab;
			fread(&(obj->col.r), sizeof(float), 1, fp);
			fread(&(obj->col.g), sizeof(float), 1, fp);
			fread(&(obj->col.b), sizeof(float), 1, fp);
			fread(&check, 1, 1, fp);
			fread(&(obj->p_count), sizeof(int), 1, fp);
			fread(&check, 1, 1, fp);
			fread(&(obj->fill), 1, 1, fp);
			fread(&check, 1, 1, fp);
			fread(&(obj->thick), 1, sizeof(double), fp);
			obj->points=(Point2D*)calloc(obj->p_count, sizeof(Point2D));
			fread(&check, 1, 1, fp);
			if(strncmp(obj->tag->type, "arc", 3)==0){
				fread(&(obj->center.x), sizeof(double), 1, fp);
				fread(&(obj->center.y), sizeof(double), 1, fp);
			}
			for(i=0; i<(obj->p_count); i++){
				fread(&(obj->points[i].x), sizeof(double), 1, fp);
				fread(&(obj->points[i].y), sizeof(double), 1, fp);
			}
			fread(&check, 1, 1, fp);
			fread(&check, 1, 1, fp);
			if(check=='$'){
				obj->txt=(Text*)malloc(sizeof(Text));
				fread(&(obj->txt->size), sizeof(int), 1, fp);
				fread(&txt_len, sizeof(int), 1, fp);
				obj->txt->str=(char*)calloc(txt_len+1, sizeof(char));
				fread(obj->txt->str, 1, txt_len, fp);
				obj->txt->str[txt_len]='\0';
				fread(&check, 1, 1, fp);
			}
		}
		else if(check=='{'){
			fp=hierarchy_import(cad, c2d_create_hierarchy_child(cad, h), fp);
		}
		else if(check=='}'){
			stop=true;
		}
	}while(stop!=true);

	return fp;
}


FILE *eps_hierarchy_export(Hierarchy *h, FILE *fp){
	Entity *holder;
	for(int i=0; h->object[i]!=NULL; i++){
		holder=h->object[i];
		if(holder->txt==NULL){
			fprintf(fp, "%lf %lf %lf setrgbcolor\n", holder->col.r, holder->col.g, holder->col.b);
			if(holder->p_count>=1)
			{
				fprintf(fp, "%lf %lf moveto\n", holder->points[0].x, holder->points[0].y);
				fprintf(fp, "1.0 1.0 rlineto\n");
			}
		
			for(int j=1; j<(holder->p_count); j++){
				fprintf(fp, "%lf %lf lineto\n", holder->points[j].x, holder->points[j].y);
			}
		}
		else{
			fprintf(fp, "/Times-Roman findfont\n");
			fprintf(fp, "%d scalefont\nsetfont\n\n", holder->txt->size);
			fprintf(fp, "%lf %lf moveto\n", holder->points[0].x, holder->points[0].y);
			fprintf(fp, "(%s) show\n", holder->txt->str);
		}
		if(holder->fill=='f')
			fprintf(fp, "fill\n");
		if(holder->txt==NULL)
			fprintf(fp, "%lf setlinewidth\n", holder->thick);
		fprintf(fp, "stroke\n");
	}

	for(int i=0; h->child[i]!=NULL; i++){
		fp=eps_hierarchy_export(h->child[i], fp);
	}

	return(fp);
}

void c2d_export(CAD2D *cad, char *file_name, char *options){
	FILE *fp;

	if(strcmp(options, "GTU")==0){
		fp=fopen(file_name, "wb");
		fwrite("!(", 1, 2, fp);
		fwrite(&(cad->width), sizeof(double), 1, fp);
		fwrite(&(cad->height), sizeof(double), 1, fp);
		fwrite(")", 1, 1, fp);
		fwrite("?(", 1, 2, fp);
		fwrite(&(cad->auto_label->prefix), sizeof(int), 1, fp);
		fwrite(&(cad->auto_label->merger), 1, 1, fp);
		fwrite(&(cad->auto_label->suffix), sizeof(int), 1, fp);
		fwrite(")?", 1, 2, fp);
		hierarchy_export(cad->root, fp);
	}

	else if(strcmp(options, "EPS")==0){
		fp=fopen(file_name, "w");
		fprintf(fp, "%%!\n%%%%BoundingBox: %lf %lf %lf %lf\nnewpath\n/Times-Roman findfont\n12 scalefont\nsetfont\n", -((cad->width)/2.0), -((cad->height)/2.0), (cad->width)/2.0, (cad->height)/2.0);
		eps_hierarchy_export(cad->root, fp);
		fprintf(fp, "showpage");
	}
	fclose(fp);
}

CAD2D *c2d_import(char *file_name, char *options){
	char check[10];
	FILE *fp;
	CAD2D *cad;
	double width, height;

	cad=(CAD2D*)malloc(sizeof(CAD2D));

	if(strcmp(options, "GTU")==0){
		fp=fopen(file_name, "rb");
		fread(&check, 1, 1, fp);
		if(check[0]=='!'){
			fread(check, 1, 1, fp);
			fread(&width, sizeof(double), 1, fp);
			fread(&height, sizeof(double), 1, fp);
			fread(check, 1, 3, fp);
			cad=c2d_start_halfcustom(width, height);
			fread(&(cad->auto_label->prefix), sizeof(int), 1, fp);
			fread(&(cad->auto_label->merger), 1, 1, fp);
			fread(&(cad->auto_label->suffix), sizeof(int), 1, fp);
			fread(check, 1, 2, fp);
			fread(check, 1, 1, fp);
			if(check[0]=='{')
				hierarchy_import(cad, cad->root, fp);
		}
	}
	fclose(fp);
	return cad;
}