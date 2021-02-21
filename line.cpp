#include "line.hpp"

Line::Line(Point* point, PointArray* field, std::string name)
{
  // Initialise the vector with this point
	m_points.push_back(point);
	m_field = field;
	m_name = name;
  // Set x and y ends to this point
	xEnd = point->x;
	yEnd = point->y;
}

Line::~Line()
{
	m_points.clear();
}

std::string Line::getName()
{
	return m_name;
}

Point* Line::getStart()
{
	return m_points.front();
}

Point* Line::getEnd()
{
	return m_points.back();
}

std::vector<Point*> Line::getPoints()
{
  return m_points;
}

void Line::add(Point* point)
{
	m_points.push_back(point);
}

void Line::shrink()
{
	m_points.pop_back();
}

int Line::getSize()
{
	return m_points.size();
}