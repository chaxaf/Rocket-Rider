/*
 * memory.c
 *
 *  Created on: Dec 18, 2021
 *      Author: nds
 */
#include <fat.h>
#include "memory.h"
#include <stdio.h>

// write custom mode in memory
void writeCustomLevel(int start, int finish, int obst[48])
{
	//Open the file in write mode to overwrite
	FILE* file = fopen("/CustomLevel.txt","w+");
	if(file != NULL)
	{
		int i;
		for(i=0; i<48; i++){
		fprintf(file,"%i\n",obst[i]);
		}
		//SAVE START AND FINISH
		fprintf(file,"%i\n", start);
		fprintf(file,"%i\n", finish);
		//Close the file
		fclose(file);
	}
}

//READ CUSTOM LEVEL FROM MEMORY
void readCustomLevel(int* start, int* finish, int obst[48])
{
	//Open the file in read mode
	FILE* file = fopen("/CustomLevel.txt","r");
	if(file != NULL)
	{
		int i;
		for(i=0; i<48; i++){
		//Read the value and put it into the variable obstacle
		fscanf(file,"%i\n",&obst[i]);
		}
		//READ START AND FINISH
		fscanf(file,"%i\n",start);
		fscanf(file,"%i\n",finish);
		// close the file
		fclose(file);
	}
}
