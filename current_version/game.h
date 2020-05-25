enum dir {OFF, R_UP, R_DOWN, L_DOWN, L_UP};      //target's state
enum level {ENTRY, NXT_LVL, GAME, DEAD, PAUSED, PAUSED_U, FAIL, ENDGAME, PREVIOUS};    //stages of the game
enum to_do {NTHG, MOV_L, MOV_R, SHOOT = 4};     //keyboard_actions() output
enum alignment {LEFT, CENTRE, RIGHT};

struct bullet{
  double x;
  double y;
  double angle;
  int active; // <0 when exploding,0 when active,0<active<1000 when counting from random number
};

struct target{
  double x;
  double y;
  double speed;
  double angle;
  int state;      //<=0 while exploding[-10->0 meaning explosion stage], >0 while moving[1->4 meaning target direction]
  enum color colour;
  char points;
  struct bullet ball;
  struct target* next;
};

struct rocket{
  int x       :12;
  int y       :12;
  int active  :2;
};

struct s_ship{
  int x      :12;
  int y      :12;
  int life   :8;
};

struct status{
  unsigned int game_enum  :4;
  unsigned int prev_game_enum :4;
  unsigned int refresh    :8;
  unsigned int user_score :20;
  unsigned int diff_score :1;
  unsigned int lvl        :8;
  unsigned int progress   :12;
  char active_targets;
};

void dummy();

int ufo_y();
int ufo_x();

void read_scores();

void update_scorebrd();

void save_score();

int random_value(int low, int high);                          //get random value from range [low,high]

double random_value_d(double low, double high);               //get random value from range [low,high]

double deg_to_rad(double deg);                                //transform degrees to radians

void draw_background();                                       //fill the background and draw the boundry line

//void draw_target(struct target *t);                           //draw target in its current state

void draw_UFO(struct target* t);                              //draw_target's successor

void draw_ship(struct s_ship* ship);                          //draws ship with its centre on (x,y)

void draw_missile(struct rocket* r);                          //draws rocket with its centre on (x,y)

void draw_lifes(int lifes);                                   //draws (lifes) hearts in the right-bottom corner

void text_align(char* text, enum alignment al, int y, enum color c, int a);     //draw text centered in the middle of the screen with a y offset

void out_text();                                              //show texts based on game's status

void target_action(struct target* t);                         //change parameters of target based on their status

void add_to_score(int x);       //increase score by x

int score();                    //return current score

int y_boundry();                //height of the line targets bounce of

char* input_string();           //pointer to input string

void clear_input();             //clear the input buffer

char keyboard_actions();

char game_state();

int add_targets(int amount);

void activate_targets();

void transition(int time);

char* num_targets();

void del_targets();

void set_game_state(enum level k);

void next_lvl();

void set_refresh_rate(int r);

struct s_ship* m_ship();

struct target* head_target();
