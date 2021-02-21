#include "game.hpp"

// Draw the current game scene.
bool Game::Draw(float deltaTime)
{
  // Clear the window
  m_window.clear(sf::Color::Black);
  switch(m_gameState)
  {
    case GAME_STATE::MAIN_MENU:
      // Drawn the main menu
      DrawMainMenu();
      break;
    case GAME_STATE::GENERATION:
      // Drawn the generation menu
      DrawGenerationMenu();
      break;
    case GAME_STATE::CAMPAIGN:
    case GAME_STATE::SIMULATION:
      // Drawn the game, if failure then quit
      if (!DrawGame(deltaTime)) return false;
      break;
    case GAME_STATE::GAME_OVER:
      // Display game over message
      DrawString("Your country has ended.\n\nPress enter to return to the main menu.", sf::Vector2f(0, 0), 30);
      break;
  }
  // Display the window
  m_window.display();
  return true;
}

void Game::DrawMainMenu()
{
  // Set the menu view
  m_menuView.setSize(m_window.getSize().x,m_window.getSize().y);
  m_window.setView(m_menuView);

  // Display the menu options
  DrawString("Generate new map",
    sf::Vector2f(0, 0), 30, m_menuOption == MENU_OPTION::GENERATE);
  DrawString("Load map from file",
    sf::Vector2f(0, 24.f), 30, m_menuOption == MENU_OPTION::LOAD);
}

void Game::DrawGenerationMenu()
{
  // Set the menu view
  m_menuView.setSize(m_window.getSize().x,m_window.getSize().y);
  m_window.setView(m_menuView);

  // Checks to display which string based on game state selected in generation
  std::string state = m_selectedState == GAME_STATE::CAMPAIGN ? "Campaign" : "Simulation";

  // Display the generation options
  DrawString("Map size: " + std::to_string(m_mapSize),
    sf::Vector2f(0, -72.f), 24, m_genOption == GEN_OPTIONS::SIZE);
  DrawString("Feature size: " + std::to_string(m_featureSize),
    sf::Vector2f(0, -48.f), 24, m_genOption == GEN_OPTIONS::FEATURES);
  DrawString("Maximum height: " + std::to_string(m_maxHeight),
    sf::Vector2f(0, -24.f), 24, m_genOption == GEN_OPTIONS::HEIGHT);
  DrawString("Population density: " + std::to_string(m_density),
    sf::Vector2f(0, 0), 24, m_genOption == GEN_OPTIONS::POPULATION);
  DrawString("Sea level: " + std::to_string(m_seaLevel),
    sf::Vector2f(0, 24.f), 24, m_genOption == GEN_OPTIONS::SEALEVEL);
  DrawString("River frequency: " + std::to_string(m_riverFrequency),
    sf::Vector2f(0, 48.f), 24, m_genOption == GEN_OPTIONS::RIVERS);
  DrawString("Game mode: " + state,
    sf::Vector2f(0, 72.f), 24, m_genOption == GEN_OPTIONS::GAMEMODE);
  DrawString("Enter to confirm", sf::Vector2f(0, 96.f), 24, false);
}

bool Game::DrawGame(float deltaTime)
{
  // Set the game view
  m_mapView.setSize(m_window.getSize().x/2,m_window.getSize().y);
  m_window.setView(m_mapView);
  // Draw the field, quit on failure
  if (!field.drawField(m_drawMode, m_gameState)) return false;
  // Draw the HUD
  DrawHUD(deltaTime);
  return true;
}

bool Game::DrawCursor(float deltaTime)
{
  // Clear window
  m_window.clear(sf::Color::Black);
  if(!DrawGame(deltaTime)) return false;
  const float mouseX = sf::Mouse::getPosition(m_window).x, mouseY = sf::Mouse::getPosition(m_window).y;
  m_window.setView(m_mapView);
  // X and Y line vertices
  sf::Vertex xLine[] = {
    sf::Vertex(sf::Vector2f(-m_mapView.getSize().x, mouseY-(m_mapView.getSize().y/2)), sf::Color::Red),
    sf::Vertex(sf::Vector2f(m_mapView.getSize().x, mouseY-(m_mapView.getSize().y/2)), sf::Color::Red)};
  sf::Vertex yLine[] = {
    sf::Vertex(sf::Vector2f(mouseX-(m_mapView.getSize().x/2), -m_mapView.getSize().y), sf::Color::Red),
    sf::Vertex(sf::Vector2f(mouseX-(m_mapView.getSize().x/2), m_mapView.getSize().y), sf::Color::Red)};

  // Draw these 2 vertices
  m_window.draw(xLine, 2, sf::Lines);
  m_window.draw(yLine, 2, sf::Lines);
  // Display window
  m_window.display();
  return true;
}

void Game::DrawHUD(float deltaTime)
{
  // Shorthands for mouse x and y as const
  const float mouseX = sf::Mouse::getPosition(m_window).x, mouseY = sf::Mouse::getPosition(m_window).y;
  // Set the HUD view
  m_hudView.setSize(m_window.getSize().x,m_window.getSize().y);
  m_window.setView(m_hudView);
  // Unused temporary names
  std::string riverName, pathName;
  switch (m_hudState)
  {
    case HUD_STATE::MAIN:
      {
      // Temporary country name variables
      std::string pauseStatus;
      // If a country is hovered over, display its name
      if (m_paused) pauseStatus = "Unpause";
      // Otherwise, nothing
      else pauseStatus = "Pause";
      DrawString("V: View mode\n" + std::string() +
                 "C: Country interactions\n" + std::string() +
                 "R: Country relations (if selected)\n" + std::string() +
                 "M: Resource management\n" + std::string() +
                 "1-9: World views\n" + std::string() +
                 "Arrow keys: Move map\n" + std::string() +
                 "-/=: Scale map\n" + std::string() +
                 "[/]: Change time speed\n" + std::string() +
                 "Space: " + pauseStatus + "\n" +
                 "Enter: Advance day interval\n" + std::string() +
                 ",/.: Change day interval",
                 sf::Vector2f(0, 0), 15);
      }
      break;
    case HUD_STATE::VIEW:
      // Scope for variable declaration
      {
      // Temporary country name variables
      std::string countryName;
      // If a country is hovered over, display its name
      if (field.getPoint(mouseX, mouseY)->country != NULL &&
          field.getPoint(mouseX, mouseY)->country->getVisible())
        countryName = field.getPoint(mouseX, mouseY)->country->getName();
      // Otherwise, nothing
      else countryName = "N/A";
      // Main info that displays in all view contexts
      const std::string mainInfo = "\nDay/Month/Year: " + field.getWeekDay() + "/" +
                                    std::to_string(field.getDay()) + "/" +
                                    std::to_string(field.getMonth()) + "/" +
                                    std::to_string(field.getYear()) +
                                    "\nHeight: " + std::to_string(field.getPoint(mouseX, mouseY)->height) + "m" +
                                    "\nTemp: " + std::to_string(field.getPoint(mouseX, mouseY)->temperature) + "*C" +
                                    "\nPopulation: " + std::to_string(field.getPoint(mouseX, mouseY)->population) +
                                    "\nMoisture: " + std::to_string(field.getPoint(mouseX, mouseY)->moisture) + "mm" +
                                    "\nRainfall: " + std::to_string(field.getPoint(mouseX, mouseY)->rainfall) + "mm" +
                                    "\nResidencies: " + std::to_string(field.getPoint(mouseX, mouseY)->residencies) +
                                    "\nIndustries: " + std::to_string(field.getPoint(mouseX, mouseY)->industries) +
                                    "\nShops: " + std::to_string(field.getPoint(mouseX, mouseY)->shops) +
                                    "\nVenues: " + std::to_string(field.getPoint(mouseX, mouseY)->venues) +
                                    "\nBarracks: " + std::to_string(field.getPoint(mouseX, mouseY)->barracks) +
                                    "\nHospitals: " + std::to_string(field.getPoint(mouseX, mouseY)->hospitals) +
                                    "\nCountry: " + countryName;

      // Draw world information if no country is currently selected or played as
      if (field.getCurrentCountry() == NULL) {
        DrawString("Total population: " + std::to_string(field.getWorldPopulation()) +
                   mainInfo, sf::Vector2f(0, 0), 15);
      }
      // Otherwise, display country info
      else
      {
        std::string vassal;
        if (field.getCurrentCountry()->getVassalOwner() != NULL)
          vassal = "Owned by: " + field.getCurrentCountry()->getVassalOwner()->getName() + "\n";
        else vassal = "";
        DrawString("Country name: " + field.getCurrentCountry()->getName() +
                   "\nTotal population: " + std::to_string(field.getCurrentCountry()->getPopulation()) +
                   "\nArea: " + std::to_string(field.getCurrentCountry()->getArea()) + "km^2" +
                   "\nDensity: " + std::to_string((int)field.getCurrentCountry()->getDensity()) + "/km^2" +
                   "\nProcessed resources:" + std::to_string(field.getCurrentCountry()->getProcessed()) +
                   "\nCrops:" + std::to_string(field.getCurrentCountry()->getCrops()) +
                   "\nMinerals:" + std::to_string(field.getCurrentCountry()->getOre()) +
                   vassal + "\n" + mainInfo, sf::Vector2f(0, 0), 15);
      }
      }
      break;
    case HUD_STATE::COMPETE:
      // Scope for variable declaration
      {
      // Word variable for option 5
      std::string canCapture;
      // Word variable for option 6
      std::string canTrade;
      // Toggle use of the word “allow” or “disallow” based on state of ability to capture unclaimed points
      if (field.getCurrentCountry()->canCaptureUnclaimed()) canCapture = "Disallow";
      else canCapture = "Allow";
      // Toggle use of the word “allow” or “disallow” based on state of ability to trade with allies
      if (field.getCurrentCountry()->canTrade()) canTrade = "Disallow";
      else canTrade = "Allow";
      DrawString("1: Add countries to allies\n" + std::string() +
                 "2: Add countries to enemies\n" + std::string() +
                 "3: Remove countries from allies\n" + std::string() +
                 "4: Remove countries from enemies\n" + std::string() +
                 "5: " + canCapture + " capturing unclaimed land\n" +
                 "6: " + canTrade + " trading with allies\n",
                 sf::Vector2f(0, 0), 15);
      break;
      }
    case HUD_STATE::MINERALS:
      // Scope for variable declaration
      {
      // Word variable for option 1
      std::string canMine;
      // Word variable for option 2
      std::string canHarvest;
      // Word variable for option 3
      std::string canProcess;
      // Toggle use of the word “allow” or “disallow” based on state of ability to mine ore
      if (field.getCurrentCountry()->canMineOre()) canMine = "Disallow";
      else canMine = "Allow";
      // Toggle use of the word “allow” or “disallow” based on state of ability to harvest crops
      if (field.getCurrentCountry()->canHarvestCrops()) canHarvest = "Disallow";
      else canHarvest = "Allow";
      // Toggle use of the word “allow” or “disallow” based on state of ability to process goods
      if (field.getCurrentCountry()->canProcessGoods()) canProcess = "Disallow";
      else canProcess = "Allow";
      DrawString("1: " + canMine + " mining ore\n" +
                 "2: " + canHarvest + " harvesting crops\n" +
                 "3: " + canProcess + " processing goods\n",
                 sf::Vector2f(0, 0), 15);
      break;
      }
    case HUD_STATE::RELATIONS:
      // String stream for HUD
      std::ostringstream relations;

      // Print allies header and 2 new lines
      relations << "Allies:\n\n";
      // If allies available
      if (!field.getCurrentCountry()->getAllies().empty())
      // Iterate through allies
      for(std::set<Country*>::iterator i = field.getCurrentCountry()->getAllies().begin();
                                          i != field.getCurrentCountry()->getAllies().end();
                                          ++i)
        // Add ally and new line
        relations << (*i)->getName() << '\n';

      // Print enemies header and 2 new lines
      relations << "\nEnemies:\n\n";
      // If enemies available
      if (!field.getCurrentCountry()->getEnemies().empty())
      // Iterate through enemies
      for(std::set<Country*>::iterator i = field.getCurrentCountry()->getEnemies().begin();
                                          i != field.getCurrentCountry()->getEnemies().end();
                                          ++i)
        // Add enemy and new line
        relations << (*i)->getName() << '\n';

      DrawString(relations.str(), sf::Vector2f(0, 0), 15);
      break;
  }
}

// Draw the given string at the given position.
void Game::DrawString(std::string text, sf::Vector2f position, unsigned int size, bool selected)
{
  // Set text string to text
  m_text.setString(text);
  // Set text to default font
  m_text.setFont(m_font);
  // Set text to specified size
  m_text.setCharacterSize(size);
  // Centre text
  m_text.setPosition(position.x - (m_text.getLocalBounds().width / 2.f),
                     position.y - (m_text.getLocalBounds().height / 2.f));

  // Change colour if selected (optional)
  if (selected) m_text.setFillColor(sf::Color::Yellow);
  else m_text.setFillColor(sf::Color::White);

  // Draw the text
  m_window.draw(m_text);
}