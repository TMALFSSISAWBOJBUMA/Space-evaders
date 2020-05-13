#include "primlib.h"
#include "game.h"
#include <stdlib.h>
#include <assert.h>


int main(){
  if (gfx_init())
    exit(3);

  gfx_initFonts();

  gfx_fontScale(3);

  int refresh_rate = 90;
  //enum level game_state() = ENTRY;

  const int targets = 4;
  struct target t[targets];
  // Targets should be sorted by size- smallest target last
  t[0].x = 0;                         //initial x coordinate
  t[0].y = gfx_screenHeight() / 4;    //initial y coordinate
  t[0].state = R_UP;                  //initial target direction
  t[0].speed = 500.0;                 //pixels per second (only positive values)
  t[0].angle = deg_to_rad(45);        //initial target angle (only positive values)
  t[0].colour = GREEN;                //target colour
  t[0].points = 1;                    //how many points user gets for hitting the target

  t[1].x = gfx_screenWidth();         //initial x coordinate
  t[1].y = gfx_screenHeight() / 2;    //initial y coordinate
  t[1].state = L_DOWN;                //initial target direction
  t[1].speed = 600.0;                 //pixels per second (only positive values)
  t[1].angle = deg_to_rad(20);        //initial target angle (only positive values)
  t[1].colour = YELLOW;               //target colour
  t[1].points = 3;                    //how many points user gets for hitting the target

  t[2].x = 0;                         //initial x coordinate
  t[2].y = 0;                         //initial y coordinate
  t[2].state = R_DOWN;                //initial target direction
  t[2].speed = 700.0;                 //pixels per second (only positive values)
  t[2].angle = deg_to_rad(5);         //initial target angle (only positive values)
  t[2].colour = CYAN;                 //target colour
  t[2].points = 5;                    //how many points user gets for hitting the target

  t[3].x = gfx_screenWidth();         //initial x coordinate
  t[3].y = y_boundry();               //initial y coordinate
  t[3].state = L_UP;                  //initial target direction
  t[3].speed = 600.0;                 //pixels per second (only positive values)
  t[3].angle = deg_to_rad(50);        //initial target angle (only positive values)
  t[3].colour = WHITE;                //target colour
  t[3].points = 3;                    //how many points user gets for hitting the target

  const int missiles = 3;
  int active_missiles = 0;
  struct rocket r[missiles];
  for(int n = 0; n < missiles; n++){
    r[n].active = 0;
  }


  int bullet_size = 8;
  double rocket_speed = 1000.0; // pixels per second

  double bullet_speed = 600.0;
  struct s_ship* ship = m_ship();
  ship->x = gfx_screenWidth() / 2;
  ship->y = gfx_screenHeight() - 80;
  ship->life = 1;
  double cannon_speed = 500.0;

  rocket_speed /= refresh_rate;
  cannon_speed /= refresh_rate;
  bullet_speed /= refresh_rate;
  for(int n = 0; n < targets; n++){
    t[n].ball.active = random_value(400,1000);
    t[n].speed /= refresh_rate;
    assert(t[n].angle > 0);
    assert(t[n].speed > 0);
  }
  t[0].ball.active = 10;
  unsigned long long time;

  while (game_state() != ENDGAME) {
    if (SDL_GetTicks() - time > (1000 / refresh_rate)){
      printf("start after:\t\t%lldms\n",SDL_GetTicks() - time);
      time = SDL_GetTicks();

      if(game_state() != PAUSED_U){
        draw_background();
        printf("%lld:",SDL_GetTicks() - time);
        if (active_missiles){
          for(int o = 0; o < missiles; o++){
            draw_missile( &(r[o]) );
          }
        }
        printf("%lld:",SDL_GetTicks() - time);
        draw_ship(ship);
        draw_lifes(ship->life);
        printf("%lld:",SDL_GetTicks() - time);
        for(int n = 0; n < targets; n++){
          draw_UFO(&(t[n]));
        }
        printf("%lld:",SDL_GetTicks() - time);

        if(game_state() == PAUSED){
          set_game_state(PAUSED_U);
          gfx_setAlpha(50);
          gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1, WHITE);
          gfx_setAlpha(100);
        }

        out_text();
        printf("(t)%lld:",SDL_GetTicks() - time);

        gfx_updateScreen();
        printf("%lld\n",SDL_GetTicks() - time);

      }

      if(game_state() < PAUSED){
        for(int n = 0; n < targets; n++){
          target_action(&(t[n]));
        }
      }

      if(game_state() == GAME){
        if(active_missiles > 0){
          for(int o = 0; o < missiles; o++){
            if(r[o].active){
              for(int n = 0; n < targets; n++){
                if(t[n].state > 0){
                  double dx = fabs(r[o].x - t[n].x);
                  if (dx <= ufo_x() + bullet_size){
                    double dy = (ufo_x() + bullet_size - dx) * (ufo_y() + bullet_size) / (ufo_x() + bullet_size);
                    if ((t[n].y - r[o].y) <= dy && (t[n].y - r[o].y) > 0){
                      t[n].state = -10;
                      r[o].active = 0;
                      add_to_score(t[n].points);
                      active_missiles--;
                    }
                  }
                }
              }
              if(r[o].active){
                r[o].y -= rocket_speed;
                if (r[o].y < -bullet_size){
                  r[o].active = 0;
                  active_missiles--;
                }
              }
            }
          }
        }

        for(int n = 0; n < targets; n++){
          if(t[n].ball.active == 1){
            t[n].ball.x = t[n].x;
            t[n].ball.y = t[n].y;
            double tan = (ship->x - t[n].x)/(ship->y - t[n].y);
            t[n].ball.angle = atan(tan);
            t[n].ball.active--;
            //printf("%d,%f\n", n, tan);
          }
          else if( !(t[n].ball.active) ){
            if(hypot(t[n].ball.x -ship->x, t[n].ball.y - ship->y) < 40){
              t[n].ball.active = random_value(400,1000);
              ship->life--;
              if(!ship->life){
                set_game_state(DEAD);
                ship->life = -200;
              }
            }
            if(!(t[n].ball.active)){
              t[n].ball.x += bullet_speed * sin(t[n].ball.angle);
              if(t[n].ball.x < 0 || t[n].ball.x > gfx_screenWidth()){
                t[n].ball.active = random_value(400,1000);
              }
              else{
                t[n].ball.y += bullet_speed * cos(t[n].ball.angle);
                if(t[n].ball.y > gfx_screenHeight()){
                  t[n].ball.active = random_value(400,1000);
                }
              }
            }
          }
        }
      }

      if(game_state() == DEAD){
        if(ship->life < 0)
          ship->life ++;
      }

      char act = keyboard_actions();
      if(act != NTHG){
        if(act & MOV_R){
          if(ship->x < gfx_screenWidth())
            ship->x += cannon_speed;
        }

        if(act & MOV_L){
          if(ship->x > 0)
            ship->x -= cannon_speed;
        }

        if(act & SHOOT){
          if(active_missiles < missiles){     //can send another missile
            int n = 0;
            while(r[n].active)     //look for first inactive missile to overwrite
              n++;
            r[n].x = ship->x;
            r[n].y = ship->y - 50 + bullet_size;
            r[n].active = 1;
            active_missiles++;
          }
        }
      }
    }
  }
  //quit loop
  printf("Congratulations %s, your final score was %d point%c :)\n", input_string(), score(), *((score()==1)?"":"s"));
  gfx_delFonts();
  return 0;
}
