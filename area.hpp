#ifndef AREA_HPP
#define AREA_HPP

#include "point.hpp"
#include <set>

class Area
{
  public:
    /**
      * Constructor and destructor for the area.
      */
  	Area(Point* point, PointArray* field, std::string name, bool visible);
  	~Area();

  	/**
      * Gets the name of the area.
      */
    std::string getName();

    /**
      * Gets all points in the set.
      */
    std::set<Point*> getPoints();

    /**
      * Add a point to the line at the end of the set.
      * @Param point The point to add.
      */
    void add(Point* point);
    void remove(Point* point);

    /**
      * Check if the set is empty by external objects, mainly for culling.
      * @Return True if empty.
      */
    bool isEmpty();

    /**
      * Sets the country selection to be a boolean value.
      * @Param selected The value to set.
      */
    void setPrimarySelected(bool selected);
    void setSecondarySelected(bool selected);

    /**
      * Sets the country tangibility to be a boolean value.
      * @Param visible The value to set.
      */
    void setVisible(bool visible);

    /**
      * Gets the size of the area.
      * @Return The total amount of all points in the set.
      */
    int getArea();

    /**
      * Gets the size of the area
      * @Return The total amount of all points in the set
      */
    long getPopulation();

    /**
      * Gets the average amount of people per point.
      * @Return Population divided by area.
      */
    float getDensity();

    /**
      * Returns if a country is primary or secondary selected to perform actions on or show on map.
      */
    bool getPrimarySelected();
    bool getSecondarySelected();

    /**
      * Returns if the country is visible and tangible.
      */
    bool getVisible();

    /**
      * Checks if a point on the map is the centre of the country.
      * @Return True if point is centre.
      */
    bool isCenter(short x, short y);
  protected:
    // The name of the area.
    std::string m_name;

    // A set of points contained within this area
    std::set<Point*> m_points;

    // A pointer to the field object for additional manipulation
    PointArray* m_field;

    // The total population of the area.
    long m_population;

    // The central administration point of the country.
    short m_centreX;
    short m_centreY;

    // Booleans for if the country is selected by the player (primary or secondary) or visible on the map.
    bool m_isPrimarySelected;
    bool m_isSecondarySelected;

    // Boolean to check if country is visible or tangible on the map
    bool m_isVisible;
};

#endif
