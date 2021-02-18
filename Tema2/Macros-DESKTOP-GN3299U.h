#pragma once

#define TILE_DIMENSION glm::vec3(1.5f, 0.35f, 5)
#define TILE_DIMENSION2 glm::vec3(1.5f, 1.5f, 1.5f)

//sphere macros
#define SPHERE_RADIUS 0.5
#define SPHERE_INITIAL_POSITION glm::vec3(0, SPHERE_RADIUS, 0)
#define SPHERE_MOVEMENT_FACTOR 4
#define JUMP_VELOCITY_VECTOR glm::vec3(0, 3, 0)

#define POS_Y_MIN SPHERE_RADIUS - TILE_DIMENSION.y/2 // if the player`s pos is below this, game ends
#define Z_FAR 100 // z far for camera. After this point, no tiles are rendered
#define LOWEST_Y_POS -100 // let the player know that he has no tile under his feet

//colors for the tile depending on their power-ups
#define GAME_OVER_TILE_COLOR glm::vec3(0.78f, 0, 0.0f)
#define LOSE_FUEL_TILE_COLOR glm::vec3(0.99f, 0.88f, 0.15f)
#define MAX_SPEED_TILE_COLOR glm::vec3(1, 0.37f, 0.07f)
#define BONUS_FUEL_TILE_COLOR glm::vec3(0, 0.4f, 0.0f)
#define BONUS_HP_TILE_COLOR glm::vec3(0.5f, 0.95f, 0.55f)
#define LOSE_HP_TILE_COLOR glm::vec3(0.33f, 0, 0)
#define SIMPLE_TILE_COLOR glm::vec3(0,0,1)
#define TOUCHED_TILE_COLOR glm::vec3(1,0,1)

//offsets between the tiles
#define MINIMUM_DIST_BETWEEN_TILES 4
#define MAXIMUM_DIST_BETWEEN_TILES 7

// IDs for different type of tiles
#define SIMPLE_TILE_ID 0
#define TOUCHED_TILE_ID 1
#define BONUS_FUEL_TILE_ID 2
#define MAX_SPEED_TILE_ID 3
#define LOSE_FUEL_TILE_ID 4
#define GAME_OVER_TILE_ID 5
#define BONUS_HP_TILE_ID 6
#define LOSE_HP_TILE_ID 7

// positions and offsets for 3rd person camera
#define THIRD_PERSON_CAMERA_INITIAL_POSITION glm::vec3(0, 3, -5)
#define THIRD_PERSON_CAMERA_INITIAL_CENTER glm::vec3(0, 1.5f, 1)
#define THIRD_PERSON_CAMERA_UP glm::vec3(0, 1, 0)
#define THIRD_PERSON_CAMERA_POSITION_OFFSET glm::vec3(0, 2.5f, -5)
#define THIRD_PERSON_CAMERA_CENTER_OFFSET glm::vec3(0, 1, 1)

// positions and offsets for 1st person camera
#define FIRST_PERSON_CAMRA_POSITION_OFFSET glm::vec3(0, 0, SPHERE_RADIUS)
#define FIRST_PERSON_CAMERA_CENTER_OFFSET glm::vec3(0, -0.5f, 4)
#define FIRST_PERSON_CAMERA_UP glm::vec3(0, 1, 0)

// world speed macros
#define INITIAL_WORLD_SPEED 0
#define MAX_WORLD_SPEED 0.3
#define MINIMUM_WORLD_SPEED 0
#define WORLD_SPEED_INCREASE_FACTOR 0.2
#define LOWEST_SPPED_LVL1 0.00f
#define LOWEST_SPPED_LVL2 0.0f
#define LOWEST_SPPED_LVL3 0.00f

// X positions for the columns of tiles
#define COL1_X 0
#define COL2_X -2*TILE_DIMENSION.x
#define COL3_X 2*TILE_DIMENSION.x
#define COL4_X -4*TILE_DIMENSION.x
#define COL5_X 4*TILE_DIMENSION.x
#define NO_COLUMNS 3

//colors for player depending on the power-ups
#define DEFAULT_PLAYER_COLOR glm::vec3(0.57f, 0.66f, 1)
#define GAME_OVER_PLAYER_COLOR glm::vec3(1,0,0)
#define LOSE_FUEL_PLAYER_COLOR glm::vec3(1,1,0)
#define MAX_SPEED_PLAYER_COLOR glm::vec3(1,0.5,0)
#define BONUS_FUEL_PLAYER_COLOR glm::vec3(0,1,0)

//power-up tiles offset
#define TILES_BETWEEN_BONUS_FUEL_TILES 17
#define TILES_BETWEEN_LOSE_FUEL_TILES 7
#define TILES_BETWEEN_GAME_OVER_TILES 13
#define TILES_BETWEEN_MAX_SPEED_TILES 11
#define TILES_BETWEEN_BONUS_HP_TILE 29
#define TILES_BETWEEN_LOSE_HP_TILE 37

//ID s for player s current power up special power
#define NO_POWER_UP_ID 0
#define BONUS_FUEL_POWER_UP_ID 2
#define MAX_SPEED_POWER_UP_ID 3
#define LOSE_FUEL_POWER_UP_ID 4
#define GAME_OVER_POWER_UP_ID 5
#define BONUS_HP_POWER_UP_ID 6
#define LOSE_HP_POWER_UP_ID 7

// a power up maintains this time
#define POWER_UP_TIME 1.5 

//fuel macros
#define FUEL_DECREASE_FACTOR 0.35
#define MAX_FUEL_COLOR glm::vec3(1,1,1)
#define CURR_FUEL_COLOR glm::vec3(0,1,0)
#define CURR_POINTS_COLOR glm::vec3(1, 0, 0)
#define MAX_FUEL 1

//the color of the circles of the user interface that represent the current hp
#define HP_COLOR glm::vec3(1,0,0)

//code macro
#define INCREASE_TILES_RENDERD tiles_rendered++;

//macros for current level of the game
#define NO_COLUMNS_LVL0 3
#define NO_COLUMNS_LVL1 3
#define NO_COLUMNS_LVL2 3
#define NO_COLUMNS_LVL3 3
#define TIME_TO_LVL1 5
#define TIME_TO_LVL2 10
#define TIME_TO_LVL3 20


#define TEXTURE_LOCATION "Source/Tema2/Textures/"

#define CRATE_DIMENSION glm::vec3(1,1,1)
#define BARREL_DIMENSION glm::vec3(1,2,1)

#define DISTANCE_BETWEEN_DECOR_ELEMS 5

#define DECOR_Z_MIN -2

#define LEFT_DECORE_MIN_X 6
#define LEFT_DECORE_MAX_X 8

#define RIGHT_DECORE_MIN_X -6
#define RIGHT_DECORE_MAX_X -8

#define SPHERE_MAX_X 4.5
#define SPHERE_MIN_X -4.5

#define ASPHALT_TILE_DIMENSION 2
#define ASPHALT_SCALE_RATIO 10

#define WINDOWS_TILE_DIMENSION 2
#define WINDOWS_SCALE_RATIO 10

#define LAMP_POST_DIMENSON glm::vec3(1, 1, 1)
#define LAMP_POST_SCALE_RATIO glm::vec3(0.5f, 6, 0.5f)

#define LAMP_DIMENSION glm::vec3(1, 1, 1)
#define LAMP_SCALE_RATIO glm::vec3(0.5f)

#define OBSTACLE_DIMENSION glm::vec3(1,1,1)

#define COLLECTABLE_DIMENSION 2
#define COLLECTABLE_SCALE_RATIO 0.35f