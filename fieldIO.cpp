#include "field.hpp"

void Field::loadField(std::string name)
{
  // Variables to store map width, map height, current x position and current y position.
  short mapWidth = 320, mapHeight = 320, x = 0, y = 0;
  // Default m_day to 0 before reading
  m_day = 0;
  // Clear all item storages
  m_rivers.clear();
  m_pathsInProgress.clear();
  m_pathsComplete.clear();
  m_countries.clear();
  // Line variable
  std::string line = "";
  // File used
  std::ifstream inputFile("bin/save/" + name + ".txt");
  if (inputFile.is_open())
  {
    // While iterating through the file
    while (getline(inputFile,line))
    {
      // While all options are covered
      for (int8_t i = 0; i < (int8_t)LOAD_OPTION3::STRUCTLENGTH; i++)
      // Read the single value sections of the file
      if(line.compare("MAP WIDTH:") == 0 ||
         line.compare("MAP HEIGHT:") == 0 ||
         line.compare("DAYS:") == 0)
      {
        // Structure loading enum
        LOAD_OPTION3 loadOption = LOAD_OPTION3::WIDTH;
        // If map width is to be loaded
        if(line.compare("MAP WIDTH:") == 0) loadOption = LOAD_OPTION3::WIDTH;
        // If map height is to be loaded
        if(line.compare("MAP HEIGHT:") == 0) loadOption = LOAD_OPTION3::HEIGHT;
        // If days into game is to be loaded
        if(line.compare("DAYS:") == 0) loadOption = LOAD_OPTION3::DAY;
        // Go down two lines
        getline(inputFile,line);
        getline(inputFile,line);

        switch(loadOption)
        {
          case LOAD_OPTION3::WIDTH:
            // Read line as int and store it in the map width variable
            mapWidth = std::stoi(line);
            m_mapWidth = mapWidth;
            break;
          case LOAD_OPTION3::HEIGHT:
            // Read line as int and store it in the map height variable
            mapHeight = std::stoi(line);
            m_mapHeight = mapHeight;
            break;
          case LOAD_OPTION3::DAY:
            // Read line as int and store it in the day variable
            m_day = std::stoi(line);
            break;
        }
      }

      // Create field
      m_field = new PointArray(mapWidth,mapHeight);
      // Resize vertices to field size
      m_fieldPoints = sf::VertexArray(sf::Points, mapWidth * mapHeight);
      // Set sprite origin in middle of field
      m_fieldSprite.setOrigin((mapWidth/2),(mapHeight/2));

      // Read the point heights section of the file
      if(line.compare("MAP HEIGHTS:") == 0)
      {
        // Go down two lines
        getline(inputFile,line);
        getline(inputFile,line);
        // While there are still points to read
        while(line.compare("") != 0) {
          // Break out if beyond set map height
          if (y > mapHeight) break;
          // Stream to read from current line
          std::istringstream numbers(line);
          // Current number value extracted
          short current;
          // While extracting numbers
          while(numbers>>current)
          {
            // Break out if beyond set map width
            if (x > mapWidth) break;

            // Set values to defaults or zero to avoid errors
            (*m_field)(x,y).residencies = 0;
            (*m_field)(x,y).industries = 0;
            (*m_field)(x,y).shops = 0;
            (*m_field)(x,y).venues = 0;
            (*m_field)(x,y).barracks = 0;
            (*m_field)(x,y).hospitals = 0;
            (*m_field)(x,y).development = DEVELOPMENT::WILD;
            (*m_field)(x,y).route = ROUTE::NONE;
            (*m_field)(x,y).country = NULL;
            (*m_field)(x,y).path = NULL;
            (*m_field)(x,y).river = NULL;

            // Set x and y values to current iteration
            (*m_field)(x,y).x = x;
            (*m_field)(x,y).y = y;

            // Position vector points
            m_fieldPoints[x+(m_mapHeight*y)].position = sf::Vector2f(x, y);

            // Set height to number read
            (*m_field)(x,y).height = current;

            // Set below sea level to water
            if ((*m_field)(x,y).height < 0) (*m_field)(x,y).surface = SURFACE::SALTWATER;
            // And above to land
            else (*m_field)(x,y).surface = SURFACE::LAND;

            // Set moisture to 5000
            (*m_field)(x,y).moisture = 5000;

            // Increment x
            x++;
          }
          // Go down one line
          getline(inputFile,line);
          // Increment y
          y++;
        }
      }
      // Reset counters
      x = 0;
      y = 0;
      // While all options are covered
      for (int8_t i = 0; i < (int8_t)LOAD_OPTION2::STRUCTLENGTH; i++)
      {
        // Read the point population and moisture sections of the file
        if(line.compare("MAP POPULATIONS:") == 0 ||
           line.compare("MOISTURE:") == 0)
        {
          // Structure loading enum
          LOAD_OPTION2 loadOption = LOAD_OPTION2::POPULATION;
          // If rivers are to be loaded
          if(line.compare("RIVERS:") == 0) loadOption = LOAD_OPTION2::POPULATION;
          // If unfinished paths are to be loaded
          if(line.compare("UNFINISHED PATHS:") == 0) loadOption = LOAD_OPTION2::MOISTURE;

          // Go down two lines
          getline(inputFile,line);
          getline(inputFile,line);
          // While there are still points to read
          while(line.compare("") != 0) {
            // Break out if beyond set map height
            if (y > mapHeight) break;
            // Stream to read from current line
            std::istringstream numbers(line);
            // Current number value extracted
            short current;
            // While extracting numbers
            while(numbers>>current)
            {
              // Break out if beyond set map width
              if (x > mapWidth) break;
              // Set population to number read
              switch (loadOption)
              {
                case LOAD_OPTION2::POPULATION:
                  (*m_field)(x,y).population = current;
                  break;
                case LOAD_OPTION2::MOISTURE:
                  (*m_field)(x,y).moisture = current;
                  break;
              }
              // Increment x
              x++;
            }
            // Go down one line
            getline(inputFile,line);
            // Increment y
            y++;
          }
        }
        // Reset counters
        x = 0;
        y = 0;
      }
      // Read the point development type section of the file
      if(line.compare("DEVELOPMENT:") == 0)
      {
        // Go down two lines
        getline(inputFile,line);
        getline(inputFile,line);
        // While there are still points to read
        while(line.compare("") != 0) {
          // Break out if beyond set map height
          if (y > mapHeight) break;
          // Stream to read from current line
          std::istringstream chars(line);
          // Current character extracted
          char current;
          // While extracting characters
          while(chars>>current)
          {
            // Skip any erroneous spaces
            if (current = ' ') continue;
            // Break out if beyond set map width
            if (x > mapWidth) break;
            // Switch based on character read
            switch(current)
            {
              // If character is F
              case 'F':
                // Development is farmland
                (*m_field)(x,y).development = DEVELOPMENT::FARM;
                break;
              // If character is U
              case 'U':
                // Development is urban
                (*m_field)(x,y).development = DEVELOPMENT::URBAN;
                break;
              // If character is W or otherwise
              case 'W':
              default:
                // Development is wilderness
                (*m_field)(x,y).development = DEVELOPMENT::WILD;
                break;
            }
            // Increment x
            x++;
          }
          // Go down one line
          getline(inputFile,line);
          // Increment y
          y++;
        }
      }
      // Reset counters
      x = 0;
      y = 0;

      // While all options are covered
      for (int8_t i = 0; i < (int8_t)LOAD_OPTION::STRUCTLENGTH; i++)
      {
        // Read the structure sections of the file
        if(line.compare("RIVERS:") == 0 ||
           line.compare("UNFINISHED PATHS:") == 0 ||
           line.compare("FINISHED PATHS:") == 0 ||
           line.compare("COUNTRIES:") == 0)
        {
          // Structure loading enum
          LOAD_OPTION loadOption = LOAD_OPTION::RIVER;
          // If rivers are to be loaded
          if(line.compare("RIVERS:") == 0) loadOption = LOAD_OPTION::RIVER;
          // If unfinished paths are to be loaded
          if(line.compare("UNFINISHED PATHS:") == 0) loadOption = LOAD_OPTION::PATH_UNFINISHED;
          // If finished paths are to be loaded
          if(line.compare("FINISHED PATHS:") == 0) loadOption = LOAD_OPTION::PATH_FINISHED;
          // If countries are to be loaded
          if(line.compare("COUNTRIES:") == 0) loadOption = LOAD_OPTION::COUNTRY;
          // Temporary item name variable
          std::string itemName = "";
          // Go down two lines
          getline(inputFile,line);
          getline(inputFile,line);
          // While there are still rivers (indicated by name field entry)
          while(line.compare("NAME:") == 0)
          {
            // Go down two lines
            getline(inputFile,line);
            getline(inputFile,line);
            // Set this line to be the name
            itemName = line;
            // Go down two lines
            getline(inputFile,line);
            getline(inputFile,line);
            // If the points field is detected
            if(line.compare("POINTS:") == 0)
            {
              // Go down two lines
              getline(inputFile,line);
              getline(inputFile,line);
              // Create an input stream based on this line
              std::istringstream pair1(line);
              // Read in first number as x
              pair1 >> x;
              // And second as y
              pair1 >> y;

              switch (loadOption)
              {
                case LOAD_OPTION::RIVER:
                  // Add the river to the collection
                  m_rivers.push_back(River(&(*m_field)(x,y),m_field,itemName));
                  break;
                case LOAD_OPTION::PATH_UNFINISHED:
                  // Add the unfinished path to the collection
                  m_pathsInProgress.push_back(Path(&(*m_field)(x,y),m_field,itemName));
                  break;
                case LOAD_OPTION::PATH_FINISHED:
                  // Add the finished path to the collection
                  m_pathsComplete.push_back(Path(&(*m_field)(x,y),m_field,itemName));
                  break;
                case LOAD_OPTION::COUNTRY:
                  // Add the country to the collection
                  m_countries.push_back(new Country(&(*m_field)(x,y),m_field,itemName,true));
                  break;
              }
              // Go down one line
              getline(inputFile,line);
              // While there are still points to read
              while(line.compare("") != 0) {
                // Create an input stream based on this line
                std::istringstream pair2(line);
                // Read in first number as x
                pair2 >> x;
                // And second as y
                pair2 >> y;

                // Switch statement based on load option decided
                switch (loadOption)
                {
                  case LOAD_OPTION::RIVER:
                    // Add this point to the river
                    m_rivers.end()->add(&(*m_field)(x,y));
                    // Set this tile to point to this river
                    (*m_field)(x,y).river = &(*m_rivers.end());
                    // Make glacier if temperature is below 0
                    if ((*m_field)(x,y).temperature < 0) (*m_field)(x,y).route = ROUTE::GLACIER;
                    // Otherwise show as river
                    else (*m_field)(x,y).route = ROUTE::RIVER;
                    break;
                  case LOAD_OPTION::PATH_UNFINISHED:
                  case LOAD_OPTION::PATH_FINISHED:
                    if (loadOption == LOAD_OPTION::PATH_UNFINISHED)
                    {
                      // Add this point to the path
                      m_pathsInProgress.end()->add(&(*m_field)(x,y));
                      // Set this tile to point to this path
                      (*m_field)(x,y).path = &(*m_pathsInProgress.end());
                    }
                    else if (loadOption == LOAD_OPTION::PATH_FINISHED)
                    {
                      // Add this point to the path
                      m_pathsComplete.end()->add(&(*m_field)(x,y));
                      // Set this tile to point to this path
                      (*m_field)(x,y).path = &(*m_pathsComplete.end());
                    }
                    // Make bridge if path is over a river
                    if ((*m_field)(x,y).route == ROUTE::RIVER) (*m_field)(x,y).route = ROUTE::BRIDGE;
                    // Otherwise show as path
                    else (*m_field)(x,y).route = ROUTE::PATH;
                    break;
                  case LOAD_OPTION::COUNTRY:
                    // Add this point to the country
                    (*m_countries.end())->add(&(*m_field)(x,y));
                    // Set this tile to point to this country
                    (*m_field)(x,y).country = *m_countries.end();
                    break;
                }
                // Go down one line
                getline(inputFile,line);
              }
              std::cout << std::endl;
            }
            // Go down one line
            getline(inputFile,line);
          }
        }
        // Reset counters
        x = 0;
        y = 0;
      }
      // Gather moisture and temperature from heights as it should be obvious from generation
      // TODO: Preserve rainfall
      updateMoistureAndTemperature();
    }
    // Close the input file
    inputFile.close();
  }
}

void Field::saveField(std::string name)
{
  // Open output file for editing to
  std::ofstream outputFile("bin/save/" + name + ".txt");

  // Create header to show map width
  outputFile << "MAP WIDTH:\n\n";
  // Add map width to file
  outputFile << m_field->getWidth();

  // Create header to show map height
  outputFile << "\n\nMAP HEIGHT:\n\n";
  // Add map height to file
  outputFile << m_field->getHeight();

  // Create header to show days into game
  outputFile << "\n\nDAYS:\n\n";
  // Add days passed to file
  outputFile << m_day;

  // Create header to show map heights
  outputFile << "\n\nMAP HEIGHTS:\n\n";

  // Iterate over map
  for (short x = 0; x < m_mapWidth; ++x)
  {
    for (short y = 0; y < m_mapHeight; ++y)
    {
      // Add height at point and add a space
      outputFile << std::to_string((*m_field)(x,y).height);
      outputFile << " ";
    }
    // Add a new line at each row
    outputFile << "\n";
  }

  // Create header to show map populations
  outputFile << "\nMAP POPULATIONS:\n\n";

  // Iterate over map
  for (short x = 0; x < m_mapWidth; ++x)
  {
    for (short y = 0; y < m_mapHeight; ++y)
    {
      // Add population at point and add a space
      outputFile << std::to_string((*m_field)(x,y).population);
      outputFile << " ";
    }
    // Add a new line at each row
    outputFile << "\n";
  }

  // Create header to show moisture
  outputFile << "\nMOISTURE:\n\n";

  // Iterate over map
  for (short x = 0; x < m_mapWidth; ++x)
  {
    for (short y = 0; y < m_mapHeight; ++y)
    {
      // Add population at point and add a space
      outputFile << std::to_string((*m_field)(x,y).moisture);
      outputFile << " ";
    }
    // Add a new line at each row
    outputFile << "\n";
  }

  // Create header to show land development
  outputFile << "\nDEVELOPMENT:\n\n";

  // Iterate over map
  for (short x = 0; x < m_mapWidth; ++x)
  {
    for (short y = 0; y < m_mapHeight; ++y)
    {
      // Add development at point and add a space
      if ((*m_field)(x,y).development == DEVELOPMENT::WILD) outputFile << "W";
      if ((*m_field)(x,y).development == DEVELOPMENT::FARM) outputFile << "F";
      if ((*m_field)(x,y).development == DEVELOPMENT::URBAN) outputFile << "U";
      outputFile << " ";
    }
    // Add a new line at each row
    outputFile << "\n";
  }

  // Create header to show rivers
  outputFile << "\nRIVERS:\n";

  // Iterate through all rivers
  for(std::vector<River>::iterator i = m_rivers.begin(); i != m_rivers.end(); ++i) {
    // Create header to show name
    outputFile << "\nNAME:\n\n";
    // Add river name
    outputFile << i->getName();
    // Create header to show points
    outputFile << "\n\nPOINTS:\n\n";
    // Store points
    std::vector<Point*> points = i->getPoints();
    // Iterate through points
    for(std::vector<Point*>::iterator j = points.begin(); j != points.end(); ++j) {
      // Add x position
      outputFile << (*j)->x;
      // Add space
      outputFile << " ";
      // Add y position
      outputFile << (*j)->y;
      // New line
      outputFile << "\n";
    }
  }

  // Create header to show unfinished paths
  outputFile << "\nUNFINISHED PATHS:\n";

  // Iterate through all unfinished paths on field
  for(std::vector<Path>::iterator i = m_pathsInProgress.begin(); i != m_pathsInProgress.end(); ++i) {
    // Create header to show name
    outputFile << "\nNAME:\n\n";
    // Add unfinished path name
    outputFile << i->getName();
    // Create header to show points
    outputFile << "\n\nPOINTS:\n\n";
    // Store points
    std::vector<Point*> points = i->getPoints();
    // Iterate through points
    for(std::vector<Point*>::iterator j = points.begin(); j != points.end(); ++j) {
      // Add x position
      outputFile << (*j)->x;
      // Add space
      outputFile << " ";
      // Add y position
      outputFile << (*j)->y;
      // New line
      outputFile << "\n";
    }
  }

  // Create header to show finished paths
  outputFile << "\nFINISHED PATHS:\n";

  // Iterate through all finished paths on field
  for(std::vector<Path>::iterator i = m_pathsComplete.begin(); i != m_pathsComplete.end(); ++i) {
    // Create header to show name
    outputFile << "\nNAME:\n\n";
    // Add finished path name
    outputFile << i->getName();
    // Create header to show points
    outputFile << "\n\nPOINTS:\n\n";
    // Store points
    std::vector<Point*> points = i->getPoints();
    // Iterate through points
    for(std::vector<Point*>::iterator j = points.begin(); j != points.end(); ++j) {
      // Add x position
      outputFile << (*j)->x;
      // Add space
      outputFile << " ";
      // Add y position
      outputFile << (*j)->y;
      // New line
      outputFile << "\n";
    }
  }

  // Create header to show countries
  outputFile << "\nCOUNTRIES:\n";

  // Iterate through all finished paths on field
  for(std::vector<Country*>::iterator i = m_countries.begin(); i != m_countries.end(); ++i) {
    // Create header to show name
    outputFile << "\nNAME:\n\n";
    // Add finished path name
    outputFile << (*i)->getName();
    // Create header to show points
    outputFile << "\n\nPOINTS:\n\n";
    // Store points
    std::set<Point*> points = (*i)->getPoints();
    // Iterate through points
    for(std::set<Point*>::iterator j = points.begin(); j != points.end(); ++j) {
      // Add x position
      outputFile << (*j)->x;
      // Add space
      outputFile << " ";
      // Add y position
      outputFile << (*j)->y;
      // New line
      outputFile << "\n";
    }
  }
  // Close the output file
  outputFile.close();
}