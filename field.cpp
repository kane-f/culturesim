#include "field.hpp"

Field::Field(sf::RenderWindow* window) :
m_window(*window),
m_highestPoint(0),
m_lowestPoint(0),
m_worldPopulation(0),
m_day(0),
m_mapWidth(320),
m_mapHeight(320),
m_monthNumber(1)
{
  m_currentCountry = NULL;
}

Field::~Field()
{

}

void Field::generateLevel(unsigned short size,
                          double featureSize,
                          short maxHeight,
                          short density,
                          short seaLevel,
                          unsigned short riverFrequency)
{
  // Set map width and height
  m_mapWidth = size;
  m_mapHeight = size;
  // Create field
  m_field = new PointArray(m_mapWidth,m_mapHeight);
  // Resize vertices to field size
  m_fieldPoints = sf::VertexArray(sf::Points, m_mapWidth * m_mapHeight);
  // Set sprite origin in middle of field
  m_fieldSprite.setOrigin((m_mapWidth/2),(m_mapHeight/2));

  // Source vectors for opensimplex function
  short source[256];
	for (short i = 0; i < 256; i++)
		source[i] = i;
	for (int i = 255; i >= 0; i--) {
		int r = (int)((rand() + 31) % (i + 1));
		if (r < 0)
			r += (i + 1);
		perm[i] = source[r];
		source[r] = source[i];
	}

  for (short x = 0; x < m_mapWidth; ++x)
  for (short y = 0; y < m_mapHeight; ++y)
  {
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
      (*m_field)(x,y).x = x;
      (*m_field)(x,y).y = y;
      m_fieldPoints[x+(m_mapHeight*y)].position = sf::Vector2f(x, y);
      (*m_field)(x,y).height = evaluateSimplexPoint(x / featureSize, y / featureSize, maxHeight)
                           - recurseEvaluate(x, y, featureSize, maxHeight / 2, 2, (*m_field)(x,y).height)
                           + seaLevel;

      if ((*m_field)(x,y).height < 0)
      {
        (*m_field)(x,y).surface = SURFACE::SALTWATER;
        (*m_field)(x,y).population = 0;
      }
      else
      {
        (*m_field)(x,y).surface = SURFACE::LAND;
      }

      (*m_field)(x,y).moisture = 5000;

      if ((*m_field)(x,y).surface != SURFACE::SALTWATER)
      {
        // Cap to population density/km^2
        (*m_field)(x,y).population = rand() % density;
        if ((*m_field)(x,y).height > 2000) (*m_field)(x,y).population -= (((*m_field)(x,y).height - 2000) * density) / 4000;
      }
  }
  
  if (riverFrequency > 100) riverFrequency = 100;
  if (riverFrequency < 0) riverFrequency = 0;
  generateRivers(featureSize/24, 100 / riverFrequency);

  updateMoistureAndTemperature();

  m_worldPopulation = updateWorldPopulation();
}

void Field::generateRivers(short iterations, short frequency)
{
  for (short a = 0; a < iterations; ++a)
  {
    m_rivers.clear();
    for (short x = 0; x < m_mapWidth; ++x)
      {
        for (short y = 0; y < m_mapHeight; ++y)
        {
          if ((*m_field)(x,y).route == ROUTE::RIVER || (*m_field)(x,y).route == ROUTE::GLACIER)
          {
            (*m_field)(x,y).route = ROUTE::NONE;
          }
        }
      }

    for (short x = 0; x < m_mapWidth; ++x)
      {
        for (short y = 0; y < m_mapHeight; ++y)
        {
          if ((*m_field)(x,y).surface == SURFACE::LAND &&
              checkNearbyTypes(x,y,SURFACE::SALTWATER) &&
              rand() % (100 / frequency) == 0)
          {
            m_rivers.push_back(River(&(*m_field)(x,y),m_field,generateName()));
          }
        }
      }

    for(std::vector<River>::iterator i = m_rivers.begin(); i != m_rivers.end(); ++i) {
        i->propogateRiver();
    }

    for(std::vector<River>::iterator i = m_rivers.begin(); i != m_rivers.end(); ++i) {
      i->erodeBasin();
    }
  }
}

void Field::clearField()
{
  for (short x = 0; x < m_mapWidth; ++x)
  for (short y = 0; y < m_mapHeight; ++y)
  {
    m_fieldPoints[x+(m_mapHeight*y)].position = sf::Vector2f(x, y);
    m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0x00,0x00,0x00);
    (*m_field)(x,y).height = 0;
    (*m_field)(x,y).population = 0;
    (*m_field)(x,y).temperature = 0;
    (*m_field)(x,y).moisture = 0;
    (*m_field)(x,y).rainfall = 0;
    (*m_field)(x,y).development = DEVELOPMENT::WILD;
    (*m_field)(x,y).route = ROUTE::NONE;
    (*m_field)(x,y).surface = SURFACE::LAND;
    (*m_field)(x,y).country = NULL;
    (*m_field)(x,y).path = NULL;
    (*m_field)(x,y).river = NULL;
    m_worldPopulation = 0;
    m_day = 0;
  }
  m_fieldPoints.clear();
  m_rivers.clear();
  m_pathsInProgress.clear();
  m_pathsComplete.clear();
  for(std::vector<Country*>::iterator i = m_countries.begin(); i != m_countries.end(); ++i) {
    delete (*i);
    *i = NULL;
  }
  m_countries.clear();
  delete m_field;
  m_field = NULL;
}

unsigned long Field::getWorldPopulation()
{
  return m_worldPopulation;
}

std::string Field::getWeekDay()
{
  return m_days[m_day % 7];
}

unsigned long Field::getDay()
{
  return m_day % 365;
}

unsigned long Field::getMonth()
{
  return m_monthNumber;
}

unsigned long Field::getYear()
{
  return m_day / 365;
}

std::vector<Country*> Field::getCountries()
{
  return m_countries;
}

bool Field::checkNearbyTypes(short x,
                             short y,
                             DEVELOPMENT development,
                             short radius)
{
  bool found = false;
  for (short i = -1 * radius; i <= radius; ++i) {
    for (short j = -1 * radius; j <= radius; ++j) {
      if(i == 0 && j == 0) continue;
      if(x + i >= 0 && y + j >= 0 && x + i < m_mapWidth && y + j < m_mapWidth) {
        if((*m_field)(x+i,y+j).development == development) found = true;
      }
    }
  }
  return found;
}

bool Field::checkNearbyTypes(short x,
                             short y,
                             ROUTE route,
                             short radius)
{
  bool found = false;
  for (short i = -1 * radius; i <= radius; ++i) {
    for (short j = -1 * radius; j <= radius; ++j) {
      if(i == 0 && j == 0) continue;
      if(x + i >= 0 && y + j >= 0 && x + i < m_mapWidth && y + j < m_mapWidth) {
        if((*m_field)(x+i,y+j).route == route) found = true;
      }
    }
  }
  return found;
}

bool Field::checkNearbyTypes(short x,
                             short y,
                             SURFACE surface,
                             short radius)
{
  bool found = false;
  for (short i = -1 * radius; i <= radius; ++i) {
    for (short j = -1 * radius; j <= radius; ++j) {
      if(i == 0 && j == 0) continue;
      if(x + i >= 0 && y + j >= 0 && x + i < m_mapWidth && y + j < m_mapWidth) {
        if((*m_field)(x+i,y+j).surface == surface) found = true;
      }
    }
  }
  return found;
}

bool Field::checkNearbyTypes(short x,
                             short y,
                             Country* country,
                             short radius)
{
  bool found = false;
  for (short i = -1 * radius; i <= radius; ++i) {
    for (short j = -1 * radius; j <= radius; ++j) {
      if(i == 0 && j == 0) continue;
      if(x + i >= 0 && y + j >= 0 && x + i < m_mapWidth && y + j < m_mapWidth) {
        if((*m_field)(x+i,y+j).country == country) found = true;
      }
    }
  }
  return found;
}

bool Field::checkOppositeCountry(short x,
                                 short y,
                                 Country* country,
                                 short radius)
{
  bool found = false;
  for (short i = -1 * radius; i <= radius; ++i) {
    for (short j = -1 * radius; j <= radius; ++j) {
      if(i == 0 && j == 0) continue;
      if(x + i >= 0 && y + j >= 0 && x + i < m_mapWidth && y + j < m_mapWidth) {
        if((*m_field)(x+i,y+j).country != country) found = true;
      }
    }
  }
  return found;
}

short Field::getHeightDifference(short x1, short y1, short x2, short y2)
{
  return (*m_field)(x1,y1).height - (*m_field)(x2,y2).height;
}

short Field::getTemperature(short x, short y)
{
  // Form an equator in the centre, map should probably not be unsigned to make this easier
  short latitude = y - (m_mapHeight / 2), maxLatitude = m_mapHeight / 2;
  // Keep values positive
  if (latitude < 0) latitude *= -1;
  // Further from the equator plus lapse rate, should cap at -50 (Add 1 to not divide by zero)
  if((*m_field)(x,y).surface != SURFACE::LAND)
    return 25 - (latitude / ((maxLatitude / 64) + 1)) - ((*m_field)(x,y).height / 150);
  else return 50 - (latitude / ((maxLatitude / 128) + 1)) - ((*m_field)(x,y).height / 150);
}

BIOME Field::getBiome(short x, short y)
{
  if((*m_field)(x,y).surface == SURFACE::SALTWATER)
  {
    if((*m_field)(x,y).temperature >= 30) return BIOME::WARM_WATER;
    else if((*m_field)(x,y).temperature >= 15) return BIOME::TEMP_WATER;
    else if((*m_field)(x,y).temperature >= 0) return BIOME::COLD_WATER;
    else return BIOME::ICE_CAP;
  }
  else if ((*m_field)(x,y).surface == SURFACE::ICECAP) return BIOME::ICE_CAP;
  else
  {
    if((*m_field)(x,y).moisture > 3000)
    {
      if((*m_field)(x,y).temperature >= 35) return BIOME::RAINFOREST;
      else if((*m_field)(x,y).temperature >= 10) return BIOME::MONSOON;
      else if((*m_field)(x,y).temperature > -5) return BIOME::TAIGA;
      else return BIOME::TUNDRA;
    }
    else if((*m_field)(x,y).moisture > 2000)
    {
      if((*m_field)(x,y).temperature >= 35) return BIOME::SEASFOREST;
      else if((*m_field)(x,y).temperature >= 10) return BIOME::FOREST;
      else if((*m_field)(x,y).temperature > -5) return BIOME::TAIGA;
      else return BIOME::TUNDRA;
    }
    else if((*m_field)(x,y).moisture > 500)
    {
      if((*m_field)(x,y).temperature >= 35) return BIOME::SAVANNA;
      else if((*m_field)(x,y).temperature >= 10) return BIOME::MEADOW;
      else if((*m_field)(x,y).temperature > -5) return BIOME::TAIGA;
      else return BIOME::TUNDRA;
    }
    else
    {
      if((*m_field)(x,y).temperature >= 20) return BIOME::DESERT;
      else if((*m_field)(x,y).temperature > -5) return BIOME::WASTELAND;
      else return BIOME::TUNDRA;
    }
  }
  return BIOME::WASTELAND;
}

short Field::propogateMoisture(short x, short y)
{
  // Initiate amount to caculate
  short amount = 0;
  // Check offset values
  for (short i = -1; i <= 1; ++i) {
    for (short j = -1; j <= 1; ++j) {
      // Skip the centre
      if(i == 0 && j == 0) continue;
      // Stay in array bounds
      if(x + i >= 0 && y + j >= 0 && x + i < m_mapWidth && y + j < m_mapWidth) {
        // Add total moisture from surrounding areas and make slight falloff
        // (8 spaces are checked so a number greater than 8 is desirable)
        if((*m_field)(x+i,y+j).moisture > 0) {
          amount += ((*m_field)(x+i,y+j).moisture / 12);
        }
      }
    }
  }
  // Return this total
  return amount;
}

short Field::adjustMoisture(short x, short y)
{
  // Form an equator in the centre, map should probably not be unsigned to make this easier
  short latitude = y - (m_mapHeight / 2), maxLatitude = m_mapHeight / 2;
  // Keep values positive
  if (latitude < 0) latitude *= -1;
  // Further from the equator plus rate above sea level, should range 0 to 4000 (Add 1 to not divide by zero)
  return (*m_field)(x,y).moisture - (latitude * ((maxLatitude / 16) + 1)) - ((*m_field)(x,y).height / 2);
}

// Recursive function to oscillate noise
short Field::recurseEvaluate(double x, double y, int size, int height, double divisor, int result) {
  // Keep divisor above 1 to prevent divide by zero error and to make sure it eventually reaches 1
  if (divisor < 1) divisor = 1;
  // Divide size and height before calculations
  size /= divisor;
  height /= divisor;
  // Add to result
  result += evaluateSimplexPoint(x / size, y / size, height);
  // When done dividing, return result, otherwise call function again with result passed in
  if (size <= 1) return result;
  else return recurseEvaluate(x, y, size, height, divisor, result);
}

// OpenSimplex algorithm by Kurt Spencer and adapted from java at https://gist.github.com/KdotJPG/b1270127455a94ac5d19
short Field::evaluateSimplexPoint(double x, double y, int height)
{
  //Place input coordinates onto grid.
		double stretchOffset = (x + y) * STRETCH_CONSTANT_2D;
		double xs = x + stretchOffset;
		double ys = y + stretchOffset;

		//Fast floor to get grid coordinates of rhombus (stretched square) super-cell origin.
		int xsb = xs < (int)xs ? (int)xs - 1 : (int)xs;
		int ysb = ys < (int)ys ? (int)ys - 1 : (int)ys;

		//Skew out to get actual coordinates of rhombus origin. We'll need these later.
		double squishOffset = (xsb + ysb) * SQUISH_CONSTANT_2D;
		double xb = xsb + squishOffset;
		double yb = ysb + squishOffset;

		//Compute grid coordinates relative to rhombus origin.
		double xins = xs - xsb;
		double yins = ys - ysb;

		//Sum those together to get a value that determines which region we're in.
		double inSum = xins + yins;

		//Positions relative to origin point.
		double dx0 = x - xb;
		double dy0 = y - yb;

		//We'll be defining these inside the next block and using them afterwards.
		double dx_ext, dy_ext;
		int xsv_ext, ysv_ext;

		double value = 0;

		//Contribution (1,0)
		double dx1 = dx0 - 1 - SQUISH_CONSTANT_2D;
		double dy1 = dy0 - 0 - SQUISH_CONSTANT_2D;
		double attn1 = 2 - dx1 * dx1 - dy1 * dy1;
		if (attn1 > 0) {
			attn1 *= attn1;
			value += attn1 * attn1 * extrapolateSimplexPoint(xsb + 1, ysb + 0, dx1, dy1);
		}

		//Contribution (0,1)
		double dx2 = dx0 - 0 - SQUISH_CONSTANT_2D;
		double dy2 = dy0 - 1 - SQUISH_CONSTANT_2D;
		double attn2 = 2 - dx2 * dx2 - dy2 * dy2;
		if (attn2 > 0) {
			attn2 *= attn2;
			value += attn2 * attn2 * extrapolateSimplexPoint(xsb + 0, ysb + 1, dx2, dy2);
		}

		if (inSum <= 1) { //We're inside the triangle (2-Simplex) at (0,0)
			double zins = 1 - inSum;
			if (zins > xins || zins > yins) { //(0,0) is one of the closest two triangular vertices
				if (xins > yins) {
					xsv_ext = xsb + 1;
					ysv_ext = ysb - 1;
					dx_ext = dx0 - 1;
					dy_ext = dy0 + 1;
				} else {
					xsv_ext = xsb - 1;
					ysv_ext = ysb + 1;
					dx_ext = dx0 + 1;
					dy_ext = dy0 - 1;
				}
			} else { //(1,0) and (0,1) are the closest two vertices.
				xsv_ext = xsb + 1;
				ysv_ext = ysb + 1;
				dx_ext = dx0 - 1 - 2 * SQUISH_CONSTANT_2D;
				dy_ext = dy0 - 1 - 2 * SQUISH_CONSTANT_2D;
			}
		} else { //We're inside the triangle (2-Simplex) at (1,1)
			double zins = 2 - inSum;
			if (zins < xins || zins < yins) { //(0,0) is one of the closest two triangular vertices
				if (xins > yins) {
					xsv_ext = xsb + 2;
					ysv_ext = ysb + 0;
					dx_ext = dx0 - 2 - 2 * SQUISH_CONSTANT_2D;
					dy_ext = dy0 + 0 - 2 * SQUISH_CONSTANT_2D;
				} else {
					xsv_ext = xsb + 0;
					ysv_ext = ysb + 2;
					dx_ext = dx0 + 0 - 2 * SQUISH_CONSTANT_2D;
					dy_ext = dy0 - 2 - 2 * SQUISH_CONSTANT_2D;
				}
			} else { //(1,0) and (0,1) are the closest two vertices.
				dx_ext = dx0;
				dy_ext = dy0;
				xsv_ext = xsb;
				ysv_ext = ysb;
			}
			xsb += 1;
			ysb += 1;
			dx0 = dx0 - 1 - 2 * SQUISH_CONSTANT_2D;
			dy0 = dy0 - 1 - 2 * SQUISH_CONSTANT_2D;
		}

		//Contribution (0,0) or (1,1)
		double attn0 = 2 - dx0 * dx0 - dy0 * dy0;
		if (attn0 > 0) {
			attn0 *= attn0;
			value += attn0 * attn0 * extrapolateSimplexPoint(xsb, ysb, dx0, dy0);
		}

		//Extra Vertex
		double attn_ext = 2 - dx_ext * dx_ext - dy_ext * dy_ext;
		if (attn_ext > 0) {
			attn_ext *= attn_ext;
			value += attn_ext * attn_ext * extrapolateSimplexPoint(xsv_ext, ysv_ext, dx_ext, dy_ext);
		}

		return (value / NORM_CONSTANT_2D) * height;
}

double Field::extrapolateSimplexPoint(int xsb, int ysb, double dx, double dy)
{
	int index = perm[(perm[xsb & 0xFF] + ysb) & 0xFF] & 0x0E;
	return gradients2D[index] * dx
		+ gradients2D[index + 1] * dy;
}

void Field::moveField(float offsetX, float offsetY)
{
  m_fieldSprite.move(offsetX, offsetY);
}

void Field::scaleField(float factorX, float factorY)
{
  m_fieldSprite.scale(factorX, factorY);
}

Point* Field::getPoint(short x, short y)
{
  if(x > 0 && y > 0 && x < m_mapWidth && y < m_mapHeight) return &(*m_field)(x,y);
  else return &(*m_field)(m_mapWidth/2,m_mapHeight/2);
}
