#ifndef WORLD_CPP
#define WORLD_CPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <functional>
#include <cstdint>
#include <iostream>
#include <vector>
#include <math.h>

#include "player.cpp"

#define WIDTH  750
#define HEIGHT 500

#define RAY_AMOUNT 250 /* Divisable with width*/
#define BRIGHTNESS 50
#define CHOPPYNESS 4
#define CHOPPY false

#define PI     3.14159265
#define SPEED  0.00000001

#define BOBB_SCALE 0.000001
#define TURN_SCALE 0.000000003

#define SCALE 30   
#define MAX_RGB 255
#define CLOSE 0
    

struct Vector2 { double x; double y;
  Vector2(double X, double Y){
    x = X;
    y = Y;
  }
};

struct Vector3 { int64_t x; int64_t y; int64_t z; };

struct ray
{
  char  character;
  char  transparent;
  
  double distance;
  double tdistance;

  short c_x;
  short c_y;
  
  void init_ray(char c, double n, char t = ' ', double tn = 0, short ch_x = 0, short ch_y = 0){
    character   = c;
    distance    = n;
    transparent = t;
    tdistance   = tn;
    c_x = ch_x;
    c_y = ch_y;
  }
};



struct Map
{
  std::vector<std::string> body;

  void add(std::string str) { body.push_back(str); } /* Append string   */
  uint16_t get_length(void) { return body.size();  } /* Get vector size */

  char get_item(uint16_t x, uint16_t y){ /* Get the x'th char in the y'th string */
    return (((x < get_width()) and (y < get_length())) ?
	    body.at(y).at(x) :           /* Get char */
	    'q');                        /* Out of bounds */
  }

  void change_item(uint32_t x, uint32_t y, char c){
    body.at(y).at(x) = c;
  }

  void print(void){ /* Print each row */
    for(unsigned int i = 0; i < body.size(); i++)
      std::cout << body.at(i) << std::endl;
  }
  
  uint16_t get_width(void){ /* Return longest string in body */
    uint16_t biggest = 0;  
    for (uint16_t i = 0; i < body.size(); i++)
      if (body.at(i).length() > biggest)
	biggest = body.at(i).length();

    return biggest;
  }
};

int fix(int n){ return ((n < 0) ? 0 : (n > 250) ? 250 : n);}
int colorval(double n){ return fix(fix((int)n) % 255); }
int go_up(double n) { return (unsigned int)(n); }
//int go_up(double v){ return v + (1 - abs(v - (unsigned int)v)) ;}

char player_block(Player p, Map m){ return m.get_item((int)p.x, (int)p.y); }

ray shoot_ray(Map *m, Player *p, uint16_t ray_n, double fov)
{ /* Shoot ray from player in provided direction 
   * and return what it hits first and the distance 
   */
  
  double   incr           = 0.05;        /* increment size / accuracy */
  uint16_t max_ray_length = 32;         /* max loop iterations */
  double   ray_length     = 0;          /* ray length          */ 
  
  double transparent_distance = 0.1;
  char   transparent_char     = ' ';
  
  bool toggled = false;
  
  /* Precalculations      */

  double additional = 0;               /* Used for emulating a non euclidean view */
  double angle =  (p->dir - p->fov / 2.0); /* angle of left side of players view      */
  angle += (p->fov / RAY_AMOUNT) * ray_n; /* get the angle of the ray_n'th ray       */

  double pos_x = cos(angle);
  double pos_y = sin(angle);
  uint32_t ray_x;
  uint32_t ray_y;
  char item;
  ray r;

  
  bool hit_obj = false;
  while (!hit_obj && (ray_length < max_ray_length))
    { 
      /* Calculate rays current position */
      ray_x = (p->x + (ray_length * pos_x));
      ray_y = (p->y + (ray_length * pos_y));

      /* Get block at current position */
      item = m->get_item(go_up(ray_x), go_up(ray_y)); /* Hit something? */

      /* Compare block and act accordingly */
      if ((item != ' ') and (item != 'O') and (item != 'p') and (item != 'o')){	 

	if (item == 'H') {
	  if (!toggled) {
	    pos_x   = cos(angle - PI / ((int)ray_length % 2 == 0 ? 1 : 2));
	    pos_y   = sin(angle - PI / ((int)ray_length % 2 == 0 ? 1 : 2));
	    toggled = true;
	  }
          /* additional += (ray_length - floor(ray_length) > 0.1) ? 0.1 : 0; */ 
	  /* additional += (ray_length / 10) * incr; */
	}else if ((item == 'w') and (ray_length <= 3)){	  
	}else if ((item == 'W') and (ray_length >= 3)){

	}else hit_obj = true;
      }    
      ray_length += incr;
  }
  r.init_ray(item, ray_length + additional, transparent_char, transparent_distance, go_up(ray_x), go_up(ray_y));
  return r;
}

void
ray_trace(Map *m, Player *p, ray *array_pointer)
{ /* Raytrace all rays and assign them into the main ray array */
  for (uint16_t current_ray = 0; current_ray < RAY_AMOUNT; current_ray++)
    *(array_pointer + current_ray) = shoot_ray(m, p, current_ray, p->fov);
}  

bool
multi_char_OR(char C1, short char_amount, char *C2)
{ /* Compare multiple chars against */
  for (short i = 0; i < char_amount; i++){
    if (C1 == C2[i]) return true;

  }return false;
}


void
handle_input(double *player_swing, double time_diff, sf::RenderWindow *window, Player *p, Map *m)
{
  sf::Event event;
  while (window->pollEvent(event))
    if (event.type == sf::Event::Closed) window->close();

  short array_size       = 5;
  char walkable_blocks[] = { ' ' , 'H', 'W', 'w', 'o'};


  
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  p->dir -= (TURN_SCALE * time_diff); /*  Turn left  */
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) p->dir += (TURN_SCALE * time_diff); /*  Turn right */
  
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) /* Walk straigt */
    if (multi_char_OR(m->get_item((unsigned int)(p->x + SPEED * cos(p->dir) * time_diff + CLOSE),
				  (unsigned int)(p->y + SPEED * sin(p->dir) * time_diff + CLOSE)),
			   array_size,
			   walkable_blocks)){
      

      p->x += SPEED * cos(p->dir) * time_diff;
      p->y += SPEED * sin(p->dir) * time_diff;
      *player_swing += PI / (BOBB_SCALE * time_diff);

      
    }
  
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) /* Walk backwards */
    if (multi_char_OR(m->get_item((unsigned int)(p->x - SPEED * cos(p->dir) * time_diff + CLOSE),
				  (unsigned int)(p->y - SPEED * sin(p->dir) * time_diff + CLOSE)),
			   array_size,
			   walkable_blocks)){

      p->x -= SPEED * cos(p->dir) * time_diff;
      p->y -= SPEED * sin(p->dir) * time_diff;
      *player_swing += PI / (BOBB_SCALE * time_diff);

    }
  
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) /* Walk left*/
    if (multi_char_OR(m->get_item((unsigned int)(p->x + SPEED * cos(p->dir - PI / 2) * time_diff  + CLOSE),
				  (unsigned int)(p->y + SPEED * sin(p->dir - PI / 2) * time_diff  + CLOSE)),
			   array_size,
			   walkable_blocks)){
	  
      p->y += SPEED * sin(p->dir - PI / 2) * time_diff;
      p->x += SPEED * cos(p->dir - PI / 2) * time_diff;

    }
  
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) /* Walk right */
    if (multi_char_OR(m->get_item((unsigned int)(p->x + SPEED * cos(p->dir + PI / 2) * time_diff  + CLOSE),
				  (unsigned int)(p->y + SPEED * sin(p->dir + PI / 2) * time_diff  + CLOSE)),
			   array_size,
			   walkable_blocks)){
	  
      p->y += SPEED * sin(p->dir + PI / 2) * time_diff;
      p->x += SPEED * cos(p->dir + PI / 2) * time_diff;

    }

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){
    ray r = shoot_ray(m, p, (short)(RAY_AMOUNT / 2), p->fov);
    if (r.character == 'c' and r.distance < 2){
      m->change_item(r.c_x, r.c_y, 'o');
    }else if (r.character == 'o' and r.distance < 2){
      m->change_item(r.c_x, r.c_y, 'c');
    }
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
    
    p->fov -= TURN_SCALE * time_diff;
  }else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
    p->fov += TURN_SCALE * time_diff;
  }

    
  
}




void
draw(double player_swing_set, short rect_width, ray *array_pointer, sf::RenderWindow *window, Player *p, Map *m, sf::Sprite *hands)
{

  /* Sky and floor O(HEIGHT) */
      for(uint16_t i = HEIGHT; i > 0; i--){
	if (i < HEIGHT / 2){
	  sf::RectangleShape rectangle(sf::Vector2f(WIDTH, ceil((HEIGHT / RAY_AMOUNT) / 2)));
	  rectangle.setPosition(0, i * ((HEIGHT / RAY_AMOUNT) / 2));
	  rectangle.setFillColor(sf::Color(255 - (unsigned int)fix((1 * i) + 100 % 255),
					   255 - (unsigned int)fix((1 * i) + 100 % 255),
					   0   + (unsigned int)fix((1 * i) + 0   % 255)));
	  window->draw(rectangle);
	}else {
	  sf::RectangleShape rectangle(sf::Vector2f(WIDTH, ceil((HEIGHT / RAY_AMOUNT) / 2)));
	  rectangle.setPosition(0, i * ((HEIGHT / RAY_AMOUNT) / 2));
	  rectangle.setFillColor(sf::Color((int)(1 * i) % 255, (int)(1 * i) % 255, (int) (1 * i) % 255));
	  window->draw(rectangle);
	}
      }


   
  /* draw everything O(RAY_AMOUNT) */
  for(uint16_t i = 0; i < RAY_AMOUNT; i++){
    ray r = *(array_pointer + i);

    /* Default rectangle */
    sf::RectangleShape rectangle(sf::Vector2f(rect_width, 2 * (HEIGHT / r.distance) + player_swing_set));
    rectangle.setPosition(i * rect_width, (HEIGHT - 2 * (HEIGHT / r.distance)) / 2 + player_swing_set);
	   
    switch (r.character)
      {

      case 't': /* Tall block ( fill up to the top of the window ) */
	if (true){
	  sf::RectangleShape trectangle(sf::Vector2f(rect_width, HEIGHT - (HEIGHT - 2 * (HEIGHT / r.distance)) / 2 + player_swing_set));
	  trectangle.setPosition(i * rect_width, 0);

	  trectangle.setFillColor(sf::Color(MAX_RGB - colorval(SCALE * r.distance),
					    MAX_RGB - colorval(SCALE * r.distance),
					    MAX_RGB - colorval(SCALE * r.distance)));
	  window->draw(trectangle);
	  rectangle.setFillColor(sf::Color(MAX_RGB - colorval(SCALE * r.distance),
					   MAX_RGB - colorval(SCALE * r.distance),
					   MAX_RGB - colorval(SCALE * r.distance)));
	}break;
      case 'c':
	rectangle.setFillColor(sf::Color(244 - r.distance,
					 164 - r.distance,
					 96 - r.distance));
	break;
	
      case 'r': /* Color r */
	rectangle.setFillColor(sf::Color(0,    
					 MAX_RGB - colorval(SCALE * r.distance),
					 MAX_RGB - colorval(SCALE * r.distance)));
	break;

      case 'g': /* color g */
	rectangle.setFillColor(sf::Color(MAX_RGB - colorval(SCALE * r.distance),
					 0 ,
					 MAX_RGB - colorval(SCALE * r.distance)));
	break;
	
      case 'b': /* color b */
	rectangle.setFillColor(sf::Color(MAX_RGB - colorval(SCALE * r.distance),
					 MAX_RGB - colorval(SCALE * r.distance),
					 0));
	break;
	    
      default: /* default color */
	rectangle.setFillColor(sf::Color(MAX_RGB - colorval(SCALE * r.distance),
					 MAX_RGB - colorval(SCALE * r.distance),
					 MAX_RGB - colorval(SCALE * r.distance)));
      }
    
    window->draw(rectangle);
  }

  sf::Sprite Dhands = *hands;
  sf::Vector2f pos = hands->getPosition();
  Dhands.setPosition(sf::Vector2f(pos.x, pos.y + 10 * player_swing_set));
  window->draw(Dhands);
}


#endif
