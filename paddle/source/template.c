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


int paddle_x = 0;
int x_loc = 0;
int y_loc = 0;
int x_dir = 1;
int y_dir = 1;

void vid_vsync() {
  while(REG_VCOUNT >= 160);   // wait till VDraw
  while(REG_VCOUNT < 160);    // wait till VBlank
}

void m3_plot(int x, int y, u16 color) {
  VIDMEM[y*240+x] = color;
}

void draw_ball(int x_pos, int y_pos) {
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
  for (int ix = 0; ix < width; ix++) {
    for (int iy = 0; iy < height; iy++) {
      m3_plot(x+ix, y+iy, color);
    }
  }
}

void handle_input() {
  u16 input = ~KEYINPUT;
  if (input & KEY_RIGHT) paddle_x+=2;
  if (input & KEY_LEFT)  paddle_x-=2;
}

void frame_tick() {
  x_loc += x_dir;
  y_loc += y_dir;
  if (x_loc == SCREEN_WIDTH - BALL_WIDTH) {
    x_dir = -1;
  } else if (x_loc == 0) {
    x_dir = 1;
  }
  if (y_loc == SCREEN_HEIGHT - BALL_HEIGHT) {
    y_dir = -1;
  } else if (y_loc == 0) {
    y_dir = 1;
  }
}

void draw_frame() {
  draw_ball(x_loc, y_loc);
  rect(paddle_x, PADDLE_Y, PADDLE_WIDTH, PADDLE_HEIGHT, CLR_WHITE);
}

void clean_frame() {
  rect(x_loc, y_loc, BALL_WIDTH, BALL_HEIGHT, CLR_BLACK);
  rect(paddle_x, PADDLE_Y, PADDLE_WIDTH, PADDLE_HEIGHT, CLR_BLACK);
}

int main(void) {
  REG_DISPCNT = 0x0403;

  while(!(~KEYINPUT & KEY_START));

  while(1) {
    handle_input();
    frame_tick();
    draw_frame();
    vid_vsync();
    clean_frame();
  }

  return 0;
}
