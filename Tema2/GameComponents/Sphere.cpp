#include "Sphere.h"

Sphere::~Sphere(){
}

Sphere::Sphere(){}

void Sphere::setRadius(float r) {
	this->radius = r;
}
void Sphere::setPosition(glm::vec3 pos) {
	this->position = pos;
}
void Sphere::setCurentMiniumYPos(float y) {
	this->current_minimum_y_pos = y;
}
void Sphere::setVelocity(glm::vec3 vel) {
	this->velocity = vel;
}
void Sphere::setColor(glm::vec3 col) {
	this->color = col;
}
void Sphere::increase_pos_x(float pos) {
	this->position += glm::vec3(pos, 0, 0);
}
void Sphere::set_jumping(bool j) {
	this->jumping = j;
}
void Sphere::set_power_up(int p) {
	this->curr_power_up = p;
}
void Sphere::set_power_up_left_time(float t) {
	this->power_up_left_time = t;
}
void Sphere::update_pos_y(float time) {

	if (position.y > this->current_minimum_y_pos && !this->jumping) {
		//the players is falling
		this->position += time * G_VECTOR;
		this->walking = false;
	}
	if (this->jumping == true) {
		//the player is jumping
		this->walking = false;
		this->position += time * this->velocity;
		this->velocity += time * G_VECTOR;

		if (this->position.y < this->current_minimum_y_pos) {
			//after the jump, player has hit the ground(a tile) and now he is walking
			this->walking = true;
			this->jumping = false;
			this->position.y = this->current_minimum_y_pos;
		}
	}
}
void Sphere::set_fuel(float f) {
	this->fuel = f;
}
void Sphere::increase_hp() {
	this->hp++;
	if (this->hp > MAXIMUM_HP) {
		this->hp = MAXIMUM_HP;
	}
}
void Sphere::decrease_hp() {
	this->hp--;
}

int Sphere::get_hp() {
	return this->hp;
}
float Sphere::getRadius() {
	return this->radius;
}
float Sphere::getCurentMiniumYPos() {
	return this->current_minimum_y_pos;
}
glm::vec3 Sphere::getPosition() {
	return this->position;
}
glm::vec3 Sphere::getVelocity() {
	return this->velocity;
}
glm::vec3 Sphere::getColor() {
	return this->color;
}
bool Sphere::getJumping() {
	return this->jumping;
}
bool Sphere::getWalking() {
	return this->walking;
}
int Sphere::get_power_up() {
	return this->curr_power_up;
}
float Sphere::get_power_up_left_time() {
	return this->power_up_left_time;
}
float Sphere::get_fuel() {
	return this->fuel;
}