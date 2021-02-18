#pragma once
#include <Core/Engine.h>

class Collectable {
private:
	glm::vec3 position;
	float radians;

public:
	Collectable(glm::vec3 pos, float rad) {
		this->position = pos;
		this->radians = rad;
	}
	void increase_radians(float r) {
		this->radians += r;
	}
	void increase_pos(glm::vec3 pos) {
		this->position += pos;
	}
	glm::vec3 getPosition() {
		return this->position;
	}
	float getRadians() {
		return this->radians;
	}
};