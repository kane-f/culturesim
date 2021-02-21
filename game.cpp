#include "game.hpp"

Game::Game(sf::RenderWindow* window) :
m_window(*window),
m_isRunning(true),
m_screenSize({ 0, 0 }),
m_screenCenter({ 0, 0 }),
m_mapSize(W_HEIGHT),
m_featureSize(96),
m_maxHeight(4096),
m_density(64),
m_seaLevel(-1024),
m_riverFrequency(20),
m_timeInterval(1.0f),
m_paused(false),
m_dayInterval(1),
field(&m_window)
{
  // Enable VSync.
	m_window.setVerticalSyncEnabled(true);

	// Calculate and store the center of the screen.
	m_screenCenter = { m_window.getSize().x / 2.0f, m_window.getSize().y / 2.0f };

	// Set enums
	m_drawMode = DRAW_MODE::BIOME;
	m_gameState = GAME_STATE::MAIN_MENU;
  m_hudState = HUD_STATE::MAIN;
	m_genOption = GEN_OPTIONS::SIZE;
  m_menuOption = MENU_OPTION::GENERATE;
  m_selectedState = GAME_STATE::SIMULATION;
}

// Initializes the game.
void Game::Initialize()
{
  // Get the screen size.
	m_screenSize = m_window.getSize();

	// Load game font
	if (!m_font.loadFromFile("bin/res/FSEX300.ttf")) m_isRunning = false;

	// Initialize menu viewport
	m_menuView.setCenter(0,0);
	m_menuView.setSize(m_window.getSize().x,m_window.getSize().y);

	// Initialize map viewport
	m_mapView.setCenter(0,0);
	m_mapView.setSize(m_window.getSize().x/2,m_window.getSize().y);
	m_mapView.setViewport(sf::FloatRect(0,0,0.5,1));

	// Initialize HUD viewport
	m_hudView.setCenter(m_window.getSize().x/4,0);
	m_hudView.setSize(m_window.getSize().x,m_window.getSize().y);
	m_hudView.setViewport(sf::FloatRect(0.5,0,1,1));

  // Draw all items in the level.
	if (!Draw(0)) m_isRunning = false;

  // Start game clock
  m_time = m_clock.restart().asSeconds();
}

// Main game loop.
void Game::Run()
{
  // Loop until there is a quiet message from the window or the user pressed escape.
	while (m_isRunning)
	{
    // Update timer
    m_time = m_clock.getElapsedTime().asSeconds();

    // Handle input
    if (!HandleInput(m_time)) m_isRunning = false;

    // Update game based on time interval and in main game modes if not paused
    if (m_time > m_timeInterval && !m_paused &&
        (m_gameState == GAME_STATE::SIMULATION || m_gameState == GAME_STATE::CAMPAIGN))
    {
      // Update and draw the game
      if (!Update(m_time)) m_isRunning = false;
      if (!Draw(m_time)) m_isRunning = false;
      // Reset the clock after this
      m_time = m_clock.restart().asSeconds();
    }
	}

	// Shut the game down.
	m_window.close();
}