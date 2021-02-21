#ifndef UTILS_HPP
#define UTILS_HPP

#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <exception>

// Initial width and height of window
const double W_HEIGHT = 320;
const double W_WIDTH = 640;

// Draw modes in main game
enum struct DRAW_MODE {
  BIOME,
  HEIGHT,
  POPULATION,
  TEMPERATURE,
  MOISTURE,
  URBANISATION,
  COUNTRY,
  INFRASTRUCTURE
};

// HUD state in main game
enum struct HUD_STATE {
  MAIN,
  VIEW,
  COMPETE,
  RELATIONS,
  MINERALS
};

// Main game state
enum struct GAME_STATE {
  MAIN_MENU,
  GENERATION,
  SIMULATION,
  CAMPAIGN,
  GAME_OVER
};

// Load options in file reading
enum struct LOAD_OPTION {
  RIVER,
  PATH_UNFINISHED,
  PATH_FINISHED,
  COUNTRY,
  STRUCTLENGTH
};
enum struct LOAD_OPTION2 {
  POPULATION,
  MOISTURE,
  STRUCTLENGTH
};
enum struct LOAD_OPTION3 {
  WIDTH,
  HEIGHT,
  DAY,
  STRUCTLENGTH
};

// Main menu options
enum struct MENU_OPTION {
  GENERATE,
  LOAD
};

// World generation options
enum struct GEN_OPTIONS {
  SIZE,
  FEATURES,
  HEIGHT,
  POPULATION,
  SEALEVEL,
  RIVERS,
  TEMPRANGE,
  MOISTNESS,
  GAMEMODE
};

/**
  * Function for generating a name
  * @Return The name generated
  */
std::string generateName();

#endif
