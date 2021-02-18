#include <Core/Engine.h>

class Tile {
private:
	glm::vec3 dimension;
	glm::vec3 position;
	glm::vec3 color;
	int type;

public:
	Tile();
	~Tile();

	void setDimension(glm::vec3 dim);
	void setPosition(glm::vec3 pos);
	void setColor(glm::vec3 col);
	void setType(int type);
	void increase_pos(glm::vec3 p);

	glm::vec3 getDimension();
	glm::vec3 getPosition();
	glm::vec3 getColor();
	int getType();
};