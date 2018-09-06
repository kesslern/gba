#include <tonc_types.h>

#define CLR_BLACK   0x0000
#define CLR_RED     0x001F
#define CLR_LIME    0x03E0
#define CLR_YELLOW  0x03FF
#define CLR_BLUE    0x7C00
#define CLR_MAG     0x7C1F
#define CLR_CYAN    0x7FE0
#define CLR_WHITE   0x7FFF

#define REG_VCOUNT2 *(vu16*)0x04000006

void vid_vsync2()
{
  while(REG_VCOUNT2 >= 160);   // wait till VDraw
  while(REG_VCOUNT2 < 160);    // wait till VBlank
}

int main(void) {
  u16* DISPCNT = (u16*) 0x04000000;
  u16* PALETTE = (u16*) 0x05000000;
  u16* FRAME_1 = (u16*) 0x06000000;
  u16* FRAME_2 = (u16*) 0x0600A000;

  PALETTE[0] = CLR_BLUE;
  PALETTE[1] = CLR_RED;
  PALETTE[2] = CLR_YELLOW;
  *DISPCNT = 0x0414;

  for (int i = 0; i < 10; i++) {
    FRAME_1[10+i] = (u16) 0x0202;
  }
  for (int i = 0; i < 10; i++) {
    FRAME_2[10+i] = (u16) 0x0101;
  }

  while(1) {
    vid_vsync2();
    vid_vsync2();
    vid_vsync2();

    *DISPCNT ^= 0x0010;
  }

  return 0;
}


