#include "gamedynamics.h"

/* For the whole game 2 coordinate systems are used:
 * => X-Y-Coordinates X(0-255), Y(0-191)
 * => Block/grid position from 0 to 47 (8 lines x 6 columns of 32x32p)
 *
 * obstacles are saved as boolean values in an array of length 48 with index as grid position
 *
 *
 */


// if Touchscreen grid is touched in CUSTOM_MOD an obstacle at that place will be added/removed
void change_obstacles(int x, int y,int obst[48]){
	if ((x != 0) && (y != 0)){
		int ie = x/32+8*(y/32);
		if (obst[ie] == 1) obst[ie] = 0;
		else if(obst[ie] == 0) obst[ie] = 1;
	}
}

int collision(Level *level, Player *player, direction dir){	//checks for collsion of player with obstacle
	if(dir == UP && player->pos > 7){					//UP and not on highest line
		if(level->obstacles[player->pos-8] == 1) {
			return 1;}
		else return 0;
	}else if(dir == DOWN && player->pos < 40){			//DOWN and not on lowest line
		if(level->obstacles[player->pos+8] == 1) return 1;
		else return 0;
	}else if(dir == LEFT && (player->pos % 8 >0)){		//LEFT and not on most left column
		if(level->obstacles[player->pos-1] == 1) return 1;
		else return 0;
	}else if(dir == RIGHT && (player->pos % 8 <7)){		//RIGHT and not on most right column
		if(level->obstacles[player->pos+1] == 1) return 1;
		else return 0;
	}
	return 0;
}


int outOfBounds(Player *player){		// checks if player leaves map
	if((player->position.x > 274) || (player->position.x < -32) ||
		(player->position.y > 220) || (player->position.y < -32)){
		Audio_PlaySoundEX( SFX_BOO );
		return 1;
	} else return 0;
}


void player_move(Player *player){		//moving the player around
		player->moves -= 1;
		if(player->dir == UP) player->position.y -=4;
		else if(player->dir == DOWN) player->position.y +=4;
		else if(player->dir == RIGHT) player->position.x +=4;
		else if(player->dir == LEFT) player->position.x -=4;
}


/* If player is moving, it moves 8 times until it is on next block position in grid.
 * From every position in grid collision is checked and the player either stops or keeps moving another block to next position in grid.
 *
 */

u16* player_update(Level *level, Player *player, int *sx, int *sy, u16* gfx) {

	*sx = player->position.x;
	*sy = player->position.y;

	if (player->moves == 0){
		player->pos = (*sx+16)/32+8*((*sy+16)/32);			//update player position from X-Y-Coordinates to grid coordinates (0-47)

		if (player->pos == level->finish) {					//winning condition
			level->won = 1;
		}
		if((keysDown() & (KEY_UP)) || (player->dir == UP)){
			if(collision(level, player,UP) == 0){			//player moves if no collision
				player->moves = 8;
				player->dir = UP;
				return gfxUP;
			} else{
				Audio_PlaySoundEX( SFX_DONK ); 				//collision sound
				player->dir = NONE;
			}

		}else if((keysDown() & (KEY_DOWN)) || (player->dir == DOWN)){
			if(collision(level, player,DOWN) == 0){
				player->moves = 8;
				player->dir = DOWN;
				return gfxDOWN;
			}else{
				Audio_PlaySoundEX( SFX_DONK );
				player->dir = NONE;
			}

		}else if((keysDown() & (KEY_RIGHT))  || (player->dir == RIGHT)){
			if(collision(level, player,RIGHT) == 0){
				player->moves = 8;
				player->dir = RIGHT;
				return gfxRIGHT;
			}else{
				Audio_PlaySoundEX( SFX_DONK );
				player->dir = NONE;
			}

		}else if((keysDown() & (KEY_LEFT)) || (player->dir == LEFT)){
			if(collision(level, player,LEFT) == 0){
				player->moves = 8;
				player->dir = LEFT;
				return gfxLEFT;
			}else{
				Audio_PlaySoundEX( SFX_DONK );
				player->dir = NONE;
			}

		}else return gfx;
	}else player_move(player);
	return gfx;
}


// function for moving start and finish postions in custom modification mode

void displaceStartFinish(Level *level,int *startFin){
	if(*startFin == 0){							//START OR FINISH DISPLACEMENT
		if (keysDown() & (KEY_LEFT)){
			if (level->start % 8 != 0) level->start -= 1; //ONLY IF START NOT ON LEFT BORDER
		}else if (keysDown() & (KEY_RIGHT)){
			if (level->start % 8 != 7) level->start += 1;
		}else if (keysDown() & (KEY_UP)){
			if (level->start > 7) level->start -= 8;
		}else if (keysDown() & (KEY_DOWN)){
			if (level->start < 40) level->start += 8;
		}
	}else if (*startFin == 1){
		if (keysDown() & (KEY_LEFT)){
			if (level->finish % 8 != 0) level->finish -= 1; //ONLY IF FINISH NOT ON LEFT BORDER
		}else if (keysDown() & (KEY_RIGHT)){
			if (level->finish % 8 != 7) level->finish += 1;
		}else if (keysDown() & (KEY_UP)){
			if (level->finish > 7) level->finish -= 8;
		}else if (keysDown() & (KEY_DOWN)){
			if (level->finish < 40) level->finish += 8;
		}
	}
}


void level_won(){
	Audio_PlaySoundEX( SFX_APPLAUSE );
}
