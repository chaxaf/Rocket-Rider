#include <nds.h>
#include <stdio.h>
#include <fat.h>
#include "Graphics.h"
#include "gamedynamics.h"
#include "Audio.h"
#include "memory.h"
#include "Wifi_minilib.h"


// SOME GLOBAL VARIABLES

wifiswitch wlan = UNDEFINED;	 	//for choosing if wifi in use
mode state = ONE;					//actual level
mode lastState;						//last level
touchPosition touch;

Level l1;		//level 1
Level l2;		//level 2
Level cust;		//custom level
Level received;	//level received from wifi

Player player1;	//player level 1
Player player2;	//player level 2
Player playerc; //player custom level

int sx = 4;		//sprite position
int sy = 4;

int startFin = 0;	 //if start or finish is being moved
int lastX;			//last touched touchscreen position for not touching twice
int lastY;
u16* gfx;			//sprite to draw
int playwin = 0;	//for player winning function only called once
int newTryCount = 0;//IRQ counter for restarting level


// interrupt timer handler
void timer_newTry_ISR();

// MAIN FUNCTION DECLARATIONS
void game_handler(mode state);
void level_handler();
void gameInit();
void gameRestart();
void sendMessage();
void receiveMessage();



int main(void) {
//Timer IRQ initialisation
	TIMER_DATA(0) = TIMER_FREQ_64(10);
	TIMER0_CR = TIMER_DIV_64 | TIMER_IRQ_REQ | TIMER_ENABLE;
	irqSet(IRQ_TIMER0, &timer_newTry_ISR);
	irqEnable(IRQ_TIMER0);
//Memory Init
	fatInitDefault();
//Graphics/Level/Audio Init
	Graphics_setup_main();
	gameInit();
	gameRestart();
	Audio_Init();
	Audio_PlayMusic();

	while (wlan == UNDEFINED){	//Play with or without wifi
		scanKeys();
		if (keysDown() & (KEY_A)) wlan = ON;
		else if (keysDown() & (KEY_B)) wlan = OFF;
	}
	wifi_update(wlan);

	if( wlan == ON){			//if playing with wifi initialize wifi
		if (!initWiFi() || !openSocket()){
			while(1){
				consoleDemoInit();
				printf("WIFI ERROR");
			}
		}
	}

	while(1){		//Game loop
    	scanKeys();
        touchRead(&touch);
        game_handler(state);
        level_handler();
        oamSet(&oamMain, 	// oam handler
                		0,				// Number of sprite
                		sx, sy,			// Coordinates
                		0,				// Priority
                		0,				// Palette to use
                		SpriteSize_32x32,			// Sprite size
                		SpriteColorFormat_256Color,	// Color format
                		gfx,			// Loaded graphic to display
                		-1,				// Affine rotation to use (-1 none)
                		false,			// Double size if rotating
                		false,			// Hide this sprite
                		false, false,	// Horizontal or vertical flip
                		false			// Mosaic
                		);

		oamUpdate(&oamMain);
		swiWaitForVBlank();
	}
}
void gameInit(){ //initialize levels obstacles map and players to 0
	//put everything to 0
	int i;
	for(i = 0; i < 48; i++){
		l1.obstacles[i] = 0;
		l2.obstacles[i] = 0;
		cust.obstacles[i] = 0;
	}
	cust.won = l1.won = l2.won = 0;

	//charge levels
	l1.obstacles[5]=l1.obstacles[15]=l1.obstacles[25]=l1.obstacles[38]=1;
	l1.start = 13;
	l1.finish = 2;

	l2.obstacles[4]=l2.obstacles[15]=l2.obstacles[19]=l2.obstacles[24]=l2.obstacles[34]=l2.obstacles[46]=1;
	l2.finish = 1;

	readCustomLevel(&cust.start, &cust.finish, cust.obstacles); //load saved custom level
	playerc.position.x = cust.start%8*32;
	playerc.position.y = cust.start/8*32;
}

void gameRestart(){	//restart level from starting position

	player1.position.x = 160;
	player1.position.y = 32;
	player1.moves = 0;
	player1.dir = NONE;


	player2.position.x = 128;
	player2.position.y = 64;
	player2.moves = 0;
	player2.dir = NONE;

	playerc.position.x = cust.start%8*32;
	playerc.position.y = cust.start/8*32;
	playerc.moves = 0;
	playerc.dir = NONE;

	playwin = 0;
	newTryCount = 0;
}


void game_handler(mode state){	//handles whole game

	if (wlan == ON) receiveMessage();

	if(state==ONE){								//level 1
		update_Tiles_Map(l1);
		if(lastState != state){		//load new map only if level changed
			update_sub(state);
			l1.won = 0;
		}
		if (l1.won == 1){			//stop moving if level won
			if (playwin == 0){
				level_won();
				playwin = 1;
			}
		} else {					//update player position
		gfx =  player_update(&l1, &player1, &sx, &sy, gfx);
		}
		if((lastState != state) ||(keysDown() & (KEY_START)) || outOfBounds(&player1)) {
			gameRestart();}

		lastState = state;

	} else if(state == TWO){					//level 2
		update_Tiles_Map(l2);
		if(lastState != state){
			update_sub(state);
			l2.won = 0;
		}
		if (l2.won == 1){
			if (playwin == 0){
				level_won();
				playwin = 1;
			}
		} else {
		gfx =  player_update(&l2, &player2, &sx, &sy, gfx);
		}
		if((lastState != state) ||(keysDown() & (KEY_START)) || outOfBounds(&player2)) gameRestart();

		lastState = state;

	} else if(state == CUSTOM_PLAY){			//Custom level mode: PLAY
		update_Tiles_Map(cust);
		if(lastState != state){
			update_sub(state);
			cust.won = 0;
		}
		if (cust.won == 1) {
			if (playwin == 0){
				level_won();
				playwin = 1;
			}
		} else {
		gfx =  player_update(&cust, &playerc, &sx, &sy, gfx);
		}
		if((lastState != state) ||(keysDown() & (KEY_START)) || outOfBounds(&playerc)) gameRestart();
		//PRESS Y TO LOAD AND PLAY CUSTOM MODE
		if(keysDown() & (KEY_Y)) {					//load from saved file
			//READ CUSTOM_MOD FROM MEMORY AND REPLACE
			readCustomLevel(&cust.start, &cust.finish, cust.obstacles);
			playerc.position.x = cust.start%8*32;
			playerc.position.y = cust.start/8*32;
		}else if(keysDown() & (KEY_X)) {			//save
			// WRITE CUSTOM_MOD IN MEMORY
			writeCustomLevel(cust.start, cust.finish, cust.obstacles);
		}

		lastState = state;

	} else if(state == CUSTOM_MOD){			//Custom level mode: Modifying
		//CUSTOM_MOD
		if(wlan == ON){				//send own custom level
			if (keysDown() & (KEY_L)){
				sendMessage();
			}
		}
		if(lastX == 0 && lastY ==0){//modify custom obstacles map only if touchscreen is NEWLY pressed
			change_obstacles((int) touch.px,(int) touch.py, cust.obstacles);
		}
		lastX = (int) touch.px;
		lastY = (int) touch.py;
		update_Tiles_Map(cust);
		if(keysDown() & (KEY_A)){	//switch between moving Player and Finish
			if(startFin == 1) startFin = 0;
			else startFin = 1;
		}
		if(keysDown() & (KEY_Y)) { 		// Load level from saved file
			readCustomLevel(&cust.start, &cust.finish, cust.obstacles);
		}else if(keysDown() & (KEY_X)) {// Save level to file
			writeCustomLevel(cust.start, cust.finish, cust.obstacles);

		}else if((keysDown() & (KEY_R)) && wlan == ON) {//Load received custom level of somebody else
			cust.start = received.start;
			cust.finish = received.finish;
			int i;
			for (i=0;i<48;i++){
				cust.obstacles[i] = received.obstacles[i];
			}
		}

		displaceStartFinish(&cust, &startFin);	//move start and finish position
		playerc.position.x = cust.start%8*32;	//update player X-Y-Coordinates from grid coordinates
		playerc.position.y = cust.start/8*32;
		sx = playerc.position.x;		//update sprite position
		sy = playerc.position.y;
		if(lastState != state) update_sub(state);
		lastState = state;
	}
}

void level_handler(){	//updating in which level we are right now
	if(state != CUSTOM_MOD && touch.px != 0 && touch.py != 0){
		if(touch.py < 56) state = ONE;
		else if (touch.py < 118) state = TWO;
		else state = CUSTOM_PLAY;
	}
	if((state == CUSTOM_PLAY) && (keysDown() & (KEY_SELECT))) state = CUSTOM_MOD;
	else if((state == CUSTOM_MOD) && (keysDown() & (KEY_SELECT))) state = CUSTOM_PLAY;
}

void sendMessage(){		//send custom level per wifi
	int msg[8];
	int i;

	for (i=0;i<6;i++){ // COMPRESSION from 50 informaiton bytes to 8 information bytes for sending
		msg[i] = (	cust.obstacles[8*i  ]      | cust.obstacles[8*i+1] << 1 |
					cust.obstacles[8*i+2] << 2 | cust.obstacles[8*i+3] << 3 |
					cust.obstacles[8*i+4] << 4 | cust.obstacles[8*i+5] << 5 |
					cust.obstacles[8*i+6] << 6 | cust.obstacles[8*i+7] << 7 );

	}
	msg[6] = cust.start;
	msg[7] = cust.finish;

	sendData(msg, 32);
}

void receiveMessage(){		//receive custom level per wifi
	int msg[8];
	int i;
	//for (i=0;i<50;i++) msg[i] = 0;

	if(receiveData(&msg,32)>0){
		for (i=0;i<6;i++){	// DECOMPRESSION from 8 information bytes to 50 information bytes after receiving
			received.obstacles[8*i]   = msg[i]    & 00000001;
			received.obstacles[8*i+1] = msg[i]>>1 & 00000001;
			received.obstacles[8*i+2] = msg[i]>>2 & 00000001;
			received.obstacles[8*i+3] = msg[i]>>3 & 00000001;
			received.obstacles[8*i+4] = msg[i]>>4 & 00000001;
			received.obstacles[8*i+5] = msg[i]>>5 & 00000001;
			received.obstacles[8*i+6] = msg[i]>>6 & 00000001;
			received.obstacles[8*i+7] = msg[i]>>7 & 00000001;
		}

		received.start = msg[6];
		received.finish = msg[7];
	}
}

void timer_newTry_ISR(){ //IRQ for restarting level if user is too slow
	if( state == CUSTOM_MOD){
		newTryCount = 0;
		BG_PALETTE[254] = ARGB16(1,15,10,6);
	}else{
		newTryCount += 1;
		if (newTryCount >= 100) {
			BG_PALETTE[254] = ARGB16(1,15,0,0);	//obstacles become red when only 2s left
			if(newTryCount >= 120) gameRestart();
		} else BG_PALETTE[254] = ARGB16(1,15,10,6);
	}

}
