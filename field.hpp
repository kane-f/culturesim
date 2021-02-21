/**
 * OpenSimplex algorithm by Kurt Spencer and adapted from https://gist.github.com/KdotJPG/b1270127455a94ac5d19
 */

#ifndef FIELD_HPP
#define FIELD_HPP

#include "point.hpp"
#include "river.hpp"
#include "path.hpp"
#include "country.hpp"

class Field
{
public:
  
  /**
    * Constructor and destructor.
    */
  Field(sf::RenderWindow* window);
  ~Field();

  /** 
    * Function to generate an initial field.
    * @Param size: The size of the map.
    * @Param featureSize: The size of islands, mountains and seabeds, used in noise generation.
    * @Param maxHeight: The maximum range of height on the map.
    * @Param density: Average amount of people per square km.
    * @Param seaLevel: The offset of the height range on the map where sea covers land.
    * @Param riverFrequency: The amount of rivers generated.
    */
  void generateLevel(unsigned short size,
                     double featureSize,
                     short maxHeight,
                     short density,
                     short seaLevel,
                     unsigned short riverFrequency);

  /** 
    * Functions to load and save the field.
    * @Param name: The filename to load/save as.
    */
  void loadField(std::string name);
  void saveField(std::string name);

  /**
    * Clears all possible values on a field for the next use.
    */
  void clearField();

  /**
    * Advances time by a given amount in the game.
    * @Param amount Time passed
    * @Param gameState Game state to pass
    */
  void advanceTime(unsigned int amount, GAME_STATE gameState);

  /**
    * Assign the current country to play as in campaign mode.
    * @Return True if country found
    */
  bool initCurrentCountry();

  /**
    * Select a current country in simulation mode.
    * @Param x X location on field
    * @Param y Y location on field
    */
  void selectCurrentCountry(short x, short y);

  /**
    * Returns pointer to current country.
    */
  Country* getCurrentCountry();

  /**
    * Select a country (primary is countries doing actions, secondary is countries affected)
    * @Param x X location on field
    * @Param y Y location on field
    */
  void primarySelectCountry(short x, short y);
  void secondarySelectCountry(short x, short y);

  /**
    * Deselect all countries on field, primary and secondary
    */
  void deselectAllCountries();

  /**
    * Get all countries on field
    * @Return std::vector<Country*> The list of countries
    */
  std::vector<Country*> getCountries();

  /**
    * Checks each point in a square radius for development/route/surface/etc.
    * @Param x X location on field
    * @Param y Y location on field
    * @Returns Boolean indicating point found.
    * TODO: look into redundant functions with lambda, also possibly make radius circle.
    */
  bool checkNearbyTypes(short x, short y, DEVELOPMENT development, short radius = 1);
  bool checkNearbyTypes(short x, short y, ROUTE route, short radius = 1);
  bool checkNearbyTypes(short x, short y, SURFACE surface, short radius = 1);
  bool checkNearbyTypes(short x, short y, Country* country, short radius = 1);
  bool checkOppositeCountry(short x, short y, Country* country, short radius = 1);

  /**
    * Return world population from encapsulation.
    */
  unsigned long getWorldPopulation();

  /**
    * Return a day of the week based on the weekdays const string and a modulo number.
    */
  std::string getWeekDay();

  /**
    * Return a day, month or year based on m_day.
    */
  unsigned long getDay();
  unsigned long getMonth();
  unsigned long getYear();

  /**
    * Draw all values on field, depending on mode and game state.
    */
  bool drawField(DRAW_MODE drawMode, GAME_STATE gameState);

  /**
    * Encapsulated functions to move the field sprite.
    */
  void moveField(float offsetX, float offsetY);
  void scaleField(float factorX, float factorY);

  /**
    * Get a point on the field.
    */
  Point* getPoint(short x, short y);

private:

  void generateRivers(short iterations, short frequency);

  void updateMoistureAndTemperature();

  /**
    * Get newest high and low values while iterating through map.
    * @Param x X location on field
    * @Param y Y location on field
    */
  void updateExtremes(short x, short y);

  /**
    * Iterate through map and add up total of all population squares.
    */
  unsigned long updateWorldPopulation();

  /**
    * Update world populations.
    */
  void updatePopulation(short x, short y);

  /**
    * Functions for creating paths and building them over time.
    * @Param x X location on field
    * @Param y Y location on field
    */
  void createPath(short x, short y);
  void buildPaths();

  /**
    * Functions for creating countries and building them over time.
    * @Param x X location on field
    * @Param y Y location on field
    */
  void startCountry(short x, short y, GAME_STATE gameState);
  void updateCountries();

  /**
    * Function for filling points in with 5 or more surrounding equal point types
    */
  void fillCountryHoles();

  /**
    * Get height difference between two points anywhere.
    * @Param x1 First x value
    * @Param x2 Second x value
    * @Param y1 First y value
    * @Param y2 Second y value
    * @Returns The height difference
    */
  short getHeightDifference(short x1, short y1, short x2, short y2);

  /**
    * Reduces moisture when further from water source.
    * @Param x X location on field
    * @Param y Y location on field
    * @Returns Propogated moisture at point
    */
  short propogateMoisture(short x, short y);

  /**
    * Adjusts moisture based on temperature and height.
    * @Param x X location on field
    * @Param y Y location on field
    * @Returns Adjusted moisture at point
    */
  short adjustMoisture(short x, short y);
  
  /**
    * Gets temperature at location based on height, latitude and moisture.
    * @Param x X location on field
    * @Param y Y location on field
    * @Returns Temperature at point
    */
  short getTemperature(short x, short y);


  /**
    * Gets biome at location based on temperature.
    * @Param x X location on field
    * @Param y Y location on field
    * @Returns Biome at point
    */
  BIOME getBiome(short x, short y);

  /**
    * Draws one of two dithered colours on a field depending on point value.
    * @Param x X location on field
    * @Param y Y location on field
    * @Param c1 The first colour used
    * @Param c2 The second colour used, optional otherwise leave as is
    */
  void drawDither(short x, short y, sf::Color c1);
  void drawDither(short x, short y, sf::Color c1, sf::Color c2);

  /**
    * Recursive function evaluating simplex noise to create realistic terrain
    */
  short recurseEvaluate(double x, double y, int size, int height, double divisor, int result = 0);

  /**
    * OpenSimplex algorithm functions by Kurt Spencer and adapted from
    * https://gist.github.com/KdotJPG/b1270127455a94ac5d19
    */
  short evaluateSimplexPoint(double x, double y, int height);
  double extrapolateSimplexPoint(int xsb, int ysb, double dx, double dy);

  void drawContours(sf::VertexArray* fieldPoints, short minHeight, short maxHeight, short step);

  // Statistics for world.
  short m_highestPoint;
  short m_lowestPoint;
  unsigned long m_worldPopulation;
  unsigned long m_day;

  // Width and height value of map.
  short m_mapWidth;
  short m_mapHeight;

  // Items in level.
  std::vector<River> m_rivers;
  std::vector<Path> m_pathsInProgress;
  std::vector<Path> m_pathsComplete;
  std::vector<Country*> m_countries;

  // Pointer to current country
  Country* m_currentCountry;

  // The main application window.
	sf::RenderWindow& m_window;

  // Values for drawing fields.
  PointArray* m_field;
  sf::VertexArray m_fieldPoints;
  sf::RenderTexture m_fieldTexture;
  sf::Sprite m_fieldSprite;

  // OpenSimplex noise constants.
  const double STRETCH_CONSTANT_2D = -0.211324865405187; //(1/Math.sqrt(2+1)-1)/2;
  const double SQUISH_CONSTANT_2D = 0.366025403784439; //(Math.sqrt(2+1)-1)/2;
  const double NORM_CONSTANT_2D = 47;

  // Gradients for 2D. They approximate the directions to the vertices of an octagon from the center.
	const char gradients2D[16] = {
		 5,  2,    2,  5,
		-5,  2,   -2,  5,
		 5, -2,    2, -5,
		-5, -2,   -2, -5,
	};

  // Days of the week for strings.
  const std::string m_days[7] = {"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};

  // Lengths of months
  const int8_t m_months[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
  int8_t m_monthNumber;

  // Colors of biomes to iterate through.
  const sf::Color m_biomeColors[14] = {sf::Color(0x00,0x00,0xFF),
                                       sf::Color(0x00,0x80,0xFF),
                                       sf::Color(0x80,0x80,0xFF),
                                       sf::Color(0xFF,0xFF,0xFF),

                                       sf::Color(0x80,0x80,0x00),
                                       sf::Color(0x80,0xFF,0x00),

                                       sf::Color(0x40,0x80,0x00),
                                       sf::Color(0x00,0x80,0x00),

                                       sf::Color(0x80,0xFF,0x40),
                                       sf::Color(0x00,0xFF,0x00),
                                       sf::Color(0x40,0xC0,0x80),

                                       sf::Color(0xFF,0xFF,0x00),
                                       sf::Color(0xC0,0xC0,0x40),
                                       sf::Color(0x80,0xFF,0xFF)};

  // Colors of populations to iterate through.
  const sf::Color m_popColors[4] = {sf::Color(0x80,0x00,0x00),
                                    sf::Color(0xFF,0x00,0x00),
                                    sf::Color(0xFF,0x80,0x00),
                                    sf::Color(0xFF,0xFF,0x00)};

  // Perm array for opensimplex vectors.
  short perm[256];
};

#endif
