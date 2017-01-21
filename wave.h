/***************************************************************************
 *
 *  Copyright (C) 1996 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       wave.h
 *  Content:    DirectSound WAV routines include file
 *
 ***************************************************************************/

#pragma once

#include <dsound.h>

void LoadWave(HINSTANCE hinst, int ResourceID,
			  LPDIRECTSOUND lpds,
			  LPDIRECTSOUNDBUFFER &lpDSB);