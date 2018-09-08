#include <tonc_types.h>

#define CLR_BLACK   0x0000
#define CLR_RED     0x001F
#define CLR_LIME    0x03E0
#define CLR_YELLOW  0x03FF
#define CLR_BLUE    0x7C00
#define CLR_MAG     0x7C1F
#define CLR_CYAN    0x7FE0
#define CLR_WHITE   0x7FFF

#define REG_VCOUNT *(vu16*) 0x04000006
#define REG_DISPCNT *(u16*) 0x04000000
#define PALETTE     ((u16*) 0x05000000)
#define VIDMEM      ((u16*) 0x06000000)

#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 160

#define BALL_WIDTH  5
#define BALL_HEIGHT 5

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

void rect(int x, int y, u16 color) {
  for (int ix = 0; ix < 5; ix++) {
    for (int iy = 0; iy < 5; iy++) {
      m3_plot(x+ix, y+iy, color);
    }
  }
}

int main(void) {
  REG_DISPCNT = 0x0403;

  while(1) {
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

    rect(x_loc, y_loc, CLR_WHITE);
    vid_vsync();
    rect(x_loc, y_loc, CLR_BLACK);
  }

  return 0;
}
