#pragma once
#include <Component/SimpleScene.h>
#include <Core/GPU/Mesh.h>
#include "Camera/Camera.h"
#include <Tema2\GameComponents\Tile.h>
#include <Tema2\GameComponents\Sphere.h>
#include <Tema2/Macros.h>

class Tema2 : public SimpleScene
{
	public:
		Tema2();
		~Tema2();

		void Init() override;


	private:
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		void RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix) override;

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;

		//create all the meshes used
		void create_meshes();

		//bind all the shaders created in "Shaders" dirctory
		void create_shaders();

		//create textures used and store them into the texture map
		void create_textures();

		//initialise all the components of the game
		void initialise_columns();
		void initialise_player();
		void initialise_camera();

		//update the components of the game
		void update_columns();
		void update_player(float time);
		void update_camera();
		void update_curr_level();

		void render_tiles();
		void render_player();
		void render_interface();

		//check for special collisions(margin of the tile collisions)
		void check_colisions(Tile* tile);

	protected:

		//world camera and its components
		Camera::Camera* camera;
		glm::mat4 projection_matrix;
		float fov = 60;
		float orto_height = 10;
		float orto_width = 10;


		float world_speed = INITIAL_WORLD_SPEED; // the speed of the player
		float lowest_pos_for_player; // lowes posible position for the player at a given time
		float lowest_speed = 0; //lowest possible speed at a given time

		int tiles_rendered = 0; // number of tiles renderd in the game untill a specific moment

		bool third_person_camera = true; // if true, game is in third person mode
		bool first_person_camera = false; // if trye, game is in first person mode

		std::vector<Tile*> columns[NO_COLUMNS]; // vector of columns. Each column contains tiles
		float columns_x_pos[NO_COLUMNS]; // the X position for every column of tiles

		Sphere* player; // the main character

		Mesh* fuel_bar; // current fuel bar
		Mesh* max_fuel; // maximum fuel bar

		int curr_level = 0;

		//map for textures
		std::unordered_map<std::string, Texture2D*> mapTextures;
};
