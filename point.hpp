#ifndef POINT_HPP
#define POINT_HPP

#include "utils.hpp"

enum struct DEVELOPMENT {
  WILD,
  FARM,
  URBAN
};

enum struct ROUTE {
  NONE,
  RIVER,
  GLACIER,
  PATH,
  BRIDGE
};

enum struct SURFACE {
  SALTWATER,
  FRESHWATER,
  LAND,
  ICECAP
};

enum struct BIOME {
  WARM_WATER,
  TEMP_WATER,
  COLD_WATER,
  ICE_CAP,
  RAINFOREST,
  MONSOON,
  SEASFOREST,
  FOREST,
  SAVANNA,
  MEADOW,
  TAIGA,
  DESERT,
  WASTELAND,
  TUNDRA
};

class Country;
class River;
class Path;

struct Point {
  short x;
  short y;
  short height;
  short temperature;
  short moisture;
  short rainfall;
  short population;
  short residencies;
  short industries;
  short shops;
  short venues;
  short barracks;
  short hospitals;
  DEVELOPMENT development;
  ROUTE route;
  SURFACE surface;
  BIOME biome;
  Country* country;
  River* river;
  Path* path;
};

/**
  * From https://stackoverflow.com/questions/340943/c-multi-dimensional-arrays-on-the-heap, modified.
  */ 
class PointArray
{
    private:
        unsigned short width;
        unsigned short height;
        Point * point;
    public:
        Point& operator() (short x, short y);
        PointArray(const unsigned short w, const unsigned short h);
        ~PointArray();
        void reset(const unsigned short w, const unsigned short h);
        unsigned short getWidth();
        unsigned short getHeight();
};

#endif
