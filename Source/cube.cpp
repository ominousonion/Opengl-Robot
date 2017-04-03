#include "cube.h"
#include "stdio.h"

Cube::Cube() {};

Cube::Cube(float x, float y, float z, float length_x, float length_y, float length_z) {
	position.x = x;
	position.y = y;
	position.z = z;

	int i;
	float length_x_2 = length_x / 2;
	float length_y_2 = length_y / 2;
	float length_z_2 = length_z / 2;

	float data[24] = {
		position.x - length_x_2, position.y - length_y_2, position.z - length_z_2,
		position.x - length_x_2, position.y - length_y_2, position.z + length_z_2,
		position.x - length_x_2, position.y + length_y_2, position.z - length_z_2,
		position.x - length_x_2, position.y + length_y_2, position.z + length_z_2,
		position.x + length_x_2, position.y - length_y_2, position.z - length_z_2,
		position.x + length_x_2, position.y - length_y_2, position.z + length_z_2,
		position.x + length_x_2, position.y + length_y_2, position.z - length_z_2,
		position.x + length_x_2, position.y + length_y_2, position.z + length_z_2
	};

	vertex_data = new float[vertex_amount * vertex_dim];
	for (i = 0; i < vertex_amount*vertex_dim; i++) {
		vertex_data[i] = data[i];
	}

	unsigned int index[21] = {
		0,1,2,3,6,7,4,5,0,1,vertex_amount,
		0,2,4,6,vertex_amount,
		1,3,5,7,vertex_amount
	};

	index_data = new unsigned int[index_length];
	for (i = 0; i < index_length; i++) {
		index_data[i] = index[i];
	}
	
	color_data = new float[vertex_amount * 3];
	for (i = 0; i < vertex_amount; i++) {
		color_data[3 * i] = color.r;
		color_data[3 * i + 1] = color.g;
		color_data[3 * i + 2] = color.b;
	}
}

float* Cube::get_vertex() {
	return vertex_data;
}

unsigned int * Cube::get_index() {
	return index_data;
}

unsigned int Cube::get_amount_of_vertex() {
	return vertex_amount;
}

unsigned int Cube::get_index_length() {
	return index_length;
}

unsigned int Cube::get_vertex_dim() {
	return vertex_dim;
}

Cube::center Cube::get_center() {
	return position;
}

void Cube::set_color(float r, float g, float b) {
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

float* Cube::get_color_data() {
	return color_data;
}