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
/* CONSTANTS*/
#define WIDTH  750
#define HEIGHT 500

/* SHOULD BE DIVISABLE WITH WIDTH TO AVOID RENDER ERRORS */
#define RAY_AMOUNT 250 

#define BRIGHTNESS 50
#define CHOPPYNESS 4
#define CHOPPY false
#define PI     3.14159265
#define SPEED  0.000000005
#define BOBB_SCALE 0.000001
#define TURN_SCALE 0.000000003
#define SCALE 30   
#define MAX_RGB 255
#define CLOSE 0
#define HAND_HEIGHT 80

/* UNUSED VECTOR CLASS */
struct Vector2 { double x; double y;
  Vector2(double X, double Y){
    x = X;
    y = Y;
  }
};

/* UNUSED VECTOR CLASS */
struct Vector3 { int64_t x; int64_t y; int64_t z; };

/* THE RAY THAT WILL BE SHOT IN THE RAYTRACER */
struct ray
{
  /* THE ACTUAL CHARS THAT WAS HIS */
  char  character;
  char  transparent;

  /* DISTANCES */
  double distance;
  double tdistance;

  /* CHAR POSITION */
  short c_x;
  short c_y;
  
  void init_ray(char c,
		double n,
		char t     = ' ',
		double tn  = 0,
		short ch_x = 0,
		short ch_y = 0){
    
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

  void add(std::string str) { body.push_back(str); } /* APPEND A STRING       */
  uint16_t get_length(void) { return body.size();  } /* GET THE LENGTH OF MAP */

  char get_item(uint16_t x, uint16_t y){ /* GET THE X'TH CHAR IN THE Y'TH STRING */
    return (((x < get_width()) and (y < get_length())) ?
	    body.at(y).at(x) :           /* GET CHAR       */
	    'q');                        /* OUT OF BOUNDS  */
  }

  void change_item(uint32_t x, uint32_t y, char c){
    body.at(y).at(x) = c;
  }

  void print(void){ /* PRINT EACH ROW  */
    for(unsigned int i = 0; i < body.size(); i++)
      std::cout << body.at(i) << std::endl;
  }
  
  uint16_t get_width(void){ /* RETURN LONGEST STRING IN STRING ARRAY */
    uint16_t biggest = 0;  
    for (uint16_t i = 0; i < body.size(); i++)
      if (body.at(i).length() > biggest)
	biggest = body.at(i).length();

    return biggest;
  }
};

int fix(int n){ return ((n < 0) ? 0 : (n > 250) ? 250 : n);} /* SHADING */
int colorval(double n){ return fix(fix((int)n) % 255);} 
int go_up(double n) { return (unsigned int)(n); } /* ROUND UP */


char player_block(Player p, Map m){ return m.get_item((int)p.x, (int)p.y); }

ray shoot_ray(Map *m, Player *p, uint16_t ray_n, double fov)
{ /* SHOOT RAY FROM PLAYERS POSITION IN A POSITION
   * RETURN THE FIRST SOLID COLORED BLOCK THAT IT HITS
   * AND KEEP COUNT OF THE NON SOLID BLOCKS
   */
  
  double   incr           = 0.05; /* INCREMENT SIZE / ACCURACY */
  uint16_t max_ray_length = 32;   /* MAX LOOP ITERATIONS       */
  double   ray_length     = 0;    /* RAY LENGTH                */ 
  
  double transparent_distance = 0.1;
  char   transparent_char     = ' ';
  
  bool toggled = false;
  
  /* PRECALCULATIONS */

  double additional = 0;                   /* FOR EMULATION OF NON EUCLIDEAN GEOMETRY   */
  double angle =  (p->dir - p->fov / 2.0); /* ANGLE OF THE LEFTEST SIDE OF PLAYERS FOV  */
  angle += (p->fov / RAY_AMOUNT) * ray_n;  /* GET THE ray_n'TH RAY ANGLE (THE ONE USED) */

  double pos_x = cos(angle);
  double pos_y = sin(angle);
  uint32_t ray_x;
  uint32_t ray_y;
  char item;
  ray r;

  
  bool hit_obj = false;
  while (!hit_obj && (ray_length < max_ray_length))
    { 
      /* CURRENT POSITION IN CORDINATE SYSTEM  */
      ray_x = (p->x + (ray_length * pos_x));
      ray_y = (p->y + (ray_length * pos_y));

      /* GET THE BLOCK IN THE MAP FOR THE CURRENT POSITION  
       * IF THIS IS A TRANSPARENT BLOCK OR OUTSIDE OF BOUNDS
       * IT WILL BE IGNORED
       */
      item = m->get_item(go_up(ray_x), go_up(ray_y)); 

      /* COMPARE BLOCKS AND ACT ACCORDINGLY */
      if ((item != ' ') and
	  (item != 'O') and
	  (item != 'p') and
	  (item != 'o')){	 

	/* NON EUCLIDEAN GEOMETRY (FOR FUN) */
	if (item == 'H') {
	  if (!toggled) {
	    pos_x   = cos(angle - PI / ((int)ray_length % 2 == 0 ? 1 : 2));
	    pos_y   = sin(angle - PI / ((int)ray_length % 2 == 0 ? 1 : 2));
	    toggled = true;
	  }
	  /* NON EUCLIDEAN (CHANGE INTERPRETED DISTANCE) */
          /* additional += (ray_length - floor(ray_length) > 0.1) ? 0.1 : 0; */ 
	  /* additional += (ray_length / 10) * incr; */
	}else if ((item == 'w') and (ray_length <= 3)){	/* TRICK WINDOW 1 */ 
	}else if ((item == 'W') and (ray_length >= 3)){ /* TRICK WINDOW 2 */ 
	}else hit_obj = true; /* THE RAY HAS HIT SOMETHING SOLID */
      }
      /* MOVE FORWARD  */
      ray_length += incr; 
  }
  r.init_ray(item,
	     ray_length + additional, /* ADD THE DIST USED TO CREATE NON EUCLIDEAN GEOMETRY */
	     transparent_char, 
	     transparent_distance,
	     go_up(ray_x), go_up(ray_y)); /* CORDINATES OF HIT BLOCK (SOLID) */
  return r;
}

void
ray_trace(Map *m, Player *p, ray *array_pointer)
{ /* RAYTRACE ALL RAYS AND ASSIGN THEM TO THE CORRESPONDING ARRAY POSITION  */
  for (uint16_t current_ray = 0; current_ray < RAY_AMOUNT; current_ray++)
    *(array_pointer + current_ray) = shoot_ray(m, p, current_ray, p->fov);
}  

bool
multi_char_OR(char C1, short char_amount, char *C2)
{ /* COMPARE MULTIPLE CHARS AGAINST ONE AND USE OR-LOGIC ON THEM */
  for (short i = 0; i < char_amount; i++){
    if (C1 == C2[i]) return true;

  }return false;
}


void
handle_input(double *player_swing, double time_diff, sf::RenderWindow *window, Player *p, Map *m, bool *shooting)
{
  sf::Event event;
  while (window->pollEvent(event))
    if (event.type == sf::Event::Closed) window->close();

  short array_size       = 4;
  char walkable_blocks[] = { ' ' , 'H', 'w', 'o'};


  
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  p->dir -= (TURN_SCALE * time_diff); /*  Turn left  */
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) p->dir += (TURN_SCALE * time_diff); /*  Turn right */
  
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) /* WALK STRAIGHT */
    if (multi_char_OR(m->get_item((unsigned int)(p->x + SPEED * cos(p->dir) * time_diff + CLOSE),
				  (unsigned int)(p->y + SPEED * sin(p->dir) * time_diff + CLOSE)),
			   array_size,
			   walkable_blocks)){
      

      p->x += SPEED * cos(p->dir) * time_diff;
      p->y += SPEED * sin(p->dir) * time_diff;
      *player_swing += ((PI / (BOBB_SCALE)) * (time_diff) / 470000000000000);
      std::cout << ((PI / (BOBB_SCALE)) * (time_diff) / 470000000000000) << std::endl;

      
    }
  
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) /* WALK BACKWARDS */
    if (multi_char_OR(m->get_item((unsigned int)(p->x - SPEED * cos(p->dir) * time_diff + CLOSE),
				  (unsigned int)(p->y - SPEED * sin(p->dir) * time_diff + CLOSE)),
			   array_size,
			   walkable_blocks)){

      p->x -= SPEED * cos(p->dir) * time_diff;
      p->y -= SPEED * sin(p->dir) * time_diff;
      *player_swing += ((int)(PI / (BOBB_SCALE)) * ((int)time_diff) / 470000000000000);

    }
  
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) /* WALK LEFT */
    if (multi_char_OR(m->get_item((unsigned int)(p->x + SPEED * cos(p->dir - PI / 2) * time_diff  + CLOSE),
				  (unsigned int)(p->y + SPEED * sin(p->dir - PI / 2) * time_diff  + CLOSE)),
			   array_size,
			   walkable_blocks)){
	  
      p->y += SPEED * sin(p->dir - PI / 2) * time_diff;
      p->x += SPEED * cos(p->dir - PI / 2) * time_diff;

    }
  
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) /* WALK RIGHT */
    if (multi_char_OR(m->get_item((unsigned int)(p->x + SPEED * cos(p->dir + PI / 2) * time_diff  + CLOSE),
				  (unsigned int)(p->y + SPEED * sin(p->dir + PI / 2) * time_diff  + CLOSE)),
			   array_size,
			   walkable_blocks)){
	  
      p->y += SPEED * sin(p->dir + PI / 2) * time_diff;
      p->x += SPEED * cos(p->dir + PI / 2) * time_diff;

    }

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){ /* INTERACTION WITH DOORS */
    ray r = shoot_ray(m, p, (short)(RAY_AMOUNT / 2), p->fov);
    if (r.character == 'c' and r.distance < 2){
      m->change_item(r.c_x, r.c_y, 'o');
    }else if (r.character == 'o' and r.distance < 2){
      m->change_item(r.c_x, r.c_y, 'c');
    }
    p->isShooting = true;
  }else{
    p->isShooting = false;
  }
  
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){ /* TURN FOV DOWN*/
    p->fov -= TURN_SCALE * time_diff;
  }else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){ /* TURN FOV UP*/
    p->fov += TURN_SCALE * time_diff;
  }

    
  
}




void
draw(double player_swing_set, short rect_width, ray *array_pointer, sf::RenderWindow *window, Player *p, Map *m, sf::Sprite *hands, sf::Sprite *shands, double time_diff, bool *shooting)
{
  /* DRAW SKY AND FLOOR GROWS IN O(HEIGHT) */
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


   
  /* DRAW EVERY OBJECT O(RAY_AMOUNT) */
  for(uint16_t i = 0; i < RAY_AMOUNT; i++){
    ray r = *(array_pointer + i);

    /* DEFAULT RECTANGLE  */
    sf::RectangleShape rectangle(sf::Vector2f(rect_width, 2 * (HEIGHT / r.distance) + player_swing_set));
    rectangle.setPosition(i * rect_width, (HEIGHT - 2 * (HEIGHT / r.distance)) / 2 + player_swing_set);
	   
    switch (r.character)
      {

      case 't': /* THE TALLBLOCK, DRAWS ALL THE WAY TO THE TOP OF THE SCREEN */
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
					 96  - r.distance));
	break;
	
      case 'r': /* 'r' */
	rectangle.setFillColor(sf::Color(0,    
					 MAX_RGB - colorval(SCALE * r.distance),
					 MAX_RGB - colorval(SCALE * r.distance)));
	break;

      case 'g': /* 'g' */
	rectangle.setFillColor(sf::Color(MAX_RGB - colorval(SCALE * r.distance),
					 0 ,
					 MAX_RGB - colorval(SCALE * r.distance)));
	break;
	
      case 'b': /* 'b' */
	rectangle.setFillColor(sf::Color(MAX_RGB - colorval(SCALE * r.distance),
					 MAX_RGB - colorval(SCALE * r.distance),
					 0));
	break;
	    
      default: /* DEFAULT COLOR (WHITE) */
	rectangle.setFillColor(sf::Color(MAX_RGB - colorval(SCALE * r.distance),
					 MAX_RGB - colorval(SCALE * r.distance),
					 MAX_RGB - colorval(SCALE * r.distance)));
      }
    
    window->draw(rectangle);
  }
  /* DRAW HANDS AND APPLY SWING EFFECT*/
  sf::Sprite Dhands;
  sf::Vector2f pos;
    
  if (p->isShooting){
    Dhands = *shands;
    pos    =  shands->getPosition();
  }else{
    Dhands = *hands;
    pos    =  hands->getPosition();
  }

  Dhands.setPosition(sf::Vector2f(pos.x, pos.y - 80 + 2 * player_swing_set ));
  window->draw(Dhands);

}

#endif
