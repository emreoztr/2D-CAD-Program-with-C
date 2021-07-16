#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CADlib.h"

Point2D *take_point(int p_count, int *return_size){
	Point2D *p;
	int i;
	char end='n';
	if(p_count!=0)
	{
		p=(Point2D*)calloc(p_count, sizeof(Point2D));
	
		for(i=0; i<p_count; i++){
			printf("Write x and y: ");
			scanf("%lf %lf", &(p[i].x), &(p[i].y));
		}
	}
	else{
		p=(Point2D*)calloc(1, sizeof(Point2D));
		printf("Write x and y: ");
		scanf("%lf %lf", &(p[i].x), &(p[i].y));
		for(i=1; end!='y'; i++){
			p=(Point2D*)realloc(p, (i+1)*sizeof(Point2D));
			printf("Write x and y: ");
			scanf("%lf %lf", &(p[i].x), &(p[i].y));
			printf("If you want to stop writing points, write 'y'\n");
			scanf(" %c ", &end);
		}
	}
	*return_size=i;
	return p;
}


Color choose_col(int i){
	Color c;
	switch(i){
		case 1:
			c.r=0.0;
			c.g=0.0;
			c.b=0.0;
			break;
		case 2:
			c.r=1.0;
			c.g=0.0;
			c.b=0.0;
			break;
		case 3:
			c.r=0.0;
			c.g=1.0;
			c.b=0.0;
			break;
		case 4:
			c.r=0.0;
			c.g=0.0;
			c.b=1.0;
			break;
		default:
			c.r=0.0;
			c.g=0.0;
			c.b=0.0;
			break;
	}
	return c;
}
int main(){
	int choice, cad_i=0, ent_i=0, cad_choice, p_count, size, b_cad1, b_cad2, ent_choice1, ent_choice2;
	double width, height, thick, r, start_ang, end_ang;
	char fill_check;
	Point2D *ps;
	Color col;
	CAD2D *cadfiles[30];
	Label *entities[100];
	Label a;
	char s[300];
	printf("SUMMER PROJECT DEMO\n");
	
	do{
		printf("1. Create new cad file\n2. Create entity\n3. Connect two CAD file\n4. Snap entities\n5. Measure the lenght between two entities\n6. Import as GTU file\n7. Export as GTU file\n8. Export as PS file\n0. EXIT\n\n");
		scanf("%d", &choice);
		if(choice==1){
			printf("1. Create default\n2. Create with custom edges\n");
			scanf("%d", &choice);
			if(choice==1){
				cadfiles[cad_i++]=c2d_start_default();
			}
			if(choice==2){
				scanf("%lf %lf", &width, &height);
				cadfiles[cad_i++]=c2d_start_halfcustom(width, height);
			}
		}

		else if(choice==2){
			printf("Choose the cad file you would want to add entity: \n");
			for(int i=0; i<cad_i; i++){
				printf("%d. CAD\n", (i+1));
			}
			scanf("%d", &cad_choice);
			cad_choice--;
			printf("Choose color\n1. Black\n2. Red\n3. Green\n4. Blue\n");
			scanf("%d", &choice);
			col=choose_col(choice);
			printf("Write thickness: ");
			scanf("%lf", &thick);
			printf("Write 'f' if you want to fill, if you not write 'e': ");
			scanf(" %c", &fill_check);
			printf("1. Create point\n2. Create line\n3. Create arc\n4. Create circle\n5. Create polyline\n6. Create polygon\n7. Create text\n8. Create ellipse\n");
			scanf("%d", &choice);
			if(choice==1){
				ps=take_point(1, &p_count);
				entities[ent_i++]=c2d_add_point(cadfiles[cad_choice], *ps, &col, thick);
			}
			else if(choice==2){
				ps=take_point(2, &p_count);
				entities[ent_i++]=c2d_add_line(cadfiles[cad_choice], ps, &col, thick);
			}
			else if(choice==3){
				printf("Choose center: ");
				ps=take_point(1, &p_count);
				printf("Write r, start angle and end angle in degrees\n");
				scanf("%lf %lf %lf", &r, &start_ang, &end_ang);
				entities[ent_i++]=c2d_add_arc(cadfiles[cad_choice], *ps, r, start_ang, end_ang, &col, thick, fill_check);
			}
			else if(choice==4){
				printf("Choose center: ");
				ps=take_point(1, &p_count);
				printf("Write r: ");
				scanf("%lf", &r);
				entities[ent_i++]=c2d_add_circle(cadfiles[cad_choice], *ps, r, &col, thick, fill_check);
			}
			else if(choice==5){
				printf("Write point count: ");
				scanf("%d", &p_count);
				ps=take_point(p_count, &p_count);
				entities[ent_i++]=c2d_add_polyline(cadfiles[cad_choice], ps, p_count, thick, &col);
			}
			else if(choice==6){
				printf("Write point count: ");
				scanf("%d", &p_count);
				ps=take_point(p_count, &p_count);
				entities[ent_i++]=c2d_add_polygon(cadfiles[cad_choice], ps, p_count, &col, thick, fill_check);
			}
			else if(choice==7){
				ps=take_point(1, &p_count);
				printf("Write your text: ");
				scanf(" %[^\n]", s);
				printf("Write text size: ");
				scanf("%d", &size);
				entities[ent_i++]=c2d_add_text(cadfiles[cad_choice], *ps, s, size, &col);
			}
			else if(choice==8){
				ps=take_point(1, &p_count);
				printf("Write width and height of ellipse: ");
				scanf("%lf %lf", &width, &height);
				entities[ent_i++]=c2d_add_ellipse(cadfiles[cad_choice], *ps, width, height, &col, thick, fill_check);
			}
		}
		else if(choice==3){
			printf("Choose the cad file you would want to bound with another cad: \n");
			for(int i=0; i<cad_i; i++){
				printf("%d. CAD\n", (i+1));
			}
			scanf("%d", &b_cad1);
			printf("Choose the second cad file you would want to bound with another cad: \n");
			for(int i=0; i<cad_i; i++){
				printf("%d. CAD\n", (i+1));
			}
			scanf("%d", &b_cad2);

			if(b_cad1!=b_cad2){
				b_cad1--;
				b_cad2--;
				cadfiles[b_cad2]->root=c2d_create_hierarchy_child(cadfiles[b_cad1], cadfiles[b_cad1]->root);
				cadfiles[b_cad2]->auto_label=cadfiles[b_cad1]->auto_label;
			}
		}
		else if(choice==4){
			printf("Choose the cad file that holds entities you will choose: \n");
			for(int i=0; i<cad_i; i++){
				printf("%d. CAD\n", (i+1));
			}
			scanf("%d", &cad_choice);
			cad_choice--;
			printf("Choose first and second entity:\n");
			for(int k=0; k<ent_i; k++){
				printf("%d. %s\n", (k+1), entities[k]->name);
			}
			scanf("%d %d", &ent_choice1, &ent_choice2);
			ent_choice1--;
			ent_choice2--;

			c2d_snap(cadfiles[cad_choice], entities[ent_choice1], entities[ent_choice2]);
		}
		else if(choice==5){
			printf("Choose the cad file that holds entities you will choose: \n");
			for(int i=0; i<cad_i; i++){
				printf("%d. CAD\n", (i+1));
			}
			scanf("%d", &cad_choice);
			cad_choice--;
			printf("Choose first and second entity:\n");
			for(int k=0; k<ent_i; k++){
				printf("%d. %s\n", (k+1), entities[k]->name);
			}
			scanf("%d %d", &ent_choice1, &ent_choice2);
			ent_choice1--;
			ent_choice2--;

			printf("Measure: %lf",c2d_measure(cadfiles[cad_choice], entities[ent_choice1], entities[ent_choice2]));
		}
		else if(choice==6){
			printf("Write filename that will be imported: ");
			scanf(" %[^\n]", s);
			cadfiles[cad_i++]=c2d_import(s, "GTU");
		}
		else if(choice==7){
			printf("Choose the cad file you would want to export: \n");
			for(int i=0; i<cad_i; i++){
				printf("%d. CAD\n", (i+1));
			}
			scanf("%d", &cad_choice);
			cad_choice--;
			printf("Write filename that will be exported as GTU file: ");
			scanf(" %[^\n]", s);
			c2d_export(cadfiles[cad_choice], s, "GTU");
		}
		else if(choice==8){
			printf("Choose the cad file you would want to export: \n");
			for(int i=0; i<cad_i; i++){
				printf("%d. CAD\n", (i+1));
			}
			scanf("%d", &cad_choice);
			cad_choice--;
			printf("Write filename that will be exported as PS file: ");
			scanf(" %[^\n]", s);
			c2d_export(cadfiles[cad_choice], s, "EPS");
		}

	}while(choice!=0);
}