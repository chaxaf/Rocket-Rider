
#include "Graphics.h"

u8 emptyTile[64] =
{
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0
};

u8 obstacleTile[64] =
{
	254,254,254,254,254,254,254,254,
	254,254,254,254,254,254,254,254,
	254,254,254,254,254,254,254,254,
	254,254,254,254,254,254,254,254,
	254,254,254,254,254,254,254,254,
	254,254,254,254,254,254,254,254,
	254,254,254,254,254,254,254,254,
	254,254,254,254,254,254,254,254
};

u8 finishTile[64] =
{
	253,253,253,253,253,253,253,253,
	253,253,253,253,253,253,253,253,
	253,253,253,253,253,253,253,253,
	253,253,253,253,253,253,253,253,
	253,253,253,253,253,253,253,253,
	253,253,253,253,253,253,253,253,
	253,253,253,253,253,253,253,253,
	253,253,253,253,253,253,253,253
};

void Graphics_setup_main()
{
//ROTOSCALE MAIN
	//Enable a proper RAM memory bank for the main engine
	VRAM_A_CR = VRAM_ENABLE //Enable
			| VRAM_A_MAIN_BG; //Bank for the main engine

	//Configure the main engine in mode 5 (2D) and activate Background 2
	REG_DISPCNT = MODE_5_2D | DISPLAY_BG2_ACTIVE;

   //Initialize Background
	BGCTRL[2] = BG_MAP_BASE(0) | BgSize_B8_256x256;

	//Affine Marix Transformation
	REG_BG2PA = 256;
	REG_BG2PC = 0;
	REG_BG2PB = 0;
	REG_BG2PD = 256;

	swiCopy(space_wifiPal, BG_PALETTE, space_wifiPalLen/2);
	swiCopy(space_wifiBitmap, BG_GFX, space_wifiBitmapLen/2);

	//Initialize pointer to the graphic memory
	mapMemoryR = BG_GFX;

//ROTOSCALE SUB
	//Enable a proper RAM memory bank for sub engine
	VRAM_C_CR = VRAM_ENABLE
			| VRAM_C_SUB_BG;
	//Configure the sub engine in mode 5 (2D) and activate Background 2
	REG_DISPCNT_SUB = MODE_5_2D | DISPLAY_BG2_ACTIVE;

	//Configure BG 2 Sub. (Do not forget the BG_BMP_BASE configuration)
	BGCTRL_SUB[2] = BG_BMP_BASE(0) | BgSize_B8_256x256;

	//Affine Marix Transformation
	REG_BG2PA_SUB = 256;
	REG_BG2PC_SUB = 0;
	REG_BG2PB_SUB = 0;
	REG_BG2PD_SUB = 256;

	dmaCopy(LevelDispPal, BG_PALETTE_SUB, LevelDispPalLen);
	dmaCopy(LevelDispBitmap, BG_GFX_SUB, LevelDispBitmapLen);

//TILES MAIN

	//Graphics (Part 3)
	//Enable a proper RAM memory bank for the main engine
	VRAM_D_CR = VRAM_ENABLE | VRAM_D_MAIN_BG; //Bank for the main engine

	//Configure the main engine in mode 0 (2D) and activate BG1 and BG2
	REG_DISPCNT = MODE_5_2D | DISPLAY_BG1_ACTIVE | DISPLAY_BG2_ACTIVE;

	//Configure BG 0 to represent the game field
	BGCTRL[1] = BG_32x32 | BG_COLOR_256 | BG_MAP_BASE(25) | BG_TILE_BASE(4);

	//Copy the empty tile and the full tile to the corresponding RAM location
	//according to the chosen TILE_BASE. If dmaCopy is used, do not forget to
	//cast the destination pointer as a 'byte pointer'
	//Hint: Use the macro BG_TILE_RAM to get the destination address
	dmaCopy(emptyTile, (u8*)BG_TILE_RAM(4), 64);
	dmaCopy(obstacleTile, (u8*)BG_TILE_RAM(4) + 64, 64);
	dmaCopy(finishTile, (u8*)BG_TILE_RAM(4) + 128, 64);

	//Assign components 254 and 255 as explained in the manual
	BG_PALETTE[254] = ARGB16(1,15,10,6);
	BG_PALETTE[253] = ARGB16(0,31,15,0);

	//Set the pointer mapMemory to the RAM location of the chosen MAP_BASE
	mapMemoryT = (u16*)BG_MAP_RAM(25);

	//FILL SCREEN WITH EMPTY TILES
	int x,y;
		for (x=0; x<32; x++){
			for(y=0; y<24; y++){
					mapMemoryT[y*32+x] = 0;
			}
		}


//SPRITE MAIN

	//Set up memory bank to work in sprite mode (offset since we are using VRAM A for backgrounds)
	VRAM_B_CR = VRAM_ENABLE | VRAM_B_MAIN_SPRITE_0x06400000;

	//Initialize sprite manager and the engine
	oamInit(&oamMain, SpriteMapping_1D_32, false);

	//Allocate space for the graphic to show in the sprite
	gfxUP = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
	gfxDOWN = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
	gfxRIGHT = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
	gfxLEFT = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);



	//Copy data for the graphic (palette and bitmap)
	swiCopy(rocketUPPal, SPRITE_PALETTE, rocketUPPalLen/2);
	swiCopy(rocketUPTiles, gfxUP, rocketUPTilesLen/2);

	swiCopy(rocketDOWNPal, SPRITE_PALETTE, rocketDOWNPalLen/2);
	swiCopy(rocketDOWNTiles, gfxDOWN, rocketDOWNTilesLen/2);

	swiCopy(rocketRIGHTPal, SPRITE_PALETTE, rocketRIGHTPalLen/2);
	swiCopy(rocketRIGHTTiles, gfxRIGHT, rocketRIGHTTilesLen/2);

	swiCopy(rocketLEFTPal, SPRITE_PALETTE, rocketLEFTPalLen/2);
	swiCopy(rocketLEFTTiles, gfxLEFT, rocketLEFTTilesLen/2);
}

// GRAPHICS FUNCTIONS

void drawTileSquare32(int x, int tileNumber){	//draws 32x32px square blocks of 16 tiles
	mapMemoryT[4*(x%8)+(x/8)*128] 		= tileNumber;
	mapMemoryT[4*(x%8)+1+(x/8)*128] 	= tileNumber;
	mapMemoryT[4*(x%8)+2+(x/8)*128] 	= tileNumber;
	mapMemoryT[4*(x%8)+3+(x/8)*128] 	= tileNumber;

	mapMemoryT[4*(x%8)+(x/8)*128+32] 	= tileNumber;
	mapMemoryT[4*(x%8)+1+(x/8)*128+32] 	= tileNumber;
	mapMemoryT[4*(x%8)+2+(x/8)*128+32] 	= tileNumber;
	mapMemoryT[4*(x%8)+3+(x/8)*128+32] 	= tileNumber;

	mapMemoryT[4*(x%8)+(x/8)*128+64] 	= tileNumber;
	mapMemoryT[4*(x%8)+1+(x/8)*128+64] 	= tileNumber;
	mapMemoryT[4*(x%8)+2+(x/8)*128+64] 	= tileNumber;
	mapMemoryT[4*(x%8)+3+(x/8)*128+64] 	= tileNumber;

	mapMemoryT[4*(x%8)+(x/8)*128+96]	= tileNumber;
	mapMemoryT[4*(x%8)+1+(x/8)*128+96] 	= tileNumber;
	mapMemoryT[4*(x%8)+2+(x/8)*128+96] 	= tileNumber;
	mapMemoryT[4*(x%8)+3+(x/8)*128+96] 	= tileNumber;
}

void update_Tiles_Map(Level level){	//removes existing tiles from map and redraws new set of tiles
	int x,y;
	for (x=0; x<32; x++){
		for(y=0; y<24; y++){
			mapMemoryT[y*32+x] = 0;
		}
	}
	for(x=0; x< 48; x++){
		if(level.obstacles[x]){
			drawTileSquare32(x,1);
		}
	}
	drawTileSquare32(level.finish,2);
}

void wifi_update(wifiswitch wlan){	//changes to normal background after wifi ON/OFF is chosen
		swiCopy(spacePal, BG_PALETTE, spacePalLen/2);
		swiCopy(spaceBitmap, BG_GFX, spaceBitmapLen/2);
		BG_PALETTE[253] = ARGB16(0,31,15,0);
}


void update_sub(mode state){		// changes between level view or grid view of SUB depending on mode
	if(state == CUSTOM_MOD){
		dmaCopy(gridPal, BG_PALETTE_SUB, gridPalLen);
		dmaCopy(gridBitmap, BG_GFX_SUB, gridBitmapLen);
	}
	else{
		dmaCopy(LevelDispPal, BG_PALETTE_SUB, LevelDispPalLen);
		dmaCopy(LevelDispBitmap, BG_GFX_SUB, LevelDispBitmapLen);
	}
}
