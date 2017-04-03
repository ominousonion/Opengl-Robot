#ifndef _SPHERE_
#define _SPHERE_

class Sphere {
protected:
	unsigned int vertex_dim = 3;
	unsigned int vertex_amount;
	unsigned int index_length;
	unsigned int tc_length;
	float *vertex_data;
	float *normal_data;
	float *tc_data;
	float *color_data;
	unsigned int *index_data;
	struct center{
		float x;
		float y;
		float z;
	}position;
	struct color {
		float r = 1.0;
		float g = 1.0;
		float b = 0.0;
	}color;

public:
	Sphere();
	Sphere(float x, float y, float z, float a, float b, float c);
	center get_center();
	float* get_vertex();
	float* get_normal_data();
	float* get_color_data();
	float* get_tc_data();
	void set_color(float r, float g, float b);
	unsigned int* get_index();
	unsigned int get_index_length();
	unsigned int get_tc_length();
	unsigned int get_amount_of_vertex();
	unsigned int get_vertex_dim();
};
#endif