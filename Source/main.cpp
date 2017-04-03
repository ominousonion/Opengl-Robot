#include "../Include/Common.h"
#include "sphere.h"
#include "cube.h"
#include "cylinder.h"

//For GLUT to handle 
#define MENU_TIMER_START 1
#define MENU_TIMER_STOP 2
#define MENU_EXIT 3
#define MENU_IDLE 4
#define MENU_GREETING 5
#define MENU_BOW 6

//GLUT timer variable
GLubyte timer_cnt = 0;
bool timer_enabled = true;
unsigned int timer_speed = 16;
int mode = 0; //0:idle 1:greeting 2:bow
int oldTimeSinceStart;
int timeSinceStart, deltaTime;

int animationOldTimeSinceStart;
int animationTimeSinceStart, animationDeltaTime;

bool greeting_phase_1, greeting_phase_2, greeting_phase_3;
float greeting_phase_1_arm_angle, greeting_phase_1_hand_angle;
float greeting_phase_2_hand_angle;
float clock;

bool bow_phase_1, bow_phase_2, bow_phase_3;
float bow_1_body_angle, bow_1_leg_angle;

bool keyStates[4] = { false, false, false, false };//0:up 1:left 2:down 3:right
using namespace glm;
using namespace std;

mat4 view;					// V of MVP, viewing matrix
mat4 projection;			// P of MVP, projection matrix
mat4 model;					// M of MVP, model matrix
mat4 mvp;

GLint um4mvp;			// shader mvp uniform id 
GLuint program;			// shader program id
GLint texture_um4mvp;
GLuint program_with_texture;

GLuint buffer1;
GLuint buffer2;
GLuint color_buffer;
GLuint texture;

class Robot {
protected:
	struct center {
		float x;
		float y;
		float z;
	}position;

	Sphere eye_L, eye_R;
	Sphere head;
	Cylinder hat_up, hat_down;
	Sphere body;
	Sphere joint_arm_L_U, joint_arm_L_D, joint_arm_R_U, joint_arm_R_D;
	Cube arm_L_U, arm_L_D, arm_R_U, arm_R_D;
	Sphere joint_leg_L_U, joint_leg_L_D, joint_leg_R_U, joint_leg_R_D;
	Cube leg_L_U, leg_L_D, leg_R_U, leg_R_D;
	Cylinder shoes_L_U, shoes_R_U;
	Sphere shoes_L_D, shoes_R_D;
	Sphere joint_hand_L, joint_hand_R;
	Cube hand_L_1, hand_L_2, hand_L_3, hand_L_4, hand_R_1, hand_R_2, hand_R_3, hand_R_4;
	Cylinder joint_finger_L_1, joint_finger_L_2, joint_finger_R_1, joint_finger_R_2;

	mat4 model_matrix;

	void draw_part_normal(float *vertex_data, unsigned int *index_data, float *color_data, unsigned int amount, unsigned int length, unsigned int dim);
	void draw_part_texture(float *vertex_data, unsigned int *index_data, float *tc_data, unsigned int amount, unsigned int tc_length, unsigned int length, unsigned int dim);
public:
	mat4 model_body;
	mat4 model_head;
	mat4 model_eye_L, model_eye_R;
	mat4 model_hat_down, model_hat_up;
	mat4 model_arm_L_U, model_arm_L_D, model_arm_R_U, model_arm_R_D;
	mat4 model_joint_arm_L_U, model_joint_arm_L_D, model_joint_arm_R_U, model_joint_arm_R_D;
	mat4 model_leg_L_U, model_leg_L_D, model_leg_R_U, model_leg_R_D;
	mat4 model_joint_leg_L_U, model_joint_leg_L_D, model_joint_leg_R_U, model_joint_leg_R_D;
	mat4 model_shoes_L_U, model_shoes_R_U, model_shoes_L_D, model_shoes_R_D;
	mat4 model_joint_hand_L, model_joint_hand_R;
	mat4 model_hand_L_1, model_hand_L_2, model_hand_L_3, model_hand_L_4, model_hand_R_1, model_hand_R_2, model_hand_R_3, model_hand_R_4;
	mat4 model_joint_finger_L_1, model_joint_finger_L_2, model_joint_finger_R_1, model_joint_finger_R_2;

	Robot();
	Robot(float x, float y, float z);
	void prepare_modle();
	void draw();
};

Robot::Robot() {}

Robot::Robot(float x, float y, float z) {
	position.x = x;
	position.y = y;
	position.z = z;

	body = Sphere(position.x, position.y, position.z, 0.55, 0.75, 0.55);
	body.set_color(1.0, 1.0, 1.0);

	head = Sphere(position.x, position.y, position.z, 0.55, 0.5, 0.55);
	head.set_color(1.0, 1.0, 1.0);

	hat_down = Cylinder(position.x, position.y, position.z, 0.6, 0.6, 0.005);
	hat_down.set_color(0.0, 0.0, 0.0);

	hat_up = Cylinder(position.x, position.y, position.z, 0.25, 0.25, 0.6);
	hat_up.set_color(0.0, 0.0, 0.0);

	eye_L = Sphere(position.x, position.y, position.z, 0.1, 0.1, 0.1);
	eye_L.set_color(1.0, 1.0, 0.0);

	eye_R = Sphere(position.x, position.y, position.z, 0.1, 0.1, 0.1);
	eye_R.set_color(1.0, 1.0, 0.0);

	joint_arm_L_U = Sphere(position.x, position.y, position.z, 0.1, 0.1, 0.1);
	joint_arm_L_U.set_color(0.0, 0.0, 0.0);

	arm_L_U = Cube(position.x, position.y, position.z, 0.1, 0.4, 0.1);
	arm_L_U.set_color(0.0, 0.0, 0.0);

	joint_arm_R_U = Sphere(position.x, position.y, position.z, 0.1, 0.1, 0.1);
	joint_arm_R_U.set_color(0.0, 0.0, 0.0);

	arm_R_U = Cube(position.x, position.y, position.z, 0.1, 0.4, 0.1);
	arm_R_U.set_color(0.0, 0.0, 0.0);

	joint_arm_L_D = Sphere(position.x, position.y, position.z, 0.075, 0.075, 0.075);
	joint_arm_L_D.set_color(0.0, 0.0, 0.0);

	arm_L_D = Cube(position.x, position.y, position.z, 0.1, 0.5, 0.1);
	arm_L_D.set_color(0.0, 0.0, 0.0);

	joint_arm_R_D = Sphere(position.x, position.y, position.z, 0.075, 0.075, 0.075);
	joint_arm_R_D.set_color(0.0, 0.0, 0.0);

	arm_R_D = Cube(position.x, position.y, position.z, 0.1, 0.5, 0.1);
	arm_R_D.set_color(0.0, 0.0, 0.0);

	joint_leg_L_U = Sphere(position.x, position.y, position.z, 0.1, 0.1, 0.1);
	joint_leg_L_U.set_color(0.0, 0.0, 0.0);

	leg_L_U = Cube(position.x, position.y, position.z, 0.1, 0.4, 0.1);
	leg_L_U.set_color(0.0, 0.0, 0.0);

	joint_leg_R_U = Sphere(position.x, position.y, position.z, 0.1, 0.1, 0.1);
	joint_leg_R_U.set_color(0.0, 0.0, 0.0);

	leg_R_U = Cube(position.x, position.y, position.z, 0.1, 0.4, 0.1);
	leg_R_U.set_color(0.0, 0.0, 0.0);

	joint_leg_L_D = Sphere(position.x, position.y, position.z, 0.075, 0.075, 0.075);
	joint_leg_L_D.set_color(0.0, 0.0, 0.0);

	leg_L_D = Cube(position.x, position.y, position.z, 0.1, 0.5, 0.1);
	leg_L_D.set_color(0.0, 0.0, 0.0);

	joint_leg_R_D = Sphere(position.x, position.y, position.z, 0.075, 0.075, 0.075);
	joint_leg_R_D.set_color(0.0, 0.0, 0.0);

	leg_R_D = Cube(position.x, position.y, position.z, 0.1, 0.5, 0.1);
	leg_R_D.set_color(0.0, 0.0, 0.0);

	shoes_L_U = Cylinder(position.x, position.y, position.z, 0.1, 0.1, 0.2);
	shoes_L_U.set_color(0.0, 0.0, 0.0);

	shoes_R_U = Cylinder(position.x, position.y, position.z, 0.1, 0.1, 0.2);
	shoes_R_U.set_color(0.0, 0.0, 0.0);

	shoes_L_D = Sphere(position.x, position.y, position.z, 0.125, 0.075, 0.25);
	shoes_L_D.set_color(0.0, 0.0, 0.0);

	shoes_R_D = Sphere(position.x, position.y, position.z, 0.125, 0.075, 0.25);
	shoes_R_D.set_color(0.0, 0.0, 0.0);

	joint_hand_L = Sphere(position.x, position.y, position.z, 0.125, 0.125, 0.125);
	joint_hand_L.set_color(1.0, 0.0, 0.0);

	hand_L_1 = Cube(position.x, position.y, position.z, 0.02, 0.1, 0.15);
	hand_L_1.set_color(1.0, 1.0, 1.0);

	joint_finger_L_1 = Cylinder(position.x, position.y, position.z, 0.02, 0.02, 0.15);
	joint_finger_L_1.set_color(1.0, 1.0, 1.0);

	hand_L_2 = Cube(position.x, position.y, position.z, 0.02, 0.1, 0.15);
	hand_L_2.set_color(1.0, 1.0, 1.0);

	hand_L_3 = Cube(position.x, position.y, position.z, 0.02, 0.1, 0.15);
	hand_L_3.set_color(1.0, 1.0, 1.0);

	joint_finger_L_2 = Cylinder(position.x, position.y, position.z, 0.02, 0.02, 0.15);
	joint_finger_L_2.set_color(1.0, 1.0, 1.0);

	hand_L_4 = Cube(position.x, position.y, position.z, 0.02, 0.1, 0.15);
	hand_L_4.set_color(1.0, 1.0, 1.0);

	joint_hand_R = Sphere(position.x, position.y, position.z, 0.125, 0.125, 0.125);
	joint_hand_R.set_color(1.0, 0.0, 0.0);

	hand_R_1 = Cube(position.x, position.y, position.z, 0.02, 0.1, 0.15);
	hand_R_1.set_color(1.0, 1.0, 1.0);

	joint_finger_R_1 = Cylinder(position.x, position.y, position.z, 0.02, 0.02, 0.15);
	joint_finger_R_1.set_color(1.0, 1.0, 1.0);

	hand_R_2 = Cube(position.x, position.y, position.z, 0.02, 0.1, 0.15);
	hand_R_2.set_color(1.0, 1.0, 1.0);

	hand_R_3 = Cube(position.x, position.y, position.z, 0.02, 0.1, 0.15);
	hand_R_3.set_color(1.0, 1.0, 1.0);

	joint_finger_R_2 = Cylinder(position.x, position.y, position.z, 0.02, 0.02, 0.15);
	joint_finger_R_2.set_color(1.0, 1.0, 1.0);

	hand_R_4 = Cube(position.x, position.y, position.z, 0.02, 0.1, 0.15);
	hand_R_4.set_color(1.0, 1.0, 1.0);

}

void Robot::draw() {
	model_matrix = model_body;
	draw_part_texture(body.get_vertex(), body.get_index(), body.get_tc_data(), body.get_amount_of_vertex(), body.get_tc_length(), body.get_index_length(), body.get_vertex_dim());

	model_matrix = model_head;
	draw_part_normal(head.get_vertex(), head.get_index(), head.get_color_data(), head.get_amount_of_vertex(), head.get_index_length(), head.get_vertex_dim());

	model_matrix = model_eye_L;
	draw_part_normal(eye_L.get_vertex(), eye_L.get_index(), eye_L.get_color_data(), eye_L.get_amount_of_vertex(), eye_L.get_index_length(), eye_L.get_vertex_dim());

	model_matrix = model_eye_R;
	draw_part_normal(eye_R.get_vertex(), eye_R.get_index(), eye_R.get_color_data(), eye_R.get_amount_of_vertex(), eye_R.get_index_length(), eye_R.get_vertex_dim());

	model_matrix = model_hat_up;
	draw_part_normal(hat_up.get_vertex(), hat_up.get_index(), hat_up.get_color_data(), hat_up.get_amount_of_vertex(), hat_up.get_index_length(), hat_up.get_vertex_dim());

	model_matrix = model_hat_down;
	draw_part_normal(hat_down.get_vertex(), hat_down.get_index(), hat_down.get_color_data(), hat_down.get_amount_of_vertex(), hat_down.get_index_length(), hat_down.get_vertex_dim());

	model_matrix = model_joint_arm_L_U;
	draw_part_normal(joint_arm_L_U.get_vertex(), joint_arm_L_U.get_index(), joint_arm_L_U.get_color_data(), joint_arm_L_U.get_amount_of_vertex(), joint_arm_L_U.get_index_length(), joint_arm_L_U.get_vertex_dim());

	model_matrix = model_arm_L_U;
	draw_part_normal(arm_L_U.get_vertex(), arm_L_U.get_index(), arm_L_U.get_color_data(), arm_L_U.get_amount_of_vertex(), arm_L_U.get_index_length(), arm_L_U.get_vertex_dim());

	model_matrix = model_joint_arm_R_U;
	draw_part_normal(joint_arm_R_U.get_vertex(), joint_arm_R_U.get_index(), joint_arm_R_U.get_color_data(), joint_arm_R_U.get_amount_of_vertex(), joint_arm_R_U.get_index_length(), joint_arm_R_U.get_vertex_dim());

	model_matrix = model_arm_R_U;
	draw_part_normal(arm_R_U.get_vertex(), arm_R_U.get_index(), arm_R_U.get_color_data(), arm_R_U.get_amount_of_vertex(), arm_R_U.get_index_length(), arm_R_U.get_vertex_dim());

	model_matrix = model_joint_arm_L_D;
	draw_part_normal(joint_arm_L_D.get_vertex(), joint_arm_L_D.get_index(), joint_arm_L_D.get_color_data(), joint_arm_L_D.get_amount_of_vertex(), joint_arm_L_D.get_index_length(), joint_arm_L_D.get_vertex_dim());

	model_matrix = model_arm_L_D;
	draw_part_normal(arm_L_D.get_vertex(), arm_L_D.get_index(), arm_L_D.get_color_data(), arm_L_D.get_amount_of_vertex(), arm_L_D.get_index_length(), arm_L_D.get_vertex_dim());

	model_matrix = model_joint_arm_R_D;
	draw_part_normal(joint_arm_R_D.get_vertex(), joint_arm_R_D.get_index(), joint_arm_R_D.get_color_data(), joint_arm_R_D.get_amount_of_vertex(), joint_arm_R_D.get_index_length(), joint_arm_R_D.get_vertex_dim());

	model_matrix = model_arm_R_D;
	draw_part_normal(arm_R_D.get_vertex(), arm_R_D.get_index(), arm_R_D.get_color_data(), arm_R_D.get_amount_of_vertex(), arm_R_D.get_index_length(), arm_R_D.get_vertex_dim());

	model_matrix = model_joint_leg_L_U;
	draw_part_normal(joint_leg_L_U.get_vertex(), joint_leg_L_U.get_index(), joint_leg_L_U.get_color_data(), joint_leg_L_U.get_amount_of_vertex(), joint_leg_L_U.get_index_length(), joint_leg_L_U.get_vertex_dim());

	model_matrix = model_leg_L_U;
	draw_part_normal(leg_L_U.get_vertex(), leg_L_U.get_index(), leg_L_U.get_color_data(), leg_L_U.get_amount_of_vertex(), leg_L_U.get_index_length(), leg_L_U.get_vertex_dim());

	model_matrix = model_joint_leg_R_U;
	draw_part_normal(joint_leg_R_U.get_vertex(), joint_leg_R_U.get_index(), joint_leg_R_U.get_color_data(), joint_leg_R_U.get_amount_of_vertex(), joint_leg_R_U.get_index_length(), joint_leg_R_U.get_vertex_dim());

	model_matrix = model_leg_R_U;
	draw_part_normal(leg_R_U.get_vertex(), leg_R_U.get_index(), leg_R_U.get_color_data(), leg_R_U.get_amount_of_vertex(), leg_R_U.get_index_length(), leg_R_U.get_vertex_dim());

	model_matrix = model_joint_leg_L_D;
	draw_part_normal(joint_leg_L_D.get_vertex(), joint_leg_L_D.get_index(), joint_leg_L_D.get_color_data(), joint_leg_L_D.get_amount_of_vertex(), joint_leg_L_D.get_index_length(), joint_leg_L_D.get_vertex_dim());

	model_matrix = model_leg_L_D;
	draw_part_normal(leg_L_D.get_vertex(), leg_L_D.get_index(), leg_L_D.get_color_data(), leg_L_D.get_amount_of_vertex(), leg_L_D.get_index_length(), leg_L_D.get_vertex_dim());

	model_matrix = model_joint_leg_R_D;
	draw_part_normal(joint_leg_R_D.get_vertex(), joint_leg_R_D.get_index(), joint_leg_R_D.get_color_data(), joint_leg_R_D.get_amount_of_vertex(), joint_leg_R_D.get_index_length(), joint_leg_R_D.get_vertex_dim());

	model_matrix = model_leg_R_D;
	draw_part_normal(leg_R_D.get_vertex(), leg_R_D.get_index(), leg_R_D.get_color_data(), leg_R_D.get_amount_of_vertex(), leg_R_D.get_index_length(), leg_R_D.get_vertex_dim());

	model_matrix = model_shoes_L_U;
	draw_part_normal(shoes_L_U.get_vertex(), shoes_L_U.get_index(), shoes_L_U.get_color_data(), shoes_L_U.get_amount_of_vertex(), shoes_L_U.get_index_length(), shoes_L_U.get_vertex_dim());

	model_matrix = model_shoes_L_D;
	draw_part_normal(shoes_L_D.get_vertex(), shoes_L_D.get_index(), shoes_L_D.get_color_data(), shoes_L_D.get_amount_of_vertex(), shoes_L_D.get_index_length(), shoes_L_D.get_vertex_dim());

	model_matrix = model_shoes_R_U;
	draw_part_normal(shoes_R_U.get_vertex(), shoes_R_U.get_index(), shoes_R_U.get_color_data(), shoes_R_U.get_amount_of_vertex(), shoes_R_U.get_index_length(), shoes_R_U.get_vertex_dim());

	model_matrix = model_shoes_R_D;
	draw_part_normal(shoes_R_D.get_vertex(), shoes_R_D.get_index(), shoes_R_D.get_color_data(), shoes_R_D.get_amount_of_vertex(), shoes_R_D.get_index_length(), shoes_R_D.get_vertex_dim());

	model_matrix = model_joint_hand_L;
	draw_part_normal(joint_hand_L.get_vertex(), joint_hand_L.get_index(), joint_hand_L.get_color_data(), joint_hand_L.get_amount_of_vertex(), joint_hand_L.get_index_length(), joint_hand_L.get_vertex_dim());

	model_matrix = model_joint_hand_R;
	draw_part_normal(joint_hand_R.get_vertex(), joint_hand_R.get_index(), joint_hand_R.get_color_data(), joint_hand_R.get_amount_of_vertex(), joint_hand_R.get_index_length(), joint_hand_R.get_vertex_dim());

	model_matrix = model_hand_L_1;
	draw_part_normal(hand_L_1.get_vertex(), hand_L_1.get_index(), hand_L_1.get_color_data(), hand_L_1.get_amount_of_vertex(), hand_L_1.get_index_length(), hand_L_1.get_vertex_dim());

	model_matrix = model_joint_finger_L_1;
	draw_part_normal(joint_finger_L_1.get_vertex(), joint_finger_L_1.get_index(), joint_finger_L_1.get_color_data(), joint_finger_L_1.get_amount_of_vertex(), joint_finger_L_1.get_index_length(), joint_finger_L_1.get_vertex_dim());

	model_matrix = model_hand_L_2;
	draw_part_normal(hand_L_2.get_vertex(), hand_L_2.get_index(), hand_L_2.get_color_data(), hand_L_2.get_amount_of_vertex(), hand_L_2.get_index_length(), hand_L_2.get_vertex_dim());

	model_matrix = model_hand_L_3;
	draw_part_normal(hand_L_3.get_vertex(), hand_L_3.get_index(), hand_L_3.get_color_data(), hand_L_3.get_amount_of_vertex(), hand_L_3.get_index_length(), hand_L_3.get_vertex_dim());

	model_matrix = model_joint_finger_L_2;
	draw_part_normal(joint_finger_L_2.get_vertex(), joint_finger_L_2.get_index(), joint_finger_L_2.get_color_data(), joint_finger_L_2.get_amount_of_vertex(), joint_finger_L_2.get_index_length(), joint_finger_L_2.get_vertex_dim());

	model_matrix = model_hand_L_4;
	draw_part_normal(hand_L_4.get_vertex(), hand_L_4.get_index(), hand_L_4.get_color_data(), hand_L_4.get_amount_of_vertex(), hand_L_4.get_index_length(), hand_L_4.get_vertex_dim());

	model_matrix = model_hand_R_1;
	draw_part_normal(hand_R_1.get_vertex(), hand_R_1.get_index(), hand_R_1.get_color_data(), hand_R_1.get_amount_of_vertex(), hand_R_1.get_index_length(), hand_R_1.get_vertex_dim());

	model_matrix = model_joint_finger_R_1;
	draw_part_normal(joint_finger_R_1.get_vertex(), joint_finger_R_1.get_index(), joint_finger_R_1.get_color_data(), joint_finger_R_1.get_amount_of_vertex(), joint_finger_R_1.get_index_length(), joint_finger_R_1.get_vertex_dim());

	model_matrix = model_hand_R_2;
	draw_part_normal(hand_R_2.get_vertex(), hand_R_2.get_index(), hand_R_2.get_color_data(), hand_R_2.get_amount_of_vertex(), hand_R_2.get_index_length(), hand_R_2.get_vertex_dim());

	model_matrix = model_hand_R_3;
	draw_part_normal(hand_R_3.get_vertex(), hand_R_3.get_index(), hand_R_3.get_color_data(), hand_R_3.get_amount_of_vertex(), hand_R_3.get_index_length(), hand_R_3.get_vertex_dim());

	model_matrix = model_joint_finger_R_2;
	draw_part_normal(joint_finger_R_2.get_vertex(), joint_finger_R_2.get_index(), joint_finger_R_2.get_color_data(), joint_finger_R_2.get_amount_of_vertex(), joint_finger_R_2.get_index_length(), joint_finger_R_2.get_vertex_dim());

	model_matrix = model_hand_R_4;
	draw_part_normal(hand_R_4.get_vertex(), hand_R_4.get_index(), hand_R_4.get_color_data(), hand_R_4.get_amount_of_vertex(), hand_R_4.get_index_length(), hand_R_4.get_vertex_dim());

}

void Robot::draw_part_normal(float *vertex_data, unsigned int *index_data, float *color_data, unsigned int amount, unsigned int length, unsigned int dim) {

	glBindBuffer(GL_ARRAY_BUFFER, buffer1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data)*amount * dim, vertex_data, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_data)*amount * dim, color_data, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_data)*length, index_data, GL_STATIC_DRAW);

	// Tell openGL to use the shader program we created before
	glUseProgram(program);

	//Calculate mvp matrix by ( projection * view * model ) 

	mvp = projection * view * model_matrix;
	// Transfer value of mvp to both shader's inner variable 'um4mvp'; 
	glUniformMatrix4fv(um4mvp, 1, GL_FALSE, value_ptr(mvp));

	// Tell openGL to draw the vertex array we had binded before
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(amount);
	glDrawElements(GL_TRIANGLE_STRIP, length, GL_UNSIGNED_INT, 0);
}

void Robot::draw_part_texture(float *vertex_data, unsigned int *index_data, float *tc_data, unsigned int amount, unsigned int tc_length, unsigned int length, unsigned int dim) {

	glBindBuffer(GL_ARRAY_BUFFER, buffer1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data)*amount * dim, vertex_data, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tc_data)*tc_length, tc_data, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_data)*length, index_data, GL_STATIC_DRAW);

	// Bind the texture to Texture Image Unit 0
	glActiveTexture(GL_TEXTURE0);
	// glBindTexturewill bind Texture Objects to Texture Image Units!
	glBindTexture(GL_TEXTURE_2D, texture);
	// Set the sampler2D uniform value to 0 (Texture Image Unit 0)
	GLuint texture_location = glGetUniformLocation(program_with_texture, "texture");
	glUniform1i(texture_location, 0);

	// Tell openGL to use the shader program we created before
	glUseProgram(program_with_texture);

	//Calculate mvp matrix by ( projection * view * model ) 

	mvp = projection * view * model_matrix;
	// Transfer value of mvp to both shader's inner variable 'um4mvp'; 
	glUniformMatrix4fv(texture_um4mvp, 1, GL_FALSE, value_ptr(mvp));

	// Tell openGL to draw the vertex array we had binded before
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(amount);
	glDrawElements(GL_TRIANGLE_STRIP, length, GL_UNSIGNED_INT, 0);
}

void Robot::prepare_modle() {
	model_head = translate(model_body, vec3(0.0f, 1.0f, 0.0f));
	model_hat_down = translate(model_head, vec3(0.0f, 0.45f, 0.0f));
	model_hat_up = translate(model_hat_down, vec3(0.0f, 0.15f, 0.0f));
	model_eye_L = translate(model_head, vec3(0.25f, 0.2f, -0.35f));
	model_eye_R = translate(model_head, vec3(-0.25f, 0.2f, -0.35f));

	model_joint_arm_L_U = translate(model_body, vec3(0.4f, 0.5f, 0.0f));
	model_arm_L_U = translate(model_joint_arm_L_U, vec3(0.2*sin(radians(40.0f)), -0.2*cos(radians(40.0f)), 0.0f))*rotate(mat4(1.0f), radians(40.0f), vec3(0, 0, 1));
	model_joint_arm_R_U = translate(model_body, vec3(-0.4f, 0.5f, 0.0f));
	model_arm_R_U = translate(model_joint_arm_R_U, vec3(0.2*sin(radians(-40.0f)), -0.2*cos(radians(-40.0f)), 0.0f))*rotate(mat4(1.0f), radians(-40.0f), vec3(0, 0, 1));
	model_joint_arm_L_D = translate(model_arm_L_U, vec3(0.0f, -0.2f, 0.0f));
	model_arm_L_D = translate(model_joint_arm_L_D, vec3(0.25*sin(radians(-30.0f)), -0.25*cos(radians(-30.0f)), 0.0f))*rotate(mat4(1.0f), radians(-30.0f), vec3(0, 0, 1));
	model_arm_L_D = translate(model_arm_L_D, vec3(0.0, 0.0, -0.25*sin(radians(30.0f))))*rotate(mat4(1.0f), radians(30.0f), vec3(1, 0, 0));
	model_joint_arm_R_D = translate(model_arm_R_U, vec3(0.0f, -0.2f, 0.0f));
	model_arm_R_D = translate(model_joint_arm_R_D, vec3(0.25*sin(radians(30.0f)), -0.25*cos(radians(30.0f)), 0.0f))*rotate(mat4(1.0f), radians(30.0f), vec3(0, 0, 1));
	model_arm_R_D = translate(model_arm_R_D, vec3(0.0, 0.0, -0.25*sin(radians(30.0f))))*rotate(mat4(1.0f), radians(30.0f), vec3(1, 0, 0));

	model_joint_hand_L = translate(model_arm_L_D, vec3(0.0f, -0.15f, 0.0f));
	model_hand_L_1 = translate(model_joint_hand_L, vec3(0.05f + 0.1*sin(radians(5.0f)), -0.015f - 0.1*cos(radians(5.0f)), 0.0f))*rotate(mat4(1.0f), radians(5.0f), vec3(0, 0, 1));
	model_joint_finger_L_1 = translate(model_hand_L_1, vec3(0.0f, -0.1 - 0.075*sin(radians(-90.0f)), 0.0))*rotate(mat4(1.0f), radians(-90.0f), vec3(1, 0, 0));
	model_hand_L_2 = translate(model_joint_finger_L_1, vec3(-0.01 - 0.05*sin(radians(15.0f)), 0.0, -0.02 - 0.05*cos(radians(15.0f))))*rotate(mat4(1.0f), radians(15.0f), vec3(0, 1, 0));
	model_hand_L_3 = translate(model_joint_hand_L, vec3(-0.05f + 0.1*sin(radians(-5.0f)), -0.015f - 0.1*cos(radians(-5.0f)), 0.0f))*rotate(mat4(1.0f), radians(-5.0f), vec3(0, 0, 1));
	model_joint_finger_L_2 = translate(model_hand_L_3, vec3(0.0f, -0.1 - 0.075*sin(radians(-90.0f)), 0.0))*rotate(mat4(1.0f), radians(-90.0f), vec3(1, 0, 0));
	model_hand_L_4 = translate(model_joint_finger_L_2, vec3(0.01 - 0.05*sin(radians(-15.0f)), 0.0, -0.02 - 0.05*cos(radians(-15.0f))))*rotate(mat4(1.0f), radians(-15.0f), vec3(0, 1, 0));

	model_joint_hand_R = translate(model_arm_R_D, vec3(0.0f, -0.15f, 0.0f));
	model_hand_R_1 = translate(model_joint_hand_R, vec3(-0.05f + 0.1*sin(radians(-5.0f)), -0.015f - 0.1*cos(radians(-5.0f)), 0.0f))*rotate(mat4(1.0f), radians(-5.0f), vec3(0, 0, 1));
	model_joint_finger_R_1 = translate(model_hand_R_1, vec3(0.0f, -0.1 - 0.075*sin(radians(-90.0f)), 0.0))*rotate(mat4(1.0f), radians(-90.0f), vec3(1, 0, 0));
	model_hand_R_2 = translate(model_joint_finger_R_1, vec3(0.01 - 0.05*sin(radians(-15.0f)), 0.0, -0.02 - 0.05*cos(radians(-15.0f))))*rotate(mat4(1.0f), radians(-15.0f), vec3(0, 1, 0));
	model_hand_R_3 = translate(model_joint_hand_R, vec3(0.05f + 0.1*sin(radians(5.0f)), -0.015f - 0.1*cos(radians(5.0f)), 0.0f))*rotate(mat4(1.0f), radians(5.0f), vec3(0, 0, 1));
	model_joint_finger_R_2 = translate(model_hand_R_3, vec3(0.0f, -0.1 - 0.075*sin(radians(-90.0f)), 0.0))*rotate(mat4(1.0f), radians(-90.0f), vec3(1, 0, 0));
	model_hand_R_4 = translate(model_joint_finger_R_2, vec3(-0.01 - 0.05*sin(radians(15.0f)), 0.0, -0.02 - 0.05*cos(radians(15.0f))))*rotate(mat4(1.0f), radians(15.0f), vec3(0, 1, 0));

	model_joint_leg_L_U = translate(model_body, vec3(0.2f, -0.7f, 0.0f));
	model_leg_L_U = translate(model_joint_leg_L_U, vec3(0.2*sin(radians(0.0f)), -0.2*cos(radians(0.0f)), 0.0f))*rotate(mat4(1.0f), radians(0.0f), vec3(0, 0, 1));
	model_joint_leg_R_U = translate(model_body, vec3(-0.2f, -0.7f, 0.0f));
	model_leg_R_U = translate(model_joint_leg_R_U, vec3(0.2*sin(radians(0.0f)), -0.2*cos(radians(0.0f)), 0.0f))*rotate(mat4(1.0f), radians(0.0f), vec3(0, 0, 1));
	model_joint_leg_L_D = translate(model_leg_L_U, vec3(0.0f, -0.2f, 0.0f));
	model_leg_L_D = translate(model_joint_leg_L_D, vec3(0.25*sin(radians(0.0f)), -0.25*cos(radians(0.0f)), 0.0f))*rotate(mat4(1.0f), radians(0.0f), vec3(0, 0, 1));
	model_joint_leg_R_D = translate(model_leg_R_U, vec3(0.0f, -0.2f, 0.0f));
	model_leg_R_D = translate(model_joint_leg_R_D, vec3(-0.25*sin(radians(0.0f)), -0.25*cos(radians(0.0f)), 0.0f))*rotate(mat4(1.0f), radians(0.0f), vec3(0, 0, 1));

	model_shoes_L_U = translate(model_leg_L_D, vec3(0.0f, -0.25f, 0.0f));
	model_shoes_L_D = translate(model_shoes_L_U, vec3(0.0f, -0.1f, -0.075f));
	model_shoes_R_U = translate(model_leg_R_D, vec3(0.0f, -0.25f, 0.0f));
	model_shoes_R_D = translate(model_shoes_R_U, vec3(0.0f, -0.1f, -0.075f));

}

Robot robot;

// Load shader file to program
char** loadShaderSource(const char* file)
{
	FILE* fp = fopen(file, "rb");
	fseek(fp, 0, SEEK_END);
	long sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *src = new char[sz + 1];
	fread(src, sizeof(char), sz, fp);
	src[sz] = '\0';
	char **srcp = new char*[1];
	srcp[0] = src;
	return srcp;
}

// Free shader file
void freeShaderSource(char** srcp)
{
	delete srcp[0];
	delete srcp;
}

// OpenGL initialization
void My_Init()
{
	// Clear display windows color to white color(1.0,1.0,1.0,1.0)
	glClearColor(0.0f, 0.65f, 0.65f, 1.0f);

	texture_data tdata = load_png("./Assets/suit.png");
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tdata.width, tdata.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Enable depth test, will not draw pixels that are blocked by other pixels 
	glEnable(GL_DEPTH_TEST);

	// Set the type of depth test
	glDepthFunc(GL_LEQUAL);

	// Create Shader Program
	program = glCreateProgram();
	program_with_texture = glCreateProgram();

	// Create customize shader by tell openGL specify shader type 
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint vertexShaderWithTexture = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderWithTexture = glCreateShader(GL_FRAGMENT_SHADER);

	// Load shader file
	char** vertexShaderSource = loadShaderSource("./Assets/vertex.vs.glsl");
	char** fragmentShaderSource = loadShaderSource("./Assets/fragment.fs.glsl");
	char** vertexShaderTextureSource = loadShaderSource("./Assets/vertex_texture.vs.glsl");
	char** fragmentShaderTextureSource = loadShaderSource("./Assets/fragment_texture.fs.glsl");

	// Assign content of these shader files to those shaders we created before
	glShaderSource(vertexShader, 1, vertexShaderSource, NULL);
	glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);
	glShaderSource(vertexShaderWithTexture, 1, vertexShaderTextureSource, NULL);
	glShaderSource(fragmentShaderWithTexture, 1, fragmentShaderTextureSource, NULL);

	// Free the shader file string(won't be used any more)
	freeShaderSource(vertexShaderSource);
	freeShaderSource(fragmentShaderSource);
	freeShaderSource(vertexShaderTextureSource);
	freeShaderSource(fragmentShaderTextureSource);

	// Compile these shaders
	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);
	glCompileShader(vertexShaderWithTexture);
	glCompileShader(fragmentShaderWithTexture);

	// Logging
	shaderLog(vertexShader);
	shaderLog(fragmentShader);
	shaderLog(vertexShaderWithTexture);
	shaderLog(fragmentShaderWithTexture);

	// Assign the program we created before with these shaders
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	glAttachShader(program_with_texture, vertexShaderWithTexture);
	glAttachShader(program_with_texture, fragmentShaderWithTexture);
	glLinkProgram(program_with_texture);

	// Get the id of inner variable 'um4mvp' in shader programs
	um4mvp = glGetUniformLocation(program, "um4mvp");
	texture_um4mvp = glGetUniformLocation(program_with_texture, "um4mvp");

	//Create vertex array object and bind it to  OpenGL (OpenGL will apply operation only to the vertex array objects it bind)
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Enable shader layout location 0 and 1 for vertex and color   
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	//Create buffer and bind it to OpenGL
	glGenBuffers(1, &buffer1);
	glGenBuffers(1, &buffer2);
	glGenBuffers(1, &color_buffer);

	robot = Robot(0, 0, 0);
}

// GLUT callback. Called to draw the scene.
void My_Display() {
	if (mode == 0) {
		// Clear display buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		robot.prepare_modle();
		robot.draw();
		glutSwapBuffers();
	}
	else if (mode == 1) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		animationTimeSinceStart = glutGet(GLUT_ELAPSED_TIME);
		animationDeltaTime = animationTimeSinceStart - animationOldTimeSinceStart;
		animationOldTimeSinceStart = animationTimeSinceStart;
		mat4 rotation_matrix, translation_matrix;

		if (greeting_phase_1 == false) {
			if (greeting_phase_1_arm_angle < 180) {
				rotation_matrix = rotate(robot.model_joint_arm_R_U, (float)animationDeltaTime / 100, vec3(1, 0, 0));
				robot.model_joint_arm_R_U = rotation_matrix;
				if ((float)animationDeltaTime != 0) {
					greeting_phase_1_arm_angle += degrees((float)animationDeltaTime / 100);
				}
				robot.model_arm_R_U = translate(robot.model_joint_arm_R_U, vec3(0.2*sin(radians(-40.0f)), -0.2*cos(radians(-40.0f)), 0.0f))*rotate(mat4(1.0f), radians(-40.0f), vec3(0, 0, 1));
				robot.model_joint_arm_R_D = translate(robot.model_arm_R_U, vec3(0.0f, -0.2f, 0.0f));
				robot.model_arm_R_D = translate(robot.model_joint_arm_R_D, vec3(0.25*sin(radians(30.0f)), -0.25*cos(radians(30.0f)), 0.0f))*rotate(mat4(1.0f), radians(30.0f), vec3(0, 0, 1));
				robot.model_joint_hand_R = translate(robot.model_arm_R_D, vec3(0.0f, -0.2f, 0.0f));
				robot.model_hand_R_1 = translate(robot.model_joint_hand_R, vec3(-0.05f + 0.1*sin(radians(-5.0f)), -0.015f - 0.1*cos(radians(-5.0f)), 0.0f))*rotate(mat4(1.0f), radians(-5.0f), vec3(0, 0, 1));
				robot.model_joint_finger_R_1 = translate(robot.model_hand_R_1, vec3(0.0f, -0.1 - 0.075*sin(radians(-90.0f)), 0.0))*rotate(mat4(1.0f), radians(-90.0f), vec3(1, 0, 0));
				robot.model_hand_R_2 = translate(robot.model_joint_finger_R_1, vec3(0.01 - 0.05*sin(radians(-15.0f)), 0.0, -0.02 - 0.05*cos(radians(-15.0f))))*rotate(mat4(1.0f), radians(-15.0f), vec3(0, 1, 0));
				robot.model_hand_R_3 = translate(robot.model_joint_hand_R, vec3(0.05f + 0.1*sin(radians(5.0f)), -0.015f - 0.1*cos(radians(5.0f)), 0.0f))*rotate(mat4(1.0f), radians(5.0f), vec3(0, 0, 1));
				robot.model_joint_finger_R_2 = translate(robot.model_hand_R_3, vec3(0.0f, -0.1 - 0.075*sin(radians(-90.0f)), 0.0))*rotate(mat4(1.0f), radians(-90.0f), vec3(1, 0, 0));
				robot.model_hand_R_4 = translate(robot.model_joint_finger_R_2, vec3(-0.01 - 0.05*sin(radians(15.0f)), 0.0, -0.02 - 0.05*cos(radians(15.0f))))*rotate(mat4(1.0f), radians(15.0f), vec3(0, 1, 0));
			}
			else if (greeting_phase_1_hand_angle < 30) {
				rotation_matrix = rotate(robot.model_joint_hand_R, (float)animationDeltaTime / 100, vec3(0, 0, 1));
				robot.model_joint_hand_R = rotation_matrix;
				robot.model_hand_R_1 = translate(robot.model_joint_hand_R, vec3(-0.05f + 0.1*sin(radians(-5.0f)), -0.015f - 0.1*cos(radians(-5.0f)), 0.0f))*rotate(mat4(1.0f), radians(-5.0f), vec3(0, 0, 1));
				robot.model_joint_finger_R_1 = translate(robot.model_hand_R_1, vec3(0.0f, -0.1 - 0.075*sin(radians(-90.0f)), 0.0))*rotate(mat4(1.0f), radians(-90.0f), vec3(1, 0, 0));
				robot.model_hand_R_2 = translate(robot.model_joint_finger_R_1, vec3(0.01 - 0.05*sin(radians(-15.0f)), 0.0, -0.02 - 0.05*cos(radians(-15.0f))))*rotate(mat4(1.0f), radians(-15.0f), vec3(0, 1, 0));
				robot.model_hand_R_3 = translate(robot.model_joint_hand_R, vec3(0.05f + 0.1*sin(radians(5.0f)), -0.015f - 0.1*cos(radians(5.0f)), 0.0f))*rotate(mat4(1.0f), radians(5.0f), vec3(0, 0, 1));
				robot.model_joint_finger_R_2 = translate(robot.model_hand_R_3, vec3(0.0f, -0.1 - 0.075*sin(radians(-90.0f)), 0.0))*rotate(mat4(1.0f), radians(-90.0f), vec3(1, 0, 0));
				robot.model_hand_R_4 = translate(robot.model_joint_finger_R_2, vec3(-0.01 - 0.05*sin(radians(15.0f)), 0.0, -0.02 - 0.05*cos(radians(15.0f))))*rotate(mat4(1.0f), radians(15.0f), vec3(0, 1, 0));
				if ((float)animationDeltaTime != 0) {
					greeting_phase_1_hand_angle += degrees((float)animationDeltaTime / 100);
				}
			}
			else {
				greeting_phase_1 = true;
			}
		}
		else if (greeting_phase_2 == false) {
			if (greeting_phase_2_hand_angle < 60) {
				rotation_matrix = rotate(robot.model_joint_hand_R, -(float)animationDeltaTime / 100, vec3(0, 0, 1));
				robot.model_joint_hand_R = rotation_matrix;
				robot.model_hand_R_1 = translate(robot.model_joint_hand_R, vec3(-0.05f + 0.1*sin(radians(-5.0f)), -0.015f - 0.1*cos(radians(-5.0f)), 0.0f))*rotate(mat4(1.0f), radians(-5.0f), vec3(0, 0, 1));
				robot.model_joint_finger_R_1 = translate(robot.model_hand_R_1, vec3(0.0f, -0.1 - 0.075*sin(radians(-90.0f)), 0.0))*rotate(mat4(1.0f), radians(-90.0f), vec3(1, 0, 0));
				robot.model_hand_R_2 = translate(robot.model_joint_finger_R_1, vec3(0.01 - 0.05*sin(radians(-15.0f)), 0.0, -0.02 - 0.05*cos(radians(-15.0f))))*rotate(mat4(1.0f), radians(-15.0f), vec3(0, 1, 0));
				robot.model_hand_R_3 = translate(robot.model_joint_hand_R, vec3(0.05f + 0.1*sin(radians(5.0f)), -0.015f - 0.1*cos(radians(5.0f)), 0.0f))*rotate(mat4(1.0f), radians(5.0f), vec3(0, 0, 1));
				robot.model_joint_finger_R_2 = translate(robot.model_hand_R_3, vec3(0.0f, -0.1 - 0.075*sin(radians(-90.0f)), 0.0))*rotate(mat4(1.0f), radians(-90.0f), vec3(1, 0, 0));
				robot.model_hand_R_4 = translate(robot.model_joint_finger_R_2, vec3(-0.01 - 0.05*sin(radians(15.0f)), 0.0, -0.02 - 0.05*cos(radians(15.0f))))*rotate(mat4(1.0f), radians(15.0f), vec3(0, 1, 0));
				robot.model_hat_down = translate(robot.model_hand_R_4, vec3(0.55f, -0.1f, 0.0f))*rotate(mat4(1.0f), radians(-90.0f), vec3(1, 0, 0));
				robot.model_hat_up = translate(robot.model_hat_down, vec3(0.0f, 0.3f, 0.0f));

				greeting_phase_2_hand_angle += degrees((float)animationDeltaTime / 100);
			}
			else {
				greeting_phase_2 = true;
			}
		}
		else if (greeting_phase_3 == false) {
			if (clock < 2000) {
				clock += animationDeltaTime;
			}
			else {
				greeting_phase_3 = true;
			}
		}
		else {
			robot.model_body = mat4(1.0f);
			mode = 0;
		}
		robot.draw();
		glutSwapBuffers();
	}
	else if (mode == 2) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		animationTimeSinceStart = glutGet(GLUT_ELAPSED_TIME);
		animationDeltaTime = animationTimeSinceStart - animationOldTimeSinceStart;
		animationOldTimeSinceStart = animationTimeSinceStart;
		mat4 rotation_matrix, translation_matrix;
		
		if (bow_phase_1 == false) {
			if (bow_1_body_angle < 30) {
				rotation_matrix = rotate(robot.model_body, -(float)animationDeltaTime / 500, vec3(1, 0, 0));
				robot.model_body = rotation_matrix;
				robot.model_head = translate(robot.model_body, vec3(0.0f, 1.0f, 0.0f));
				robot.model_hat_down = translate(robot.model_head, vec3(0.0f, 0.45f, 0.0f));
				robot.model_hat_up = translate(robot.model_hat_down, vec3(0.0f, 0.15f, 0.0f));
				robot.model_eye_L = translate(robot.model_head, vec3(0.25f, 0.2f, -0.35f));
				robot.model_eye_R = translate(robot.model_head, vec3(-0.25f, 0.2f, -0.35f));

				robot.model_joint_arm_L_U = translate(robot.model_body, vec3(0.4f, 0.5f, 0.0f));
				robot.model_arm_L_U = translate(robot.model_joint_arm_L_U, vec3(0.2*sin(radians(40.0f)), -0.2*cos(radians(40.0f)), 0.0f))*rotate(mat4(1.0f), radians(40.0f), vec3(0, 0, 1));
				robot.model_joint_arm_R_U = translate(robot.model_body, vec3(-0.4f, 0.5f, 0.0f));
				robot.model_arm_R_U = translate(robot.model_joint_arm_R_U, vec3(0.2*sin(radians(-40.0f)), -0.2*cos(radians(-40.0f)), 0.0f))*rotate(mat4(1.0f), radians(-40.0f), vec3(0, 0, 1));
				robot.model_joint_arm_L_D = translate(robot.model_arm_L_U, vec3(0.0f, -0.2f, 0.0f));
				robot.model_arm_L_D = translate(robot.model_joint_arm_L_D, vec3(0.25*sin(radians(-30.0f)), -0.25*cos(radians(-30.0f)), 0.0f))*rotate(mat4(1.0f), radians(-30.0f), vec3(0, 0, 1));
				robot.model_arm_L_D = translate(robot.model_arm_L_D, vec3(0.0, 0.0, -0.25*sin(radians(30.0f))))*rotate(mat4(1.0f), radians(30.0f), vec3(1, 0, 0));
				robot.model_joint_arm_R_D = translate(robot.model_arm_R_U, vec3(0.0f, -0.2f, 0.0f));
				robot.model_arm_R_D = translate(robot.model_joint_arm_R_D, vec3(0.25*sin(radians(30.0f)), -0.25*cos(radians(30.0f)), 0.0f))*rotate(mat4(1.0f), radians(30.0f), vec3(0, 0, 1));
				robot.model_arm_R_D = translate(robot.model_arm_R_D, vec3(0.0, 0.0, -0.25*sin(radians(30.0f))))*rotate(mat4(1.0f), radians(30.0f), vec3(1, 0, 0));

				robot.model_joint_hand_L = translate(robot.model_arm_L_D, vec3(0.0f, -0.15f, 0.0f));
				robot.model_hand_L_1 = translate(robot.model_joint_hand_L, vec3(0.05f + 0.1*sin(radians(5.0f)), -0.015f - 0.1*cos(radians(5.0f)), 0.0f))*rotate(mat4(1.0f), radians(5.0f), vec3(0, 0, 1));
				robot.model_joint_finger_L_1 = translate(robot.model_hand_L_1, vec3(0.0f, -0.1 - 0.075*sin(radians(-90.0f)), 0.0))*rotate(mat4(1.0f), radians(-90.0f), vec3(1, 0, 0));
				robot.model_hand_L_2 = translate(robot.model_joint_finger_L_1, vec3(-0.01 - 0.05*sin(radians(15.0f)), 0.0, -0.02 - 0.05*cos(radians(15.0f))))*rotate(mat4(1.0f), radians(15.0f), vec3(0, 1, 0));
				robot.model_hand_L_3 = translate(robot.model_joint_hand_L, vec3(-0.05f + 0.1*sin(radians(-5.0f)), -0.015f - 0.1*cos(radians(-5.0f)), 0.0f))*rotate(mat4(1.0f), radians(-5.0f), vec3(0, 0, 1));
				robot.model_joint_finger_L_2 = translate(robot.model_hand_L_3, vec3(0.0f, -0.1 - 0.075*sin(radians(-90.0f)), 0.0))*rotate(mat4(1.0f), radians(-90.0f), vec3(1, 0, 0));
				robot.model_hand_L_4 = translate(robot.model_joint_finger_L_2, vec3(0.01 - 0.05*sin(radians(-15.0f)), 0.0, -0.02 - 0.05*cos(radians(-15.0f))))*rotate(mat4(1.0f), radians(-15.0f), vec3(0, 1, 0));

				robot.model_joint_hand_R = translate(robot.model_arm_R_D, vec3(0.0f, -0.15f, 0.0f));
				robot.model_hand_R_1 = translate(robot.model_joint_hand_R, vec3(-0.05f + 0.1*sin(radians(-5.0f)), -0.015f - 0.1*cos(radians(-5.0f)), 0.0f))*rotate(mat4(1.0f), radians(-5.0f), vec3(0, 0, 1));
				robot.model_joint_finger_R_1 = translate(robot.model_hand_R_1, vec3(0.0f, -0.1 - 0.075*sin(radians(-90.0f)), 0.0))*rotate(mat4(1.0f), radians(-90.0f), vec3(1, 0, 0));
				robot.model_hand_R_2 = translate(robot.model_joint_finger_R_1, vec3(0.01 - 0.05*sin(radians(-15.0f)), 0.0, -0.02 - 0.05*cos(radians(-15.0f))))*rotate(mat4(1.0f), radians(-15.0f), vec3(0, 1, 0));
				robot.model_hand_R_3 = translate(robot.model_joint_hand_R, vec3(0.05f + 0.1*sin(radians(5.0f)), -0.015f - 0.1*cos(radians(5.0f)), 0.0f))*rotate(mat4(1.0f), radians(5.0f), vec3(0, 0, 1));
				robot.model_joint_finger_R_2 = translate(robot.model_hand_R_3, vec3(0.0f, -0.1 - 0.075*sin(radians(-90.0f)), 0.0))*rotate(mat4(1.0f), radians(-90.0f), vec3(1, 0, 0));
				robot.model_hand_R_4 = translate(robot.model_joint_finger_R_2, vec3(-0.01 - 0.05*sin(radians(15.0f)), 0.0, -0.02 - 0.05*cos(radians(15.0f))))*rotate(mat4(1.0f), radians(15.0f), vec3(0, 1, 0));
				
				bow_1_body_angle += degrees((float)animationDeltaTime / 500);
			}
			else {
				bow_phase_1 = true;
			}
		}
		else if (bow_phase_2 == false) {
			if (clock < 2000) {
				clock += animationDeltaTime;
			}
			else {
				bow_phase_2 = true;
			}
		}
		else {
			robot.model_body = mat4(1.0f);
			mode = 0;
		}
		robot.draw();
		glutSwapBuffers();
	}
}

// Setting up viewing matrix
// This function will be called whenever the size of windows is changing
void My_Reshape(int width, int height)
{
	// Setting where to display and size of OpenGL display area
	glViewport(0, 0, width, height);

	float viewportAspect = (float)width / (float)height;
	projection = perspective(radians(60.0f), viewportAspect, 0.1f, 1000.0f);
	view = lookAt(vec3(0.0f, 1.0f, -4.0f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));
}


void My_Timer(int val)
{
	timer_cnt++;
	glutPostRedisplay();
	if (timer_enabled)
	{
		glutTimerFunc(timer_speed, My_Timer, val);
	}
}

void My_Keyboard(unsigned char key, int x, int y)
{
	printf("Key %c is pressed at (%d, %d)\n", key, x, y);
}

void My_SpecialKeys(int key, int x, int y)
{
	mat4 rotation_matrix;

	switch (key)
	{
	case GLUT_KEY_UP:
		if (keyStates[0] == false) {
			oldTimeSinceStart = glutGet(GLUT_ELAPSED_TIME);
			keyStates[0] = true;
			keyStates[1] = false;
			keyStates[2] = false;
			keyStates[3] = false;
		}
		else {
			if (mode == 0) {
				timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
				deltaTime = timeSinceStart - oldTimeSinceStart;
				oldTimeSinceStart = timeSinceStart;

				rotation_matrix = rotate(robot.model_body, (float)deltaTime / 1000, vec3(1, 0, 0));
				robot.model_body = rotation_matrix;
			}
		}
		break;
	case GLUT_KEY_LEFT:
		if (keyStates[1] == false) {
			oldTimeSinceStart = glutGet(GLUT_ELAPSED_TIME);
			keyStates[0] = false;
			keyStates[1] = true;
			keyStates[2] = false;
			keyStates[3] = false;
		}
		else {
			if (mode == 0) {
				timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
				deltaTime = timeSinceStart - oldTimeSinceStart;
				oldTimeSinceStart = timeSinceStart;

				rotation_matrix = rotate(robot.model_body, (float)deltaTime / 1000, vec3(0, 1, 0));
				robot.model_body = rotation_matrix;
			}
		}
		break;
	case GLUT_KEY_DOWN:
		if (keyStates[2] == false) {
			oldTimeSinceStart = glutGet(GLUT_ELAPSED_TIME);
			keyStates[0] = false;
			keyStates[1] = false;
			keyStates[2] = true;
			keyStates[3] = false;
		}
		else {
			if (mode == 0) {
				timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
				deltaTime = timeSinceStart - oldTimeSinceStart;
				oldTimeSinceStart = timeSinceStart;

				rotation_matrix = rotate(robot.model_body, -(float)deltaTime / 1000, vec3(1, 0, 0));
				robot.model_body = rotation_matrix;
			}
		}
		break;
	case GLUT_KEY_RIGHT:
		if (keyStates[3] == false) {
			oldTimeSinceStart = glutGet(GLUT_ELAPSED_TIME);
			keyStates[0] = false;
			keyStates[1] = false;
			keyStates[2] = false;
			keyStates[3] = true;
		}
		else {
			if (mode == 0) {
				timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
				deltaTime = timeSinceStart - oldTimeSinceStart;
				oldTimeSinceStart = timeSinceStart;

				rotation_matrix = rotate(robot.model_body, -(float)deltaTime / 1000, vec3(0, 1, 0));
				robot.model_body = rotation_matrix;
			}
		}
		break;
	default:
		printf("Other special key is pressed at (%d, %d)\n", x, y);
		break;
	}
}

void My_SpecialKeysUp(int key, int x, int y) {
	switch (key)
	{
	case GLUT_KEY_UP:
		if (keyStates[0] == true) {
			keyStates[0] = false;
		}
		break;
	case GLUT_KEY_LEFT:
		if (keyStates[1] == true) {
			keyStates[1] = false;
		}
		break;
	case GLUT_KEY_DOWN:
		if (keyStates[2] == true) {
			keyStates[2] = false;
		}
		break;
	case GLUT_KEY_RIGHT:
		if (keyStates[3] == true) {
			keyStates[3] = false;
		}
		break;
	default:
		printf("Other special key is pressed at (%d, %d)\n", x, y);
		break;
	}
}

void My_Menu(int id)
{
	switch (id)
	{
	case MENU_TIMER_START:
		if (!timer_enabled)
		{
			timer_enabled = true;
			glutTimerFunc(timer_speed, My_Timer, 0);
			animationOldTimeSinceStart = glutGet(GLUT_ELAPSED_TIME);
			animationTimeSinceStart = glutGet(GLUT_ELAPSED_TIME);
		}
		break;
	case MENU_TIMER_STOP:
		timer_enabled = false;
		break;
	case MENU_EXIT:
		exit(0);
		break;
	case MENU_IDLE:
		mode = 0;
		robot.model_body = mat4(1.0f);
		break;
	case MENU_GREETING:
		if (mode != 1) {
			mode = 1;
			robot.model_body = mat4(1.0f);
			greeting_phase_1_arm_angle = 0;
			greeting_phase_1_hand_angle = 0;
			greeting_phase_2_hand_angle = 0;
			clock = 0;
			greeting_phase_1 = false;
			greeting_phase_2 = false;
			greeting_phase_3 = false;
			animationOldTimeSinceStart = glutGet(GLUT_ELAPSED_TIME);
			robot.prepare_modle();
		}
		break;
	case MENU_BOW:
		if (mode != 2) {
			mode = 2;
			robot.model_body = mat4(1.0f);
			bow_1_body_angle = 0;
			bow_1_leg_angle = 0;
			bow_phase_1 = false;
			bow_phase_2 = false;
			bow_phase_3 = false;
			clock = 0;
			animationOldTimeSinceStart = glutGet(GLUT_ELAPSED_TIME);
			robot.prepare_modle();
		}
		break;
	default:
		break;
	}
}

void My_Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			printf("Mouse %d is pressed at (%d, %d)\n", button, x, y);
		}
		else if (state == GLUT_UP)
		{
			printf("Mouse %d is released at (%d, %d)\n", button, x, y);
		}
	}
}

int main(int argc, char *argv[])
{
#ifdef __APPLE__
	// Change working directory to source code path
	chdir(__FILEPATH__("/../Assets/"));
#endif
	// Initialize GLUT and GLEW, then create a window.
	glutInit(&argc, argv);
#ifdef _MSC_VER
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#else
	glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif

	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);
	glutCreateWindow("103062230_AS1");


#ifdef _MSC_VER
	glewInit();
#endif
	dumpInfo();
	My_Init();

	int menu_main = glutCreateMenu(My_Menu);
	int menu_timer = glutCreateMenu(My_Menu);
	int menu_speed = glutCreateMenu(My_Menu);

	glutSetMenu(menu_main);
	glutAddSubMenu("Timer", menu_timer);
	glutAddMenuEntry("Idle", MENU_IDLE);
	glutAddMenuEntry("Greeting", MENU_GREETING);
	glutAddMenuEntry("Bow", MENU_BOW);
	glutAddMenuEntry("Exit", MENU_EXIT);

	glutSetMenu(menu_timer);
	glutAddMenuEntry("Start", MENU_TIMER_START);
	glutAddMenuEntry("Stop", MENU_TIMER_STOP);

	glutSetMenu(menu_main);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// Register GLUT callback functions.
	glutDisplayFunc(My_Display);
	glutReshapeFunc(My_Reshape);
	glutKeyboardFunc(My_Keyboard);
	glutSpecialFunc(My_SpecialKeys);
	glutSpecialUpFunc(My_SpecialKeysUp);
	glutTimerFunc(timer_speed, My_Timer, 0);
	glutMouseFunc(My_Mouse);

	// Enter main event loop.
	glutMainLoop();

	return 0;
}