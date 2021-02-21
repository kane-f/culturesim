#include "point.hpp"
#include "country.hpp"
#include "river.hpp"
#include "path.hpp"

Point& PointArray::operator() (short x, short y)
{
  return point[y*width + x];
}

PointArray::PointArray(const unsigned short w, const unsigned short h) :
width(w), height(h)
{
  // Create new array with size width times height
  point = new Point[w*h];
}

PointArray::~PointArray()
{
  delete [] point;
}

void PointArray::reset(const unsigned short w, const unsigned short h)
{
  delete [] point;
  width = w;
  height = h;
  point = new Point[w*h];
}

unsigned short PointArray::getWidth()
{
	return width;
}

unsigned short PointArray::getHeight()
{
	return height;
}