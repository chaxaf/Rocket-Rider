/*
 * Audio.c
 *
 *  Created on: Dec 17, 2021
 *      Author: nds
 */

#include "Audio.h"

void Audio_Init()
{
	//Init the sound library
	mmInitDefaultMem((mm_addr)soundbank_bin);
	//Load module
	mmLoad(MOD_SUPERSONIC4);
	//Load effect
	mmLoadEffect(SFX_BOO);
	mmLoadEffect(SFX_AIR_RAID);
	mmLoadEffect(SFX_DONK);
	mmLoadEffect(SFX_APPLAUSE);
	mmLoadEffect(SFX_SWISH);

}

void Audio_PlayMusic()
{
	//Start playing music in a loop
	mmStart(MOD_SUPERSONIC4, MM_PLAY_LOOP);
	//Set module volume to 512 (range 0...1024) using the function mmSetModuleVolume(...)
	mmSetModuleVolume(512);
}

void Audio_PlaySoundEX( int i )
{
	//Declare a sound effect
	mm_sound_effect sound;
	//Set the id of the sound effect with the input parameter
	sound.id = i;
	//Set the rate to the default one (1024)
	sound.rate = 1024;
	//Set the volume to the maximum (range 0...255)
	sound.volume = 255;
	//Set the panning depending on the effect (0-left....255-right)
	// i.e left of right ear
	if(i == SFX_BOO)
		sound.panning = 255;
	if(i == SFX_DONK)
		sound.panning = 255;
	if(i == SFX_APPLAUSE)
			sound.panning = 255;
	if(i == SFX_SWISH)
			sound.panning = 120;
	if(i == SFX_AIR_RAID)
		sound.panning = 120;


	//Play the effect using the sound structure
	mmEffectEx(&sound);
}
