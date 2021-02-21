#ifndef LINE_HPP
#define LINE_HPP

#include "point.hpp"
#include <vector>

class Line
{
  public:
    /**
      * Constructor and destructor for the line.
      */
  	Line(Point* point, PointArray* field, std::string name = "");
  	~Line();

  	/**
      * Gets the name of the line.
      */
    std::string getName();

  	/**
      * Gets the points at the start or end of the vector.
      * @Return The point object at the start or end.
      */
    Point* getStart();
  	Point* getEnd();

    /**
      * Gets all points in the vector.
      */
    std::vector<Point*> getPoints();

  	/**
      * Add a point to the line at the end of the vector.
      * @Param point The point to add.
      */
    void add(Point* point);

    /**
      * Shrink the line by one from the end.
      */
  	void shrink();

  	/**
      * Gets the length of the line
      * @Return The total amount of all points in the vector
      */
    int getSize();
  protected:
    // The name of the area.
    std::string m_name;

    // A vector of points contained within this line
    std::vector<Point*> m_points;

    // A pointer to the field object for additional manipulation
    PointArray* m_field;
    
    // The end of the river, used in generation
    short xEnd;
    short yEnd;
};

#endif
