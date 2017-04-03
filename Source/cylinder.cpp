#include "cylinder.h"
#include "cmath"
#include "vector"
#include "stdio.h"
#define M_E        2.71828182845904523536   // e
#define M_LOG2E    1.44269504088896340736   // log2(e)
#define M_LOG10E   0.434294481903251827651  // log10(e)
#define M_LN2      0.693147180559945309417  // ln(2)
#define M_LN10     2.30258509299404568402   // ln(10)
#define M_PI       3.14159265358979323846   // pi
#define M_PI_2     1.57079632679489661923   // pi/2
#define M_PI_4     0.785398163397448309616  // pi/4
#define M_1_PI     0.318309886183790671538  // 1/pi
#define M_2_PI     0.636619772367581343076  // 2/pi
#define M_2_SQRTPI 1.12837916709551257390   // 2/sqrt(pi)

Cylinder::Cylinder() {};

Cylinder::Cylinder(float x, float y, float z, float radius_top, float radius_bottom, float height) {
	position.x = x;
	position.y = y;
	position.z = z;
	float height_2 = height / 2;
	int theta, dtheta=5;
	center top_circle,bottom_circle;
	std::vector<float> vertex_vector;

	top_circle.x = position.x;
	top_circle.y = position.y + height_2;
	top_circle.z = position.z;
	vertex_vector.push_back(top_circle.x);
	vertex_vector.push_back(top_circle.y);
	vertex_vector.push_back(top_circle.z);
	
	for (theta = 0; theta <= 360; theta += dtheta) {
		vertex_vector.push_back(top_circle.x + radius_top*cos((float)(theta)*M_PI / 180));
		vertex_vector.push_back(top_circle.y);
		vertex_vector.push_back(top_circle.z + radius_top*sin((float)(theta)*M_PI / 180));
	}

	bottom_circle.x = position.x;
	bottom_circle.y = position.y - height_2;
	bottom_circle.z = position.z;
	vertex_vector.push_back(bottom_circle.x);
	vertex_vector.push_back(bottom_circle.y);
	vertex_vector.push_back(bottom_circle.z);

	for (theta = 0; theta <= 360; theta += dtheta) {
		vertex_vector.push_back(bottom_circle.x + radius_bottom*cos((float)(theta)*M_PI / 180));
		vertex_vector.push_back(bottom_circle.y);
		vertex_vector.push_back(bottom_circle.z + radius_bottom*sin((float)(theta)*M_PI / 180));
	}

	vertex_amount = vertex_vector.size() / 3;
	vertex_data = new float[vertex_vector.size()];
	copy(vertex_vector.begin(), vertex_vector.end(), vertex_data);

	int i;
	color_data = new float[vertex_amount * 3];
	for (i = 0; i < vertex_amount; i++) {
		color_data[3 * i] = color.r;
		color_data[3 * i + 1] = color.g;
		color_data[3 * i + 2] = color.b;
	}

	std::vector<unsigned int> index_vector;
	int vertex_per_ring = (int)(360 / dtheta);

	for (theta = 0; theta <= 360; theta += dtheta) {
		index_vector.push_back((int)(theta / dtheta) + 1);
		index_vector.push_back((int)(theta / dtheta) + 1 + vertex_per_ring + 2);
	}
	index_vector.push_back(vertex_amount);
	
	for (theta = 0; theta <= 360; theta += dtheta) {
		index_vector.push_back(0);
		index_vector.push_back((int)(theta / dtheta) + 1);
	}
	index_vector.push_back(vertex_amount);

	for (theta = 0; theta <= 360; theta += dtheta) {
		index_vector.push_back(vertex_per_ring + 2);
		index_vector.push_back((int)(theta / dtheta) + 1 + vertex_per_ring + 2);
	}
	index_vector.push_back(vertex_amount);

	index_length = index_vector.size();
	index_data = new unsigned int[index_length];
	copy(index_vector.begin(), index_vector.end(), index_data);
}

float* Cylinder::get_vertex() {
	return vertex_data;
}

unsigned int * Cylinder::get_index() {
	return index_data;
}

unsigned int Cylinder::get_amount_of_vertex() {
	return vertex_amount;
}

unsigned int Cylinder::get_index_length() {
	return index_length;
}

unsigned int Cylinder::get_vertex_dim() {
	return vertex_dim;
}

Cylinder::center Cylinder::get_center() {
	return position;
}

void Cylinder::set_color(float r, float g, float b) {
	color.r = r;
	color.g = g;
	color.b = b;
	int i;
	color_data = new float[vertex_amount * 3];
	for (i = 0; i < vertex_amount; i++) {
		color_data[3 * i] = color.r;
		color_data[3 * i + 1] = color.g;
		color_data[3 * i + 2] = color.b;
	}
}

float* Cylinder::get_color_data() {
	return color_data;
}