/*
 * gamedynamics.h
 *
 *  Created on: Dec 8, 2021
 *      Author: nds
 */

#ifndef GAMEDYNAMICS_H_
#define GAMEDYNAMICS_H_

#include <nds.h>
#include <stdio.h>
#include "structures.h"
#include "Audio.h"
#include "Graphics.h"





void change_obstacles(int x, int y,int obst[48]);

int collision(Level *level, Player *player, direction dir);

int outOfBounds(Player *player);

void player_move(Player *player);

u16* player_update(Level *level, Player *player, int *sx, int *sy, u16* gfx);

void displaceStartFinish(Level *level,int *startFin);

void level_won();

#endif /* GAMEDYNAMICS_H_ */
