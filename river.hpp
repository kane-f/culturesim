#ifndef RIVER_HPP
#define RIVER_HPP

#include "line.hpp"

class River: public Line
{
  public:
    /**
      * Constructor and destructor for the river, inherited from Line.
      */
  	River(Point* point, PointArray* field, std::string name = "");
    ~River();

  	/**
      * Function that propogates a river based on increasing heights from its mouth, until nowhere higher is found.
  	  */
  	void propogateRiver();

  	/** 
      * Function that iterates through river points to erode land on and around it to create "basins".
  	  */
  	void erodeBasin();
};

#endif
