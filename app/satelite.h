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

extern bool gSateliteMode, gSateliteDownCounting;
extern uint16_t gSateliteRemainTime, gSateliteStageRemainTime;
extern char gSateliteName[6];

void SATELITE_mode_switch(void);
void SATELITE_next_stage(void);

#define SATELITE_start() do { \
	gSateliteDownCounting = true;		\
} while (0)

#endif	// __APP_SATELITE_H_INC__
