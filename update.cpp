#include "game.hpp"

bool Game::Update(float deltaTime)
{
	switch(m_gameState)
  {
    // For campaign mode
    case GAME_STATE::CAMPAIGN:
      // If country is empty or not populated
      if (field.getCurrentCountry()->isEmpty() || field.getCurrentCountry()->getPopulation() == 0)
        // Switch to game over state
        m_gameState == GAME_STATE::GAME_OVER;
    // For both campaign and simulation mode (no break statement to seperate previous)
    case GAME_STATE::SIMULATION:
      // Advance time by day interval
      field.advanceTime(m_dayInterval,m_gameState);
      break;
    default:
      break;
  }
	return true;
}
