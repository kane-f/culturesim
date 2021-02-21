#include "game.hpp"
#include "utils.hpp"

// Entry point
int main()
{
  // Set a random seed.
	srand(static_cast<unsigned int>(time(nullptr)));

	// Create the main game object.
	sf::RenderWindow window(sf::VideoMode(W_WIDTH, W_HEIGHT), "CultureSim");
	Game game(&window);

	// Initialize and run the game object.
	game.Initialize();
	game.Run();

	// Exit the application.
	return 0;
}
