#include <Core/Engine.h>

class DecorObject {
private:
	glm::vec3 position;
	std::string name;
	float radians;
	int special;

public:
	DecorObject(std::string name, glm::vec3 position, float radians) {
		this->position = position;
		this->name = name;
		this->radians = radians;
		this->special = rand() % 5;
	}

	void increasePosition(glm::vec3 p) {
		this->position += p;
	}

	void setPosition(glm::vec3 p) {
		this->position = p;
	}

	glm::vec3 getPosition() {
		return this->position;
	}

	std::string getName() {
		return this->name;
	}

	float getRadians() {
		return this->radians;
	}

	int getSpecial() {
		return this->special;
	}

};