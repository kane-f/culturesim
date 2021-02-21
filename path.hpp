#ifndef PATH_HPP
#define PATH_HPP

#include "line.hpp"

class Path: public Line
{
    public:
      /**
        * Constructor and destructor for the path, inherited from Line.
        */
      Path(Point* point, PointArray* field, std::string name = "");
      ~Path();

    	/**
        * Function that creates a path from one high density area to another.
    	  */
    	void initPath(short radius);

    	/** 
        * Function that propogates said path.
    	  */
    	bool propogatePath();

    private:
      /** 
        * Function that checks if the first location of two is nearer in height to the end of the path.
        */
      bool isNearerInHeight(short x1, short y1, short x2, short y2);
};

#endif
