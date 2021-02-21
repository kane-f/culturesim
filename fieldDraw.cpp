#include "field.hpp" 

bool Field::drawField(DRAW_MODE drawMode, GAME_STATE gameState)
{
  for (short x = 0; x < m_mapWidth; ++x)
  for (short y = 0; y < m_mapHeight; ++y)
  {
    if((*m_field)(x,y).surface == SURFACE::SALTWATER)
      drawDither(x,y,sf::Color(0x00,0x80,0x80),sf::Color(0x00,0x00,0x80));

    if ((*m_field)(x,y).surface == SURFACE::ICECAP)
      m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0xFF,0xFF,0xFF);

    if ((*m_field)(x,y).route == ROUTE::BRIDGE) m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0x00,0x00,0xFF);

    switch(drawMode)
    {
      case DRAW_MODE::BIOME:
        for (short i = 0; i < sizeof(m_biomeColors); ++i)
          if ((*m_field)(x,y).biome == BIOME(i)) m_fieldPoints[x+(m_mapHeight*y)].color = m_biomeColors[i];
        break;
      case DRAW_MODE::HEIGHT:
        if((*m_field)(x,y).surface == SURFACE::SALTWATER)
        {
          if ((*m_field)(x,y).height < -8000) m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0x00,0x00,0xFF);
          else if ((*m_field)(x,y).height < -7000) drawDither(x,y,sf::Color(0x00,0x00,0xFF),sf::Color(0x00,0x40,0xFF));
          else if ((*m_field)(x,y).height < -6000) m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0x00,0x40,0xFF);
          else if ((*m_field)(x,y).height < -5000) drawDither(x,y,sf::Color(0x00,0x40,0xFF),sf::Color(0x00,0x80,0xFF));
          else if ((*m_field)(x,y).height < -4000) m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0x00,0x80,0xFF);
          else if ((*m_field)(x,y).height < -3000) drawDither(x,y,sf::Color(0x00,0x80,0xFF),sf::Color(0x00,0xC0,0xFF));
          else if ((*m_field)(x,y).height < -2000) m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0x00,0xC0,0xFF);
          else if ((*m_field)(x,y).height < -1500) drawDither(x,y,sf::Color(0x00,0xC0,0xFF),sf::Color(0x00,0xFF,0xFF));
          else if ((*m_field)(x,y).height < -1000) m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0x00,0xFF,0xFF);
          else if ((*m_field)(x,y).height < -500) drawDither(x,y,sf::Color(0x00,0xFF,0xFF),sf::Color(0x80,0xFF,0xFF));
          else m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0x80,0xFF,0xFF);
        }
        else if((*m_field)(x,y).surface == SURFACE::LAND)
        {
          if ((*m_field)(x,y).height > 8000) m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0xFF,0xFF,0xFF);
          else if ((*m_field)(x,y).height > 7000) drawDither(x,y,sf::Color(0xFF,0xFF,0xFF),sf::Color(0xC0,0xC0,0xC0));
          else if ((*m_field)(x,y).height > 6000) m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0xC0,0xC0,0xC0);
          else if ((*m_field)(x,y).height > 5000) drawDither(x,y,sf::Color(0xC0,0xC0,0xC0),sf::Color(0xA0,0xA0,0xA0));
          else if ((*m_field)(x,y).height > 4000) m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0xA0,0xA0,0xA0);
          else if ((*m_field)(x,y).height > 3000) drawDither(x,y,sf::Color(0xA0,0xA0,0xA0),sf::Color(0x80,0x80,0x80));
          else if ((*m_field)(x,y).height > 2000) m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0x80,0x80,0x80);
          else if ((*m_field)(x,y).height > 1500) drawDither(x,y,sf::Color(0x80,0x80,0x80),sf::Color(0xC0,0x80,0x80));
          else if ((*m_field)(x,y).height > 1000) m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0xC0,0x80,0x80);
          else if ((*m_field)(x,y).height > 500) drawDither(x,y,sf::Color(0xC0,0x80,0x80),sf::Color(0xF0,0xC0,0x80));
          else if ((*m_field)(x,y).height > 0) m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0xF0,0xC0,0x80);
          else m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0xF0,0xC0,0xC0);
        }
        break;
      case DRAW_MODE::POPULATION:
        if((*m_field)(x,y).surface != SURFACE::SALTWATER)
        {
          if ((*m_field)(x,y).route == ROUTE::BRIDGE) m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0x00,0x00,0xFF);

          // Scope to keep population iterator within this loop
          {
            short popIter = 100;
            for (sf::Color popColor : m_popColors)
            {
              if ((*m_field)(x,y).population >= popIter) m_fieldPoints[x+(m_mapHeight*y)].color = popColor;

              if (popIter % 2 == 0) popIter *= 5;
              else popIter *= 2;
            }
          }

          if ((*m_field)(x,y).population < 100) m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0x00,0x00,0x00);
        }
        break;
      case DRAW_MODE::TEMPERATURE:
        if((*m_field)(x,y).surface == SURFACE::LAND)
        {
          if ((*m_field)(x,y).temperature >= 40) m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0xFF,0x00,0x00);
          else if ((*m_field)(x,y).temperature >= 35) drawDither(x,y,sf::Color(0xFF,0x00,0x00),sf::Color(0xFF,0x80,0x00));
          else if ((*m_field)(x,y).temperature >= 30) m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0xFF,0x80,0x00);
          else if ((*m_field)(x,y).temperature >= 25) drawDither(x,y,sf::Color(0xFF,0x80,0x00),sf::Color(0xFF,0xFF,0x00));
          else if ((*m_field)(x,y).temperature >= 20) m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0xFF,0xFF,0x00);
          else if ((*m_field)(x,y).temperature >= 15) drawDither(x,y,sf::Color(0xFF,0xFF,0x00),sf::Color(0x00,0xFF,0x00));
          else if ((*m_field)(x,y).temperature >= 10) m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0x00,0xFF,0x00);
          else if ((*m_field)(x,y).temperature >= 5) drawDither(x,y,sf::Color(0x00,0xFF,0x00),sf::Color(0x00,0xFF,0x80));
          else if ((*m_field)(x,y).temperature >= 0) m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0x00,0xFF,0x80);
          else if ((*m_field)(x,y).temperature > -10) drawDither(x,y,sf::Color(0x00,0xFF,0x80),sf::Color(0x00,0xFF,0xFF));
          else if ((*m_field)(x,y).temperature > -20) m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0x00,0xFF,0xFF);
          else if ((*m_field)(x,y).temperature > -30) drawDither(x,y,sf::Color(0x00,0xFF,0xFF),sf::Color(0x80,0xFF,0xFF));
          else if ((*m_field)(x,y).temperature > -40) m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0x80,0xFF,0xFF);
          else if ((*m_field)(x,y).temperature > -50) drawDither(x,y,sf::Color(0x80,0xFF,0xFF),sf::Color(0xFF,0xFF,0xFF));
          else m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0xFF,0xFF,0xFF);
        }
        break;
      case DRAW_MODE::MOISTURE:
        if((*m_field)(x,y).surface == SURFACE::LAND)
        {
          if ((*m_field)(x,y).moisture >= 5000) m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0x00,0x00,0x80);
          else if ((*m_field)(x,y).moisture >= 4000) drawDither(x,y,sf::Color(0x00,0x00,0x80),sf::Color(0x00,0x00,0xFF));
          else if ((*m_field)(x,y).moisture >= 3000) m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0x00,0x00,0xFF);
          else if ((*m_field)(x,y).moisture >= 2000) drawDither(x,y,sf::Color(0x00,0x00,0xFF),sf::Color(0x00,0x80,0xFF));
          else if ((*m_field)(x,y).moisture >= 1000) m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0x00,0x80,0xFF);
          else if ((*m_field)(x,y).moisture >= 500) drawDither(x,y,sf::Color(0x00,0x80,0xFF),sf::Color(0x00,0xFF,0xFF));
          else if ((*m_field)(x,y).moisture >= 400) m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0x00,0xFF,0xFF);
          else if ((*m_field)(x,y).moisture >= 300) drawDither(x,y,sf::Color(0x00,0xFF,0xFF),sf::Color(0x80,0xFF,0xFF));
          else if ((*m_field)(x,y).moisture >= 200) m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0x80,0xFF,0xFF);
          else if ((*m_field)(x,y).moisture >= 100) drawDither(x,y,sf::Color(0x80,0xFF,0xFF),sf::Color(0xFF,0xFF,0xFF));
          else m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0xFF,0xFF,0xFF);
        }
        break;
      case DRAW_MODE::URBANISATION:
        if((*m_field)(x,y).surface == SURFACE::SALTWATER ||
           (*m_field)(x,y).surface == SURFACE::ICECAP) drawDither(x,y,sf::Color(0x00,0x80,0x80),sf::Color(0x00,0x00,0x80));
        else if((*m_field)(x,y).route == ROUTE::BRIDGE)
          m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0x80,0x00,0x00);
        else if((*m_field)(x,y).route == ROUTE::PATH)
          m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0xFF,0x00,0x00);
        else if((*m_field)(x,y).development == DEVELOPMENT::URBAN)
          m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0x80,0x80,0x80);
        else if((*m_field)(x,y).development == DEVELOPMENT::FARM)
          m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0x00,0xFF,0x00);
        else drawDither(x,y,sf::Color(0xC0,0xC0,0xC0),sf::Color(0xFF,0xFF,0xFF));
        break;
      case DRAW_MODE::COUNTRY:
        if((*m_field)(x,y).surface == SURFACE::SALTWATER ||
           (*m_field)(x,y).surface == SURFACE::ICECAP)
        {
          drawDither(x,y,sf::Color(0x00,0x80,0x80),sf::Color(0x00,0x00,0x80));
        }
        else if((*m_field)(x,y).country != NULL)
        {
          if ((*m_field)(x,y).country->getPrimarySelected())
            m_fieldPoints[(x)+(m_mapHeight*(y))].color = sf::Color(0xFF,0xFF,0x80);
          else if ((*m_field)(x,y).country->getSecondarySelected())
            m_fieldPoints[(x)+(m_mapHeight*(y))].color = sf::Color(0x80,0xFF,0xFF);
          else m_fieldPoints[x+(m_mapHeight*y)].color = (*m_field)(x,y).country->getColor();
        }
        else
        {
          drawDither(x,y,sf::Color(0xC0,0xC0,0xC0),sf::Color(0x80,0x80,0x80));
        }

        for (short i = -1; i <= 1; ++i)
        for (short j = -1; j <= 1; ++j) {
          if(i == 0 && j == 0) continue;
          if(x + i >= 0 && y + j >= 0 && x + i < m_mapWidth && y + j < m_mapHeight)
          if((*m_field)(x+i,y+j).country != (*m_field)(x,y).country) {
            if ((*m_field)(x,y).country != NULL) {
              if ((*m_field)(x+i,y+j).country != NULL && (*m_field)(x+i,y+j).country->getPrimarySelected())
                m_fieldPoints[(x+i)+(m_mapHeight*(y+j))].color = sf::Color(0xC0,0xC0,0x00);
              else if ((*m_field)(x+i,y+j).country != NULL && (*m_field)(x+i,y+j).country->getSecondarySelected())
                m_fieldPoints[(x+i)+(m_mapHeight*(y+j))].color = sf::Color(0x00,0xC0,0xC0);
              else if (checkNearbyTypes(x+i,y+j,SURFACE::SALTWATER))
                m_fieldPoints[(x+i)+(m_mapHeight*(y+j))].color = sf::Color(0x00,0x00,0x00);
              else m_fieldPoints[(x+i)+(m_mapHeight*(y+j))].color = (*m_field)(x,y).country->getHalfColor();
            }
          }
        }
        break;
      case DRAW_MODE::INFRASTRUCTURE:
        if((*m_field)(x,y).surface == SURFACE::LAND)
        {
          m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0x00,0x00,0x00);
        }
        break;
    }

    if ((*m_field)(x,y).surface == SURFACE::SALTWATER &&
        (checkNearbyTypes(x,y,SURFACE::LAND) ||
         checkNearbyTypes(x,y,SURFACE::ICECAP)))
      m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0x00,0x00,0x00);

    if ((*m_field)(x,y).route == ROUTE::GLACIER)
      m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0xFF,0xFF,0xFF);

    if ((*m_field)(x,y).route == ROUTE::RIVER)
      m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0x00,0x00,0xFF);
    
    // If in campaign mode and country is not current country
    if (gameState == GAME_STATE::CAMPAIGN && (*m_field)(x,y).country != m_currentCountry)
      // If not in country view mode and point is country border draw blank
      if (drawMode != DRAW_MODE::COUNTRY && checkNearbyTypes(x,y,m_currentCountry))
        m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0x00,0x00,0x00);
      // If country is visible or point is within 10km of country, draw dither
      else if (((*m_field)(x,y).country != NULL && (*m_field)(x,y).country->getVisible()) ||
          (checkNearbyTypes(x,y,m_currentCountry,5)))
        drawDither(x,y,sf::Color(0x00,0x00,0x00));
      // Otherwise draw blank
      else m_fieldPoints[x+(m_mapHeight*y)].color = sf::Color(0x00,0x00,0x00);
  }
  
  if (!m_fieldTexture.create(m_mapWidth, m_mapHeight)) return false;

  m_fieldTexture.clear();
  m_fieldTexture.draw(m_fieldPoints);
  m_fieldTexture.display();

  m_fieldSprite.setTexture(m_fieldTexture.getTexture());
  m_window.draw(m_fieldSprite);

  return true;
}

void Field::drawDither(short x, short y, sf::Color c1)
{
  // If point is odd number, make colour a.
  if (x % 2 == 1 && y % 2 == 1) m_fieldPoints[x+(m_mapHeight*y)].color = c1;
}

void Field::drawDither(short x, short y, sf::Color c1, sf::Color c2)
{
  // If point is odd number, make colour a.
  if (x % 2 == 1 && y % 2 == 1) m_fieldPoints[x+(m_mapHeight*y)].color = c1;
  // Otherwise, colour b.
  else m_fieldPoints[x+(m_mapHeight*y)].color = c2;
}

void Field::drawContours(sf::VertexArray* fieldPoints, short minHeight, short maxHeight, short step)
{
  for (short workHeight = minHeight + step; workHeight < maxHeight; workHeight += step)
  for (short x = 0; x < m_mapWidth; ++x)
  for (short y = 0; y < m_mapHeight; ++y)
  {
    if ((*m_field)(x,y).height < workHeight) {
      //TODO: also check if nearby is higher than workheight while lower
      //m_fieldPoints[x+(m_mapHeight*y)]->color = sf::Color(0x00,0x00,0x00);
    }
  }
}