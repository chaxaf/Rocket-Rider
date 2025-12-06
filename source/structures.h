/*
 * structures.h
 *
 *  Created on: Dec 8, 2021
 *      Author: nds
 */

#ifndef STRUCTURES_H_
#define STRUCTURES_H_

#define true 1
#define false 0

typedef enum {ONE, TWO, CUSTOM_PLAY, CUSTOM_MOD} mode;	//different levels
typedef enum {NONE, UP, DOWN, LEFT, RIGHT} direction;	//player moving directions
typedef enum {ON, OFF, UNDEFINED} wifiswitch;			//if wifi is used



typedef struct position
{
	int x;
	int y;
} Pos;

typedef struct level
{
	int obstacles[48];	//array with index as grid coordinate and boolean value for obstacle
	int start;			//start position in grid coordinates
	int finish;			//finishing position in grid coordinates
	int won;
} Level;

typedef struct player
{
	Pos position;		//player position in X-Y-Coordinates (0-255),(0-191)
	int pos;			//player position in block/grid coordinates (0-47)
	direction dir;		//moving direction
	int moves;			//moves left until next place evaluation
} Player;

#endif /* STRUCTURES_H_ */
