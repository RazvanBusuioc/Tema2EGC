#include <Core/Engine.h>
#define MAXIMUM_HP 3

#define G_VECTOR glm::vec3(0, -3, 0)

class Sphere {
private:
	
	glm::vec3 position;
	glm::vec3 velocity; // velocity for the jump
	glm::vec3 color;

	float radius;
	float current_minimum_y_pos; // the player knows if he has a tile under his feet or nothing at all
	float power_up_left_time = 0;
	float fuel;

	bool jumping = false; // if true, the player is in the jump process
	bool walking = true; // if true, the player is in the walking process

	int curr_power_up = 0; // current powe up id
	int hp = 1; // current hp

public:
	~Sphere();
	Sphere();
	
	void increase_hp();
	void decrease_hp();
	void set_fuel(float f);
	void set_power_up_left_time(float t);
	void set_power_up(int p);
	void set_jumping(bool j);
	void setCurentMiniumYPos(float y);
	void setRadius(float r);
	void setPosition(glm::vec3 pos);
	void setVelocity(glm::vec3 vel);
	void setColor(glm::vec3 col);
	void increase_pos_x(float pos);
	void update_pos_y(float time);


	int get_hp();
	int get_power_up();
	float get_fuel();
	float getRadius();
	float getCurentMiniumYPos();
	float get_power_up_left_time();
	bool getWalking();
	bool getJumping();
	glm::vec3 getPosition();
	glm::vec3 getVelocity();
	glm::vec3 getColor();
};