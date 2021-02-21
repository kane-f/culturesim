#include "field.hpp"

void Field::updateMoistureAndTemperature()
{
  for (short x = 0; x < m_mapWidth; ++x)
  for (short y = 0; y < m_mapHeight; ++y)
  {
      updateExtremes(x,y);
      if ((*m_field)(x,y).surface == SURFACE::LAND &&
          (*m_field)(x,y).route != ROUTE::RIVER) (*m_field)(x,y).moisture = propogateMoisture(x,y);
  }

  for (short x = 0; x < m_mapWidth; ++x)
  for (short y = 0; y < m_mapHeight; ++y)
    {
        if ((*m_field)(x,y).surface == SURFACE::LAND) (*m_field)(x,y).moisture = adjustMoisture(x,y);
        // Keep moisture from wrapping under
        if ((*m_field)(x,y).moisture < 0) (*m_field)(x,y).moisture = 0;
        (*m_field)(x,y).rainfall = (*m_field)(x,y).moisture;
        (*m_field)(x,y).temperature = getTemperature(x,y);
        (*m_field)(x,y).biome = getBiome(x,y);
        if ((*m_field)(x,y).temperature < 0 && (*m_field)(x,y).surface == SURFACE::SALTWATER)
          (*m_field)(x,y).surface = SURFACE::ICECAP;

        if ((*m_field)(x,y).surface != SURFACE::SALTWATER)
        {
          // The "preferred" habitat temperature difference, focusing on the number 15 (average earth temperature)
          const short preferredDiff = (*m_field)(x,y).temperature - 15;
          // Keep positive
          if (preferredDiff < 0) (*m_field)(x,y).population += preferredDiff * 10;
          else (*m_field)(x,y).population -= preferredDiff * 10;
          // Keep population from wrapping under
          if ((*m_field)(x,y).population < 0) (*m_field)(x,y).population = 0;
        }
    }
}

// Find highs and lows on map, to be called within loops
void Field::updateExtremes(short x, short y)
{
  // If above highest point, make new highest point
  m_highestPoint = (*m_field)(x,y).height > m_highestPoint ? (*m_field)(x,y).height : m_highestPoint;
  // If below lowest point, make new lowest point
  m_lowestPoint = (*m_field)(x,y).height < m_lowestPoint ? (*m_field)(x,y).height : m_lowestPoint;
}

unsigned long Field::updateWorldPopulation()
{
  // Temporary variable
  unsigned long total = 0;
  // For entire field
  for (short x = 0; x < m_mapWidth; ++x)
  for (short y = 0; y < m_mapHeight; ++y)
  {
    // If above 0 add to world population
    if((*m_field)(x,y).population > 0) total += (*m_field)(x,y).population;
  }
  return total;
}

void Field::advanceTime(unsigned int amount, GAME_STATE gameState)
{
  m_day += amount;
  // Do things an amount of times
  for (unsigned int t = 0; t < amount; ++t)
  {
    buildPaths();
    updateCountries();
    // Iterate over map
    for (short x = 0; x < m_mapWidth; ++x)
    for (short y = 0; y < m_mapHeight; ++y)
    {
      updatePopulation(x,y);
      createPath(x,y);
      startCountry(x,y,gameState);
      if ((*m_field)(x,y).moisture < (*m_field)(x,y).rainfall && m_day % 30 == 0) (*m_field)(x,y).moisture++;
      if ((*m_field)(x,y).development != DEVELOPMENT::WILD) (*m_field)(x,y).biome = getBiome(x,y);
    }
  }
  m_worldPopulation = updateWorldPopulation();
}

void Field::updatePopulation(short x, short y)
{
  // Initialise transfer amount
  short transferAmount = 0;
  // X and Y position offsets for population to move to, random point in any direction
  short xOffset = (rand() % 3) - 1;
  short yOffset = (rand() % 3) - 1;
  // Avoid array out of bounds exception
  if(x + xOffset >= 0 && y + yOffset >= 0 &&
     x + xOffset < m_mapWidth && y + yOffset < m_mapWidth)
  {
    // Skip water tiles to save calculation
    if ((*m_field)(x+xOffset,y+yOffset).surface != SURFACE::LAND) return;
    // Ignore central tile
    if (xOffset != 0 && yOffset != 0)
    {
      // Attempt at over/underpopulation management, somewhat like game of life
      if ((*m_field)(x,y).population < (*m_field)(x+xOffset,y+yOffset).population * 2)
        transferAmount = (*m_field)(x,y).population / 16;
      else if ((*m_field)(x,y).population < (*m_field)(x+xOffset,y+yOffset).population)
        transferAmount = (*m_field)(x,y).population / 4;
      else if ((*m_field)(x,y).population < (*m_field)(x+xOffset,y+yOffset).population / 2)
        transferAmount = (*m_field)(x,y).population / 8;
      else transferAmount = (*m_field)(x,y).population / 32;

      // Make people move to lower altitudes in slight amounts
      //transferAmount += getHeightDifference(x,y,x+xOffset,y+yOffset) / 128;

      // Keep people away from high, hypoxia inducing altitudes
      if ((*m_field)(x+xOffset,y+yOffset).height > 2000)
        transferAmount += (*m_field)(x+xOffset,y+yOffset).height / 4000;

      // People prefer temperatures around 20
      if ((*m_field)(x,y).temperature < 15) transferAmount -= ((*m_field)(x,y).temperature - 20) / 10;
      else transferAmount += ((*m_field)(x,y).temperature - 20) / 10;

      // And prefer higher moisture
      transferAmount += ((*m_field)(x+xOffset,y+yOffset).moisture - 4000) / 100;

      // And their own countries
      if ((*m_field)(x,y).country != NULL) transferAmount /= 4;
      if ((*m_field)(x,y).country != (*m_field)(x+xOffset,y+yOffset).country) transferAmount /= 4;

      // And allies on non-null points
      if ((*m_field)(x,y).country != NULL)
      if ((*m_field)(x,y).country->getAllies().find((*m_field)(x,y).country)
          == (*m_field)(x,y).country->getAllies().end()) transferAmount /= 4;

      // If on farmland
      if ((*m_field)(x,y).development == DEVELOPMENT::FARM)
      {
        // Decreasing transfer amount multipliers for rivers, paths, sea, wilderness and urban areas
        if (checkNearbyTypes(x+xOffset,y+yOffset,ROUTE::RIVER))
          transferAmount *= 15;
        else if (checkNearbyTypes(x+xOffset,y+yOffset,ROUTE::PATH))
          transferAmount *= 12;
        else if (checkNearbyTypes(x+xOffset,y+yOffset,SURFACE::SALTWATER))
          transferAmount *= 9;
        else if (checkNearbyTypes(x+xOffset,y+yOffset,DEVELOPMENT::WILD))
          transferAmount *= 6;
        else if (checkNearbyTypes(x+xOffset,y+yOffset,DEVELOPMENT::URBAN))
          transferAmount *= 3;
      }
      // If in urban area
      else if ((*m_field)(x,y).development == DEVELOPMENT::URBAN)
      {
        // Decreasing transfer amount multipliers for rivers, paths, sea, wilderness and farmlands
        if (checkNearbyTypes(x+xOffset,y+yOffset,ROUTE::RIVER))
          transferAmount *= 20;
          else if (checkNearbyTypes(x+xOffset,y+yOffset,ROUTE::PATH))
          transferAmount *= 16;
        else if (checkNearbyTypes(x+xOffset,y+yOffset,SURFACE::SALTWATER))
          transferAmount *= 12;
        else if (checkNearbyTypes(x+xOffset,y+yOffset,DEVELOPMENT::WILD))
          transferAmount *= 8;
        else if (checkNearbyTypes(x+xOffset,y+yOffset,DEVELOPMENT::FARM))
          transferAmount *= 4;
      }
      // If in wilderness
      else
      {
        // Increasing transfer amount multipliers for rivers, paths, sea, farmlands and urban areas
        if (checkNearbyTypes(x+xOffset,y+yOffset,ROUTE::RIVER))
          transferAmount *= 2;
        else if (checkNearbyTypes(x+xOffset,y+yOffset,ROUTE::PATH))
          transferAmount *= 4;
        else if (checkNearbyTypes(x+xOffset,y+yOffset,SURFACE::SALTWATER))
          transferAmount *= 6;
        else if (checkNearbyTypes(x+xOffset,y+yOffset,DEVELOPMENT::FARM))
          transferAmount *= 8;
        else if (checkNearbyTypes(x+xOffset,y+yOffset,DEVELOPMENT::URBAN))
          transferAmount *= 10;
      }

      // Have infrastructures account for population moving
      transferAmount += (*m_field)(x+xOffset,y+yOffset).residencies / 4;
      transferAmount += (*m_field)(x+xOffset,y+yOffset).industries / 8;
      transferAmount += (*m_field)(x+xOffset,y+yOffset).shops / 16;
      transferAmount += (*m_field)(x+xOffset,y+yOffset).venues / 32;
      transferAmount += (*m_field)(x+xOffset,y+yOffset).hospitals / 64;

      // Remove a random proportion of the transfer 1% of the original amount
      if(transferAmount != 0) transferAmount = transferAmount - ((rand() % transferAmount) / 100);
      // If transfer amount exceeds population of the tiles, cap it to population
      if (transferAmount > (*m_field)(x,y).population ||
          transferAmount > (*m_field)(x+xOffset,y+yOffset).population)
        transferAmount = (*m_field)(x,y).population > (*m_field)(x+xOffset,y+yOffset).population ?
                         (*m_field)(x,y).population : (*m_field)(x+xOffset,y+yOffset).population;

      // Do the transfer
      (*m_field)(x,y).population -= transferAmount;
      (*m_field)(x+xOffset,y+yOffset).population += transferAmount;

      // Update farm and urban tiles based on population within
      if((*m_field)(x,y).moisture > 100 && (*m_field)(x,y).temperature > 0 && (*m_field)(x,y).temperature < 40)
      {
        if (((*m_field)(x,y).route == ROUTE::RIVER ||
             checkNearbyTypes(x,y,SURFACE::SALTWATER) ||
             (*m_field)(x,y).route == ROUTE::PATH) && (*m_field)(x,y).population >= 100) 
            (*m_field)(x,y).development == DEVELOPMENT::URBAN;

        if ((*m_field)(x,y).population >= 100 &&
            (*m_field)(x,y).development == DEVELOPMENT::WILD && rand() % 5000 < (*m_field)(x,y).moisture)
          (*m_field)(x,y).development = DEVELOPMENT::FARM;

        if ((*m_field)(x,y).population >= 1000 &&
            checkNearbyTypes(x,y,DEVELOPMENT::FARM) && rand() % 4 > 1)
          (*m_field)(x,y).development = DEVELOPMENT::URBAN;

        if ((*m_field)(x,y).population < 1000 &&
            (*m_field)(x,y).development == DEVELOPMENT::URBAN && rand() % 100 == 0)
          (*m_field)(x,y).development = DEVELOPMENT::FARM;

        if ((*m_field)(x,y).population < 100 &&
            (*m_field)(x,y).development == DEVELOPMENT::FARM && rand() % 1000 == 0)
          (*m_field)(x,y).development = DEVELOPMENT::WILD;
      }

      // Cap population above 0
      (*m_field)(x,y).population = (*m_field)(x,y).population >= 0 ? (*m_field)(x,y).population : 0;
      (*m_field)(x+xOffset,y+yOffset).population = (*m_field)(x+xOffset,y+yOffset).population >= 0 ?
                                                   (*m_field)(x+xOffset,y+yOffset).population : 0;

      // Add to infrastructure from populations in settled areas
      if((*m_field)(x,y).development != DEVELOPMENT::WILD)
      {
        // For cities
        if((*m_field)(x,y).development == DEVELOPMENT::URBAN)
        {
          // Distribute shopping, industry, healthcare, and venues in decreasing frequency
          (*m_field)(x,y).shops += (*m_field)(x,y).population / 64;
          (*m_field)(x,y).industries += (*m_field)(x,y).population / 128;
          (*m_field)(x,y).hospitals += (*m_field)(x,y).population / 256;
          (*m_field)(x,y).venues += (*m_field)(x,y).shops / 2;
        }

        // Add barracks to border areas or else decrease them
        if (!checkOppositeCountry(x,y,(*m_field)(x,y).country))
          (*m_field)(x,y).barracks += (*m_field)(x,y).population / 64;
        else (*m_field)(x,y).barracks -= (*m_field)(x,y).barracks / 32;

        // Cap barracks above 0
        (*m_field)(x,y).barracks = (*m_field)(x,y).barracks >= 0 ? (*m_field)(x,y).barracks : 0;

        // Create housing in all settled land
        (*m_field)(x,y).residencies += (*m_field)(x,y).population / 32;
      }
    }
  }
}

void Field::createPath(short x, short y)
{
  // If on land and in built area where no path already exists, with 1% of population as chance
  if ((*m_field)(x,y).surface == SURFACE::LAND &&
      ((*m_field)(x,y).development == DEVELOPMENT::URBAN ||
      (*m_field)(x,y).development == DEVELOPMENT::FARM) &&
      (*m_field)(x,y).route != ROUTE::PATH &&
      rand() % 32767 <= (*m_field)(x,y).population/100)
  {
    // Create path and initalise it
    Path temp(&(*m_field)(x,y),m_field,"");
    temp.initPath(50);
    // Add to unfinished path vector
    m_pathsInProgress.push_back(temp);
  }
}

void Field::buildPaths()
{
  // Iterate through all unfinished paths on field
  for(std::vector<Path>::iterator i = m_pathsInProgress.begin(); i != m_pathsInProgress.end(); ++i) {
    // If path propogation is done
    if (i->propogatePath()) {
      // Add to completed paths
      m_pathsComplete.push_back(*i);
      // Remove from unfinished paths
      m_pathsInProgress.erase(i);
    }
  }
}

void Field::startCountry(short x, short y, GAME_STATE gameState)
{
  // Create a country if population is high enough on a point and null
  if ((*m_field)(x,y).population >= 5000 && (*m_field)(x,y).country == NULL &&
      (*m_field)(x,y).development == DEVELOPMENT::URBAN && rand() % 100 == 0)
  // Use of new allocator for heap countries, not visible on campaign mode
  if (gameState == GAME_STATE::CAMPAIGN) m_countries.push_back(new Country(&(*m_field)(x,y),m_field,generateName(),false));
  else m_countries.push_back(new Country(&(*m_field)(x,y),m_field,generateName(),true));
}

void Field::updateCountries()
{
  // Iterate through all countries
  for(std::vector<Country*>::iterator i = m_countries.begin(); i != m_countries.end(); ++i) {
    if (*i == NULL) continue;
    // If country is empty (has 0 points)
    if ((*i)->isEmpty())
    {
      // Iterate through all countries again
      for(std::vector<Country*>::iterator j = m_countries.begin(); j != m_countries.end(); ++j)
      {

        if (*j == NULL) continue;
        // Clear country from ally and enemy lists
        (*j)->removeAlly(*i);
        (*j)->removeEnemy(*i);
      }
      // Remove from list
      m_countries.erase(i);
      // Delete pointer
      delete (*i);
      // Set pointer to null
      (*i) = NULL;
      // Go to next iteration
      continue;
    }

    // Update each one
    if (*i != NULL) (*i)->updateCountry();
  }

  //fillCountryHoles();
} 

void Field::fillCountryHoles()
{
  // Country to fill in hole
  Country* holeClaimant = NULL;
  // Nearby tile count
  int8_t count = 0;
  
  // Iterate through map
  for (short x = 0; x < m_mapWidth; ++x)
  for (short y = 0; y < m_mapHeight; ++y)
  // On blank spaces
  if((*m_field)(x,y).country == NULL) {
    // Iterate around area
    for (short i = -1; i <= 1; ++i)
    for (short j = -1; j <= 1; ++j) {
      // Skip centre
      if (i == 0 && j == 0) continue;
      // Stay in bounds
      if (x + i >= 0 && y + j >= 0 && x + i < m_mapWidth && y + j < m_mapHeight) {
        // If no hole claimant, create with country on this tile
        if ((*m_field)(x+i,y+j).country != NULL && holeClaimant == NULL) holeClaimant = (*m_field)(x+i,y+j).country;
        // If tile matches claimant, increase count
        if ((*m_field)(x+i,y+j).country == holeClaimant) count++;
      }
    }
    // If count greater than 5, fill in hole
    if (count >= 5) (*m_field)(x,y).country = holeClaimant;
  }
} 

bool Field::initCurrentCountry()
{
  // If no countries, exit function with failure
  if(m_countries.empty()) return false;
  // Country vector iterator
  std::vector<Country*>::iterator i = m_countries.begin();
  // Jump to a random position in vector
  i += rand() % (m_countries.size());
  // Set current country to this
  m_currentCountry = *i;
  // Return success
  return true;
}

void Field::selectCurrentCountry(short x, short y)
{
  // Make current country if available
  if ((*m_field)(x,y).country != NULL)
    m_currentCountry = (*m_field)(x,y).country;
  else m_currentCountry = NULL;
}

Country* Field::getCurrentCountry()
{
  return m_currentCountry;
}

void Field::primarySelectCountry(short x, short y)
{
  // If country is clicked on and visible
  if ((*m_field)(x,y).country != NULL && (*m_field)(x,y).country->getVisible())
    // Primary select country
    (*m_field)(x,y).country->setPrimarySelected(!(*m_field)(x,y).country->getPrimarySelected());
  // Otherwise deselect all countries
  else deselectAllCountries();
}

void Field::secondarySelectCountry(short x, short y)
{
  // If country is clicked on and visible
  if ((*m_field)(x,y).country != NULL && (*m_field)(x,y).country->getVisible())
    // Secondary select country
    (*m_field)(x,y).country->setSecondarySelected(!(*m_field)(x,y).country->getSecondarySelected());
  // Otherwise deselect all countries
  else deselectAllCountries();
}

void Field::deselectAllCountries()
{
  // For all countries
  for(Country* country : m_countries)
  {
    // Deselect each one
    country->setPrimarySelected(false);
    country->setSecondarySelected(false);
  }
}