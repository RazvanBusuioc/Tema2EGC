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

	initialise_decor();

	create_meshes(); // create all the meshes used

	create_shaders(); // bind all the shaders used

	create_textures();
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

	update_obstacles();

	check_obstacle_colision();

	update_collectables();

	check_collectables_colision();

	update_player(deltaTimeSeconds); // update the player positions and atributes
	
	update_camera(); // update the position and state of the camra

	update_decor();

	

	render_obstacles();

	render_colectables();

	render_tiles(); // render the tile

	render_player(); // render the sphere

	render_interface(); // render the user interface

	render_decor();
}

void Tema2::FrameEnd()
{
	//DrawCoordinatSystem(camera->GetViewMatrix(), projection_matrix);
}

void Tema2::RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, Texture2D* texture1)
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

	int location9 = glGetUniformLocation(shader->program, "curr_points_color2D");
	if (location9 != -1) {
		glUniform3fv(location9, 1, glm::value_ptr(CURR_POINTS_COLOR));
	}
	
	if (texture1)
	{
		//TODO : activate texture location 0
		glActiveTexture(GL_TEXTURE0);

		//TODO : Bind the texture1 ID
		glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());

		//TODO : Send texture uniform value
		glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);

		/*int flag = 0;
		int flag_loc = glGetUniformLocation(shader->program, "flag");
		glUniform1i(flag_loc, flag);*/
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
	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
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
	}
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
	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float sensivityOX = 0.001f;
		float sensivityOY = 0.001f;

		if (window->GetSpecialKeyState() == 0) {
			//renderCameraTarget = false;
			// rotate the camera in First-person mode around OX and OY using deltaX and deltaY
			// use the sensitivity variables for setting up the rotation speed
			camera->RotateFirstPerson_OX(deltaY * sensivityOX);
			camera->RotateFirstPerson_OY(deltaX * sensivityOY);
		}

		if (window->GetSpecialKeyState() && GLFW_MOD_CONTROL) {
			//renderCameraTarget = true;
			// rotate the camera in Third-person mode around OX and OY using deltaX and deltaY
			// use the sensitivity variables for setting up the rotation speed
			camera->RotateThirdPerson_OX(deltaY * sensivityOX);
			camera->RotateThirdPerson_OY(deltaX * sensivityOY);
		}

	}
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

	{
		Shader* shader = new Shader("CurrPointsShader");
		shader->AddShader("Source/Tema2/Shaders/FuelVS.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Tema2/Shaders/CurrPointsFS.glsl", GL_FRAGMENT_SHADER);
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

	{
		Shader* shader = new Shader("TextureShader");
		shader->AddShader("Source/Tema2/Shaders/TextureVertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Tema2/Shaders/TextureFragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
}

void Tema2::create_textures() {
	const string textureLoc = "Source/Tema2/Textures/";
	// Load textures
	{
		Texture2D* texture = new Texture2D();
		bool b = texture->Load2D((textureLoc + "crate2.jpg").c_str(), GL_REPEAT);
		mapTextures["crate"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		bool b = texture->Load2D((textureLoc + "asphalt2.jpg").c_str(), GL_REPEAT);
		mapTextures["asphalt"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		bool b = texture->Load2D((textureLoc + "windows.png").c_str(), GL_REPEAT);
		mapTextures["windows"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		bool b = texture->Load2D((textureLoc + "sky.jpg").c_str(), GL_REPEAT);
		mapTextures["downtown"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		bool b = texture->Load2D((textureLoc + "barrel.jpg").c_str(), GL_REPEAT);
		mapTextures["barrel"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		bool b = texture->Load2D((textureLoc + "metal.jpg").c_str(), GL_REPEAT);
		mapTextures["metal"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		bool b = texture->Load2D((textureLoc + "light.jpg").c_str(), GL_REPEAT);
		mapTextures["light"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		bool b = texture->Load2D((textureLoc + "rust.jpg").c_str(), GL_REPEAT);
		mapTextures["rusted_metal"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		bool b = texture->Load2D((textureLoc + "green_metal.jpg").c_str(), GL_REPEAT);
		mapTextures["green_metal"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		bool b = texture->Load2D((textureLoc + "green_metal2.jpg").c_str(), GL_REPEAT);
		mapTextures["green_metal2"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		bool b = texture->Load2D((textureLoc + "orange_metal.jpg").c_str(), GL_REPEAT);
		mapTextures["orange_metal"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		bool b = texture->Load2D((textureLoc + "yellow_metal.jpg").c_str(), GL_REPEAT);
		mapTextures["yellow_metal"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		bool b = texture->Load2D((textureLoc + "red_metal.jpg").c_str(), GL_REPEAT);
		mapTextures["red_metal"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		bool b = texture->Load2D((textureLoc + "red_metal2.jpg").c_str(), GL_REPEAT);
		mapTextures["red_metal2"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		bool b = texture->Load2D((textureLoc + "obstacle.jpg").c_str(), GL_REPEAT);
		mapTextures["obstacle"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		bool b = texture->Load2D((textureLoc + "collectable.png").c_str(), GL_REPEAT);
		mapTextures["collectable"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		bool b = texture->Load2D((textureLoc + "player.jpg").c_str(), GL_REPEAT);
		mapTextures["player"] = texture;
	}
}

void Tema2::create_meshes() 
{	
	//create meshes for all kind of tiles
	/*meshes["simple_tile"] = Object3D::CreateTile("simple_tile", TILE_DIMENSION, SIMPLE_TILE_COLOR);
	meshes["touched_tile"] = Object3D::CreateTile("touched_tile", TILE_DIMENSION, TOUCHED_TILE_COLOR);
	meshes["bonus_fuel_tile"] = Object3D::CreateTile("bonus_fuel_tile", TILE_DIMENSION, BONUS_FUEL_TILE_COLOR);
	meshes["max_speed_tile"] = Object3D::CreateTile("max_speed_tile", TILE_DIMENSION, MAX_SPEED_TILE_COLOR);
	meshes["lose_fuel_tile"] = Object3D::CreateTile("lose_fuel_tile", TILE_DIMENSION, LOSE_FUEL_TILE_COLOR);
	meshes["bonus_hp_tile"] = Object3D::CreateTile("bonus_hp_tile", TILE_DIMENSION, BONUS_HP_TILE_COLOR);
	meshes["lose_hp_tile"] = Object3D::CreateTile("lose_hp_tile", TILE_DIMENSION, LOSE_HP_TILE_COLOR);
	meshes["game_over_tile"] = Object3D::CreateTile("game_over_tile", TILE_DIMENSION, GAME_OVER_TILE_COLOR);*/

	meshes["simple_tile"] = Object3D::CreteTexturedTile("simple_tile", TILE_DIMENSION );
	meshes["touched_tile"] = Object3D::CreteTexturedTile("touched_tile", TILE_DIMENSION );
	meshes["bonus_fuel_tile"] = Object3D::CreteTexturedTile("bonus_fuel_tile", TILE_DIMENSION );
	meshes["max_speed_tile"] = Object3D::CreteTexturedTile("max_speed_tile", TILE_DIMENSION );
	meshes["lose_fuel_tile"] = Object3D::CreteTexturedTile("lose_fuel_tile", TILE_DIMENSION );
	meshes["bonus_hp_tile"] = Object3D::CreteTexturedTile("bonus_hp_tile", TILE_DIMENSION );
	meshes["lose_hp_tile"] = Object3D::CreteTexturedTile("lose_hp_tile", TILE_DIMENSION);
	meshes["game_over_tile"] = Object3D::CreteTexturedTile("game_over_tile", TILE_DIMENSION);

	

	//create mesh for the player
	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("obstacle");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* m1 = new Mesh("max_fuel");
		m1->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "screen_quad.obj");
		meshes[m1->GetMeshID()] = m1;
	}

	{
		Mesh* m2 = new Mesh("curr_fuel");
		m2->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "screen_quad.obj");
		meshes[m2->GetMeshID()] = m2;
	}

	{
		Mesh* m2 = new Mesh("curr_points");
		m2->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "screen_quad.obj");
		meshes[m2->GetMeshID()] = m2;
	}

	{
		Mesh* m3 = new Mesh("crate");
		m3->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		decor_meshes[m3->GetMeshID()] = m3;
	}

	{
		Mesh* m4 = new Mesh("asphalt");
		m4->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "quad.obj");
		decor_meshes[m4->GetMeshID()] = m4;
	}

	{
		Mesh* m5 = new Mesh("windows");
		m5->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "quad.obj");
		decor_meshes[m5->GetMeshID()] = m5;
	}

	{
		Mesh* m6 = new Mesh("downtown");
		m6->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "quad.obj");
		decor_meshes[m6->GetMeshID()] = m6;
	}

	{
		Mesh* m7 = new Mesh("post");
		m7->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		decor_meshes[m7->GetMeshID()] = m7;
	}

	{
		Mesh* m8 = new Mesh("lamp");
		m8->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		decor_meshes[m8->GetMeshID()] = m8;
	}

	{
		Mesh* m9 = new Mesh("light");
		m9->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		decor_meshes[m9->GetMeshID()] = m9;
	}

	{
		Mesh* m5 = new Mesh("collectable");
		m5->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "quad.obj");
		meshes[m5->GetMeshID()] = m5;
	}

	decor_meshes["barrel"] = Object3D::CreateCylinder("cylinder", 2, 0.5f);


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

			int random = rand() % 10;
			if (random == 0) {
				this->obstacles.push_back(tile->getPosition() + glm::vec3(0, OBSTACLE_DIMENSION.y/2 , OBSTACLE_DIMENSION.z));
			}
			else if (random < 5) {
				Collectable* c = new Collectable(tile->getPosition() + 
					glm::vec3(0, COLLECTABLE_DIMENSION * COLLECTABLE_SCALE_RATIO / 2, 0), 0);
				this->collectables.push_back(c);
			}
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

void Tema2::initialise_decor() {
	for (int i = 0; i < Z_FAR; i+=DISTANCE_BETWEEN_DECOR_ELEMS) {
		
		if (rand() % 2 == 0) {
			DecorObject* obj = new DecorObject("crate",
				glm::vec3(rand() % (LEFT_DECORE_MAX_X - LEFT_DECORE_MIN_X) + LEFT_DECORE_MIN_X, CRATE_DIMENSION.y / 2 - TILE_DIMENSION.y, i),
				rand() % 90);
			left_decor.push_back(obj);
		}
		else {
			DecorObject* obj = new DecorObject("barrel",
				glm::vec3(rand() % (LEFT_DECORE_MAX_X - LEFT_DECORE_MIN_X) + LEFT_DECORE_MIN_X, BARREL_DIMENSION.y / 2 - TILE_DIMENSION.y, i),
				rand() % 90);
			left_decor.push_back(obj);
		}
	}

	for (int i = 2; i < Z_FAR; i += DISTANCE_BETWEEN_DECOR_ELEMS) {
		if (i > Z_FAR / 2 && this->is_lampPost_rendered == 0) {
			glm::vec3 lampPost_position = 
				glm::vec3(rand() % (RIGHT_DECORE_MIN_X - RIGHT_DECORE_MAX_X) + RIGHT_DECORE_MAX_X,
				LAMP_POST_DIMENSON.y * LAMP_POST_SCALE_RATIO.y / 2 - TILE_DIMENSION.y,
				i);

			DecorObject* obj = new DecorObject("punctiform_lampost", lampPost_position, 0);
			this->is_lampPost_rendered = 1;
			this->lampPost = obj;
			right_decor.push_back(obj);



			this->light_position = lampPost_position + glm::vec3(LAMP_DIMENSION.x * LAMP_POST_SCALE_RATIO.x,
				LAMP_POST_SCALE_RATIO.y * LAMP_POST_DIMENSON.y / 2 - TILE_DIMENSION.y, 0);
				continue;
		}
		if (rand() % 2 == 0) {
			DecorObject* obj = new DecorObject("crate",
				glm::vec3(rand() % (RIGHT_DECORE_MIN_X - RIGHT_DECORE_MAX_X) + RIGHT_DECORE_MAX_X, CRATE_DIMENSION.y / 2 - TILE_DIMENSION.y, i),
				rand() % 90);
			right_decor.push_back(obj);
		}
		else {
			DecorObject* obj = new DecorObject("barrel",
			glm::vec3(rand() % (RIGHT_DECORE_MIN_X - RIGHT_DECORE_MAX_X) + RIGHT_DECORE_MAX_X, BARREL_DIMENSION.y / 2 - TILE_DIMENSION.y, i),
			rand() % 90);
			right_decor.push_back(obj);
		}
	}

	
	for (int z = 0; z <= Z_FAR; z += ASPHALT_SCALE_RATIO * ASPHALT_TILE_DIMENSION) {
		asphalt_tiles.push_back(glm::vec3(0, 0, z));
	}

	for (int z = 0; z <= Z_FAR; z += WINDOWS_TILE_DIMENSION * WINDOWS_SCALE_RATIO) {
		left_windows.push_back(glm::vec3(ASPHALT_SCALE_RATIO * ASPHALT_TILE_DIMENSION / 2,
			WINDOWS_TILE_DIMENSION * WINDOWS_SCALE_RATIO / 2 - 2 * TILE_DIMENSION.y,
			z));

		right_windows.push_back(glm::vec3(-ASPHALT_SCALE_RATIO * ASPHALT_TILE_DIMENSION / 2,
			WINDOWS_TILE_DIMENSION * WINDOWS_SCALE_RATIO / 2 - 2 * TILE_DIMENSION.y,
			z));
	}
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

	if (player->getPosition().x >= SPHERE_MAX_X) {
		glm::vec3 currPosition = player->getPosition();
		player->setPosition(glm::vec3(SPHERE_MAX_X, currPosition.y, currPosition.z));
	}

	if (player->getPosition().x <= SPHERE_MIN_X) {
		glm::vec3 currPosition = player->getPosition();
		player->setPosition(glm::vec3(SPHERE_MIN_X, currPosition.y, currPosition.z));
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

					int random = rand() % 10;
					if (random == 0) {
						this->obstacles.push_back(tile->getPosition() + glm::vec3(0, OBSTACLE_DIMENSION.y / 2, OBSTACLE_DIMENSION.z));
					}
					else if (random < 5) {
						Collectable* c = new Collectable(tile->getPosition() +
							glm::vec3(0, COLLECTABLE_DIMENSION * COLLECTABLE_SCALE_RATIO / 2, 0), 0);
						this->collectables.push_back(c);
					}

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

void Tema2::update_decor() {

	if (left_decor[0]->getPosition().z < DECOR_Z_MIN) {
		left_decor.erase(left_decor.begin());

		DecorObject* last_elem = left_decor[left_decor.size() - 1];

		if (rand() % 2 == 0) {
			DecorObject* obj = new DecorObject("crate",
				glm::vec3(rand() % (LEFT_DECORE_MAX_X - LEFT_DECORE_MIN_X) + LEFT_DECORE_MIN_X, CRATE_DIMENSION.y / 2 - TILE_DIMENSION.y, last_elem->getPosition().z + DISTANCE_BETWEEN_DECOR_ELEMS),
				rand() % 90);
			left_decor.push_back(obj);
		}
		else {
			DecorObject* obj = new DecorObject("barrel",
				glm::vec3(rand() % (LEFT_DECORE_MAX_X - LEFT_DECORE_MIN_X) + LEFT_DECORE_MIN_X, BARREL_DIMENSION.y / 2 - TILE_DIMENSION.y, last_elem->getPosition().z + DISTANCE_BETWEEN_DECOR_ELEMS),
				rand() % 90);
			left_decor.push_back(obj);
		}
	}
	for (int i = 0; i < left_decor.size(); i++) {
		left_decor[i]->increasePosition(glm::vec3(0, 0, -this->world_speed));
	}

	if (right_decor[0]->getPosition().z < DECOR_Z_MIN) {
		DecorObject* last_elem = right_decor[right_decor.size() - 1];
		if (right_decor[0]->getName() == "punctiform_lampost") {
			right_decor.erase(right_decor.begin());
			glm::vec3 lampPost_position =
				glm::vec3(rand() % (RIGHT_DECORE_MIN_X - RIGHT_DECORE_MAX_X) + RIGHT_DECORE_MAX_X,
					LAMP_POST_DIMENSON.y * LAMP_POST_SCALE_RATIO.y / 2 - TILE_DIMENSION.y,
					last_elem->getPosition().z + DISTANCE_BETWEEN_DECOR_ELEMS);

			DecorObject* obj = new DecorObject("punctiform_lampost", lampPost_position, 0);
			this->is_lampPost_rendered = 1;
			this->lampPost = obj;

			this->light_position = lampPost_position + glm::vec3(LAMP_DIMENSION.x * LAMP_POST_SCALE_RATIO.x,
				LAMP_POST_SCALE_RATIO.y * LAMP_POST_DIMENSON.y / 2 - TILE_DIMENSION.y, 0);

			right_decor.push_back(obj);

		}
		else {
			right_decor.erase(right_decor.begin());


			if (rand() % 2 == 0) {
				DecorObject* obj = new DecorObject("crate",
					glm::vec3(rand() % (RIGHT_DECORE_MIN_X - RIGHT_DECORE_MAX_X) + RIGHT_DECORE_MAX_X, CRATE_DIMENSION.y / 2 - TILE_DIMENSION.y, last_elem->getPosition().z + DISTANCE_BETWEEN_DECOR_ELEMS),
					rand() % 90);

				right_decor.push_back(obj);
			}
			else {
				DecorObject* obj = new DecorObject("barrel",
					glm::vec3(rand() % (RIGHT_DECORE_MIN_X - RIGHT_DECORE_MAX_X) + RIGHT_DECORE_MAX_X, BARREL_DIMENSION.y / 2 - TILE_DIMENSION.y, last_elem->getPosition().z + DISTANCE_BETWEEN_DECOR_ELEMS),
					rand() % 90);

				right_decor.push_back(obj);
			}
		}
	}

	for (int i = 0; i < right_decor.size(); i++) {
		right_decor[i]->increasePosition(glm::vec3(0, 0, -this->world_speed));
	}
	//modify light position
	this->light_position -= glm::vec3(0, 0, this->world_speed);



	for (int i = 0; i < asphalt_tiles.size(); i++) {
		asphalt_tiles[i] -= glm::vec3(0, 0, this->world_speed);
	}
	if (asphalt_tiles[0].z <= -(ASPHALT_TILE_DIMENSION * ASPHALT_SCALE_RATIO)) {
		float x = asphalt_tiles[0].x;

		glm::vec3 last_pos = asphalt_tiles[asphalt_tiles.size() - 1];

		asphalt_tiles.push_back(last_pos + glm::vec3(0, 0, ASPHALT_TILE_DIMENSION * ASPHALT_SCALE_RATIO));

		asphalt_tiles.erase(asphalt_tiles.begin());
	}

	for (int i = 0; i < left_windows.size(); i++) {
		left_windows[i] -= glm::vec3(0, 0, this->world_speed);
	}

	if (left_windows[0].z < -(WINDOWS_SCALE_RATIO * WINDOWS_TILE_DIMENSION)/2) {
		left_windows.erase(left_windows.begin());

		glm::vec3 last_position = left_windows[left_windows.size() - 1];

		left_windows.push_back(last_position + glm::vec3(0, 0, WINDOWS_SCALE_RATIO * WINDOWS_TILE_DIMENSION));
	}

	for (int i = 0; i < right_windows.size(); i++) {
		right_windows[i] -= glm::vec3(0, 0, this->world_speed);
	}

	if (right_windows[0].z < -(WINDOWS_SCALE_RATIO * WINDOWS_TILE_DIMENSION) / 2) {
		right_windows.erase(right_windows.begin());

		glm::vec3 last_position = right_windows[right_windows.size() - 1];

		right_windows.push_back(last_position + glm::vec3(0, 0, WINDOWS_SCALE_RATIO * WINDOWS_TILE_DIMENSION));
	}
	
	//this->lampPost->increasePosition(glm::vec3(0, 0, -this->world_speed));
}

void Tema2::update_obstacles() {
	for (int i = 0; i < this->obstacles.size(); i++) {
		
		this->obstacles[i] += glm::vec3(0, 0, -this->world_speed);
		if (this->obstacles[i].z < DECOR_Z_MIN) {
			this->obstacles.erase(this->obstacles.begin() + i);
		}
	}
}

void Tema2::update_collectables() {
	//cout << collectables.size() << endl;
	
	for (int i = 0; i < this->collectables.size(); i++) {
		
		this->collectables[i]->increase_pos(glm::vec3(0, 0, -this->world_speed));
		this->collectables[i]->increase_radians(RADIANS(90));
		if (this->collectables[i]->getPosition().z < DECOR_Z_MIN) {
			this->collectables.erase(this->collectables.begin() + i);
		}
	}
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
				RenderMesh(meshes["simple_tile"], shaders["TextureShader"], modelMatrix, mapTextures["metal"]);
			}
			else if (columns[j][i]->getType() == TOUCHED_TILE_ID) {
				RenderMesh(meshes["touched_tile"], shaders["TextureShader"], modelMatrix, mapTextures["rusted_metal"]);
			}
			else if (columns[j][i]->getType() == BONUS_FUEL_TILE_ID) {
				RenderMesh(meshes["bonus_fuel_tile"], shaders["TextureShader"], modelMatrix, mapTextures["green_metal"]);
			}
			else if (columns[j][i]->getType() == MAX_SPEED_TILE_ID) {
				RenderMesh(meshes["max_speed_tile"], shaders["TextureShader"], modelMatrix, mapTextures["orange_metal"]);
			}
			else if (columns[j][i]->getType() == LOSE_FUEL_TILE_ID) {
				RenderMesh(meshes["lose_fuel_tile"], shaders["TextureShader"], modelMatrix, mapTextures["yellow_metal"]);
			}
			else if (columns[j][i]->getType() == BONUS_HP_TILE_ID) {
				RenderMesh(meshes["bonus_hp_tile"], shaders["TextureShader"], modelMatrix, mapTextures["green_metal2"]);
			}
			else if (columns[j][i]->getType() == LOSE_HP_TILE_ID) {
				RenderMesh(meshes["lose_hp_tile"], shaders["TextureShader"], modelMatrix, mapTextures["red_metal2"]);
			}
			else if (columns[j][i]->getType() == GAME_OVER_TILE_ID) {
				RenderMesh(meshes["game_over_tile"], shaders["TextureShader"], modelMatrix, mapTextures["red_metal"]);
			}
		}
	}
}

void Tema2::render_obstacles() {
	for (int i = 0; i < this->obstacles.size(); i++) {
		glm::vec3 currObstaclePosition = this->obstacles[i];
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, currObstaclePosition);
		RenderMesh(meshes["obstacle"], shaders["TextureShader"], modelMatrix, mapTextures["obstacle"]);
	}
}

void Tema2::render_colectables() {
	for (int i = 0; i < collectables.size(); i++) {
		glm::vec3 currCollectablePosition = this->collectables[i]->getPosition();

		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, currCollectablePosition);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(COLLECTABLE_SCALE_RATIO));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(this->collectables[i]->getRadians()), glm::vec3(0, 1, 0));
		RenderMesh(meshes["collectable"], shaders["TextureShader"], modelMatrix, mapTextures["collectable"]);
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
		RenderMesh(meshes["sphere"], shaders["TextureShader"], modelMatrix, mapTextures["player"]);
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

	//render curr points  block
	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.9f, 0.8f, 0)) *
		glm::scale(modelMatrix, glm::vec3(this->score, 1, 1)) *
		glm::scale(modelMatrix, glm::vec3(0.1f, 0.05f, 0.1f)) *
		glm::translate(modelMatrix, glm::vec3(1, 1, 0));
	RenderMesh(meshes["curr_points"], shaders["CurrPointsShader"], modelMatrix);


	//render max points block
	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.9f, 0.8f, 0)) *
		glm::scale(modelMatrix, glm::vec3(0.1f, 0.05f, 0.1f)) *
		glm::translate(modelMatrix, glm::vec3(1, 1, 0));
	RenderMesh(meshes["max_fuel"], shaders["MaxFuelShader"], modelMatrix);

}


void Tema2::render_decor() {
	for (int i = 0; i < left_decor.size(); i++) {
		
		if (left_decor[i]->getName() == "crate") {
			render_crate(left_decor[i]);
		}
		if (left_decor[i]->getName() == "barrel") {
			render_barrel(left_decor[i]);
		}
		
	}

	for (int i = 0; i < right_decor.size(); i++) {
		if (right_decor[i]->getName() == "crate") {
			render_crate(right_decor[i]);
		}
		if (right_decor[i]->getName() == "barrel") {
			render_barrel(right_decor[i]);
		}
		if (right_decor[i]->getName() == "punctiform_lampost") {
			render_punctiform_lampPost(this->lampPost);
		}
	}

	for (int j = 0; j < asphalt_tiles.size(); j++) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -1.1 * TILE_DIMENSION.y, 0) + asphalt_tiles[j]);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(ASPHALT_SCALE_RATIO));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(90), glm::vec3(1, 0, 0));
		RenderMesh(decor_meshes["asphalt"], shaders["TextureShader"], modelMatrix, mapTextures["asphalt"]);

	}
	
	

	for (int i = 0; i < left_windows.size(); i++) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, left_windows[i]);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(WINDOWS_SCALE_RATIO));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(90), glm::vec3(0, 1, 0));
		RenderMesh(decor_meshes["windows"], shaders["TextureShader"], modelMatrix, mapTextures["windows"]);
	}

	for (int i = 0; i < right_windows.size(); i++) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, right_windows[i]);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(WINDOWS_SCALE_RATIO));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(90), glm::vec3(0, 1, 0));
		RenderMesh(decor_meshes["windows"], shaders["TextureShader"], modelMatrix, mapTextures["windows"]);
	}

	//render sky
	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0, WINDOWS_SCALE_RATIO * WINDOWS_TILE_DIMENSION/2, 0.7*Z_FAR));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(2 * WINDOWS_SCALE_RATIO));
	modelMatrix = glm::rotate(modelMatrix, RADIANS(-180), glm::vec3(0, 0, 1));
	RenderMesh(decor_meshes["downtown"], shaders["TextureShader"], modelMatrix, mapTextures["downtown"]);

	//render post
	

}

void Tema2::render_punctiform_lampPost(DecorObject* obj) {

	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, obj->getPosition());
	modelMatrix = glm::scale(modelMatrix, LAMP_POST_SCALE_RATIO);
	RenderMesh(decor_meshes["post"], shaders["TextureShader"], modelMatrix, mapTextures["metal"]);

	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(LAMP_DIMENSION.x * LAMP_POST_SCALE_RATIO.x,
						LAMP_POST_SCALE_RATIO.y * LAMP_POST_DIMENSON.y/2 - TILE_DIMENSION.y, 0));

	modelMatrix = glm::translate(modelMatrix, obj->getPosition());
	modelMatrix = glm::scale(modelMatrix, LAMP_SCALE_RATIO);
	RenderMesh(decor_meshes["light"], shaders["TextureShader"], modelMatrix, mapTextures["light"]);
}

void Tema2::render_crate(DecorObject *obj) {
	if (obj->getSpecial() == 3) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, obj->getPosition());
		RenderMesh(decor_meshes[obj->getName()], shaders["TextureShader"],
			modelMatrix, mapTextures[obj->getName()]);

		modelMatrix = glm::translate(modelMatrix, glm::vec3((CRATE_DIMENSION.z + CRATE_DIMENSION.z / 5), 0, 0));
		RenderMesh(decor_meshes[obj->getName()], shaders["TextureShader"],
			modelMatrix, mapTextures[obj->getName()]);

		modelMatrix = glm::translate(modelMatrix, glm::vec3(-CRATE_DIMENSION.z / 2, CRATE_DIMENSION.z, 0));
		RenderMesh(decor_meshes[obj->getName()], shaders["TextureShader"],
			modelMatrix, mapTextures[obj->getName()]);
	}
	else if (obj->getSpecial() == 2) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, obj->getPosition()) *
			glm::rotate(modelMatrix, RADIANS(obj->getRadians()), glm::vec3(0, 1, 0));
		RenderMesh(decor_meshes[obj->getName()], shaders["TextureShader"],
			modelMatrix, mapTextures[obj->getName()]);

		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, obj->getPosition() + glm::vec3(0, CRATE_DIMENSION.y, 0)) *
			glm::rotate(modelMatrix, RADIANS(-obj->getRadians()), glm::vec3(0, 1, 0));
		RenderMesh(decor_meshes[obj->getName()], shaders["TextureShader"],
			modelMatrix, mapTextures[obj->getName()]);
	}
	else {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, obj->getPosition());
		modelMatrix = glm::rotate(modelMatrix, RADIANS(obj->getRadians()), glm::vec3(0, 1, 0));
		RenderMesh(decor_meshes[obj->getName()], shaders["TextureShader"],
			modelMatrix, mapTextures[obj->getName()]);
	}
}

void Tema2::render_barrel(DecorObject* obj) {

	if(obj->getSpecial() == 2){
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, obj->getPosition());
		modelMatrix = glm::rotate(modelMatrix, RADIANS(-90), glm::vec3(1, 0, 0));
		RenderMesh(decor_meshes[obj->getName()], shaders["TextureShader"], modelMatrix, mapTextures[obj->getName()]);

		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, obj->getPosition() + glm::vec3(BARREL_DIMENSION.x + 0.1f, 0, 0));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(-90), glm::vec3(1, 0, 0));
		RenderMesh(decor_meshes[obj->getName()], shaders["TextureShader"], modelMatrix, mapTextures[obj->getName()]);

		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, obj->getPosition() + glm::vec3(BARREL_DIMENSION.x/2, BARREL_DIMENSION.y, 0));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(-90), glm::vec3(1, 0, 0));
		RenderMesh(decor_meshes[obj->getName()], shaders["TextureShader"], modelMatrix, mapTextures[obj->getName()]);
	}
	else if (obj->getSpecial() == 1) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, obj->getPosition() + glm::vec3(0, -BARREL_DIMENSION.x/2, 0));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(obj->getRadians()), glm::vec3(0, 1, 0));
		RenderMesh(decor_meshes[obj->getName()], shaders["TextureShader"], modelMatrix, mapTextures[obj->getName()]);
	}
	else {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, obj->getPosition());
		modelMatrix = glm::rotate(modelMatrix, RADIANS(-90), glm::vec3(1, 0, 0));
		RenderMesh(decor_meshes[obj->getName()], shaders["TextureShader"], modelMatrix, mapTextures[obj->getName()]);
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

void Tema2::check_obstacle_colision() {
	for (int i = 0; i < this->obstacles.size(); i++) {
		glm::vec3 obstacle = this->obstacles[i];

		//player positions
		float cz = this->player->getPosition().z;
		float cy = this->player->getPosition().y;
		float cx = this->player->getPosition().x;

		//tile dimensions
		float rw = OBSTACLE_DIMENSION.z;
		float rh = OBSTACLE_DIMENSION.y;
		float rl = OBSTACLE_DIMENSION.x;

		float rz = obstacle.z - OBSTACLE_DIMENSION.z / (float)2;
		float ry = obstacle.y - OBSTACLE_DIMENSION.y / (float)2;
		float rx = obstacle.x - OBSTACLE_DIMENSION.x / (float)2;

		float testX = this->player->getPosition().x;
		float testZ = this->player->getPosition().z;
		float testY = this->player->getPosition().y;

		if (cz < rz) testZ = rz;
		else if (cz > rz + rw) testZ = rz + rw;

		if (cy < ry) testY = ry;
		else if (cy > ry + rh) testY = ry + rh;

		if (cx < rx) testX = rx;
		else if (cx > rx + rl) testX = rx + rl;

		float distZ = cz - testZ;
		float distY = cy - testY;
		float distX = cx - testX;
		float distance = sqrt((distZ * distZ) + (distY * distY) + (distX * distX));

		if (distance < player->getRadius()) {
			this->player->set_power_up(LOSE_HP_POWER_UP_ID);
			this->player->set_power_up_left_time(POWER_UP_TIME);
			this->player->setColor(LOSE_HP_TILE_COLOR);
			this->player->decrease_hp();
			this->obstacles.erase(this->obstacles.begin() + i);
			break;
		}
	}
}

void Tema2::check_collectables_colision() {
	for (int i = 0; i < this->collectables.size(); i++) {
		glm::vec3 collectable = this->collectables[i]->getPosition();

		//player positions
		float cz = this->player->getPosition().z;
		float cy = this->player->getPosition().y;
		float cx = this->player->getPosition().x;

		//tile dimensions
		float rw = COLLECTABLE_DIMENSION * COLLECTABLE_SCALE_RATIO;
		float rh = COLLECTABLE_DIMENSION * COLLECTABLE_SCALE_RATIO;
		float rl = COLLECTABLE_DIMENSION * COLLECTABLE_SCALE_RATIO;

		float rz = collectable.z - rw / (float)2;
		float ry = collectable.y - rh / (float)2;
		float rx = collectable.x - rl / (float)2;

		float testX = this->player->getPosition().x;
		float testZ = this->player->getPosition().z;
		float testY = this->player->getPosition().y;

		if (cz < rz) testZ = rz;
		else if (cz > rz + rw) testZ = rz + rw;

		if (cy < ry) testY = ry;
		else if (cy > ry + rh) testY = ry + rh;

		if (cx < rx) testX = rx;
		else if (cx > rx + rl) testX = rx + rl;

		float distZ = cz - testZ;
		float distY = cy - testY;
		float distX = cx - testX;
		float distance = sqrt((distZ * distZ) + (distY * distY) + (distX * distX));

		if (distance < player->getRadius()) {
			this->score += 0.01f;
			this->collectables.erase(this->collectables.begin() + i);
			break;
		}
	}
}