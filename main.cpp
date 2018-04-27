/* SFML */
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

/* OTHER LIBS */
#include <functional>
#include <cstdint>
#include <iostream>
#include <vector> 
#include <unistd.h> 
#include <thread>
#include <chrono>

/* LOCAL FILES */
#include "world.cpp"
#include "player.cpp"
  
int main()
{  
  sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "MathMagics");

  /* PLAYER HEAD BOBBING AND HAND BOBBING */
  double player_swing     = 0;
  double player_swing_set = 0; 

  Map    m;       
  Player p;

  /* 360 DEG IS EQUAL TO 2 * PI */
  p.dir = PI / 4.0;
  p.x   = 3;
  p.y   = 3;  
  p.fov = 2; 
     
  m.add("#ggggggggggg####rgbrgb################rrrrr#############");
  m.add("g          #   #      #   c   #      #     #      #    #");
  m.add("g          c   c      #   #   #      #     #      #    #");
  m.add("g          #   #      #   r   ###c####     c      #c####");
  m.add("g          #   #      c   #          W     #      c    #");
  m.add("#####c##############c############c####     ######c######"); 
  m.add("#              #       #      #      #     #           #");
  m.add("#              #       #      #      c     c    t  t   #");
  m.add("#              #       #      #      #     #           #");
  m.add("#              #######c#      ##c######www###g#r#c#r#g##");
  m.add("# t  t  t  t   #       ##    ##            #           #");
  m.add("#              c       W      # t  t  t  t #           #");
  m.add("# t  t  t  t   #       #                   ######c######");
  m.add("#              ################ t  t  t  t #           #"); 
  m.add("# t  t  t  t   #t t t t t t t #            #           #");
  m.add("#                                          ######c######");
  m.add("# t  t  t  t                               #           #"); 
  m.add("#              #t t t t t t t #            #           #");
  m.add("################################           ######c######"); 
  m.add("#tttttttttttttttttttttttttttttt#           c           #");
  m.add("#tt   t   t   t   t   t   t   t#           #           #");
  m.add("#t  t   t   t   t   t   t   t              # t t t t t #");
  m.add("#tttttttttttttttttttttttttttttt#           W           #");
  m.add("########################################################");      

  /* WIDTH OF VERTICAL DRAW LINES */
  int rect_width = WIDTH / RAY_AMOUNT; 

  /* THE RAYTRACED RAYS */
  ray ray_array[RAY_AMOUNT];           
  
  bool shooting = false;

  /* DELTA TIME */
  auto time_start   =   std::chrono::steady_clock::now();
  auto time_end     =   std::chrono::steady_clock::now();
  auto time_diff_ms =   (time_end - time_start).count(); 

  
  sf::Texture Thands;
  sf::Texture Tshands;

  /* LOAD TEXTURES */
  if (!Thands.loadFromFile("img/hands.png")){printf("ERROR: file loading");return 0;}
  if (!Tshands.loadFromFile("img/hands_lambda.png")){printf("ERROR: file loading"); return 0;}

  /* CREATE SPRITE USING LOADED TEXTURES */
  sf::Sprite hands(Thands);
  sf::Sprite shands(Tshands);

  /* INITIALIZE BOTH HANDS */
  hands.setPosition(sf::Vector2f(WIDTH / 2 - 250, HEIGHT - 200));
  hands.setScale(sf::Vector2f(2.5, 2.5));
  shands.setPosition(hands.getPosition());  
  shands.setScale(hands.getScale()); 
  
  while (window.isOpen())   
    {
      time_start = std::chrono::steady_clock::now(); 

      /* CALCULATE ACTUAL BOBBING */
      player_swing_set = sin(player_swing) * 6.5;
      if(player_swing >= PI) player_swing = 0;

      /* RAYTRACE */
      ray_trace(&m, &p, ray_array);

      /* HANDLE USER INPUT */
      handle_input(&player_swing, (double)time_diff_ms, &window, &p, &m, &shooting);      

      /* DRAW EVERYTHING */
      draw(player_swing_set, rect_width, ray_array, &window, &p, &m, &hands, &shands,(double)time_diff_ms, &shooting);
  
      /* CLEAR SCREEN */
      window.display();   
      window.clear(sf::Color::Black);  

      /* CALCULATE TIME DIFFERENCE */
      time_end     = std::chrono::steady_clock::now();
      time_diff_ms =  (time_end - time_start).count();
         
    }    
      return 0; 
}
   
 
 
 
 
 
