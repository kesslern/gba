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
void vid_vsync()
{
  while(REG_VCOUNT >= 160);   // wait till VDraw
  while(REG_VCOUNT < 160);    // wait till VBlank
}

int main(void) {
  u16* FRAME_1 = (u16*) 0x06000000;
  u16* FRAME_2 = (u16*) 0x0600A000;

  PALETTE[0] = CLR_BLUE;
  PALETTE[1] = CLR_RED;
  PALETTE[2] = CLR_YELLOW;
  REG_DISPCNT = 0x0414;

  for (int i = 0; i < 10; i++) {
    FRAME_1[10+i] = (u16) 0x0202;
  }
  for (int i = 0; i < 10; i++) {
    FRAME_2[10+i] = (u16) 0x0101;
  }

  while(1) {
    vid_vsync();
    vid_vsync();
    vid_vsync();

    REG_DISPCNT ^= 0x0010;
  }

  return 0;
}


