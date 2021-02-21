#include "game.hpp"

bool Game::HandleInput(float deltaTime)
{
  sf::Event event;
  while (m_window.pollEvent(event))
  {
    // Check if the game was closed.
    if (event.type == sf::Event::Closed)
    {
      return false;
    }

    if (event.type == sf::Event::Resized)
    {
      if (!Draw(deltaTime)) return false;
    }

    if (event.type == sf::Event::MouseLeft)
    {
      if (!Draw(deltaTime)) return false;
    }

    switch(m_gameState)
    {
      // Handle controls at the main menu
      case GAME_STATE::MAIN_MENU:
        if (event.type == sf::Event::KeyPressed)
        {
          switch(event.key.code) {
            case sf::Keyboard::Key::Return:
              switch(m_menuOption) {
                case MENU_OPTION::LOAD:
                  // Load the field
                  field.loadField("save1");
                  // Switch gamestate to PLAYING
                  m_gameState = GAME_STATE::SIMULATION;
                  break;
                case MENU_OPTION::GENERATE:
                  // Switch gamestate to GENERATION menu
                  m_gameState = GAME_STATE::GENERATION;
                  break;
              }
              // Draw the world.
              if (!Draw(deltaTime)) m_isRunning = false;
              break;
            case sf::Keyboard::Key::Escape:
              return false;
              break;
            case sf::Keyboard::Key::Up:
              switch(m_menuOption) {
                case MENU_OPTION::LOAD:
                  m_menuOption = MENU_OPTION::GENERATE;
                  break;
                case MENU_OPTION::GENERATE:
                  m_menuOption = MENU_OPTION::LOAD;
                  break;
              }
              if (!Draw(deltaTime)) m_isRunning = false;
              break;
            case sf::Keyboard::Key::Down:
              switch(m_menuOption) {
                case MENU_OPTION::GENERATE:
                  m_menuOption = MENU_OPTION::LOAD;
                  break;
                case MENU_OPTION::LOAD:
                  m_menuOption = MENU_OPTION::GENERATE;
                  break;
              }
              if (!Draw(deltaTime)) m_isRunning = false;
              break;
          }
        }
        break;
      // Handle controls at the generation menu
      case GAME_STATE::GENERATION:
        if (event.type == sf::Event::KeyPressed)
        {
          switch(event.key.code) {
            case sf::Keyboard::Key::Return:

              // Generate the level
              field.generateLevel(m_mapSize,m_featureSize,m_maxHeight,m_density,m_seaLevel,m_riverFrequency);

              if (m_selectedState == GAME_STATE::CAMPAIGN)
              {
                // Switch gamestate to CAMPAIGN
                m_gameState = GAME_STATE::CAMPAIGN;

                // Display the first loading message
                m_window.clear(sf::Color::Black);
                DrawString("Searching for country...", sf::Vector2f(0, 0), 24);
                m_window.display();

                // Until a country is available
                while (!field.initCurrentCountry())
                // Advance the clock a day
                field.advanceTime(1,m_gameState);

                // Display the second loading message
                m_window.clear(sf::Color::Black);
                DrawString("Country found. Advancing time...", sf::Vector2f(0, 0), 24);
                m_window.display();

                // Advance the clock 3 months
                field.advanceTime(90,m_gameState);

                // Set all countries bordering this to visible
                for (Country* country: field.getCountries())
                  if (country != field.getCurrentCountry())
                    for (Point* point: country->getPoints())
                      if (field.checkNearbyTypes(point->x,point->y,field.getCurrentCountry(),5))
                        country->setVisible(true);
              }
              else
              {
                // Switch gamestate to SIMULATION
                m_gameState = GAME_STATE::SIMULATION;
              }
              // Draw the world.
    		      if (!Draw(deltaTime)) m_isRunning = false;
              break;
            case sf::Keyboard::Key::Escape:
              // Return to main menu
              m_gameState == GAME_STATE::MAIN_MENU;
              break;
            case sf::Keyboard::Key::Up:
              switch(m_genOption) {
                case GEN_OPTIONS::SIZE:
                  m_genOption = GEN_OPTIONS::GAMEMODE;
                  break;
                case GEN_OPTIONS::FEATURES:
                  m_genOption = GEN_OPTIONS::SIZE;
                  break;
                case GEN_OPTIONS::HEIGHT:
                  m_genOption = GEN_OPTIONS::FEATURES;
                  break;
                case GEN_OPTIONS::POPULATION:
                  m_genOption = GEN_OPTIONS::HEIGHT;
                  break;
                case GEN_OPTIONS::SEALEVEL:
                  m_genOption = GEN_OPTIONS::POPULATION;
                  break;
                case GEN_OPTIONS::RIVERS:
                  m_genOption = GEN_OPTIONS::SEALEVEL;
                  break;
                case GEN_OPTIONS::GAMEMODE:
                  m_genOption = GEN_OPTIONS::RIVERS;
                  break;
              }
              if (!Draw(deltaTime)) m_isRunning = false;
              break;
            case sf::Keyboard::Key::Down:
              switch(m_genOption) {
                case GEN_OPTIONS::SIZE:
                  m_genOption = GEN_OPTIONS::FEATURES;
                  break;
                case GEN_OPTIONS::FEATURES:
                  m_genOption = GEN_OPTIONS::HEIGHT;
                  break;
                case GEN_OPTIONS::HEIGHT:
                  m_genOption = GEN_OPTIONS::POPULATION;
                  break;
                case GEN_OPTIONS::POPULATION:
                  m_genOption = GEN_OPTIONS::SEALEVEL;
                  break;
                case GEN_OPTIONS::SEALEVEL:
                  m_genOption = GEN_OPTIONS::RIVERS;
                  break;
                case GEN_OPTIONS::RIVERS:
                  m_genOption = GEN_OPTIONS::GAMEMODE;
                  break;
                case GEN_OPTIONS::GAMEMODE:
                  m_genOption = GEN_OPTIONS::SIZE;
                  break;
              }
              if (!Draw(deltaTime)) m_isRunning = false;
              break;
            case sf::Keyboard::Key::Left:
              switch(m_genOption) {
                case GEN_OPTIONS::SIZE:
                  if(m_mapSize <= 32) m_mapSize = 32;
                  else m_mapSize -= 32;
                  break;
                case GEN_OPTIONS::FEATURES:
                  if(m_featureSize <= 1) m_featureSize = 1;
                  else m_featureSize--;
                  break;
                case GEN_OPTIONS::HEIGHT:
                  if(m_maxHeight <= 32) m_maxHeight = 32;
                  else m_maxHeight -= 32;
                  break;
                case GEN_OPTIONS::POPULATION:
                  if(m_density <= 1) m_density = 1;
                  else m_density -= 1;
                  break;
                case GEN_OPTIONS::SEALEVEL:
                  if(m_seaLevel <= -20000) m_seaLevel = 20000;
                  else m_seaLevel -= 32;
                  break;
                case GEN_OPTIONS::RIVERS:
                  if(m_riverFrequency <= 0) m_riverFrequency = 0;
                  else m_riverFrequency--;
                  break;
                case GEN_OPTIONS::GAMEMODE:
                  if(m_selectedState == GAME_STATE::SIMULATION) m_selectedState = GAME_STATE::CAMPAIGN;
                  else m_selectedState = GAME_STATE::SIMULATION;
                  break;
              }
              if (!Draw(deltaTime)) m_isRunning = false;
              break;
            case sf::Keyboard::Key::Right:
              switch(m_genOption) {
                case GEN_OPTIONS::SIZE:
                  if(m_mapSize >= 40000) m_mapSize = 40000;
                  else m_mapSize += 32;
                  break;
                case GEN_OPTIONS::FEATURES:
                  if(m_featureSize >= 20000) m_featureSize = 20000;
                  else m_featureSize++;
                  break;
                case GEN_OPTIONS::HEIGHT:
                  if(m_maxHeight >= 20000) m_maxHeight = 20000;
                  else m_maxHeight += 32;
                  break;
                case GEN_OPTIONS::POPULATION:
                  if(m_density >= 20000) m_density = 20000;
                  else m_density += 1;
                  break;
                case GEN_OPTIONS::SEALEVEL:
                  if(m_seaLevel >= 20000) m_seaLevel = 20000;
                  else m_seaLevel += 32;
                  break;
                case GEN_OPTIONS::RIVERS:
                  if(m_riverFrequency >= 100) m_riverFrequency = 100;
                  else m_riverFrequency++;
                  break;
                case GEN_OPTIONS::GAMEMODE:
                  if(m_selectedState == GAME_STATE::SIMULATION) m_selectedState = GAME_STATE::CAMPAIGN;
                  else m_selectedState = GAME_STATE::SIMULATION;
                  break;
              }
              if (!Draw(deltaTime)) m_isRunning = false;
              break;
          }
        }
        break;
      // Handle controls while playing
      case GAME_STATE::SIMULATION:
      case GAME_STATE::CAMPAIGN:
        if (event.type == sf::Event::KeyPressed)
        {
          switch(event.key.code) {
            case sf::Keyboard::Key::Up:
              field.moveField(0,-32);
              break;
            case sf::Keyboard::Key::Down:
              field.moveField(0,32);
              break;
            case sf::Keyboard::Key::Left:
              field.moveField(-32,0);
              break;
            case sf::Keyboard::Key::Right:
              field.moveField(32,0);
              break;
            case sf::Keyboard::Key::Dash:
              field.scaleField(7.0f/8.0f,7.0f/8.0f);
              break;
            case sf::Keyboard::Key::Equal:
              field.scaleField(8.0f/7.0f,8.0f/7.0f);
              break;
            case sf::Keyboard::Key::Return:
    		      // Update the world.
    		      if (!Update(deltaTime)) m_isRunning = false;
              break;
            case sf::Keyboard::Key::Escape:
              // If in the main HUD state
              if (m_hudState == HUD_STATE::MAIN) {
                // Go to main menu
                m_gameState = GAME_STATE::MAIN_MENU;
                // Save the field to file
                field.saveField("save1");
                // Clear the field for future use
                field.clearField();
              }
              // Otherwise change to main HUD state
              else m_hudState = HUD_STATE::MAIN;
              break;
            case sf::Keyboard::Key::Num1:
              if (m_hudState == HUD_STATE::COMPETE)
              {
                if (m_gameState == GAME_STATE::SIMULATION)
                  for (Country* country : field.getCountries())
                  if (country->getPrimarySelected())
                    for (Country* ally : field.getCountries())
                    if (ally->getSecondarySelected())
                      country->addAlly(ally);
                else if (m_gameState == GAME_STATE::CAMPAIGN)
                  for (Country* ally : field.getCountries())
                    if (ally->getSecondarySelected())
                      field.getCurrentCountry()->addAlly(ally);

                m_hudState = HUD_STATE::MAIN;
              }
              else if (m_hudState == HUD_STATE::MINERALS)
              {
                if (m_gameState == GAME_STATE::SIMULATION)
                  for (Country* country : field.getCountries())
                  if (country->getPrimarySelected() || country->getSecondarySelected())
                    country->toggleMineOre();
                else if (m_gameState == GAME_STATE::CAMPAIGN)
                  field.getCurrentCountry()->toggleMineOre();
              }
              // Set draw mode to environment
              else m_drawMode = DRAW_MODE::BIOME;
              break;
            case sf::Keyboard::Key::Num2:
              if (m_hudState == HUD_STATE::COMPETE)
              {
                if (m_gameState == GAME_STATE::SIMULATION)
                  for (Country* country : field.getCountries())
                  if (country->getPrimarySelected())
                    for (Country* enemy : field.getCountries())
                    if (enemy->getSecondarySelected())
                      country->addEnemy(enemy);
                else if (m_gameState == GAME_STATE::CAMPAIGN)
                  for (Country* ally : field.getCountries())
                    if (ally->getSecondarySelected())
                      field.getCurrentCountry()->addEnemy(enemy);

                m_hudState = HUD_STATE::MAIN;
              }
              else if (m_hudState == HUD_STATE::MINERALS)
              {
                if (m_gameState == GAME_STATE::SIMULATION)
                  for (Country* country : field.getCountries())
                  if (country->getPrimarySelected() || country->getSecondarySelected())
                    country->toggleHarvestCrops();
                else if (m_gameState == GAME_STATE::CAMPAIGN)
                  field.getCurrentCountry()->toggleHarvestCrops();
              }
              // Set draw mode to height
              else m_drawMode = DRAW_MODE::HEIGHT;
              break;
            case sf::Keyboard::Key::Num3:
              if (m_hudState == HUD_STATE::COMPETE)
              {
                if (m_gameState == GAME_STATE::SIMULATION)
                  for (Country* country : field.getCountries())
                  if (country->getPrimarySelected())
                    for (Country* ally : field.getCountries())
                    if (ally->getSecondarySelected())
                      country->removeAlly(ally);
                else if (m_gameState == GAME_STATE::CAMPAIGN)
                  for (Country* ally : field.getCountries())
                    if (ally->getSecondarySelected())
                      field.getCurrentCountry()->removeAlly(ally);

                m_hudState = HUD_STATE::MAIN;
              }
              else if (m_hudState == HUD_STATE::MINERALS)
              {
                if (m_gameState == GAME_STATE::SIMULATION)
                  for (Country* country : field.getCountries())
                  if (country->getPrimarySelected() || country->getSecondarySelected())
                    country->toggleProcessGoods();
                else if (m_gameState == GAME_STATE::CAMPAIGN)
                  field.getCurrentCountry()->toggleProcessGoods();
              }
              // Set draw mode to population
              else m_drawMode = DRAW_MODE::POPULATION;
              break;
            case sf::Keyboard::Key::Num4:
              if (m_hudState == HUD_STATE::COMPETE)
              {
                if (m_gameState == GAME_STATE::SIMULATION)
                  for (Country* country : field.getCountries())
                  if (country->getPrimarySelected())
                    for (Country* enemy : field.getCountries())
                    if (enemy->getSecondarySelected())
                      country->removeEnemy(enemy);
                else if (m_gameState == GAME_STATE::CAMPAIGN)
                  for (Country* ally : field.getCountries())
                    if (ally->getSecondarySelected())
                      field.getCurrentCountry()->removeEnemy(enemy);

                m_hudState = HUD_STATE::MAIN;
              }
              // Set draw mode to temperature
              else m_drawMode = DRAW_MODE::TEMPERATURE;
              break;
            case sf::Keyboard::Key::Num5:
              if (m_hudState == HUD_STATE::COMPETE)
              {
                if (m_gameState == GAME_STATE::SIMULATION)
                  for (Country* country : field.getCountries())
                  if (country->getPrimarySelected() || country->getSecondarySelected())
                    country->toggleCaptureUnclaimed();
                else if (m_gameState == GAME_STATE::CAMPAIGN)
                  field.getCurrentCountry()->toggleCaptureUnclaimed();

                m_hudState = HUD_STATE::MAIN;
              }
              // Set draw mode to moisture
              else m_drawMode = DRAW_MODE::MOISTURE;
              break;
            case sf::Keyboard::Key::Num6:
              if (m_hudState == HUD_STATE::COMPETE)
              {
                if (m_gameState == GAME_STATE::SIMULATION)
                  for (Country* country : field.getCountries())
                  if (country->getPrimarySelected() || country->getSecondarySelected())
                    country->toggleTrade();
                else if (m_gameState == GAME_STATE::CAMPAIGN)
                  field.getCurrentCountry()->toggleTrade();

                m_hudState = HUD_STATE::MAIN;
              }
              // Set draw mode to urbanisation
              else m_drawMode = DRAW_MODE::URBANISATION;
              break;
            case sf::Keyboard::Key::Num7:
              // Set draw mode to urbanisation
              m_drawMode = DRAW_MODE::COUNTRY;
              break;
            case sf::Keyboard::Key::Num8:
              // Set draw mode to infrastructure
              m_drawMode = DRAW_MODE::INFRASTRUCTURE;
              break;
            case sf::Keyboard::Key::Num9:
              break;
            case sf::Keyboard::Key::LBracket:
              // Halve time interval
              m_timeInterval /= 2;
              break;
            case sf::Keyboard::Key::RBracket:
              // Double time interval
              m_timeInterval *= 2;
              break;
            case sf::Keyboard::Key::Space:
              // Toggle game pause
              m_paused = !m_paused;
              break;
            case sf::Keyboard::Key::Period:
              // Increase day interval
              m_dayInterval++;
              break;
            case sf::Keyboard::Key::Comma:
              // Decrease day interval if above one
              if (m_dayInterval >= 1) m_dayInterval--;
              break;
            case sf::Keyboard::Key::C:
              // Show country interaction menu
              if(field.getCurrentCountry() != NULL) m_hudState = HUD_STATE::COMPETE;
              break;
            case sf::Keyboard::Key::M:
              // Show mineral extraction menu
              if(field.getCurrentCountry() != NULL) m_hudState = HUD_STATE::MINERALS;
              break;
            case sf::Keyboard::Key::R:
              // Show country relations
              if(field.getCurrentCountry() != NULL) m_hudState = HUD_STATE::RELATIONS;
              break;
            case sf::Keyboard::Key::V:
              // Show field values
              m_hudState = HUD_STATE::VIEW;
              break;
          }
          // Draw the world
          if (!Draw(deltaTime)) m_isRunning = false;
        }
        if (event.type == sf::Event::MouseMoved && m_hudState == HUD_STATE::VIEW)
        {
          if (!DrawCursor(deltaTime)) return false;
        }
        if (event.type == sf::Event::MouseButtonPressed)
        {
          if (event.mouseButton.button == sf::Mouse::Left)
          {
            if (m_gameState == GAME_STATE::SIMULATION)
            {
              field.selectCurrentCountry(event.mouseButton.x,event.mouseButton.y);
              field.primarySelectCountry(event.mouseButton.x,event.mouseButton.y);
            }
            else if (m_gameState == GAME_STATE::CAMPAIGN)
            {
              field.secondarySelectCountry(event.mouseButton.x,event.mouseButton.y);
            }
          }
          if (event.mouseButton.button == sf::Mouse::Right)
          {
            if (m_gameState == GAME_STATE::SIMULATION)
            {
              field.secondarySelectCountry(event.mouseButton.x,event.mouseButton.y);
            }
          }
        }
        break;
      case GAME_STATE::GAME_OVER:
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Return)
        {
          m_gameState == GAME_STATE::MAIN_MENU;
        }
        break;
    }
  }
	return true;
}
