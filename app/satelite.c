/*
 *	uvk5-fw
 *	/app/satelite.c
 *	This file is distributed under Apache License Version 2.0
 *	Copyright (c) 2024 Yao Zi. All rights reserved.
 */

#include"app.h"
#include"misc.h"

bool gSateliteMode;
uint16_t gSateliteRemainTime, gSateliteStageRemainTime;

void
SATELITE_mode_switch(void)
{
	gSateliteMode = !gSateliteMode;
	gUpdateStatus = true;
	gSateliteRemainTime = 11 * 60 + 45;
	gSateliteStageRemainTime = 191;
	gUpdateDisplay = true;
	return;
}
