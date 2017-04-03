#ifndef _CYLINDER_
#define _CYLINDER_

class Cylinder {
protected:
	unsigned int vertex_dim = 3;
	unsigned int vertex_amount;
	unsigned int index_length;
	float *vertex_data;
	float *color_data;
	unsigned int *index_data;
	struct center {
		float x;
		float y;
		float z;
	}position;
	struct color {
		float r = 0.5;
		float g = 0.5;
		float b = 0.5;
	}color;

public:
	Cylinder();
	Cylinder(float x, float y, float z, float radius_top, float radius_bottom, float height);
	center get_center();
	float* get_vertex();
	float* get_color_data();
	void set_color(float r, float g, float b);
	unsigned int* get_index();
	unsigned int get_index_length();
	unsigned int get_amount_of_vertex();
	unsigned int get_vertex_dim();
};
#endif