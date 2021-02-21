#include "area.hpp"

Area::Area(Point* point, PointArray* field, std::string name, bool visible)
{
  // Initialise the set with this point
	m_points.insert(point);
	m_field = field;
	m_name = name;
  // Set x and y centre to this point
  m_centreX = point->x;
  m_centreY = point->y;
  m_isPrimarySelected = false;
  m_isSecondarySelected = false;
  m_isVisible = visible;
}

Area::~Area()
{
	m_points.clear();
}

std::string Area::getName()
{
	return m_name;
}

std::set<Point*> Area::getPoints()
{
  return m_points;
}

void Area::add(Point* point)
{
	m_points.insert(point);
}

void Area::remove(Point* point)
{
	m_points.erase(m_points.find(point));
}

bool Area::isEmpty()
{
  return m_points.empty();
}

int Area::getArea()
{
	return m_points.size();
}

long Area::getPopulation()
{
  return m_population;
}

float Area::getDensity()
{
  return m_population / m_points.size();
}

bool Area::getPrimarySelected()
{
  return m_isPrimarySelected;
}

bool Area::getSecondarySelected()
{
  return m_isSecondarySelected;
}

bool Area::getVisible()
{
  return m_isVisible;
}

void Area::setPrimarySelected(bool selected)
{
  m_isPrimarySelected = selected;
}

void Area::setSecondarySelected(bool selected)
{
  m_isSecondarySelected = selected;
}

void Area::setVisible(bool visible)
{
  m_isVisible = visible;
}

bool Area::isCenter(short x, short y)
{
  return x == m_centreX && y == m_centreY;
}