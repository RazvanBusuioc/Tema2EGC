#include "Tile.h"

Tile::Tile(){}

Tile::~Tile() {}

glm::vec3 Tile::getPosition() {
	return this->position;
}
glm::vec3 Tile::getDimension() {
	return this->dimension;
}
glm::vec3 Tile::getColor() {
	return this->color;
}
int Tile::getType() {
	return this->type;
}

void Tile::setColor(glm::vec3 col) {
	this->color = col;
}
void Tile::setDimension(glm::vec3 dim) {
	this->dimension = dim;
}
void Tile::setPosition(glm::vec3 pos) {
	this->position = pos;
}
void Tile::setType(int t) {
	this->type = t;
}
void Tile::increase_pos(glm::vec3 v) {
	this->position += v;
}