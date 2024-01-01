/*
 *	uvk5-fw
 *	/app/satelite.h
 *	This file is distributed under Apache License Version 2.0
 *	Copyright (c) 2024 Yao Zi. All rights reserved.
 */

#ifndef __APP_SATELITE_H_INC__
#define __APP_SATELITE_H_INC__

#include<stdbool.h>
#include<stdint.h>

extern bool gSateliteMode;
extern uint16_t gSateliteRemainTime, gSateliteStageRemainTime;

void SATELITE_mode_switch(void);

#endif	// __APP_SATELITE_H_INC__
