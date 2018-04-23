#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
     
#include <functional>
#include <cstdint>
#include <iostream>
#include <vector> 
#include <unistd.h>
#include <thread>
#include <chrono>

#include "world.cpp"
#include "player.cpp"
  
int main()
{  
 
  sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "ray_tracer_test");
  
  double player_swing     = 0;
  double player_swing_set = 0; 
          
  Player p;    
  p.dir = PI / 4.0;
  p.x   = 3;
  p.y   = 3;  
  p.fov = (2 * PI) / PI; 
    
  Map m;       
     
  m.add("########################################################");
  m.add("#          #                                           #");
  m.add("#          o                            rrrrrrr        #");
  m.add("#          o                            r     r        #");
  m.add("#          #                            r     r        #");
  m.add("####cc######                            rrrrrrr        #"); 
  m.add("#                                                      #");
  m.add("#                                       ggggggg        #");
  m.add("#                                       g     g        #");
  m.add("#                                       g     g        #");
  m.add("#                                       ggggggg        #");
  m.add("#                                                      #");
  m.add("#   #    #                                             #");
  m.add("#   #    #                              bbbbbbb        #"); 
  m.add("#   #    #                              b     b        #");
  m.add("#   ######                              bbbbbbb        #");
  m.add("#                                                      #");
  m.add("#   wwwwwww     WWWWWWW     HHHHHHH     ttttttt        #");
  m.add("#   w     w     W     W     H     H     t     t        #"); 
  m.add("#   w     w     W     W     H     H     t     t        #");
  m.add("#   wwwwwww     WWWWWWW     HHHHHHH     ttttttt        #");
  m.add("#                                                      #");
  m.add("#                                                      #");
  m.add("########################################################");      
 
  int rect_width = WIDTH / RAY_AMOUNT;
  ray ray_array[RAY_AMOUNT];  
   
   
  auto time_start   =   std::chrono::steady_clock::now();
  auto time_end     =   std::chrono::steady_clock::now();
  auto time_diff_ms =   (time_end - time_start).count(); 


  sf::Texture Thands;
  if (!Thands.loadFromFile("hands.png"))
    {
      std::cout << "Cant load texture" << std::endl;
      return 0;
    }
  sf::Sprite hands(Thands);
  hands.setPosition(sf::Vector2f(WIDTH / 2 - 250, HEIGHT - 200));
  hands.setScale(sf::Vector2f(2.5, 2.5));

  while (window.isOpen())   
    {
      time_start = std::chrono::steady_clock::now(); 
       
      player_swing_set = sin(player_swing) * 6.5;
      if(player_swing >= PI) player_swing = 0;
   
      ray_trace(&m, &p, ray_array);   
            
      handle_input(&player_swing, (double)time_diff_ms, &window, &p, &m);      
      draw(player_swing_set, rect_width, ray_array, &window, &p, &m, &hands);

 
      window.display();   
      window.clear(sf::Color::Black);

       
      time_end     = std::chrono::steady_clock::now();
      time_diff_ms =  (time_end - time_start).count();
         
    }    
      return 0;
}
   
 
 
 
 
