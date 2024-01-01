/*
 *	uvk5-fw
 *	/app/satelite.c
 *	This file is distributed under Apache License Version 2.0
 *	Copyright (c) 2024 Yao Zi. All rights reserved.
 */

#include<string.h>
#include<stdbool.h>

#include"app/app.h"
#include"app/generic.h"
#include"misc.h"
#include"settings.h"
#include"radio.h"

bool gSateliteMode, gSateliteDownCounting;
uint16_t gSateliteRemainTime, gSateliteStageRemainTime;
char gSateliteName[6];
uint16_t gSateliteNo;
uint8_t gSateliteStage;

static void
satelite_get_time_and_name(void)
{
	gSateliteRemainTime = 40;

	strcpy(gSateliteName, "     ");
	strncpy(gSateliteName, "ISS", strlen("ISS"));
	return;
}

static void
satelite_set_stage_time(void)
{
	gSateliteStageRemainTime = 5;
	return;
}

static inline void
do_set_freq(uint32_t rx, uint32_t tx)
{
	(void)tx;
	GENERIC_Set_Freq(0, rx);
}

static void
satelite_set_freq(void)
{
	uint32_t baseFreq = 43850000;

	gEeprom.CROSS_BAND_RX_TX = CROSS_BAND_OFF;
	gEeprom.DUAL_WATCH = DUAL_WATCH_OFF;
	gEeprom.TX_VFO = 0;
	RADIO_SelectVfos();

	do_set_freq(baseFreq - 250 * gSateliteStage, 0);
	return;
}

static inline void
satelite_enter(void)
{
	gSateliteNo			= 0;
	gSateliteStage			= 0;

	satelite_get_time_and_name();
	satelite_set_stage_time();
	satelite_set_freq();
	return;
}

static inline void
satelite_exit(void)
{
	gSateliteDownCounting = false;
	return;
}

void
SATELITE_mode_switch(void)
{
	if (gSateliteMode)
		satelite_exit();
	else
		satelite_enter();
	gSateliteMode = !gSateliteMode;
	gUpdateStatus = true;
	gUpdateDisplay = true;
	return;
}

void
SATELITE_next_stage(void)
{
	gSateliteStage++;
	satelite_set_stage_time();
	satelite_set_freq();
	return;
}
