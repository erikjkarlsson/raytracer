#ifndef PLAYER_CPP
#define PLAYER_CPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <functional>
#include <cstdint>
#include <iostream>
#include <vector>

#include "world.cpp"

struct Player
{
  double  x    = 0;
  double  y    = 0;
  double  dir  = 0;
  double  fov  = 3.14 / 4.0;
};

struct sprite_128C{};
  
struct sprite_64C
{
  char Xcolor[64];
  char Ycolor[64];

  double x;
  double y;
};

#endif
