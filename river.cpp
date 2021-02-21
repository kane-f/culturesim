#include "river.hpp"

River::River(Point* point, PointArray* field, std::string name) :
Line(point,field,name)
{
  // Set path on point to this path
  point->river = this;
  // Make glacier if below freezing, otherwise keep as river
  if(point->temperature >= 0) point->route = ROUTE::RIVER;
  else point->route = ROUTE::GLACIER;
}

River::~River()
{
  m_points.clear();
}

void River::propogateRiver()
{
  // Variable to break out of loop increasing in height.
	bool nothingHigher = false;
  // Candidate variables for best point to propogate river towards
	short xCandidate = xEnd > 1 ? xEnd-1 : 0;
	short yCandidate = yEnd > 1 ? yEnd-1 : 0;
  // While there is possibility to increase in height, and in bounds
	while(!nothingHigher) {
    // Check nearby heights
		for (short x = -1; x <= 1; ++x) {
      for (short y = -1; y <= 1; ++y) {
        // Skip the current point
      	if((x == 0 && y == 0) || x+xEnd < 1 || y+yEnd < 1 ||
            x+xEnd > (*m_field).getWidth()-1 || y+yEnd > (*m_field).getHeight()-1 ||
            xCandidate < 1 || yCandidate < 1 ||
            xCandidate > (*m_field).getWidth()-1 || yCandidate > (*m_field).getHeight()-1) continue;
        // If this height is better than previous candidate
      	if((*m_field)(x+xEnd,y+yEnd).height - (*m_field)(xEnd,yEnd).height >=
      		 (*m_field)(xCandidate,yCandidate).height - (*m_field)(xEnd,yEnd).height)
      	{
          // Use this instead
      		xCandidate = x+xEnd;
      		yCandidate = y+yEnd;
      	}
    	}
  	}
    // If no higher point is found, break out of loop and end function
  	if((xEnd > (*m_field).getWidth()-1 || yEnd > (*m_field).getHeight()-1 || xEnd < 1 || yEnd < 1 ||
      ((*m_field)(xCandidate,yCandidate).height - (*m_field)(xEnd,yEnd).height) <= 0) && rand() % 2 == 0)
  	{
  		nothingHigher = true;
  	}
    // Move current propogation point to candidate and reset candidate loop
  	xEnd = xCandidate;
  	yEnd = yCandidate;
  	xCandidate = xEnd > 0 ? xEnd-1 : 0;
  	yCandidate = yEnd > 0 ? yEnd-1 : 0;
    // Add point to vector
  	m_points.push_back(&(*m_field)(xEnd,yEnd));
    // Set route type on point
  	if((*m_field)(xEnd,yEnd).temperature >= 0) (*m_field)(xEnd,yEnd).route = ROUTE::RIVER;
    else (*m_field)(xEnd,yEnd).route = ROUTE::GLACIER;
    // Set river pointer on end
    (*m_field)(xEnd,yEnd).river = this;
  }
}

void River::erodeBasin()
{
  // Initialised drain range variable
  short drainRange = 0;
  // Backwards through vector, at river source
  for(std::vector<Point*>::iterator pnt = m_points.end()-1; pnt != m_points.begin(); --pnt) {
    // For area around drain range
    for (short x = (*pnt)->x - drainRange/2; x < (*pnt)->x + drainRange/2; ++x) {
      for (short y = (*pnt)->y - drainRange/2; y < (*pnt)->y + drainRange/2; ++y) {
        // Avoid array out of bounds exception
        if((*pnt)->x - drainRange >= 0 && (*pnt)->y - drainRange >= 0 &&
           (*pnt)->x + drainRange < (*m_field).getWidth() && (*pnt)->y + drainRange < (*m_field).getHeight() &&
           (*pnt)->surface == SURFACE::LAND) {
          // Subtract from height
          (*m_field)(x,y).height -= (*m_field)(x,y).height/64;
        }
      }
    }
    // Increase drain range with each point
    drainRange++;
  }
}