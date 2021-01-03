#pragma output REGISTER_SP = 0xD000

#include <input.h>
#include <sound.h>
#include <arch/zx.h>
#include <arch/zx/sp1.h>
#include <intrinsic.h>
#include <z80.h>
#include <im2.h>
#include <string.h>
#include <stdlib.h>

IM2_DEFINE_ISR(isr) {}
#define TABLE_HIGH_BYTE        ((unsigned int)0xD0)
#define JUMP_POINT_HIGH_BYTE   ((unsigned int)0xD1)

#define UI_256                 ((unsigned int)256)
#define TABLE_ADDR             ((void*)(TABLE_HIGH_BYTE*UI_256))
#define JUMP_POINT             ((unsigned char*)( (unsigned int)(JUMP_POINT_HIGH_BYTE*UI_256) + JUMP_POINT_HIGH_BYTE ))

extern uint8_t sprite_protar1[];
extern uint8_t sprite_protar2[];
extern uint8_t sprite_protar3[];

const uint8_t test[] = {0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55};

struct sp1_Rect full_screen = {0, 0, 32, 24};
char * chars = "0000000\0";

void print_points(uint16_t points, uint8_t row, uint8_t col) {
    uint8_t idx;
    utoa(points, chars, 10);
    col = 5 - strlen(chars);

    if(col != 0) {
        for(idx = 0; idx != 5; ++idx) {
            sp1_PrintAtInv(0, 25 + idx, INK_CYAN | PAPER_BLACK, '0');
        }
    }
    idx = 0;
    while(chars[idx] != '\0') {
        sp1_PrintAtInv(0, 25 + idx + col, INK_CYAN | PAPER_BLACK, chars[idx]);
        ++idx;
    }

}

int main()
{
  struct sp1_ss  *circle_sprite;
  uint16_t x, y;

  memset( TABLE_ADDR, JUMP_POINT_HIGH_BYTE, 257 );
  z80_bpoke( JUMP_POINT,   195 );
  z80_wpoke( JUMP_POINT+1, (unsigned int)isr );
  im2_init( TABLE_ADDR );
  intrinsic_ei();

  zx_border(INK_BLACK);

  sp1_Initialize( SP1_IFLAG_MAKE_ROTTBL | SP1_IFLAG_OVERWRITE_TILES | SP1_IFLAG_OVERWRITE_DFILE,
                  INK_BLACK | PAPER_CYAN,
                  ' ' );
  sp1_TileEntry('A', test);

  sp1_Invalidate(&full_screen);

  for (x = 0; x <32; ++x) {
    for(y = 0; y <24; ++y) {
        sp1_PrintAt(y, x, INK_BLACK | PAPER_CYAN, 'A');
    }
  }


  circle_sprite = sp1_CreateSpr(SP1_DRAW_MASK2LB, SP1_TYPE_2BYTE, 4, 0, 1);
  sp1_AddColSpr(circle_sprite, SP1_DRAW_MASK2,    SP1_TYPE_2BYTE, 64*9, 1);
  sp1_AddColSpr(circle_sprite, SP1_DRAW_MASK2,    SP1_TYPE_2BYTE, 128*9, 1);
  sp1_AddColSpr(circle_sprite, SP1_DRAW_MASK2RB,  SP1_TYPE_2BYTE, 0, 1);

  uint16_t  frame = 16;
  x=5;
  while(1) {
    sp1_MoveSprPix(circle_sprite, &full_screen,(int)sprite_protar1+frame - 16, x, 5);
	sp1_UpdateNow();
    intrinsic_halt();
    intrinsic_halt();
	intrinsic_halt();

    if(in_key_pressed(IN_KEY_SCANCODE_0)) {
        frame = 16;
    } else if(in_key_pressed(IN_KEY_SCANCODE_1)) {
        frame += 64;
    } else if(in_key_pressed(IN_KEY_SCANCODE_2)) {
        ++x;
        if(frame == 16) {
            frame = 80;
        }  else {
            frame = 16;
        }
    }

    if(frame == 528) {
        frame = 16;
    }

    if(x > 100) {
        x = 0;
    }
    print_points(frame, 20, 20);

    in_wait_nokey();
  }
}
