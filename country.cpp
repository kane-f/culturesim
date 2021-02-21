#include "country.hpp"

Country::Country(Point* point, PointArray* field, std::string name, bool visible) :
Area(point,field,name,visible)
{
  // Set country on point to this country
  point->country = this;
  // Initialise population from this point
  m_population = point->population;
  m_vassalOwner = NULL;
  // Default to true for all
  m_captureUnclaimed = true;
  m_trade = true;
  m_harvestCrops = true;
  m_mineOre = true;
  m_processGoods = true;
  m_processed = 0;
  m_crops = 0;
  m_ore = 0;
}

Country::~Country()
{
  m_points.clear();
}

void Country::decreasePopulation(unsigned long amount)
{
  m_population -= amount;
}

bool Country::explore(Point* pnt)
{
  // Chance of capturing point
  int8_t chanceOfCapture = 8;

  // Keep in bounds
  if(pnt->x < 1 || pnt->y < 1 || pnt->x > (*m_field).getWidth()-1 || pnt->y > (*m_field).getHeight()-1) return false;

  // If military forces on point to capture are weaker, make chance of capture 4 times higher
  if (pnt->barracks < pnt->barracks)
    chanceOfCapture /= 4;

  // If not an enemy, make chance of capture 4 times lower
  if(m_enemies.find(pnt->country) == m_enemies.end())
    chanceOfCapture *= 4;

  // Check if point is not owned by other country and own country is set to capture these points
  if (((pnt->country == NULL && m_captureUnclaimed) ||
      // or if it is another country that can be captured and not an ally
      (rand() % (chanceOfCapture + 1) == 0 && pnt->country != NULL) && m_allies.find(pnt->country) == m_allies.end()) && 
      // and is on land and has enough resources
      pnt->surface == SURFACE::LAND && m_processed > 0)
  {
    // Slight chance of becoming allies, elaborate on later
    if (rand() % 8 < 1 && m_vassalOwner == NULL) addAlly(pnt->country);

    // If country is found
    if (pnt->country != NULL)
    {
      // If centre point captured
      if (pnt->country->isCenter(pnt->x,pnt->y))
      {
        // More than likely make vassal, but sometimes merge with this country
        if (rand() % 4 == 0) mergeCountry(pnt->country);
        else pnt->country->setVassalOwner(this);
      }

      // Remove this points population from the host country
      pnt->country->decreasePopulation(pnt->population);

      // Higher chance of becoming enemies, elaborate later
      if (rand() % 4 < 1 && m_vassalOwner == NULL) addEnemy(pnt->country);
    }
    // Claim this land
    m_points.insert(pnt);
    pnt->country = this;
    m_population += pnt->population;

    // Drain from resources
    m_processed -= 1;
  }

  return true;
}

void Country::trade(Point* pnt)
{
  // Randomly take a portion of each resource
  int processedAmount = rand() % ((m_processed / 10) + 1);
  int cropsAmount = rand() % ((m_crops / 10) + 1);
  int oreAmount = rand() % ((m_ore / 10) + 1);
  // Check if point is another country and an ally
  if (pnt->country != NULL && m_allies.find(pnt->country) != m_allies.end() 
      // and is on land
      && pnt->surface == SURFACE::LAND)
  {
    // Take portions from each resources.
    m_processed -= processedAmount;
    m_crops -= cropsAmount;
    m_ore -= oreAmount;
    // Give these portion to this country.
    pnt->country->giveProcessed(processedAmount);
    pnt->country->giveCrops(cropsAmount);
    pnt->country->giveOre(oreAmount);
  }
}

void Country::harvest(Point* pnt)
{
  // Transfer amount
  int8_t amount = 0;
  // If land is developed for resource mining
  if (pnt->development == DEVELOPMENT::FARM) {
    // If land is wet enough to harvest crops from and can harvest
    if (pnt->moisture >= 100 && m_harvestCrops)
    {
      // Calculate amount
      amount = rand() % 4;
      // Subtract from moisture
      pnt->moisture -= amount;
      // And add to crop
      m_crops += amount;
    }
    // Otherwise, if dry enough to mine rocks from and can mine
    else if (m_mineOre)
    {
      // Calculate amount
      amount = rand() % 4;
      if (rand() % 10 == 0)
      {
        // Subtract from height
        pnt->height -= amount;
        // And add to ore
        m_ore += amount;
      }
    }
  }

  // Cap above negative for all resources
  m_crops = m_crops > 0 ? m_crops : 0;
  m_ore = m_ore > 0 ? m_ore : 0;
}

void Country::process(Point* pnt)
{  
  // Transfer amount
  int8_t amount = 0;
  // If land is urban developed and can process goods
  if (pnt->development == DEVELOPMENT::URBAN && m_processGoods) {
    // Calculate amount
    amount = rand() % 4;
    // If stocks are sufficient
    if (m_crops >= amount/2 && m_ore >= amount/2) {
      // Subtract half each from crops and ore
      m_crops -= amount/2;
      m_ore -= amount/2;
      // And add to processed
      m_processed += amount;
    }
  }

  // Cap above negative for all resources
  m_processed = m_processed > 0 ? m_processed : 0;
}

void Country::updateCountry()
{
  // X and Y position offsets for country
  short xOffset, yOffset;
  // For all pieces of land this country owns
  for (Point* pnt : m_points)
  {
    // Init to zero
    xOffset = 0;
    yOffset = 0;

    // Check nearby values
    for (short i = -1; i <= 1; ++i)
    for (short j = -1; j <= 1; ++j) {
      if (pnt == NULL) continue;
      // Skip the centre and keep in bounds and on land
      if((i == 0 && j == 0) || pnt->x + i < 1 || pnt->y + j < 1 ||
        pnt->x + i > (*m_field).getWidth()-1 || pnt->y + j > (*m_field).getHeight()-1 ||
        (*m_field)(pnt->x+i,pnt->y+j).surface != SURFACE::LAND) continue;

      // If this population is better than previous candidate
      if((*m_field)(pnt->x+i,pnt->y+j).population >
         (*m_field)(pnt->x+xOffset,pnt->y+yOffset).population)
      {
        // Use this instead
        xOffset = i;
        yOffset = j;
      }
    }

    // If area cannot be claimed, continue in the loop
    if (!explore(&(*m_field)(pnt->x+xOffset,pnt->y+yOffset))) continue;

    // Trade with country on this non-null point
    if (&(*m_field)(pnt->x+xOffset,pnt->y+yOffset) != NULL)
      trade(&(*m_field)(pnt->x+xOffset,pnt->y+yOffset));

    // Process this point
    process(pnt);

    // Harvest this point
    harvest(pnt);
  }
}

void Country::mergeCountry(Country* mergedCountry)
{
  for(Point* pnt : mergedCountry->getPoints()) {
    // Point is already claimed in above function, do not claim here
    if (pnt->country->isCenter(pnt->x,pnt->y)) continue;
    // Add point into set
    m_points.insert(pnt);
    // Set tile pointer to current country
    pnt->country = this;
    // Add point population to total population
    m_population += pnt->population;
  }
  // Make other country empty
  mergedCountry->getPoints().clear();
}

sf::Color Country::getColor() {
  return m_color;
}

sf::Color Country::getHalfColor() {
  return m_hColor;
}

int Country::getProcessed()
{
  return m_processed;
}

int Country::getCrops()
{
  return m_crops;
}

int Country::getOre()
{
  return m_ore;
}

void Country::giveProcessed(int amount)
{
  m_processed += amount;
}

void Country::giveCrops(int amount)
{
  m_crops += amount;
}

void Country::giveOre(int amount)
{
  m_ore += amount;
}

void Country::setVassalOwner(Country* owner)
{
  m_vassalOwner = owner;
}

Country* Country::getVassalOwner()
{
  return m_vassalOwner;
}

std::set<Country*> Country::getAllies()
{
  return m_allies;
}

std::set<Country*> Country::getEnemies()
{
  return m_enemies;
}

void Country::addAlly(Country* ally)
{
  // Null check, and if ally is not in each others list already, and not adding itself
  if (ally != NULL && m_allies.find(ally) == m_allies.end() &&
      m_enemies.find(ally) == m_enemies.end() && this != ally)
  {
    // Add to each others’ list
    m_allies.insert(ally);
    ally->addAlly(this);
  }
}

void Country::addEnemy(Country* enemy)
{
  // Null check, and if ally is not in each others list already, and not adding itself
  if (enemy != NULL && m_enemies.find(enemy) == m_enemies.end() &&
      m_allies.find(enemy) == m_allies.end() && this != enemy)
  {
    // Add to each others’ list
    m_enemies.insert(enemy);
    enemy->addEnemy(this);
  }
}

void Country::removeAlly(Country* ally)
{
  // Null check, erase function already makes sure elements are in list
  if (ally != NULL) {
    // Remove from each others’ list
    m_allies.erase(ally);
    ally->removeAlly(this);
  }
}

void Country::removeEnemy(Country* enemy)
{
  // Null check, erase function already makes sure elements are in list
  if (enemy != NULL) {
    // Remove from each others’ list
    m_enemies.erase(enemy);
    enemy->removeEnemy(this);
  }
}

void Country::toggleCaptureUnclaimed()
{
  m_captureUnclaimed = !m_captureUnclaimed;
}

bool Country::canCaptureUnclaimed()
{
  return m_captureUnclaimed;
}

void Country::toggleTrade()
{
  m_trade = !m_trade;
}

bool Country::canTrade()
{
  return m_trade;
}

void Country::toggleHarvestCrops()
{
  m_harvestCrops = !m_harvestCrops;
}

bool Country::canHarvestCrops()
{
  return m_harvestCrops;
}

void Country::toggleMineOre()
{
  m_mineOre = !m_mineOre;
}

bool Country::canMineOre()
{
  return m_mineOre;
}

void Country::toggleProcessGoods()
{
  m_processGoods = !m_processGoods;
}

bool Country::canProcessGoods()
{
  return m_processGoods;
}