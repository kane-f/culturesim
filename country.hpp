#ifndef COUNTRY_HPP
#define COUNTRY_HPP

#include "area.hpp"

class Country: public Area
{
  public:
    using Area::Area;
    /**
      * Constructor and destructor for the country, inherited from Area.
      */
    Country(Point* point, PointArray* field, std::string name, bool visible);
    ~Country();

    /**
      * Function to decrease population in a country, mainly when it loses area.
      * @Param amount The amount to decrease by.
      */
    void decreasePopulation(unsigned long amount);

    /**
      * Subfunctions for doing actions in each part of the update function.
      * @Param point The point to check in.
      * @Return True if exited with success, otherwise skipped.
      */
    bool explore(Point* pnt);
    void trade(Point* pnt);
    void harvest(Point* pnt);
    void process(Point* pnt);

    /**
      * Main update country function, called within update loops for each one.
      */
    void updateCountry();

    /**
      * Merge this country with another.
      * @Param country The country to merge.
      */
    void mergeCountry(Country* mergedCountry);

    /**
      * Gets color from encapsulation and color half of this in brightness.
      */
    sf::Color getColor();
    sf::Color getHalfColor();

    /**
      * Gets resources from encapsulation.
      */
    int getCrops();
    int getOre();
    int getProcessed();

    /**
      * Public functions allowing other countries to give them their resources in trade.
      * @Param point The point to check in.
      */
    void giveCrops(int amount);
    void giveOre(int amount);
    void giveProcessed(int amount);

    /**
      * Sets vassal owner to a country, used by another.
      * @Param point The point to check in.
      */
    void setVassalOwner(Country* owner);

    /**
      * Gets vassal owner, if any.
      */
    Country* getVassalOwner();

    /**
      * Gets list of allies and enemies for external modification.
      */
    std::set<Country*> getAllies();
    std::set<Country*> getEnemies();

    /**
      * Functions allowing safe ease of adding and removing allies/enemies from lists, with checks.
      * @Param ally The ally to add.
      * @Param enemy The enemy to add/remove.
      */
    void addAlly(Country* ally);
    void removeAlly(Country* ally);
    void addEnemy(Country* enemy);
    void removeEnemy(Country* enemy);

    /**
      * Function to toggle allowing capture of unclaimed land on and off.
      */
    void toggleCaptureUnclaimed();

    /**
      * Function to toggle allowing trade with allies on and off.
      */
    void toggleTrade();

    /**
      * Functions to toggle obtaining minerals.
      */
    void toggleHarvestCrops();
    void toggleMineOre();
    void toggleProcessGoods();

    /**
      * Gets if country can capture unclaimed land.
      */
    bool canCaptureUnclaimed();

    /**
      * Gets if country can trade with allies.
      */
    bool canTrade();

    /**
      * Gets if country can obtain minerals.
      */
    bool canHarvestCrops();
    bool canMineOre();
    bool canProcessGoods();
  private:
    /** Three types of resources the country uses; 
      * crops from moisture, ore from the ground, (both in farmlands) processed manufactured in urban centres.
      * To be expanded upon later.
      */
    int m_crops;
    int m_ore;
    int m_processed;

    // List of countries allied to and at war with country.
    std::set<Country*> m_allies;
    std::set<Country*> m_enemies;

    // Color used to display the country
    const sf::Color m_color =
      sf::Color(((rand() % 0x10) * 8) + 0x40,((rand() % 0x10) * 8) + 0x40,((rand() % 0x10) * 8) + 0x40);
    const sf::Color m_hColor = sf::Color(m_color.r/2,m_color.g/2,m_color.b/2);

    // Pointer to country that owns current country, if any
    Country* m_vassalOwner;

    // Boolean to decide if country should capture unclaimed land
    bool m_captureUnclaimed;

    // Boolean to decide if country should trade
    bool m_trade;

    // BooleanS to decide if country should obtain minerals
    bool m_harvestCrops;
    bool m_mineOre;
    bool m_processGoods;
};

#endif
