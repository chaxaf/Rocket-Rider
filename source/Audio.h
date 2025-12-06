/*
 * Audio.h
 *
 *  Created on: Dec 17, 2021
 *      Author: nds
 */

#ifndef AUDIO_H_
#define AUDIO_H_

#pragma once

#include <nds.h>
#include <maxmod9.h>
#include "soundbank.h"
#include "soundbank_bin.h"

void Audio_Init();
void Audio_PlaySoundEX( int i );
void Audio_PlayMusic();


#endif /* AUDIO_H_ */
