#include "primlib.h"
#include "game.h"
#include <stdlib.h>
#include <assert.h>
#include <math.h>

typedef struct target* PTR;

void step_to_zero(int* num){
  if(*num > 0)    --(*num);
  else            ++(*num);
}

int main(){
  read_scores();
  //cos
  if (gfx_init())
    exit(3);

  gfx_initFonts();

  gfx_fontScale(2);

  int refresh_rate = 90;
  set_refresh_rate(refresh_rate);

  const int missiles = 3;
  int active_missiles = 0;
  struct rocket r[missiles];
  for(int n = 0; n < missiles; n++)
    r[n].active = 0;


  int bullet_size = 8;
  double rocket_speed = 1000.0; // pixels per second
  int next_score = 10;

  double bullet_speed = 600.0;
  struct s_ship* ship = m_ship();
  ship->x = gfx_screenWidth() / 2;
  ship->y = gfx_screenHeight() - 80;
  ship->life = 5;
  double cannon_speed = 500.0;

  rocket_speed /= refresh_rate;
  cannon_speed /= refresh_rate;
  bullet_speed /= refresh_rate;

  if(add_targets(3) == -1)
    set_game_state(FAIL);
  activate_targets();
  PTR tar;

  unsigned long long time;
  int dt;

  while (game_state() != ENDGAME) {
    if (SDL_GetTicks() - time > (1000 / refresh_rate)){
      // printf("%lldms\n",SDL_GetTicks() - time);
      dt = SDL_GetTicks() - time;
      time = SDL_GetTicks();

      if(game_state() != PAUSED_U){
        draw_background();
        if (active_missiles){
          for(int o = 0; o < missiles; o++){
            draw_missile( &(r[o]) );
          }
        }
        draw_ship(ship);
        draw_lifes(ship->life);

        tar = head_target();
        while(tar != NULL){
          draw_UFO(tar);
          tar = tar->next;
        }

        if(game_state() == PAUSED){
          set_game_state(PAUSED_U);
          gfx_setAlpha(50);
          gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1, WHITE);
          gfx_setAlpha(100);
        }

        out_text();

        gfx_updateScreen();

      }

      if(game_state() < PAUSED){
        tar = head_target();
        while(tar != NULL){
          target_action(tar);
          tar = tar->next;
        }
      }

      if(game_state() == GAME){
        if(active_missiles > 0){
          for(int o = 0; o < missiles; o++){
            if(r[o].active){
              tar = head_target();
              while(tar != NULL){
                if(tar->state > 0){
                  double dx = fabs(r[o].x - tar->x);
                  if (dx <= ufo_x() + bullet_size){
                    double dy = (ufo_x() + bullet_size - dx) * (ufo_y() + bullet_size) / (ufo_x() + bullet_size);
                    if ((tar->y - r[o].y) <= dy && (tar->y - r[o].y) > 0){
                      tar->state = -10;
                      *(num_targets()) -= 1;
                      r[o].active = 0;
                      add_to_score(tar->points);
                      active_missiles--;
                    }
                  }
                }
                tar = tar->next;
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

        tar = head_target();
        while(tar != NULL && game_state() != DEAD){
          switch(tar->ball.active){
            case 0:
              dummy();
              double dy = fabs(tar->ball.y - ship->y + 16);
              if(dy < 50){
                double dx = fabs(tar->ball.x - ship->x);
                if(dx < 80){
                  if(hypot(dx - 56.5, dy) + hypot(dx + 56.5, dy) < 162){
                    tar->ball.active = -10;
                    ship->life--;
                    if(!ship->life){
                      set_game_state(DEAD);
                      ship->life = -60;
                      if(active_missiles > 0){
                        for(int o = 0; o < missiles; o++){
                          r[o].active = 0;
                        }
                        active_missiles = 0;
                      }
                      PTR temp = head_target(); // new pointer to clear remaining bullets
                      while(temp != NULL){
                        if(temp->ball.active <= 0)
                          temp->ball.active = random_value(400,1000);
                        temp = temp->next;
                      }
                    }
                  }
                }
              }
              if(!(tar->ball.active)){
                tar->ball.x += bullet_speed * sin(tar->ball.angle);
                if(tar->ball.x < 0 || tar->ball.x > gfx_screenWidth()){
                  tar->ball.active = random_value(400,1000);
                }
                else{
                  tar->ball.y += bullet_speed * cos(tar->ball.angle);
                  if(tar->ball.y > gfx_screenHeight()){
                    tar->ball.active = random_value(400,1000);
                  }
                }
              }
              break;

            case 1:
              tar->ball.x = tar->x;
              tar->ball.y = tar->y ;
              double tan = (ship->x - tar->x)/(ship->y - 16 - tar->y);
              tar->ball.angle = atan(tan);
              tar->ball.active--;
              break;

            case -1:
              tar->ball.active = random_value(400,1000);
              break;

            default:
              if(tar->state != OFF || tar->ball.active < 0)
                step_to_zero(&(tar->ball.active));
              break;
          }
          tar = tar->next;
        }

        if(score() > next_score){
          next_score *= 2;
          if(ship->life < 10)  //10 lifes max
            ship->life += 1;
        }

        if(*num_targets() == 0)
          next_lvl();//
      }
//
      else if(game_state() == NXT_LVL){
        transition(dt);

        if(active_missiles > 0){
          for(int o = 0; o < missiles; o++){
            if(r[o].active){
              r[o].y -= rocket_speed;
              if(r[o].y < -bullet_size){
                r[o].active = 0;
                active_missiles--;
              }
            }
          }
        }

        tar = head_target();
        while(tar != NULL && game_state() != DEAD){
          if(!(tar->ball.active)){
              double dy = fabs(tar->ball.y - ship->y + 16);
              if(dy < 50){
                double dx = fabs(tar->ball.x - ship->x);
                if(dx < 80){
                  if(hypot(dx - 56.5, dy) + hypot(dx + 56.5, dy) < 162){
                    tar->ball.active = -10;
                    ship->life--;
                    if(!ship->life){
                      set_game_state(DEAD);
                      ship->life = -60;
                      if(active_missiles > 0){
                        for(int o = 0; o < missiles; o++){
                          r[o].active = 0;
                        }
                        active_missiles = 0;
                      }
                      PTR temp = head_target(); // new pointer to clear remaining bullets
                      while(temp != NULL){
                        if(temp->ball.active < 0)
                          temp->ball.active = random_value(400,1000);
                        temp = temp->next;
                      }
                    }
                  }
                }
              }
              if(!(tar->ball.active)){
                tar->ball.x += bullet_speed * sin(tar->ball.angle);
                if(tar->ball.x < 0 || tar->ball.x > gfx_screenWidth()){
                  tar->ball.active = random_value(400,1000);
                }
                else{
                  tar->ball.y += bullet_speed * cos(tar->ball.angle);
                  if(tar->ball.y > gfx_screenHeight()){
                    tar->ball.active = random_value(400,1000);
                  }
                }
              }

          }
          else if(tar->ball.active < 0){
            step_to_zero(&(tar->ball.active));
            if(tar->ball.active == -1)
              tar->ball.active = random_value(400,1000);
          }
          tar = tar->next;
        }

      }

      else if(game_state() == DEAD){
        if(ship->life < 0)
          ship->life ++;
      }

      char act = keyboard_actions();
      if(act != NTHG){
        if(act & MOV_R){
          if(ship->x < gfx_screenWidth() - cannon_speed)
            ship->x += cannon_speed;
        }

        if(act & MOV_L){
          if(ship->x > cannon_speed)
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
  save_score();
  del_targets();
  gfx_delFonts();
  return 0;
}
