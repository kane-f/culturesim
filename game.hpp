#ifndef GAME_HPP
#define GAME_HPP

#include "field.hpp"

class Game
{
public:
	/**
	 * Constructor.
	 * @param window A pointer to the main render window.
	 */
	Game(sf::RenderWindow* window);

	/**
	 * Initializes the game object by initializing all objects the main game uses.
	 * @return True if the game was initialized successfully.
	 */
	void Initialize();

	/**
	 * The main game loop. This loop in turn updates the game, and draws all objects to screen.
	 * It's also responsible for the game logic timing.
	 */
	void Run();

	/**
	 * The input handle loop.
	 * @return False if the game is quit.
	 */
	bool HandleInput(float deltaTime);

	/**
	 * The main update loop. This loop in turns calls the update loops of all game objects.
	 */
	bool Update(float deltaTime);

	/**
	 * Draws all game objects to screen.
	 * @return False if objects could not be drawn.
	 */
	bool Draw(float deltaTime);
  bool DrawCursor(float deltaTime);

private:

	/**
	 * Sub-functions for drawing individual parts of the game.
	 */
  void DrawMainMenu();
	void DrawGenerationMenu();
	bool DrawGame(float deltaTime);
	void DrawHUD(float deltaTime);

	/**
	 * Draw the given string at the given position.
	 */
	void DrawString(std::string text, sf::Vector2f position, unsigned int size, bool selected = false);

	/**
	 * Text used by the DrawText() function.
	 */
	sf::Text m_text;

	/**
	 * The main application window.
	 */
	sf::RenderWindow& m_window;

  /**
   * Used in intervals.
   */
  sf::Clock m_clock;
  float m_time;

  /**
   * Controllable time interval with pause.
   */
  float m_timeInterval;
  bool m_paused;

  /**
   * Days advanced on each step.
   */
  int m_dayInterval;

	/**
	 * A bool that tracks the running state of the game for the loop.
	 */
	bool m_isRunning;

	/**
	 * The size of the screen and window.
	 */
	sf::Vector2u m_screenSize;

	/**
	 * The center of the screen.
	 */
	sf::Vector2f m_screenCenter;

	/**
	 * The main views.
	 */
	sf::View m_menuView;
	sf::View m_mapView;
	sf::View m_hudView;

	/**
	 * Draw mode used
	 */
	DRAW_MODE m_drawMode;

	/**
	 * The state of the Game
	 */
	GAME_STATE m_gameState;

  /**
   * The state of the HUD in-game
   */
  HUD_STATE m_hudState;

	/**
	 * The generation option for the respective menu
	 */
	GEN_OPTIONS m_genOption;

  /**
   * The main menu option
   */
  MENU_OPTION m_menuOption;

	/**
	 * The main game font
	 */
  sf::Font m_font;

	/**
	 * An instance of the game field.
	 */
	Field field;

	/**
	 * Values for generation.
	 */
	unsigned short m_mapSize;
	short m_featureSize;
	short m_maxHeight;
	short m_density;
	short m_seaLevel;
  unsigned short m_riverFrequency;
  GAME_STATE m_selectedState;
};
#endif
