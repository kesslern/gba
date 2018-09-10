#include <tonc_types.h>

#define CLR_BLACK   0x0000
#define CLR_RED     0x001F
#define CLR_LIME    0x03E0
#define CLR_YELLOW  0x03FF
#define CLR_BLUE    0x7C00
#define CLR_MAG     0x7C1F
#define CLR_CYAN    0x7FE0
#define CLR_WHITE   0x7FFF

#define CLR_GRAY (u16) ((245) << 10 | (245) << 5 | (245))

#define REG_VCOUNT *(vu16*) 0x04000006
#define REG_DISPCNT *(u16*) 0x04000000
#define PALETTE     ((u16*) 0x05000000)
#define VIDMEM      ((u16*) 0x06000000)
#define KEYINPUT   (*(vu16*) 0x04000130)

#define KEY_START (1UL << 3)
#define KEY_RIGHT (1UL << 4)
#define KEY_LEFT  (1UL << 5)
#define KEY_DOWN  (1UL << 6)
#define KEY_UP    (1UL << 7)

#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 160

#define BALL_WIDTH  5
#define BALL_HEIGHT 5

#define PADDLE_WIDTH  20
#define PADDLE_HEIGHT 3
#define PADDLE_Y      150

struct ball_type {
  int x_loc;
  int y_loc;
  int x_dir;
  int y_dir;
};

struct ball_type ball;

int paddle_x;
bool game_over;

void vid_vsync() {
  while(REG_VCOUNT >= SCREEN_HEIGHT);   // wait till VDraw
  while(REG_VCOUNT < SCREEN_HEIGHT);    // wait till VBlank
}

void m3_plot(int x, int y, u16 color) {
  VIDMEM[y*SCREEN_WIDTH+x] = color;
}

void draw_ball() {
  int x_pos = ball.x_loc;
  int y_pos = ball.y_loc;

  int row0 = (y_pos+0)*SCREEN_WIDTH;
  int row1 = (y_pos+1)*SCREEN_WIDTH;
  int row2 = (y_pos+2)*SCREEN_WIDTH;
  int row3 = (y_pos+3)*SCREEN_WIDTH;
  int row4 = (y_pos+4)*SCREEN_WIDTH;

  VIDMEM[row0+1+x_pos] = CLR_GRAY;
  VIDMEM[row0+2+x_pos] = CLR_WHITE;
  VIDMEM[row0+3+x_pos] = CLR_GRAY;

  VIDMEM[row1+0+x_pos] = CLR_GRAY;
  VIDMEM[row1+1+x_pos] = CLR_WHITE;
  VIDMEM[row1+2+x_pos] = CLR_WHITE;
  VIDMEM[row1+3+x_pos] = CLR_WHITE;
  VIDMEM[row1+4+x_pos] = CLR_GRAY;

  VIDMEM[row2+0+x_pos] = CLR_WHITE;
  VIDMEM[row2+1+x_pos] = CLR_WHITE;
  VIDMEM[row2+2+x_pos] = CLR_WHITE;
  VIDMEM[row2+3+x_pos] = CLR_WHITE;
  VIDMEM[row2+4+x_pos] = CLR_WHITE;

  VIDMEM[row4+1+x_pos] = CLR_GRAY;
  VIDMEM[row4+2+x_pos] = CLR_WHITE;
  VIDMEM[row4+3+x_pos] = CLR_GRAY;

  VIDMEM[row3+0+x_pos] = CLR_GRAY;
  VIDMEM[row3+1+x_pos] = CLR_WHITE;
  VIDMEM[row3+2+x_pos] = CLR_WHITE;
  VIDMEM[row3+3+x_pos] = CLR_WHITE;
  VIDMEM[row3+4+x_pos] = CLR_GRAY;
}

void rect(int x, int y, int width, int height, u16 color) {
  for (int ix = 0; ix < width; ix++)
    for (int iy = 0; iy < height; iy++)
      m3_plot(x+ix, y+iy, color);
}

void handle_input() {
  u16 input = ~KEYINPUT;
  if (input & KEY_RIGHT) paddle_x+=2;
  if (input & KEY_LEFT)  paddle_x-=2;
}

void move_ball() {
  ball.x_loc += ball.x_dir;
  ball.y_loc += ball.y_dir;

  /* Right side screen collision */
  if (ball.x_loc == SCREEN_WIDTH - BALL_WIDTH)
    ball.x_dir = -1;
  /* Left side screen collision */
  else if (ball.x_loc == 0)
    ball.x_dir = 1;

  /* Bottom side screen collision */
  if (ball.y_loc == SCREEN_HEIGHT - BALL_HEIGHT)
    game_over = true;
  /* Top side screen collision */
  else if (ball.y_loc == 0)
    ball.y_dir = 1;
}

void init_game() {
  game_over = false;
  ball.x_loc = 0;
  ball.y_loc = 0;
  ball.x_dir = 1;
  ball.y_dir = 1;
  paddle_x = (SCREEN_WIDTH - PADDLE_WIDTH) / 2;
}

bool check_paddle_collision() {
  bool movingDown = ball.y_dir == 1;
  int ball_min_x = ball.x_loc;
  int ball_max_x = ball.x_loc + BALL_WIDTH;
  int ball_max_y = ball.y_loc + BALL_HEIGHT;
  int bounce_range = paddle_x + 2 * BALL_WIDTH;
  int bounce_min = paddle_x - BALL_WIDTH;
  int bounce_max = bounce_min + bounce_range;

  return movingDown
    && ball_min_x >= bounce_min
    && ball_max_x <= bounce_max
    && ball_max_y == PADDLE_Y + 1;
}

void frame_tick() {
  move_ball();
  if (check_paddle_collision()) {
    ball.y_dir = -1;
  }
}

void draw_frame() {
  draw_ball();
  /* Draw the paddle */
  rect(paddle_x, PADDLE_Y, PADDLE_WIDTH, PADDLE_HEIGHT, CLR_WHITE);
  rect(paddle_x, PADDLE_Y+PADDLE_HEIGHT-1, PADDLE_WIDTH, 1, CLR_GRAY);
  rect(paddle_x+PADDLE_WIDTH-1, PADDLE_Y, 1, PADDLE_HEIGHT, CLR_GRAY);
}

void clean_frame() {
  /* Black box over the current ball */
  rect(ball.x_loc, ball.y_loc, BALL_WIDTH, BALL_HEIGHT, CLR_BLACK);
  /* Black box over the paddle */
  rect(paddle_x, PADDLE_Y, PADDLE_WIDTH, PADDLE_HEIGHT, CLR_BLACK);
}

int main(void) {
  REG_DISPCNT = 0x0403;


  while (1){
    while(!(~KEYINPUT & KEY_START));
    init_game();

    while(!game_over) {
      handle_input();
      frame_tick();
      draw_frame();
      vid_vsync();
      clean_frame();
    }
  }

  return 0;
}
