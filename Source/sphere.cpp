#include "sphere.h"
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

Sphere::Sphere() {};

Sphere::Sphere(float x, float y, float z, float a, float b, float c) {
	position.x = x;
	position.y = y;
	position.z = z;

	int n=0;
	int dtheta=1, dphi=5;
	int theta, phi;
	std::vector<float> vertex_vector;
	std::vector<float> normal_vector;
	std::vector<float> tc_vector;
	vertex_amount = ((int)(180 / dtheta)+1) * ((int)(360 / dphi)+1)+2;
	vertex_data = new float[vertex_amount * 3];
	for (theta = 0; theta <= 180; theta += dtheta) {
		for (phi = 0; phi <= 360; phi += dphi) {
			vertex_vector.push_back(position.x + sin((float)(theta)*M_PI / 180) * cos((float)(phi)*M_PI / 180)* a);
			vertex_vector.push_back(position.y + cos((float)(theta)*M_PI / 180) * b);
			vertex_vector.push_back(position.z + sin((float)(theta)*M_PI / 180) * sin((float)(phi)*M_PI / 180)* c);
			normal_vector.push_back(sin((float)(theta)*M_PI / 180) * cos((float)(phi)*M_PI / 180));
			normal_vector.push_back(cos((float)(theta)*M_PI / 180));
			normal_vector.push_back(sin((float)(theta)*M_PI / 180) * sin((float)(phi)*M_PI / 180));
			tc_vector.push_back(abs(phi -90.0f) / 360.f);
			tc_vector.push_back(abs(theta - 180.0f) / 180.0f);
		}
	}
	vertex_amount = vertex_vector.size()/3;
	vertex_data = new float[vertex_vector.size()];
	copy(vertex_vector.begin(), vertex_vector.end(), vertex_data);
	normal_data = new float[normal_vector.size()];
	copy(normal_vector.begin(), normal_vector.end(), normal_data);
	tc_length = tc_vector.size();
	tc_data = new float[tc_vector.size()];
	copy(tc_vector.begin(), tc_vector.end(), tc_data);
	//printf("(%f, %f, %f)\n", vertex_data[((int)(theta / dtheta) - 1)*(int)(360 / dphi) + (int)(phi / dphi) + 1], vertex_data[((int)(theta / dtheta) - 1)*(int)(360 / dphi) + (int)(phi / dphi) + 2], vertex_data[((int)(theta / dtheta) - 1)*(int)(360 / dphi) + (int)(phi / dphi) + 3]);
	
	int i;
	color_data = new float[vertex_amount * 3];
	for (i = 0; i < vertex_amount; i++) {
		color_data[3 * i] = color.r;
		color_data[3 * i + 1] = color.g;
		color_data[3 * i + 2] = color.b;
	}
	
	std::vector<unsigned int> index_vector;
	int vertex_per_ring = (int)(360 / dphi)+1;

	for (theta = 0; theta <= 180; theta += dtheta) {
		for (phi = 0; phi <= 360; phi += dphi) {
			if (phi == 360) {
				index_vector.push_back(((int)(theta / dtheta))*vertex_per_ring + (int)(phi / dphi));
				index_vector.push_back(((int)((theta + dtheta) / dtheta) )*vertex_per_ring + (int)((phi) / dphi));
				index_vector.push_back(((int)(theta / dtheta))*vertex_per_ring);
				index_vector.push_back(((int)((theta + dtheta) / dtheta) )*vertex_per_ring);
			}
			else {
				index_vector.push_back(((int)(theta / dtheta))*vertex_per_ring + (int)(phi / dphi));
				index_vector.push_back(((int)((theta + dtheta) / dtheta))*vertex_per_ring + (int)((phi) / dphi));
			}
		}
		index_vector.push_back((int)vertex_amount);
	}
	index_length = index_vector.size();
	index_data = new unsigned int[index_length];
	copy(index_vector.begin(), index_vector.end(), index_data);
};

float* Sphere::get_vertex() {
	return vertex_data;
}

unsigned int * Sphere::get_index() {
	return index_data;
}

unsigned int Sphere::get_amount_of_vertex() {
	return vertex_amount;
}

unsigned int Sphere::get_index_length() {
	return index_length;
}

unsigned int Sphere::get_tc_length() {
	return tc_length;
}

unsigned int Sphere::get_vertex_dim() {
	return vertex_dim;
}

Sphere::center Sphere::get_center() {
	return position;
}

void Sphere::set_color(float r, float g, float b) {
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

float * Sphere::get_normal_data() {
	return normal_data;
}

float * Sphere::get_tc_data() {
	return tc_data;
}

float* Sphere::get_color_data() {
	return color_data;
}