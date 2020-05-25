#include "primlib.h"
#include "game.h"
#include <stdlib.h>
#include <stdio.h>

#define UFO_WIDTH     30
#define UFO_HEIGHT    20
#define BUFF_SIZE     10
#define SCORE_SIZE    8
#define BACK          0
#define GO            1

typedef struct target* PTR;

FILE* brd_ptr;

void dummy(){}    //do nothing

static struct status gamestat = {ENTRY, 1, 1, 0, 1, 1, 0, -1};
static char input[BUFF_SIZE];
static struct s_ship mothership;
static struct target first;
static struct user{
  char name [BUFF_SIZE];
  unsigned int scores;
} us[SCORE_SIZE];

void read_scores(){
  brd_ptr = fopen("./scores.txt","r");
  if(brd_ptr != NULL){
    int pos = 0;
    char buff[16];
    while(pos < SCORE_SIZE){
      if(fgets(buff ,16, brd_ptr) != NULL)
        sscanf(buff,"%s\t%u\n", &(us[pos].name[0]), &(us[pos].scores));
      pos++;
    }
    fclose(brd_ptr);
  }
}

void print_scores(){
  gfx_fontScale(3);
  text_align("SCOREBOARD", LEFT, -300, WHITE, 255);
  gfx_fontScale(2);
  int pos = 0;
  char text[30];
  while(pos < SCORE_SIZE){
    if(us[pos].name[0] != '0'){
      sprintf(text,"%d.%10s %u", pos + 1, &(us[pos].name[0]), us[pos].scores);
      text_align(text, LEFT, -260 + 20 * pos, WHITE, 255);
    }
    pos++;
  }
}

void update_scorebrd(){
  if(gamestat.diff_score){
    int pos = 0;
    while(us[pos].scores > gamestat.user_score)
      pos++;
    if(pos < SCORE_SIZE){
      for(int i = SCORE_SIZE - 1; i > pos; i--){
        strcpy(&(us[i].name[0]), &(us[i-1].name[0]));
        us[i].scores = us[i-1].scores;
      }
      us[pos].scores = gamestat.user_score;
      strcpy(&(us[pos].name[0]), input_string());
    }
    gamestat.diff_score = 0;
  }
}

void save_score(){
  printf("Congratulations %s, your final score was %d point%c :)\n", input_string(), gamestat.user_score, *((gamestat.user_score==1)?"":"s"));
  brd_ptr = fopen("./scores.txt","w");
  if(brd_ptr != NULL){
    update_scorebrd();
    int pos = -1;
    while(++pos < SCORE_SIZE)
      fprintf(brd_ptr,"%s\t%u\n", &(us[pos].name[0]), us[pos].scores);
    fclose(brd_ptr);
  }
  gamestat.user_score = 0;
}

int random_value(int low, int high){
 return (int)(low + rand() / (RAND_MAX + 1.0) * (high - low));
}

double random_value_d(double low, double high){
 return (double)(low + rand() / (RAND_MAX + 1.0) * (high - low));
}

double deg_to_rad(double deg){
  return (deg * (M_PI / 180.0));
}

void draw_background(){
  gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1, BLACK);
  gfx_line(0, y_boundry(), gfx_screenWidth(), y_boundry(), WHITE);
}

/*
void draw_target(struct target *t){
  if(t->state <= 0){
    int scaled_size = t->size * t->state / -10;
    int scaled_distance = 5 * (t->state + 10);

    gfx_filledRect(t->x - scaled_size, t->y - scaled_size, t->x + scaled_size, t->y + scaled_size, t->colour);

    gfx_filledRect(t->x - scaled_size - scaled_distance, t->y - scaled_size - scaled_distance, t->x - scaled_distance, t->y - scaled_distance, t->colour);

    gfx_filledRect(t->x + scaled_distance, t->y - scaled_size - scaled_distance, t->x + scaled_size + scaled_distance, t->y - scaled_distance, t->colour);

    gfx_filledRect(t->x - scaled_size - scaled_distance, t->y + scaled_distance, t->x - scaled_distance, t->y + scaled_size + scaled_distance, t->colour);

    gfx_filledRect(t->x + scaled_distance, t->y + scaled_distance, t->x + scaled_size + scaled_distance, t->y + scaled_size + scaled_distance, t->colour);
  }
  else{
    gfx_filledRect(t->x - t->size, t->y - t->size, t->x + t->size, t->y + t->size, t->colour);
  }
}
*/

int ufo_x(){
  return UFO_WIDTH;
}
int ufo_y(){
  return UFO_HEIGHT;
}

void draw_UFO(PTR t){
  if(t->state != OFF){
    if(t->state > OFF){
      int dx = UFO_WIDTH;
      int dy = UFO_HEIGHT/2;
      //gfx_filledEllipse(t->x ,t->y,dy-1,dx-9,GREEN);
      gfx_filledCircle(t->x, t->y + dy, dy, t->colour);
      gfx_filledCircle(t->x, t->y - dy, dy, t->colour);
      gfx_filledEllipse(t->x ,t->y, dx, dy, t->colour + 1);
      gfx_line(t->x - dx, t->y, t->x + dx, t->y, t->colour);
      if(t->ball.active > 0){
        int nx = dx * t->ball.active / 1000;
        gfx_line(t->x - nx, t->y, t->x + nx,t->y, ORANGE);
      }
    }
    else
      gfx_filledCircle(t->x, t->y, (t->state % 5) * -15 , YELLOW);
  }
  if( !(t->ball.active) )
    gfx_filledCircle(t->ball.x, t->ball.y, 5, ORANGE);
  else if(t->ball.active < 0)
    gfx_filledCircle(t->ball.x, t->ball.y, -(t->ball.active), YELLOW);
}

void draw_ship(struct s_ship* ship){
  double scale = 2.0;
  if(gamestat.game_enum == DEAD && ship->life <= 0)
    scale = (-(ship->life) % 6) * 0.5;
  else
    gfx_ellipse(ship->x, ship->y - 16 * scale, 33 * scale, 19 * scale, BLUE);

  gfx_filledRect(ship->x - 6 * scale, ship->y - 25 * scale, ship->x + 6 * scale, ship->y, WHITE);
  gfx_filledTriangle(ship->x - 6 * scale, ship->y, ship->x + 6 * scale, ship->y, ship->x, ship->y + 7 * scale, ORANGE);
  gfx_filledTriangle(ship->x - 10 * scale, ship->y, ship->x + 10 * scale, ship->y, ship->x, ship->y + 3 * scale, WHITE);
  gfx_filledTriangle(ship->x - 10 * scale, ship->y, ship->x - 6 * scale, ship->y - 6 * scale, ship->x - 6 * scale, ship->y, WHITE);
  gfx_filledTriangle(ship->x + 10 * scale, ship->y, ship->x + 6 * scale, ship->y - 6 * scale, ship->x + 6 * scale, ship->y, WHITE);
  gfx_filledTriangle(ship->x - 27 * scale, ship->y - 27 * scale, ship->x - 6 * scale, ship->y - 20 * scale, ship->x - 6 * scale, ship->y - 6 * scale, WHITE);
  gfx_filledTriangle(ship->x + 27 * scale, ship->y - 27 * scale, ship->x + 6 * scale, ship->y - 20 * scale, ship->x + 6 * scale, ship->y - 6 * scale, WHITE);
  gfx_filledTriangle(ship->x - 16 * scale, ship->y - 21 * scale, ship->x, ship->y - 14 * scale, ship->x, ship->y - 5 * scale, GRAY);
  gfx_filledTriangle(ship->x + 16 * scale, ship->y - 21 * scale, ship->x, ship->y - 14 * scale, ship->x, ship->y - 5 * scale, GRAY);
}

void draw_missile(struct rocket* r){
  if(r->active){
    gfx_filledTriangle(r->x - 7, r->y - 4, r->x + 7, r->y - 4, r->x, r->y - 8, RED);
    gfx_filledRect(r->x - 7, r->y - 4, r->x + 7, r->y + 4, GRAY);
    gfx_filledTriangle(r->x - 4, r->y + 4, r->x + 4, r->y + 4, r->x, r->y + 8, YELLOW);
  }
}

void draw_lifes(int lifes){
  int x = gfx_screenWidth() - 50;
  int y = gfx_screenHeight() - 50;
  for(int n = 0; n < lifes; n++){
    gfx_filledCircle(x - 4 , y - 3, 5, RED);
    gfx_filledCircle(x + 4 , y - 3, 5, RED);
    gfx_filledTriangle(x - 8, y, x, y + 10, x + 8, y, RED);
    x -= 25;
  }
}

void text_align(char* text, enum alignment al, int y_offset, enum color c, int a){
  int size = strlen(text) - 1;
  int x;
  switch(al){
    case LEFT:
      x = 50;
      break;

    case CENTRE:
      x = gfx_screenWidth() / 2 - 4 * gfx_fontSize() * size;
      break;

    case RIGHT:
      x = gfx_screenWidth() - 50 - 8 * gfx_fontSize() * size;
      break;
  }
  y_offset += (gfx_screenHeight() / 2 + 4 * gfx_fontSize());
  gfx_textoutA(x, y_offset, text, c, a);
}

void out_text(){
  char text[32];
  switch(gamestat.game_enum){
    case ENTRY:
      gfx_fontScale(3);
      text_align("WELCOME", CENTRE, -200, RED, 255);
      text_align("Type in your name below", CENTRE, -100, RED, 255);
      //gfx_rect(gfx_screenWidth() / 2 - 240, gfx_screenHeight() / 2 + 12, gfx_screenWidth() / 2 + 240, gfx_screenHeight() / 2 - 12, WHITE);
      text_align(input_string(), CENTRE, 0, GREEN, 255);
      text_align("Press ENTER to START", CENTRE, 50, RED, 255);
      gfx_fontScale(2);
      print_scores();
      break;

    case PAUSED:
    case PAUSED_U:
      text_align("Press SPACE to continue", CENTRE, 80, RED, 255);
      text_align("Press ESCAPE to exit", CENTRE, 120, RED, 255);
      gfx_fontScale(3);
      text_align("PAUSED", CENTRE, -150, RED, 255);
      gfx_fontScale(2);
      update_scorebrd();
      print_scores();
      break;

    case NXT_LVL:
      gfx_fontScale(3);
      text_align("TARGETS SPAWNING IN", CENTRE, -150, WHITE, 255);
      sprintf(text,"%u", gamestat.progress/1000);
      text_align(text, CENTRE, 0, WHITE, 255);
      gfx_fontScale(2);

    case GAME:
      sprintf(text,"Your score is: %d point%c", gamestat.user_score, *((gamestat.user_score==1)?"":"s"));
      // gfx_textout(50, gfx_screenHeight() - 40, text, WHITE);
      text_align(text, LEFT, gfx_screenHeight() / 2 - 40, WHITE, 255);
      sprintf(text,"LVL %d", gamestat.lvl);
      text_align(text, CENTRE, gfx_screenHeight() / 2 - 40, WHITE, 255);
      gfx_fontScale(1);
      int diff = 10;
      while(gamestat.user_score > diff)
        diff *= 2;
      sprintf(text,"Bonus life for %d more point%c", diff - gamestat.user_score, *((gamestat.user_score==1)?"":"s"));
      text_align(text, RIGHT, gfx_screenHeight() / 2 - 35, WHITE, 255);
      gfx_fontScale(2);
      break;

    case DEAD:
      dummy();
      int tr = 255 * (1.0 + (double)mothership.life / 60);
      gfx_fontScale(3);
      text_align("YOU HAVE LOST", CENTRE, -200, RED, tr);
      gfx_fontScale(2);
      sprintf(text,"Your score was: %d point%c", gamestat.user_score, *((gamestat.user_score==1)?"":"s"));
      text_align(text, CENTRE, 0, RED, tr);
      text_align("Press ENTER to start a new game", CENTRE, 80, RED, tr);
      text_align("Press ESCAPE to exit", CENTRE, 120, RED, tr);
      break;

    case FAIL:
      gfx_fontScale(3);
      text_align("Could not allocate memory", CENTRE, -150, RED, 255);
      printf("%s\n", "Failed to allocate memory");
      gfx_fontScale(2);
      text_align("Press ESCAPE to exit", CENTRE, 120, RED, 255);
      break;

    case ENDGAME:
      break;
  }
}

void target_action(PTR t){
  switch(t->state){
    case OFF:
      break;

    case R_UP:
      t->x += t->speed * cos(t->angle);
      t->y -= t->speed * sin(t->angle);

      if(t->x >= gfx_screenWidth() - ufo_x()){
        t->state = L_UP;
      }
      else if(t->y <= ufo_y()){
        t->state = R_DOWN;
      }
      break;

    case R_DOWN:
      t->x += t->speed * cos(t->angle);
      t->y += t->speed * sin(t->angle);

      if(t->x >= gfx_screenWidth() - ufo_x()){
        t->state = L_DOWN;
      }
      else if(t->y >= y_boundry() - ufo_y()){
        t->state = R_UP;
      }
      break;

    case L_UP:
      t->x -= t->speed * cos(t->angle);
      t->y -= t->speed * sin(t->angle);

      if(t->x <= ufo_x()){
        t->state = R_UP;
      }
      else if(t->y <= ufo_y()){
        t->state = L_DOWN;
      }
      break;

    case L_DOWN:
      t->x -= t->speed * cos(t->angle);
      t->y += t->speed * sin(t->angle);

      if(t->x <= ufo_x()){
        t->state = R_DOWN;
      }
      else if(t->y >= y_boundry() - ufo_y()){
        t->state = L_UP;
      }
      break;

    default:
      t->state ++;
  }
}

void add_to_score(int x){
  gamestat.user_score += x;
  gamestat.diff_score = 1;
}

int score(){
  return gamestat.user_score;
}

int y_boundry(){
  return gfx_screenHeight() * 0.73;
}

char* input_string(){
  return input;
}

void clear_input(){
  for(int i = 0; i < BUFF_SIZE; i++)
    input[i] = 0;
}

char keyboard_actions(){
  int key = gfx_pollkey();
  //printf("%d\n", key);
  char to_return = 0;
  switch(gamestat.game_enum){
    case ENTRY:
      dummy();
      int index = 0;
      char* pos = input;
      switch(key){

        case SDLK_RETURN:  //ENTER
          if(strlen(input) > 0)
            set_game_state(GAME);
          break;

        case SDLK_BACKSPACE:
          while(*(pos + index))   index++;
          if(index > 0)
            *(pos + index - 1) = 0;
          break;

        case SDLK_DELETE:
          clear_input();
          break;

        default:
          if(key > 31 && key < 127){
            while(*(pos + index))   index++;
            if(index < BUFF_SIZE){
              *(pos + index) = (char)key;
            }
          }
      }
      break;

    case PAUSED:
    case PAUSED_U:
      switch(key){

        case SDLK_SPACE:
          set_game_state(PREVIOUS);
          break;

        case SDLK_ESCAPE:
          set_game_state(ENDGAME);
          break;

        default:
          break;
      }
      break;

    case NXT_LVL:
    case GAME:
      switch(key){

        case SDLK_ESCAPE:
          set_game_state(PAUSED);
          break;

        case SDLK_SPACE:
            to_return |= SHOOT;

        default:
          if(gfx_isKeyDown(SDLK_RIGHT)||gfx_isKeyDown(SDLK_d))
            to_return |= MOV_R;

          if(gfx_isKeyDown(SDLK_LEFT)||gfx_isKeyDown(SDLK_a))
            to_return |= MOV_L;

          break;
      }
      break;

    case DEAD:
      switch(key){
        case SDLK_RETURN:  //ENTER
          set_game_state(ENTRY);
          mothership.x = gfx_screenWidth() / 2;
          mothership.life = 5;
          save_score();
          break;

        case SDLK_ESCAPE:
          set_game_state(ENDGAME);
          break;
      }
      break;

    case FAIL:
      if(key == SDLK_ESCAPE)
        set_game_state(ENDGAME);

    case ENDGAME:
      break;
  }
  return to_return;
}

char game_state(){
  return gamestat.game_enum;
}

void init_target(PTR new){
  new->state = OFF;
  new->colour = 2 * random_value(0,6) + 1;
  new->y = random_value(gfx_screenHeight() / 8, gfx_screenHeight() / 2);
  new->angle = deg_to_rad(random_value_d(10.0, 60.0));
  new->points = (char)random_value(1,10);
  new->speed = 100 * new->points / gamestat.refresh;
  new->ball.active = random_value(400,1000);
}

int add_targets(int amount){
  if(amount > 0){
    int num = 1;
    PTR f = head_target();
    init_target(f);
    while(f->next != NULL){
      init_target(f);
      num++;
      f = f->next;
    }
    if(gamestat.active_targets == -1)
      gamestat.active_targets = amount;
    else
      gamestat.active_targets = num + amount;

    while(++num <= gamestat.active_targets){
      f->next = malloc(sizeof(*f));
      if(f->next == NULL)
        return -1;
      else{
        f = f->next;
        init_target(f);
      }
    }
    f->next = NULL;
    return 1;
  }
  return 0;
}

void activate_targets(){
  PTR f = head_target();
  while(f != NULL){
    f->state = random_value(R_UP,L_UP);
    if(f->state < L_DOWN)
      f->x = -ufo_x();
    else
      f->x = gfx_screenWidth() + ufo_x();
    f = f->next;
  }
}

void transition(int time){
  if(gamestat.progress > 999)
    gamestat.progress -= time;
  else{
    activate_targets();
    set_game_state(GAME);
  }
}

char* num_targets(){
  return &gamestat.active_targets;
}

void del_targets(){
  PTR f = head_target();
  PTR b = f->next;
  while(b){
    f = b;
    b = f->next;
    free(f);
  }
}

void set_game_state(enum level k){
  if(k == NXT_LVL)
    gamestat.progress = 3999;
  else if(k == PAUSED)
    gamestat.prev_game_enum = gamestat.game_enum;
  else if(k == PREVIOUS)
    k = gamestat.prev_game_enum;
  gamestat.game_enum = k;
}

void next_lvl(){
  set_game_state(NXT_LVL);
  if(add_targets(gamestat.lvl) == -1)
    set_game_state(FAIL);
  gamestat.lvl ++;
}

void set_refresh_rate(int r){
  // assert(r > 0);
  gamestat.refresh = r;
}

struct s_ship* m_ship(){
  return &mothership;
}

PTR head_target(){
  return &first;
}
