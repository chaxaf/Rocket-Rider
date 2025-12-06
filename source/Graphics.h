#ifndef Graphics
#define Graphics

#include <nds.h>
#include "space.h"
#include "space_wifi.h"
#include "LevelDisp.h"
#include "grid.h"
#include "rocketUP.h"
#include "rocketDOWN.h"
#include "rocketRIGHT.h"
#include "rocketLEFT.h"
#include "structures.h"

u16* mapMemoryR;
u16* mapMemoryT;
u16* gfxUP;
u16* gfxDOWN;
u16* gfxRIGHT;
u16* gfxLEFT;



void Graphics_setup_main();

void drawTileSquare32(int x, int tileNumber);

void update_Tiles_Map(Level level);

void wifi_update(wifiswitch wlan);

void update_sub(mode state);

#endif
