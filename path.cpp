#include "path.hpp"

Path::Path(Point* point, PointArray* field, std::string name) :
Line(point,field,name)
{
  // Set path on point to this path
  point->path = this;
  // Make bridge if over river, otherwise path
  if(point->route == ROUTE::RIVER) point->route = ROUTE::BRIDGE;
  else point->route = ROUTE::PATH;
}

Path::~Path()
{
  m_points.clear();
}

void Path::initPath(short radius)
{
  // Initialise the end for the path end with basic bounds checking
	xEnd = m_points.front()->x > 1 ? m_points.front()->x-1 : 0;
	yEnd = m_points.front()->y > 1 ? m_points.front()->y-1 : 0;

  // // Check offset values within the radius
	for (short i = -1 * radius; i <= radius; ++i) {
    for (short j = -1 * radius; j <= radius; ++j) {
      // Skip the centre
      if(i == 0 && j == 0) continue;
      // Stay on land and in array bounds
      if(m_points.front()->x + i >= 1 &&
         m_points.front()->y + j >= 1 &&
      	 m_points.front()->x + i < (*m_field).getWidth()-1 &&
         m_points.front()->y + j < (*m_field).getHeight()-1 &&
         (*m_field)(m_points.front()->x+i,m_points.front()->y+j).surface == SURFACE::LAND) {
        // If the population is higher than the previous end
        if((*m_field)(m_points.front()->x+i,m_points.front()->y+j).population >=
        	(*m_field)(xEnd,xEnd).population) {
          // Make this the new end
        	xEnd = m_points.front()->x+i;
        	yEnd = m_points.front()->y+j;
        }
      }
    }
  }

  // Declare the type on the start tile
  if((*m_field)(xEnd,yEnd).route == ROUTE::RIVER) (*m_field)(xEnd,yEnd).route = ROUTE::BRIDGE;
  else (*m_field)(xEnd,yEnd).route = ROUTE::PATH;
  (*m_field)(xEnd,yEnd).path = this;
}

bool Path::propogatePath()
{
  // If already at the end, return true to signal that the path is finished
	if (m_points.back()->x == xEnd && m_points.back()->y == yEnd) return true;

  // Candidate variables for best point to propogate path on
  short xCandidate = m_points.back()->x > 1 ? m_points.back()->x - 1 : 0;
  short yCandidate = m_points.back()->y > 1 ? m_points.back()->y - 1 : 0;
  short xDistance = abs(xEnd - m_points.back()->x);
  short yDistance = abs(yEnd - m_points.back()->y);

  // Check offset values
  for (short i = -1; i <= 1; ++i) {
    for (short j = -1; j <= 1; ++j) {
      // Skip the centre
      if(i == 0 && j == 0) continue;
      // Stay on land and in array bounds
      if(m_points.back()->x + i >= 1 &&
         m_points.back()->y + j >= 1 &&
         m_points.back()->x + i < (*m_field).getWidth()-1 &&
         m_points.back()->y + j < (*m_field).getHeight()-1 &&
         (*m_field)(m_points.front()->x+i,m_points.front()->y+j).surface == SURFACE::LAND) {
        // If this point is nearer, move towards it
        if (abs(xEnd - m_points.back()->x + i) <= xDistance) xCandidate = m_points.back()->x + i;
        if (abs(yEnd - m_points.back()->y + j) <= yDistance) yCandidate = m_points.back()->y + j;
      }
    }
  }
  
  // Checking if there is a smaller height difference on each tile surrounding the candidate and previous end.
  if (isNearerInHeight(m_points.back()->x, yCandidate, xCandidate, m_points.back()->y))
  {
    if (isNearerInHeight(m_points.back()->x, yCandidate, xCandidate, yCandidate))
      xCandidate = m_points.back()->x;
  }
  // If not on the x axis, do y axis
  else
  {
    if (isNearerInHeight(xCandidate, m_points.back()->y, xCandidate, yCandidate))
      yCandidate = m_points.back()->y;
  }

  // Add point to vector
  m_points.push_back(&(*m_field)(xCandidate,yCandidate));

  // Declare the type on the next tile
  if((*m_field)(xCandidate,yCandidate).route == ROUTE::RIVER) (*m_field)(xCandidate,yCandidate).route = ROUTE::BRIDGE;
  else (*m_field)(xCandidate,yCandidate).route = ROUTE::PATH;

  // If already at the end, return true to signal that the path is finished
  if (xCandidate == xEnd && yCandidate == yEnd) return true;
  // Else, return false
  return false;
}

bool Path::isNearerInHeight(short x1, short y1, short x2, short y2)
{
  return abs((*m_field)(x1,y1).height - (*m_field)(m_points.back()->x,m_points.back()->y).height) <
         abs((*m_field)(x2,y2).height - (*m_field)(m_points.back()->x,m_points.back()->y).height);
}