#include "Tema2.h"
#include "Object3D.h"

#include <vector>
#include <string>
#include <iostream>

#include <stdio.h> 
#include <stdlib.h> 
#include <time.h> 
#include <math.h>

#include <Core/Engine.h>

using namespace std;

Tema2::Tema2()
{
}

Tema2::~Tema2()
{
}

void Tema2::Init()
{	
	initialise_camera(); // set the camera position and center of focus

	initialise_columns(); // create the firs tiles

	initialise_player(); // create the player/sphere

	create_meshes(); // create all the meshes used

	create_shaders(); // bind all the shaders used
}

void Tema2::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Tema2::Update(float deltaTimeSeconds)
{
	update_curr_level(); // updates the current level of the game

	update_columns(); // create new columns, erase the columns that aren t needed, check for collisions

	update_player(deltaTimeSeconds); // update the player positions and atributes
	
	update_camera(); // update the position and state of the camra
	
	render_tiles(); // render the tile

	render_player(); // render the sphere

	render_interface(); // render the user interface

}

void Tema2::FrameEnd()
{
	//DrawCoordinatSystem(camera->GetViewMatrix(), projectionMatrix);
}

void Tema2::RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// get shader location for uniform mat4 "Model"
	int location1 = glGetUniformLocation(shader->program, 
						"Model");
	
	// set shader uniform "Model" to modelMatrix
	glUniformMatrix4fv(location1, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// get shader location for uniform mat4 "View"
	int location2 = glGetUniformLocation(shader->program,
		"View");

	// set shader uniform "View" to viewMatrix
	glm::mat4 viewMatrix = camera->GetViewMatrix();
	glUniformMatrix4fv(location2, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// get shader location for uniform mat4 "Projection"
	int location3 = glGetUniformLocation(shader->program,
		"Projection");

	// set shader uniform "Projection" to projectionMatrix
	glm::mat4 projectionMatrix = this->projection_matrix;
	glUniformMatrix4fv(location3, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// uniform attribute used for the coloring of the player
	int location4 = glGetUniformLocation(shader->program, "color");
	if (location4 != -1) {
		glUniform3fv(location4, 1, glm::value_ptr(this->player->getColor()));
	}

	// uniform attribute used for vertex shader deformation
	int location5 = glGetUniformLocation(shader->program, "time");
	if (location5 != -1) {
		double seconds = Engine::GetElapsedTime();
		glUniform1f(location5, seconds);
	}

	//uniform attribute used for the coloring of the max-fuel panel
	int location6 = glGetUniformLocation(shader->program, "max_fuel_color2D");
	if (location6 != -1) {
		glUniform3fv(location6, 1, glm::value_ptr(MAX_FUEL_COLOR));
	}

	// uniform attribute used for the coloring of the curr-fuel panel
	int location7 = glGetUniformLocation(shader->program, "curr_fuel_color2D");
	if (location7 != -1) {
		glUniform3fv(location7, 1, glm::value_ptr(CURR_FUEL_COLOR));
	}

	//uniform attrivutte used for the coloring of the hp-bars
	int location8 = glGetUniformLocation(shader->program, "curr_HP_color2D");
	if (location8 != -1) {
		glUniform3fv(location8, 1, glm::value_ptr(HP_COLOR));
	}

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

void Tema2::OnInputUpdate(float deltaTime, int mods)
{
	if (window->KeyHold(GLFW_KEY_W) && this->player->get_power_up() != MAX_SPEED_POWER_UP_ID) {
		// w = the character accelerates
		this->world_speed += WORLD_SPEED_INCREASE_FACTOR * deltaTime;
		if (this->world_speed > MAX_WORLD_SPEED) {
			this->world_speed = MAX_WORLD_SPEED;
		}
	}
	if (window->KeyHold(GLFW_KEY_S) && this->player->get_power_up() != MAX_SPEED_POWER_UP_ID) { 
		// s = the character decelerates
		this->world_speed -= WORLD_SPEED_INCREASE_FACTOR * deltaTime;
		if (this->world_speed < lowest_speed) {
			this->world_speed = lowest_speed;
		}
	}
	if (window->KeyHold(GLFW_KEY_A)) { 
		// a = the character moves left
		this->player->increase_pos_x(SPHERE_MOVEMENT_FACTOR * deltaTime);
	}
	if (window->KeyHold(GLFW_KEY_D)) {
		// d = the character moves right
		this->player->increase_pos_x(-SPHERE_MOVEMENT_FACTOR * deltaTime);
	}
	
	//use this for debung - move the camera around
	/*if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float cameraSpeed = 2.0f;

		if (window->KeyHold(GLFW_KEY_W)) {
			// translate the camera forward
			camera->TranslateForward(deltaTime * cameraSpeed);
		}

		if (window->KeyHold(GLFW_KEY_A)) {
			// translate the camera to the left
			camera->TranslateRight(-deltaTime * cameraSpeed);
		}

		if (window->KeyHold(GLFW_KEY_S)) {
			// translate the camera backwards
			camera->TranslateForward(-deltaTime * cameraSpeed);
		}

		if (window->KeyHold(GLFW_KEY_D)) {
			// translate the camera to the right
			camera->TranslateRight(deltaTime * cameraSpeed);
		}

		if (window->KeyHold(GLFW_KEY_Q)) {
			// translate the camera down
			camera->TranslateUpword(deltaTime * cameraSpeed);
		}

		if (window->KeyHold(GLFW_KEY_E)) {
			// translate the camera up
			camera->TranslateUpword(-deltaTime * cameraSpeed);

		}
	}*/
}

void Tema2::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_SPACE && !player->getJumping() && player->getWalking()) {
		// if the player is not already in the jumping process and he also has a tile under his feet, he can jump
		this->player->setVelocity(JUMP_VELOCITY_VECTOR);
		this->player->set_jumping(true);
	}
	if (key == GLFW_KEY_C) {
		// switch between camera mods
		this->first_person_camera = !this->first_person_camera;
		this->third_person_camera = !this->third_person_camera;
	}
}

void Tema2::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{	
	//keys for camera rotations - use this for debugging
	/*if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float sensivityOX = 0.001f;
		float sensivityOY = 0.001f;

		if (window->GetSpecialKeyState() == 0) {
			renderCameraTarget = false;
			// rotate the camera in First-person mode around OX and OY using deltaX and deltaY
			// use the sensitivity variables for setting up the rotation speed
			camera->RotateFirstPerson_OX(deltaY * sensivityOX);
			camera->RotateFirstPerson_OY(deltaX * sensivityOY);
		}

		if (window->GetSpecialKeyState() && GLFW_MOD_CONTROL) {
			renderCameraTarget = true;
			// rotate the camera in Third-person mode around OX and OY using deltaX and deltaY
			// use the sensitivity variables for setting up the rotation speed
			camera->RotateThirdPerson_OX(deltaY * sensivityOX);
			camera->RotateThirdPerson_OY(deltaX * sensivityOY);
		}

	}*/
}

void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema2::OnWindowResize(int width, int height)
{
}

void Tema2::create_shaders() {
	//shader used for tiles
	{
		Shader* shader = new Shader("ShaderTema2");
		shader->AddShader("Source/Tema2/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Tema2/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
	//shader used for  the player
	{
		Shader* shader = new Shader("SphereShader");
		shader->AddShader("Source/Tema2/Shaders/sphereVS.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Tema2/Shaders/sphereFS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
	//shader used for the max-fuel panel
	{
		Shader* shader = new Shader("MaxFuelShader");
		shader->AddShader("Source/Tema2/Shaders/FuelVS.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Tema2/Shaders/MaxFuelFS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
	//shader used for the curr-fuel panel
	{
		Shader* shader = new Shader("CurrFuelShader");
		shader->AddShader("Source/Tema2/Shaders/FuelVS.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Tema2/Shaders/CurrFuelFS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
	//shader used for the curr-hp bars
	{
		Shader* shader = new Shader("CurrHPShader");
		shader->AddShader("Source/Tema2/Shaders/FuelVS.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Tema2/Shaders/CurrHPFS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
}

void Tema2::create_meshes() 
{	
	//create meshes for all kind of tiles
	meshes["simple_tile"] = Object3D::CreateTile("simple_tile", TILE_DIMENSION, SIMPLE_TILE_COLOR);
	meshes["touched_tile"] = Object3D::CreateTile("touched_tile", TILE_DIMENSION, TOUCHED_TILE_COLOR);
	meshes["bonus_fuel_tile"] = Object3D::CreateTile("bonus_fuel_tile", TILE_DIMENSION, BONUS_FUEL_TILE_COLOR);
	meshes["max_speed_tile"] = Object3D::CreateTile("max_speed_tile", TILE_DIMENSION, MAX_SPEED_TILE_COLOR);
	meshes["lose_fuel_tile"] = Object3D::CreateTile("lose_fuel_tile", TILE_DIMENSION, LOSE_FUEL_TILE_COLOR);
	meshes["bonus_hp_tile"] = Object3D::CreateTile("bonus_hp_tile", TILE_DIMENSION, BONUS_HP_TILE_COLOR);
	meshes["lose_hp_tile"] = Object3D::CreateTile("lose_hp_tile", TILE_DIMENSION, LOSE_HP_TILE_COLOR);
	meshes["game_over_tile"] = Object3D::CreateTile("game_over_tile", TILE_DIMENSION, GAME_OVER_TILE_COLOR);

	//create mesh for the player
	Mesh* mesh = new Mesh("sphere");
	mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
	meshes[mesh->GetMeshID()] = mesh;

	Mesh* m1 = new Mesh("max_fuel");
	m1->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "screen_quad.obj");
	meshes[m1->GetMeshID()] = m1;

	Mesh* m2 = new Mesh("curr_fuel");
	m2->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "screen_quad.obj");
	meshes[m2->GetMeshID()] = m2;


}

void Tema2::initialise_camera() {
	projection_matrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, (float)Z_FAR);
	camera = new Camera::Camera();
	camera->Set(THIRD_PERSON_CAMERA_INITIAL_POSITION, THIRD_PERSON_CAMERA_INITIAL_CENTER, THIRD_PERSON_CAMERA_UP);
}

void Tema2::initialise_columns() 
{	
	int distance_between_tiles; // distance between each set of tiles, changes every tile

	//initialise the x position for every column of tiles
	columns_x_pos[0] = COL1_X; columns_x_pos[1] = COL2_X; columns_x_pos[2] = COL3_X;
	columns_x_pos[3] = COL4_X; columns_x_pos[4] = COL5_X;

	//for every column, add tiles with a random distance(minimum_dist - maximum dist) between the tiles
	for (int j = 0; j < NO_COLUMNS; j++) {
		for (float i = 0; i < Z_FAR; i += TILE_DIMENSION.z + distance_between_tiles) {
			Tile* tile = new Tile();
			tile->setPosition(glm::vec3(columns_x_pos[j], 0, i + TILE_DIMENSION.z / 2));
			tile->setDimension(TILE_DIMENSION);
			tile->setType(SIMPLE_TILE_ID);
			tile->setColor(SIMPLE_TILE_COLOR);
			columns[j].push_back(tile);
			distance_between_tiles = MINIMUM_DIST_BETWEEN_TILES + rand() % (MAXIMUM_DIST_BETWEEN_TILES - MINIMUM_DIST_BETWEEN_TILES)*10 * 0.1;

			INCREASE_TILES_RENDERD;
		}
	}
}

void Tema2::initialise_player() {
	this->player = new Sphere();
	this->player->setPosition(SPHERE_INITIAL_POSITION);
	this->player->setRadius(SPHERE_RADIUS);
	this->player->setVelocity(glm::vec3(0, 0, 0));
	this->player->setCurentMiniumYPos(SPHERE_RADIUS);
	this->player->setColor(DEFAULT_PLAYER_COLOR);
	this->player->set_fuel(MAX_FUEL);
}

void Tema2::update_player(float time) {

	player->update_pos_y(time); // update player height

	//check if the player has fallen under the minimum Y position
	if (player->getPosition().y < POS_Y_MIN) {
		exit(-1);
	}

	//decrease the power-up left time
	player->set_power_up_left_time(player->get_power_up_left_time() - time);
	if (player->get_power_up_left_time() <= 0) {
		// if power-up time has expired, set the player aspect back to default
		player->setColor(DEFAULT_PLAYER_COLOR);
		player->set_power_up(NO_POWER_UP_ID);
	}

	//decrease the fuel left
	player->set_fuel(player->get_fuel() - FUEL_DECREASE_FACTOR * this->world_speed * time);
	if (player->get_fuel() <= 0) {
		//if the fuel tank is empty, decrease hp
		player->decrease_hp();
		player->set_fuel(MAX_FUEL);
	}

	// check for player hp
	if (player->get_hp() <= 0) {
		exit(-1);
	}
}

void Tema2::update_camera() {
	if (this->third_person_camera) {
		//third person camera mode
		this->camera->Set(this->player->getPosition() + THIRD_PERSON_CAMERA_POSITION_OFFSET,
						  this->player->getPosition() + THIRD_PERSON_CAMERA_CENTER_OFFSET,
						  THIRD_PERSON_CAMERA_UP);
	}
	if (this->first_person_camera) {
		//first person camera mode
		this->camera->Set(this->player->getPosition() + FIRST_PERSON_CAMRA_POSITION_OFFSET,
						  this->player->getPosition() + FIRST_PERSON_CAMERA_CENTER_OFFSET,
						  FIRST_PERSON_CAMERA_UP);
	}
}

void Tema2::update_curr_level() {
	if (Engine::GetElapsedTime() >= TIME_TO_LVL1) { 
		this->curr_level = 1; 
		this->lowest_speed = LOWEST_SPPED_LVL1;
	}
	if (Engine::GetElapsedTime() >= TIME_TO_LVL2) {
		this->curr_level = 2;
		this->lowest_speed = LOWEST_SPPED_LVL2;
	}
	if (Engine::GetElapsedTime() >= TIME_TO_LVL3) {
		this->curr_level = 3;
		this->lowest_speed = LOWEST_SPPED_LVL3;
	}
}

void Tema2::update_columns() {
	
	lowest_pos_for_player = LOWEST_Y_POS; // no tile under the player

	//number of columns based on the current level
	int nocol;
	if (this->curr_level == 0) nocol = NO_COLUMNS_LVL0;
	if (this->curr_level == 1) nocol = NO_COLUMNS_LVL1;
	if (this->curr_level == 2) nocol = NO_COLUMNS_LVL2;
	if (this->curr_level > 2) nocol = NO_COLUMNS_LVL3;

	//for every column, update the position of tiles, erase the tiles that are behind the player, and add new tiles.
	for (int j = 0; j < NO_COLUMNS; j++) {
		for (int i = 0; i < columns[j].size(); i++) {

			//update position of curr tile
			columns[j][i]->increase_pos(glm::vec3(0, 0, -this->world_speed));
			
			check_colisions(columns[j][i]); //check for special collisions(margin of the tile collisions)

			//search for the tile that is under the player - colision with the surface of the tile
			float pos = columns[j][i]->getPosition().z;
			float dim = columns[j][i]->getDimension().z;
			float curr_tile_z1 = pos + dim / (float)2;
			float curr_tile_z2 = pos - dim / (float)2;
			pos = columns[j][i]->getPosition().x;
			dim = columns[j][i]->getDimension().x;
			float curr_tile_x1 = pos + dim / (float)2;
			float curr_tile_x2 = pos - dim / (float)2;

			if (player->getPosition().x <= curr_tile_x1 && player->getPosition().x >= curr_tile_x2 &&
				player->getPosition().z <= curr_tile_z1 && player->getPosition().z >= curr_tile_z2 &&
				player->getPosition().y > 0) {
				//found the tile under the player

				//update the player about the tile under him
				lowest_pos_for_player = columns[j][i]->getPosition().y + player->getRadius();
				
				if (player->getWalking() && columns[j][i]->getType() != TOUCHED_TILE_ID ) {
					//the player is walking on a tile that is first being touched

					if (columns[j][i]->getType() != SIMPLE_TILE_ID) {
						//the player touched a power-up tile

						//let the player know about the power-up that he got
						this->player->set_power_up(columns[j][i]->getType());
						this->player->set_power_up_left_time(POWER_UP_TIME);
						this->player->setColor(columns[j][i]->getColor());

						if (this->player->get_power_up() == BONUS_FUEL_POWER_UP_ID) {
							this->player->set_fuel(MAX_FUEL);
						}
						else if (this->player->get_power_up() == LOSE_FUEL_POWER_UP_ID) {
							this->player->set_fuel(this->player->get_fuel() / 2);
						}
						else if (this->player->get_power_up() == MAX_SPEED_POWER_UP_ID) {
							this->world_speed = MAX_WORLD_SPEED;
						}
						else if (this->player->get_power_up() == BONUS_HP_POWER_UP_ID) {
							this->player->increase_hp();
						}
						else if (this->player->get_power_up() == LOSE_HP_POWER_UP_ID) {
							this->player->decrease_hp();
						}
						else if (this->player->get_power_up() == GAME_OVER_POWER_UP_ID) {
							exit(-1);
						}
					}

					//the current column has been touched, set the color and type
					columns[j][i]->setType(TOUCHED_TILE_ID);
					columns[j][i]->setColor(TOUCHED_TILE_COLOR);
				}
			}

			//erase tile if needed
			if (columns[j][i]->getPosition().z < -columns[j][i]->getDimension().z) {
				columns[j].erase(columns[j].begin() + i);
				i--;
			}

			//if current column needs to be updated(depend on the current level)
			if (j < nocol && columns[j].size() >= 1) {
				//add new tile
				if (columns[j][columns[j].size() - 1]->getPosition().z < Z_FAR - (float)3 / 2 * TILE_DIMENSION.z) {
					Tile* tile = new Tile();
					tile->setPosition(glm::vec3(columns_x_pos[j], 0, Z_FAR - ((float)1 / 2 * TILE_DIMENSION.z) +
						(float)MINIMUM_DIST_BETWEEN_TILES + rand() % (MAXIMUM_DIST_BETWEEN_TILES - MINIMUM_DIST_BETWEEN_TILES) * 10 * 0.1));
					tile->setDimension(TILE_DIMENSION);

					if (this->tiles_rendered % TILES_BETWEEN_BONUS_FUEL_TILES == 0) {
						tile->setType(BONUS_FUEL_TILE_ID);
						tile->setColor(BONUS_FUEL_TILE_COLOR);
					}
					else if (this->tiles_rendered % TILES_BETWEEN_GAME_OVER_TILES == 0) {
						tile->setType(GAME_OVER_TILE_ID);
						tile->setColor(GAME_OVER_TILE_COLOR);
					}
					else if (this->tiles_rendered % TILES_BETWEEN_LOSE_FUEL_TILES == 0) {
						tile->setType(LOSE_FUEL_TILE_ID);
						tile->setColor(LOSE_FUEL_TILE_COLOR);
					}
					else if (this->tiles_rendered % TILES_BETWEEN_MAX_SPEED_TILES == 0) {
						tile->setType(MAX_SPEED_TILE_ID);
						tile->setColor(MAX_SPEED_TILE_COLOR);
					}
					else if (this->tiles_rendered % TILES_BETWEEN_BONUS_HP_TILE == 0) {
						tile->setType(BONUS_HP_TILE_ID);
						tile->setColor(BONUS_HP_TILE_COLOR);
					}
					else if (this->tiles_rendered % TILES_BETWEEN_LOSE_HP_TILE == 0) {
						tile->setType(LOSE_HP_TILE_ID);
						tile->setColor(LOSE_HP_TILE_COLOR);
					}
					else {
						tile->setType(SIMPLE_TILE_ID);
						tile->setColor(SIMPLE_TILE_COLOR);

					}
					columns[j].push_back(tile);
					INCREASE_TILES_RENDERD;

					//try an add obstacles -- Need to work on this!!ww
					/*if (rand() % 20 == 0) {
						cout << "ntra\n";
						Tile* tile = new Tile();
						tile->setPosition(glm::vec3(columns_x_pos[j], 3, Z_FAR - ((float)1 / 2 * TILE_DIMENSION.z) +
							(float)MINIMUM_DIST_BETWEEN_TILES + rand() % (MAXIMUM_DIST_BETWEEN_TILES - MINIMUM_DIST_BETWEEN_TILES) * 10 * 0.1));
						tile->setDimension(TILE_DIMENSION);

						tile->setType(SIMPLE_TILE_ID);
						tile->setColor(SIMPLE_TILE_COLOR);
						columns[j].push_back(tile);
					}*/
				}

				
			}
		}
	}
	//update the player about the tile under him(or if he has no tile under his butt xD)
	this->player->setCurentMiniumYPos(lowest_pos_for_player);
}

void Tema2::render_tiles() {
	//for every column, render the tiles, based on their type and position


	for (int j = 0; j < NO_COLUMNS; j++) {
		for (int i = 0; i < columns[j].size(); i++)
		{
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, columns[j][i]->getPosition());
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -TILE_DIMENSION.y / 2, 0));
			if (columns[j][i]->getType() == SIMPLE_TILE_ID) {
				RenderMesh(meshes["simple_tile"], shaders["ShaderTema2"], modelMatrix);
			}
			else if (columns[j][i]->getType() == TOUCHED_TILE_ID) {
				RenderMesh(meshes["touched_tile"], shaders["ShaderTema2"], modelMatrix);
			}
			else if (columns[j][i]->getType() == BONUS_FUEL_TILE_ID) {
				RenderMesh(meshes["bonus_fuel_tile"], shaders["ShaderTema2"], modelMatrix);
			}
			else if (columns[j][i]->getType() == MAX_SPEED_TILE_ID) {
				RenderMesh(meshes["max_speed_tile"], shaders["ShaderTema2"], modelMatrix);
			}
			else if (columns[j][i]->getType() == LOSE_FUEL_TILE_ID) {
				RenderMesh(meshes["lose_fuel_tile"], shaders["ShaderTema2"], modelMatrix);
			}
			else if (columns[j][i]->getType() == BONUS_HP_TILE_ID) {
				RenderMesh(meshes["bonus_hp_tile"], shaders["ShaderTema2"], modelMatrix);
			}
			else if (columns[j][i]->getType() == LOSE_HP_TILE_ID) {
				RenderMesh(meshes["lose_hp_tile"], shaders["ShaderTema2"], modelMatrix);
			}
			else if (columns[j][i]->getType() == GAME_OVER_TILE_ID) {
				RenderMesh(meshes["game_over_tile"], shaders["ShaderTema2"], modelMatrix);
			}
		}
	}
}

void Tema2::render_player()
{
	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, this->player->getPosition());
	if (this->player->get_power_up_left_time() > 0) {
		//the players has power-up left, use SphereShader - deformation shader
		RenderMesh(meshes["sphere"], shaders["SphereShader"], modelMatrix);
	}
	else {
		//the player has no current power-ups, use the default color shader
		RenderMesh(meshes["sphere"], shaders["Color"], modelMatrix);
	}
}

void Tema2::render_interface() {
	
	//render curr fuel block
	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.7f, 0.7f, 0)) *  
				  glm::scale(modelMatrix, glm::vec3(this->player->get_fuel(), 1, 1)) *
				  glm::scale(modelMatrix, glm::vec3(0.1f)) * 
		          glm::translate(modelMatrix, glm::vec3(1, 1, 0));
	RenderMesh(meshes["curr_fuel"], shaders["CurrFuelShader"], modelMatrix);


	//render max fuel block
	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.7f, 0.7f, 0)) *
				  glm::scale(modelMatrix, glm::vec3(0.1f)) *
				  glm::translate(modelMatrix, glm::vec3(1, 1, 0));
	RenderMesh(meshes["max_fuel"], shaders["MaxFuelShader"], modelMatrix);

	//render hp blocks
	for (int i = 0; i < this->player->get_hp(); i++) {
		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.68f + i * 0.07f, 0.5f, 0)) *
					  glm::scale(modelMatrix, glm::vec3(0.05f, 0.1f, 1)) *
					  glm::translate(modelMatrix, glm::vec3(1, 1, 0));
		RenderMesh(meshes["sphere"], shaders["CurrHPShader"], modelMatrix);
	}

}

void Tema2::check_colisions(Tile* tile) {

	//bibliography : http://www.jeffreythompson.org/collision-detection/circle-rect.php

	//player positions
	float cz = this->player->getPosition().z; 
	float cy = this->player->getPosition().y; 
	float cx = this->player->getPosition().x; 

	//tile dimensions
	float rw = tile->getDimension().z;
	float rh = tile->getDimension().y;
	float rl = tile->getDimension().x;

	//tile positions(left corner)
	float rz = tile->getPosition().z - tile->getDimension().z / (float)2;
	float ry = tile->getPosition().y - tile->getDimension().y / (float)2;
	float rx = tile->getPosition().x - tile->getDimension().x / (float)2;

	float testX = this->player->getPosition().x;
	float testZ = this->player->getPosition().z;
	float testY = this->player->getPosition().y;

	if (cz < rz) testZ = rz;
	else if (cz > rz + rw) testZ = rz + rw;

	if (cy < ry) testY = ry;
	else if (cy > ry + rh) testY = ry + rh;

	if (cx < rx) testX = rx;
	else if (cx > rx + rl) testX = rx + rl;

	//distances on every axe between sphere center and closest point of the tile
	float distZ = cz - testZ;
	float distY = cy - testY;
	float distX = cx - testX;
	float distance = sqrt((distZ * distZ) + (distY * distY) + (distX * distX));

	if (distance < player->getRadius()) {
		//collision
		if (cz < rz)
		{
			//front margin collision
			this->world_speed = 0;
		}
		else if (cz > rz + rw) {
			//back margin collision
			this->player->setPosition(player->getPosition() + glm::vec3(0, 0, player->getRadius() - distance));
		}
		else if (cx < rx) {
			//left margin collision
			this->player->setPosition(player->getPosition() + glm::vec3(-(player->getRadius() - distance), 0, 0));
		}
		else if (cx > rx + rl) {
			//right margin collision
			this->player->setPosition(player->getPosition() + glm::vec3(player->getRadius() - distance, 0, 0));
		}
	}
}
