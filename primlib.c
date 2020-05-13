#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "primlib.h"
#include "SDL2_gfxPrimitives_font.h"

static SDL_Renderer *renderer = NULL;
static SDL_Window *window = NULL;

// static Uint32 colors[COLOR_MAX] = {0xff000000,
//                               0xff0000ff, 0xff00ff00, 0xffff0000,
//                               0xffffff00, 0xffff00ff, 0xff00ffff,
//                               0xffffffff};

struct RGB {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

static struct RGB colors[COLOR_MAX] = {
    {0, 0, 0}, {255, 0, 0}, {0, 255, 0}, {0, 100, 0}, {0, 0, 255},
    {0, 255, 255}, {255, 0, 255}, {255, 255, 0}, {100, 100, 0},
    {255, 140, 0}, {255, 255, 255}, {100, 100, 100}};

// Screen dimension constants
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 700;

//  added
#define NUM_FONTS   3
static int alpha = 255;
static int font_size = 1;
static unsigned char* fonts[NUM_FONTS];

void gfx_setAlpha(int pct){ //0-transparent, 100-full colour
  if(pct < 0) pct = 0;
  else if(pct > 100)  pct = 100;
  alpha = pct * 255 / 100;
}

void gfx_filledEllipse(int x, int y, int rx, int ry, enum color c){
  assert(c < COLOR_MAX);
  filledEllipseRGBA(renderer, x, y, rx, ry, colors[c].r, colors[c].g, colors[c].b, alpha);
}

void gfx_textoutA(int x, int y, const char *s, enum color c, int a) { //makes use of aplha value
  assert(c < COLOR_MAX);
  stringRGBA(renderer, x, y, s, colors[c].r, colors[c].g, colors[c].b, a);
}

unsigned char* create_font(int m){
  /*
  Consider m=3.
  For each input byte we have to create m^2 = 9 output bytes.

  -bytes 0-2 are created by "stretching" the input byte
  -bytes 3-5 i 6-8 are copies of bytes 0-2
  ie. in[1]=11000011 => out[m^2] = 11111100 00000000 00111111 11111100 00000000 00111111 11111100 00000000 00111111
                        font_pos = 77766655 54443332 22111000
                             bit = 01234567 01234567 01234567
                               j = 0        0        0        1        1        1        2        2        2
                               k = 0        1        2        0        1        2        0        1        2

  font_pos(bit,k)=7-(int)((bit+8*k)/m)
  */
  unsigned char* font = gfxPrimitivesFontdata;
  if(m == 1)
    return font;
  const int size = (8 * 256 * m * m);
  unsigned char* new_font = malloc(size);
  if(new_font != NULL){
    unsigned char* s_f = new_font;
    for(int n = 0; n < 256; n++){       //ASCII table
      for(int i = 0; i < 8; i++){       //8 bytes/letter in font source
        unsigned char buff = *(font++); //store input byte as local variable
        for(int j = 0; j < m; j++){     //m new rows/1 old row
          for(int k = 0; k < m; k++){   //m new bytes/1 new row
            *s_f = 0;
            if(!j){     //first new row for a byte
              for(int bit = 0; bit < 8; bit++){
                int f_pos = (bit + 8 * k) / m;     //bit position in source byte - 7
                *s_f |= (((buff >> (7 - f_pos)) & 1)<<(7-bit));
              }
            }
            else{       //following rows are the same as the first one
              *s_f = *(s_f - m);
            }
            s_f++;
          }
        }
      }
    }
    return new_font;
  }
  else{
    return font;
  }
}

void gfx_initFonts(){
  for(int num = 0; num < NUM_FONTS; num++){
    fonts[num] = create_font(num + 1);
  }
}

void gfx_delFonts(){
  for(int num = 1; num < NUM_FONTS; num++){
    free(fonts[num]);
  }
}

void gfx_fontScale(int m) { //set global font scale
  if(m < 1 || m > NUM_FONTS)
    m = 1;
  gfxPrimitivesSetFont(fonts[m - 1], 8 * m, 8 * m);
  font_size = m;
}

int gfx_fontSize(){
  return font_size;
}

//  /added

void gfx_pixel(int x, int y, enum color c) {
  pixelRGBA(renderer, x, y, colors[c].r, colors[c].g, colors[c].b, alpha);
}

void gfx_line(int x1, int y1, int x2, int y2, enum color c) {
  assert(c < COLOR_MAX);
  lineRGBA(renderer, x1, y1, x2, y2, colors[c].r, colors[c].g, colors[c].b,
           alpha);
}

void gfx_filledTriangle(int x1, int y1, int x2, int y2, int x3, int y3, enum color c) {
  assert(c < COLOR_MAX);
  Sint16 xtab[] = {x1, x2, x3};
  Sint16 ytab[] = {y1, y2, y3};
  filledPolygonRGBA(renderer, xtab, ytab, 3, colors[c].r, colors[c].g, colors[c].b,
                alpha);
}

void gfx_rect(int x1, int y1, int x2, int y2, enum color c) {
  assert(c < COLOR_MAX);
  rectangleRGBA(renderer, x1, y1, x2, y2, colors[c].r, colors[c].g, colors[c].b,
                alpha);
}

void gfx_filledRect(int x1, int y1, int x2, int y2, enum color c) {
  assert(c < COLOR_MAX);
  boxRGBA(renderer, x1, y1, x2, y2, colors[c].r, colors[c].g, colors[c].b, alpha);
}

void gfx_circle(int x, int y, int r, enum color c) {
  assert(c < COLOR_MAX);
  circleRGBA(renderer, x, y, r, colors[c].r, colors[c].g, colors[c].b, alpha);
}

void gfx_filledCircle(int x, int y, int r, enum color c) {
  assert(c < COLOR_MAX);
  filledCircleRGBA(renderer, x, y, r, colors[c].r, colors[c].g, colors[c].b, alpha);
}

int gfx_screenWidth() { return SCREEN_WIDTH; }

int gfx_screenHeight() { return SCREEN_HEIGHT; }

void gfx_updateScreen() {
  SDL_RenderPresent(renderer);
  SDL_RenderClear(renderer);
}

void gfx_textout(int x, int y, const char *s, enum color c) {
  assert(c < COLOR_MAX);
  stringRGBA(renderer, x, y, s, colors[c].r, colors[c].g, colors[c].b, alpha);
}

int gfx_pollkey() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_KEYDOWN:
      return event.key.keysym.sym;
    case SDL_QUIT:
      exit(3);
    }
  }
  return -1;
}

int gfx_getkey() {
  SDL_Event event;
  while (1) {
    SDL_WaitEvent(&event);
    if (event.type == SDL_KEYDOWN)
      break;
    if (event.type == SDL_QUIT)
      exit(3);
  };
  return event.key.keysym.sym;
}

int gfx_isKeyDown(int key) {
  const Uint8 *keytable;
  int numkeys;
  SDL_PumpEvents();
  SDL_Event event;
  while(SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      exit(3);
    }
  }
  keytable = SDL_GetKeyboardState(&numkeys);
  SDL_Scancode code = SDL_GetScancodeFromKey(key);
  assert(code < numkeys);
  return keytable[code];
}

static void gfx_close() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

int gfx_init() {

  /* Initialize SDL */
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
    return 1;
  }
  atexit(gfx_close);

  window = SDL_CreateWindow("SDL", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                            SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (window == NULL) {
    fprintf(stderr, "Window could not be created! SDL Error: %s\n", SDL_GetError());
    return 2;
  } else {
    // Create renderer for window
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL) {
      renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    }

    if (renderer == NULL) {
      fprintf(stderr,"Renderer could not be created! SDL Error: %s\n", SDL_GetError());
      return 3;
    }
  }

  SDL_Delay(10);
  return 0;
}
